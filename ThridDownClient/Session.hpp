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

    //将目标文件进行分割为临时文件
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

    //将文件块发送到存储服务端
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
            //将const char* 分割
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
                //发送文件到存储端
            }
        }
        
        //文件传输完成后，
        std::string fileNameTmp;
        //标记已完成传输的文件 ,并设置定时器响应触发事件
        g_ComData.DateChage |= (size_t) 2;
        for (int index = 0; index < g_ComData.FileListT.file_size( ); ++index)
        {
            if (g_ComData.FileListT.file(index).filename( ) == vstr[0])
            {
                auto x = g_ComData.FileListT.mutable_file(index);
                x->set_filestyle("已上传");
            }
        }
        
        auto xxx = g_ComData.m_UploadFile.begin( );
        fileNameTmp = xxx->m_FileName;
        g_ComData.m_UploadFile.erase(xxx);

        g_ComData.DoneUploadFile.emplace_back(std::make_pair(fileNameTmp, ""));
        //文件传输完成时，向服务端发送文件完整信息
        std::string filename=fileNameTmp;
        filename += "+";
        filename += vstr[1];
        filename += "+";
        char md5[33];
        getFileMD5(fileNameTmp.c_str( ), md5);
        filename += md5;
        filename += "+";

        //获取时间
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

        //获取文件长度
        boost::filesystem::fstream fileTmp(fileNameTmp, std::ios::in);
        fileTmp.seekg(0, std::ios_base::end);
        filename += std::to_string(fileTmp.tellg( ));
        filename += "+";
        filename += std::to_string((fileTmp.tellg( ) / BLOCK_SIZE) + 1);

        //获取用户ID和PS
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

        //删除临时文件
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

        //用于存储分割后的文件块对应的文件名
        std::vector<std::string> vSendFileList;
        
        //遍历当前正打算上传的文件列表
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
		//在文件接收完成时，打开文件，解析文件参数
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

		//用async_read, 不能用async_read_some，防止路径名超长时，一次接收不完
		boost::asio::async_read (socket_
                                 , boost::asio::buffer (buffer_, file_info_.m_FileNameLength)
                                 , boost::bind (&Session::CallBackDiffType
                                                 , shared_from_this ()
                                                 , boost::asio::placeholders::error));
	}

    //先接收文件，在调用析构函数时，调用进程去传数据
    void SaveFileToServer()
    {
        //先解析服务器列表，在上传文件
        DoType = 6;
        //CurKeepFileName = basename;
        //接收文件
        fopen_s(&fp_, vstr[0].c_str( ), "wb");
        if (fp_ == NULL)
        {
            std::cerr << "Failed to open file to write\n";
            return;
        }

        receive_file_content( );
    }

    //根据请求不同，调用不同的处理函数
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

        //将const char* 分割
        std::string str(base_name_msg);
        vstr.clear( );
        boost::split(vstr, str, boost::is_any_of("+"), boost::token_compress_on);
        basename = vstr[0].c_str( );
        const char* vcheck=vstr[1].c_str( );//验证信息


        //解析请求类型，调用不同的处理函数

        if(file_info_.m_ReqiureType=='b')
        {
            //文件秒传,通过设置公共变量通知界面，
            g_ComData.DateChage |= (size_t)1;
            g_ComData.DoneUploadFile.emplace_back(std::make_pair(vstr[0], vstr[1]));
            
            for (int index = 0; index < g_ComData.FileListT.file_size( ); ++index)
            {
                if (g_ComData.FileListT.file(index).filename()==vstr[0])
                {
                    auto x=g_ComData.FileListT.mutable_file(index);
                    x->set_filestyle("已上传");
                }
            }

            return;
        }
        //解析服务器信息进行上传文件
        if (file_info_.m_ReqiureType == 'c')
        {
            SaveFileToServer( );
        }

        //文件块上传结果（成功/失败）
        if (file_info_.m_ReqiureType == 'e')
        {
            if (vstr[2]=="Success")
            {
                //文件块发送成功
            }
            if (vstr[2] == "Fail")
            {
                //文件块发送失败
                //添加到重复发送队列，再次发送
            }
        }

        //返回上传文件的完整性
        if (file_info_.m_ReqiureType == 'h')
        {
            g_ComData.DateChage |= (size_t) 4;
        }


        //case 'x':       //文件下载块INFO
        //    socket_.async_receive(
        //        boost::asio::buffer(buffer_, k_buffer_size)
        //        , boost::bind(&Session::CheckKey
        //        , shared_from_this( )
        //        , boost::asio::placeholders::error));
        //    break;
        //case 'f':       //发送验证结果shibai
        //    socket_.async_receive(
        //        boost::asio::buffer(buffer_, k_buffer_size)
        //        , boost::bind(&Session::handle_file
        //        , this
        //        , boost::asio::placeholders::error));
        //    break;
    }


    //检查客户端携带的PirateKey的MD5是否正确，失败则关闭链接
    void CheckKey(const boost::system::error_code& error)
    {
        //获取本机Prikey的md5与此比较;
        qiuwanli::BlockListForDownCheckTable DownListTable;

        if (!DownListTable.ParseFromString(buffer_))
        {	//打开失败
           // optlog.log("File Parse Fail!");
        }
        else
        {	//解析配置文件
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
                
                //启用线程进行文件传输
                //auto FailList = SendFileToClient(ListForUp);
                //if (FailList.size()==0)
                //{
                //    optlog.log("Sent File Success!");
                //}
                //else
                //{
                //    //尝试重传
                //    SendFileToClient(ListForUp);
                //}

           // }
        }
    }
    
    ////发送文件块，并将发送失败的列表，返回
    //PairVec& SendFileToClient(PairVec& DownList)
    //{
    //    //遍历文件列表，传输完成的从列表中删除
    //    for (auto& FileBlocks : DownList)
    //    {
    //        std::string File512 = FileBlocks.first;
    //        for (auto Block : FileBlocks.second)
    //        {
    //            //查找文件块，若存在则读取文件块和对应块的MD5，并发送给客户端

    //        }
    //    }

    //    return DownList;
    //}


	//将接受到的数据块，解析为文件名+文件数据
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

		//将const char* 分割
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
                //传输文件到存储服务端
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
        //在文件传输完成时，若果传输类型为文件块则，调用文件块写入函数
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

   
	//发送文件
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

        //使用智能指针，防止程序出现异常时，fclose未被调用。
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
        file_info_.m_ReqiureType = msg_type;             //消息类型
        fseek(fp_, 0, SEEK_END);
        file_info_.m_FileSize = ftell(fp_);
        rewind(fp_);

        memcpy(buffer_, &file_info_, file_info_size);						//文件信息
        memcpy(buffer_ + file_info_size, filename, filename_size);			//文件名/消息类型

        std::cout << "Sending file : " << filename << " MsgType:" << msg_type << std::endl;
        size_t len = total_size;

        unsigned long long total_bytes_read = 0;
        while (true)
        {
            //先发送文件头，之后发送data
            socket_.send(boost::asio::buffer(buffer_, len), 0);

            if (feof(fp_))
            {
                break;
            }

            len = fread(buffer_, 1, k_buffer_size, fp_);
            total_bytes_read += len;
        }

        //计算时间、大小和速度//
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

    int DoType;     //当文件接收完成时，记录该做什么事。
    //std::string CurKeepFileName;
};


#endif // !SESSION_HPP
