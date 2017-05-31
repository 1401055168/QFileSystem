#ifndef SESSION_HPP
#define SESSION_HPP

#include <algorithm>
#include <boost/bind.hpp>
#include <iostream>
#include <list>
#include <mutex>
#include <cstdio>
#include <ctime>
#include <locale>


#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>

#include "handler_allocator.hpp"
#include "MD5.hpp"
#include "public.h"

#include "SendFile.hpp"

class Session : public boost::enable_shared_from_this<Session>
{
public:

	static void print_asio_error (const boost::system::error_code& error)
	{
		std::cerr << error.message () << "\n";
	}

	static boost::shared_ptr<Session> create (boost::asio::io_service& io)
	{
		return boost::shared_ptr<Session> (new Session (io));
	}

	boost::asio::ip::tcp::socket& socket () 
    { 
        return socket_; 
    }

    //��Ŀ���ļ����зָ�Ϊ��ʱ�ļ�
    void splistFile(std::string& filName,std::vector<std::string>& vSendFileList)
    {
        boost::filesystem::fstream InFile(filName, std::ios::in | std::ios::binary);
        InFile.seekg(0, std::ios_base::end);
        size_t endpos = InFile.tellg( );
        char bufferFile[BLOCK_SIZE];
        InFile.seekg(0, std::ios_base::beg);
        //int curpos = 0;
        size_t countNum = endpos / BLOCK_SIZE + 1;
        int cunt = 0;
        size_t readSize = 0;

        do {
            ++cunt;
            std::string blockname = vstr[0];
            blockname += "-";
            blockname += std::to_string(cunt);
            readSize = BLOCK_SIZE;
            boost::filesystem::fstream files(blockname, std::ios::out | std::ios::binary);
            if ( countNum>1)
            {
                InFile.read(bufferFile, BLOCK_SIZE);
            }
            else
            {
                readSize = endpos % BLOCK_SIZE;
                InFile.read(bufferFile, readSize);
            }

            files.write(bufferFile, readSize);
            files.close( );

            vSendFileList.push_back(blockname);
        } while (--countNum);

        InFile.close( );
    }

    //���ļ��鷢�͵��洢�����
    void sendFileBlockToServer(std::vector<std::string>& vSendFileList )
    {
        auto iterFile = vSendFileList.begin( );
        //auto iterServerIP = vpList.begin( );
        size_t index = 0;

        for (;iterFile!=vSendFileList.end()&&1;++iterFile,++index)
        {
            if (index == g_ComData.ServerTable.clientinfo_size())
            {
                index = 0;
            }
            char md5[33];
            getFileMD5(iterFile->c_str(), md5);

            std::string filename1 = *iterFile;
            //��const char* �ָ�
            std::vector<std::string> uuu;
            boost::split(uuu, filename1, boost::is_any_of("-"), boost::token_compress_on);

            auto Itemm = g_ComData.ServerTable.clientinfo(index);

            std::string filenames = filename1;
            filenames += "+";
            filenames += uuu[1];
            filenames += "+";
            filenames += Itemm.keymd5();
            filenames += "+";
            filenames += md5;

            try
            {
                boost::asio::io_service io_ser;
                SendFile sends;
                sends.sender(io_ser, Itemm.saveip().c_str(), 8289, filenames.c_str( ), 'd');
            }
            catch (CException* e)
            {
                //�����ļ����洢��
            }
        }
        
        //�ļ�������ɺ�
        std::string fileNameTmp;
        //�������ɴ�����ļ� ,�����ö�ʱ����Ӧ�����¼�
        g_ComData.DateChage |= (size_t) 2;
        for (int index = 0; index < g_ComData.FileListT.file_size( ); ++index)
        {
            if (g_ComData.FileListT.file(index).filename( ) == vstr[0])
            {
                auto x = g_ComData.FileListT.mutable_file(index);
                x->set_filestyle("���ϴ�");
            }
        }
        
        auto xxx = g_ComData.m_UploadFile.begin( );
        fileNameTmp = xxx->m_FileName;
        g_ComData.m_UploadFile.erase(xxx);

        g_ComData.DoneUploadFile.emplace_back(std::make_pair(fileNameTmp, ""));
        //�ļ��������ʱ�������˷����ļ�������Ϣ
        std::string filename=fileNameTmp;
        filename += "+";
        filename += vstr[1];
        filename += "+";
        char md5[33];
        getFileMD5(fileNameTmp.c_str( ), md5);
        filename += md5;
        filename += "+";

        //��ȡʱ��
        std::string stimeTmp;
        char mbstr[32];
        time_t now;
        time(&now);
        struct tm tmTmp;
        localtime_s(&tmTmp, &now);
        if (std::strftime(mbstr, 32, "%F %T",&tmTmp ))
        {
            stimeTmp = mbstr;
        }
        filename += stimeTmp;

        //��ȡ�ļ�����
        boost::filesystem::fstream fileTmp(fileNameTmp, std::ios::in);
        fileTmp.seekg(0, std::ios_base::end);
        filename += std::to_string(fileTmp.tellg( ));
        filename += "+";
        filename += std::to_string((fileTmp.tellg( ) / BLOCK_SIZE) + 1);

        //��ȡ�û�ID��PS
        filename += "+";
        filename += g_ComData.user.userid( );
        filename += "+";
        filename += g_ComData.user.userps( );

        try
        {
            SendFile send; 
            boost::asio::io_service io_ser;
            send.senderLitter(io_ser, "127.0.0.1", 8189, filename.c_str( ), 'g');
        } catch (std::exception* e)
        {
           // std::cout << e.what( ) << std::endl;
        }

        //ɾ����ʱ�ļ�
        for (auto x:vSendFileList)
        {
            boost::filesystem::path p(x);
            boost::filesystem::remove(p);
        }
    }

    void DoSendFile( )
    {
        boost::filesystem::fstream InFile("ClientConfigFile", std::ios::in | std::ios::binary);

        //qiuwanli::ClientConfigFileTable ServerTable;

        if (!g_ComData.ServerTable.ParsePartialFromIstream(&InFile))
            ;

        //���ڴ洢�ָ����ļ����Ӧ���ļ���
        std::vector<std::string> vSendFileList;
        
        //������ǰ�������ϴ����ļ��б�
        for (auto iter:g_ComData.m_UploadFile)
        {
            std::string file = iter.m_FileName;
            splistFile( file,vSendFileList);
            sendFileBlockToServer(vSendFileList);
        }

        g_ComData.curUploadFile.resize(0);
    }

	~Session ()
	{
        WriteBlock_ToFile( );

		std::cout << "\n Thread ID: " << boost::this_thread::get_id () << std::endl;

        clock_ = clock( ) - clock_;
        DataBlockTypeInfo::Size_type bytes_writen = total_bytes_writen_;
        if (clock_ == 0)
        {
            clock_ = 1;
        }

        double speed = bytes_writen * (CLOCKS_PER_SEC / 1024.0 / 1024.0) / clock_;
		//���ļ��������ʱ�����ļ��������ļ�����
	}

	void start ()
	{
		clock_ = clock ();
		std::cout << "client: " << socket_.remote_endpoint ().address () << "\n";

        socket_.async_receive(
                    boost::asio::buffer(reinterpret_cast<char*>(&file_info_)
                                        , sizeof(file_info_))
                    , boost::bind(&Session::handle_header
                                , shared_from_this( )
                                , boost::asio::placeholders::error));

	}

private:
	Session (boost::asio::io_service& io) 
        : socket_ (io)
        , fp_ (NULL)
        , total_bytes_writen_ (0)
	{ 
        DoType = 0;
    }

	void handle_header (const boost::system::error_code& error)
	{
        if (error)
        {
            return print_asio_error(error);
        }

		size_t filename_size = file_info_.m_FileNameLength;
		if ((filename_size) > k_buffer_size) 
        {
			std::cerr << "Path name is too long!\n";

			return;
		}

		//��async_read, ������async_read_some����ֹ·��������ʱ��һ�ν��ղ���
		boost::asio::async_read (socket_
                                 , boost::asio::buffer (buffer_, file_info_.m_FileNameLength)
                                 , boost::bind (&Session::CallBackDiffType
                                                 , shared_from_this ()
                                                 , boost::asio::placeholders::error));
	}

    //�Ƚ����ļ����ڵ�����������ʱ�����ý���ȥ������
    void SaveFileToServer()
    {
        //�Ƚ����������б������ϴ��ļ�
        DoType = 6;
        //CurKeepFileName = basename;
        //�����ļ�
        fopen_s(&fp_, vstr[0].c_str( ), "wb");
        if (fp_ == NULL)
        {
            std::cerr << "Failed to open file to write\n";
            return;
        }

        receive_file_content( );
    }

    //��������ͬ�����ò�ͬ�Ĵ�������
    void CallBackDiffType(const boost::system::error_code &error)
    {
        if (error)
        {
            return print_asio_error(error);
        }

        const char *base_name_msg = buffer_ + file_info_.m_FileNameLength - 1;
        while (base_name_msg >= buffer_ &&
            (*base_name_msg != '\\'
               && *base_name_msg != '/'))
        {
            --base_name_msg;
        }
        ++base_name_msg;

        const char *basename = base_name_msg;

        //��const char* �ָ�
        std::string str(base_name_msg);
        vstr.clear( );
        boost::split(vstr, str, boost::is_any_of("+"), boost::token_compress_on);
        basename = vstr[0].c_str( );
        const char* vcheck=vstr[1].c_str( );//��֤��Ϣ


        //�����������ͣ����ò�ͬ�Ĵ�������

        if(file_info_.m_ReqiureType=='b')
        {
            //�ļ��봫,ͨ�����ù�������֪ͨ���棬
            g_ComData.DateChage |= (size_t)1;
            g_ComData.DoneUploadFile.emplace_back(std::make_pair(vstr[0], vstr[1]));
            
            for (int index = 0; index < g_ComData.FileListT.file_size( ); ++index)
            {
                if (g_ComData.FileListT.file(index).filename()==vstr[0])
                {
                    auto x=g_ComData.FileListT.mutable_file(index);
                    x->set_filestyle("���ϴ�");
                }
            }

            return;
        }
        //������������Ϣ�����ϴ��ļ�
        if (file_info_.m_ReqiureType == 'c')
        {
            SaveFileToServer( );
        }

        //�ļ����ϴ�������ɹ�/ʧ�ܣ�
        if (file_info_.m_ReqiureType == 'e')
        {
            if (vstr[2]=="Success")
            {
                //�ļ��鷢�ͳɹ�
            }
            if (vstr[2] == "Fail")
            {
                //�ļ��鷢��ʧ��
                //���ӵ��ظ����Ͷ��У��ٴη���
            }
        }

        //�����ϴ��ļ���������
        if (file_info_.m_ReqiureType == 'h')
        {
            g_ComData.DateChage |= (size_t) 4;
        }


        //case 'x':       //�ļ����ؿ�INFO
        //    socket_.async_receive(
        //        boost::asio::buffer(buffer_, k_buffer_size)
        //        , boost::bind(&Session::CheckKey
        //        , shared_from_this( )
        //        , boost::asio::placeholders::error));
        //    break;
        //case 'f':       //������֤���shibai
        //    socket_.async_receive(
        //        boost::asio::buffer(buffer_, k_buffer_size)
        //        , boost::bind(&Session::handle_file
        //        , this
        //        , boost::asio::placeholders::error));
        //    break;
    }


    //���ͻ���Я����PirateKey��MD5�Ƿ���ȷ��ʧ����ر�����
    void CheckKey(const boost::system::error_code& error)
    {
        //��ȡ����Prikey��md5��˱Ƚ�;
        qiuwanli::BlockListForDownCheckTable DownListTable;

        if (!DownListTable.ParseFromString(buffer_))
        {	//��ʧ��
           // optlog.log("File Parse Fail!");
        }
        else
        {	//���������ļ�
            //std::string md5= g_ComData.Conf.prikeymd5( );
            /*if (DownListTable.prikeymd5().compare(md5) == 0)
            {
                PairVec ListForUp;
                for (size_t index = 0; index < DownListTable.blocklistfordown_size( ); ++index)
                {
                    const qiuwanli::BlockListForDown& DownList = DownListTable.blocklistfordown(index);
                    std::pair<std::string, std::vector<unsigned long>> pairs;
                    pairs.first = DownList.filesha512( );
                    std::vector<unsigned long> vec;
                    for (size_t index = 0; index < DownList.blocks_size( ); ++index)
                    {
                        const qiuwanli::Block& Block = DownList.blocks(index);
                        vec.emplace_back(Block.blockitem( ));
                    }

                    ListForUp.emplace_back(pairs);
                }*/
                
                //�����߳̽����ļ�����
                //auto FailList = SendFileToClient(ListForUp);
                //if (FailList.size()==0)
                //{
                //    optlog.log("Sent File Success!");
                //}
                //else
                //{
                //    //�����ش�
                //    SendFileToClient(ListForUp);
                //}

           // }
        }
    }
    
    ////�����ļ��飬��������ʧ�ܵ��б�������
    //PairVec& SendFileToClient(PairVec& DownList)
    //{
    //    //�����ļ��б���������ɵĴ��б���ɾ��
    //    for (auto& FileBlocks : DownList)
    //    {
    //        std::string File512 = FileBlocks.first;
    //        for (auto Block : FileBlocks.second)
    //        {
    //            //�����ļ��飬���������ȡ�ļ���Ͷ�Ӧ���MD5�������͸��ͻ���

    //        }
    //    }

    //    return DownList;
    //}


	//�����ܵ������ݿ飬����Ϊ�ļ���+�ļ�����
	void handle_file (const boost::system::error_code& error)
	{
        if (error)
        {
            return print_asio_error(error);
        }

		const char *base_name_msg = buffer_ + file_info_.m_FileNameLength - 1;
        while (base_name_msg >= buffer_&& 
             (*base_name_msg != '\\'
               && *base_name_msg != '/'))
        {
            --base_name_msg;
        }
		++base_name_msg;

		const char *basename = base_name_msg;
		//const char *msg_type = "";

		//��const char* �ָ�
        std::string str(base_name_msg);
        //std::vector<std::string>  vstr;
        boost::split(vstr, str, boost::is_any_of("+"), boost::token_compress_on);
		basename = vstr[0].c_str ();

        fopen_s(&fp_, basename, "wb");
		if (fp_ == NULL) 
        {
			std::cerr << "Failed to open file to write\n";
			return;
		}

		receive_file_content ();
	}

	void receive_file_content ()
	{
		socket_.async_receive (boost::asio::buffer (buffer_, k_buffer_size)
               , boost::bind (&Session::handle_write
                                , shared_from_this ()
                                , boost::asio::placeholders::error
                                , boost::asio::placeholders::bytes_transferred)
        );
	}

	void handle_write (const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (error) 
        {
            if (error != boost::asio::error::eof)
            {
                return print_asio_error(error);
            }

            DataBlockTypeInfo::Size_type filesize = file_info_.m_FileSize;
            if (total_bytes_writen_ == filesize)
            {
                if (fp_)
                {
                    fclose(fp_);
                }
                //�����ļ����洢�����
                if (DoType == 6)
                    DoSendFile( );
            }

            return;
		}
		total_bytes_writen_ += fwrite (buffer_, 1, bytes_transferred, fp_);

		receive_file_content ();
	}


    void WriteBlock_ToFile( )
    {
        //���ļ��������ʱ��������������Ϊ�ļ����򣬵����ļ���д�뺯��
        if (file_info_.m_ReqiureType == 'a')
        {
            fopen_s(&fp_, vstr[0].c_str( ), "rb");
            if (fp_ == NULL)
            {
                std::cerr << "Failed to open file to write\n";
                return;
            }
            else
            {
                std::string filesha512;
                //size_t offset = m_FileManage.WriteFileBlockEnd(buffer_,k_buffer_size);
                extern ComData g_ComData;
                qiuwanli::BlockInfo* addItem = g_ComData.BlockTableDiff.add_block( );
                
                //addItem->filesha512 = vstr[0];
                //addItem->blockmd5 = vstr[1];
                //addItem->savefilename = m_FileManage.getFileName( );
                //addItem->blockmd5 = vstr[2];
                //addItem->cursize = total_bytes_writen_;
                //addItem->fileblockoffset = offset;

                //m_serial.MakeBlockInfo(addItem, vstr[0], vstr[1]
                //                       , m_FileManage.getFileName( )
                //                       , std::atoi(vstr[2].c_str( ))
                //                       , total_bytes_writen_, offset);
                
            }

        }
    }

   
	//�����ļ�
    void send_file_content_sender(  const char*	ip_address
                                  , unsigned	port
                                  , const char* filename
                                  , const char msg_type)
    {
        fopen_s(&fp_, filename, "rb");
        if (fp_ == NULL)
        {
            std::cerr << "cannot open file\n";
            return;
        }

        //ʹ������ָ�룬��ֹ��������쳣ʱ��fcloseδ�����á�
        boost::shared_ptr<FILE> file_ptr(fp_, fclose);
        clock_t cost_time = clock( );

        size_t filename_size = strlen(filename) + 1;
        size_t file_info_size = sizeof(file_info_);
        size_t total_size = file_info_size + filename_size;

        if (total_size > k_buffer_size)
        {
            std::cerr << "File name is too long";
            return;
        }

        file_info_.m_FileNameLength = filename_size;
        file_info_.m_ReqiureType = msg_type;             //��Ϣ����
        fseek(fp_, 0, SEEK_END);
        file_info_.m_FileSize = ftell(fp_);
        rewind(fp_);

        memcpy(buffer_, &file_info_, file_info_size);						//�ļ���Ϣ
        memcpy(buffer_ + file_info_size, filename, filename_size);			//�ļ���/��Ϣ����

        std::cout << "Sending file : " << filename << " MsgType:" << msg_type << std::endl;
        size_t len = total_size;

        unsigned long long total_bytes_read = 0;
        while (true)
        {
            //�ȷ����ļ�ͷ��֮����data
            socket_.send(boost::asio::buffer(buffer_, len), 0);

            if (feof(fp_))
            {
                break;
            }

            len = fread(buffer_, 1, k_buffer_size, fp_);
            total_bytes_read += len;
        }

        //����ʱ�䡢��С���ٶ�//
        cost_time = clock( ) - cost_time;
        if (cost_time == 0)
        {
            cost_time = 1;
        }

        double speed = total_bytes_read * (CLOCKS_PER_SEC / 1024.0 / 1024.0) / cost_time;
        std::cout << "cost time: " << cost_time / (double) CLOCKS_PER_SEC
            << " s " << "  transferred_bytes: " << total_bytes_read
            << " bytes\n" << "speed: " << speed << " MB/s\n\n";
    }

private:
	clock_t clock_;
	boost::asio::ip::tcp::socket socket_;
	FILE *fp_;
    DataBlockTypeInfo file_info_;
    DataBlockTypeInfo::Size_type total_bytes_writen_;
	char buffer_[k_buffer_size];
    std::vector<std::string>  vstr;

    //FileManage m_FileManage;
    //qiuwanli::SerialToStream m_serial;

    int DoType;     //���ļ��������ʱ����¼����ʲô�¡�
    //std::string CurKeepFileName;
};


#endif // !SESSION_HPP