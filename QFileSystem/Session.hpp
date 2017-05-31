#ifndef SESSION_HPP
#define SESSION_HPP

#include <algorithm>
#include <boost/bind.hpp>
#include <iostream>
#include <list>
#include <mutex>
#include <cstdio>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>

#include "handler_allocator.hpp"
//#include "FileManage.hpp"
//#include "SerializationToStream.hpp"
#include "public.h"

//ComData g_ComData;

class Session : public boost::enable_shared_from_this<Session>
{
public:

    static void print_asio_error(const boost::system::error_code& error)
    {
        std::cerr << error.message( ) << "\n";
    }

    static boost::shared_ptr<Session> create(boost::asio::io_service& io)
    {
        return boost::shared_ptr<Session>(new Session(io));
    }

    boost::asio::ip::tcp::socket& socket( )
    {
        return socket_;
    }

    ~Session( )
    {
        if (fp_)
        {
            fclose(fp_);
        }

        WriteBlock_ToFile( );

        std::cout << "\n Thread ID: " << boost::this_thread::get_id( ) << std::endl;

        clock_ = clock( ) - clock_;
        DataBlockTypeInfo::Size_type bytes_writen = total_bytes_writen_;
        if (clock_ == 0)
        {
            clock_ = 1;
        }

        double speed = bytes_writen * (CLOCKS_PER_SEC / 1024.0 / 1024.0) / clock_;

        //���ļ��������ʱ�����ļ��������ļ�����
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
                //extern ComData g_ComData;
                //qiuwanli::BlockInfo* addItem = g_ComData.BlockTableDiff.add_block( );

            }

        }
    }

    void start( )
    {
        clock_ = clock( );
        std::cout << "client: " << socket_.remote_endpoint( ).address( ) << "\n";

        socket_.async_receive(
            boost::asio::buffer(reinterpret_cast<char*>(&file_info_)
            , sizeof(file_info_))
            , boost::bind(&Session::handle_header
            , shared_from_this( )
            , boost::asio::placeholders::error));

    }

private:
    Session(boost::asio::io_service& io)
        : socket_(io)
        , fp_(NULL)
        , total_bytes_writen_(0)
    {

    }

    void handle_header(const boost::system::error_code& error)
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
        boost::asio::async_read(socket_
                                , boost::asio::buffer(buffer_, file_info_.m_FileNameLength)
                                , boost::bind(&Session::CallBackDiffType
                                , shared_from_this( )
                                , boost::asio::placeholders::error));
    }

    //��������ͬ�����ò�ͬ�Ĵ�����
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
        boost::split(vstr, str, boost::is_any_of("+"), boost::token_compress_on);
        basename = vstr[0].c_str( );

        if (file_info_.m_ReqiureType == 'a')
        {
            //�����û������룬��֤
            bool isok = false;
            for (int index = 0; index < g_ComData.m_UserhasFile.user_size( ); ++index)
            {
                if (g_ComData.m_UserhasFile.user(index).userid().c_str()==vstr[2])
                {
                    if (g_ComData.m_UserhasFile.user(index).userps( ).c_str( )==vstr[3])
                    {
                        isok = true;
                    }
                }
            }
            if (!isok)
            {
                return;
            }

            //�����ļ�SHA512
            for (int i = 0; i < g_ComData.m_FileListTable.filelist_size( ); ++i)
            {
                if (g_ComData.m_FileListTable.filelist(i).filesha512( ) == vstr[1])
                {
                    boost::asio::io_service io_ser;
                    SendFile sender;
                    try
                    {
                        std::string name = vstr[0];
                        name += "+";
                        name += vstr[1];
                        sender.senderLitter(io_ser, "127.0.0.1", 8089, name.c_str( ), 'b');
                    } catch (std::exception& e)
                    {
                        std::cout << e.what( ) << std::endl;
                    }
                    return;
                }
            }

            //�ļ������ڣ����ҵ�ǰ�ɴ洢�ķ���ˣ�������
            qiuwanli::ClientConfigFileTable ClientInfo;
            for (int i = 0; i < g_ComData.m_ClientConfigFile.clientinfo_size( ); ++i)
            {
                const qiuwanli::ClientConfigFile& conf = g_ComData.m_ClientConfigFile.clientinfo(i);

                PublicData.DoClientConfigFileTable(ClientInfo.add_clientinfo( )
                                                   , conf.cilentid( )
                                                   , conf.saveip( )
                                                   , conf.prikey( )
                                                   , conf.keymd5( )
                                                   , conf.totalsize( )
                                                   , conf.remainsize( ));
            }
                boost::filesystem::fstream Op("ClientConfigFile", std::ios::out | std::ios::binary | std::ios::app);

                if (!ClientInfo.SerializePartialToOstream(&Op))
                {
                    return ;
                }
                Op.close( );
                std::string  filename = "ClientConfigFile";
                filename += "+";
                filename += vstr[1];

                boost::asio::io_service io_ser;
                SendFile sender;
                try
                {
                    sender.sender(io_ser, "127.0.0.1", 8089, filename.c_str(), 'c');
                } catch (std::exception& e)
                {
                    std::cout << e.what( ) << std::endl;
                }
            

            ////�����������ͣ����ò�ͬ�Ĵ�����
            //switch (file_info_.m_ReqiureType)
            //{
            //case 'a':       //����������
            //    socket_.async_receive(
            //        boost::asio::buffer(buffer_, k_buffer_size)
            //        , boost::bind(&Session::CheckKey
            //        , shared_from_this( )
            //        , boost::asio::placeholders::error));
            //    break;
            //case 'b':       //������֤���
            //    socket_.async_receive(
            //        boost::asio::buffer(buffer_, k_buffer_size)
            //        , boost::bind(&Session::handle_file
            //        , this
            //        , boost::asio::placeholders::error));
            //    break;
            //default:
            //    break;
            //}
        }

        //�����Ѿ��ϴ����洢����˵��ļ����Ӧ����Ϣ
        if (file_info_.m_ReqiureType == 'f')
        {
            PublicData.DoBlockList4DownTable(g_ComData.m_BlockToFileTableTmp.add_blocklistfordown( )
                                             , vstr[0], std::stol(vstr[2]), vstr[1], vstr[3]);
            // g_ComData.BlockUploadList.emplace_back(ComData::Vec4(vstr[0], vstr[1], std::stol(vstr[2]), vstr[3]));
        }

        //�ļ�������ɣ����п�ͳ��
        if (file_info_.m_ReqiureType == 'g')
        {
            //����ɾѡ����
            std::vector<ComData::Vec4> CheckBlockList;
            for (auto iter = g_ComData.BlockUploadList.begin( ); iter != g_ComData.BlockUploadList.end( ); ++iter)
            {
                if (iter->m_Sha512.compare( vstr[1]))
                {
                    CheckBlockList.emplace_back(ComData::Vec4(iter->m_Sha512, iter->m_md5, iter->m_BlockNumer, iter->m_IP));
                    iter = g_ComData.BlockUploadList.erase(iter);
                }
            }

            for (int index = 0; index < g_ComData.m_BlockToFileTable.blocklistfordown_size( ); ++index)
            {
                auto item = g_ComData.m_BlockToFileTableTmp.blocklistfordown(index);
                PublicData.DoBlockList4DownTable(g_ComData.m_BlockToFileTableTmp.add_blocklistfordown( ), item.filesha512( ), item.blocknumer( ), item.blockmd5( ), item.saveserversip( ));
            }


            //����ļ��Ƿ�����
            bool Full = false;
            if (CheckBlockList.size()==std::stoi(vstr[5]))
            {
                long num = 0;
                //������������ӵ�BlockInfoTable��,�����л����ļ�
                for (auto x : CheckBlockList)
                {
                    num += x.m_BlockNumer;
                }
                long count = std::stol(vstr[5]);
                if (num ==((count + long(1))*count / 2))
                {
                    Full = true;
                }
            }


            std::string  filenamed = vstr[0];
            filenamed += "+";
            if (Full)
            {
                filenamed += "true";
                PublicData.DoFileInfoListTable(g_ComData.m_FileListTable.add_filelist( )
                                               , vstr[0], vstr[1], vstr[2], vstr[3], vstr[4]
                                               , atoll(vstr[5].c_str( )), atoll(vstr[6].c_str( )), vstr[7]);
                
                //������Ϣ��ʱ��ű�д�뵽����Ϣ�б�
                g_ComData.m_BlockToFileTable.MergeFrom(g_ComData.m_BlockToFileTableTmp);

                //������Ӧ����������UI
                g_ComData.m_UIChange |= (size_t)1;
                g_ComData.NotifyFileList.emplace_back(ComData::VecStr5(vstr[0], vstr[1], vstr[5], vstr[3], "��"));
            }
            else
            {   //�����ݲ������򽫣�Ը���ݿ���Ϣ���뵽����Ϣ��ʱ��ű�
                filenamed += "false";
                //for (auto x : CheckBlockList)
                //{
                //    g_ComData.BlockUploadList.push_back(x);
                //}
                //g_ComData.BlockUploadList.insert(g_ComData.BlockUploadList.end( ), CheckBlockList.begin( ), CheckBlockList.end( ));
            }

            try
            {
                boost::asio::io_service io_ser;
                SendFile send;
                send.senderLitter(io_ser, "127.0.0.1", 8089, filenamed.c_str( ), 'h');
            } catch (std::exception& e)
            { 
                std::cout << e.what( ) << std::endl;
            }
            
        }

        //�洢�˷�������������
        if (file_info_.m_ReqiureType=='i')
        {
            g_ComData.m_UIChange |= size_t(2);
            g_ComData.m_HeartList.emplace_back(ComData::HeartLink(vstr[0], atol(vstr[1].c_str( )), atol(vstr[2].c_str( )), vstr[3]));
        }

    }
    //���ͻ���Я����PirateKey��MD5�Ƿ���ȷ��ʧ����ر�����
    void CheckKey(const boost::system::error_code& error)
    {
        //��ȡ����Prikey��md5��˱Ƚ�;
        qiuwanli::BlockListForDownCheckTable DownListTable;

        if (!DownListTable.ParseFromString(buffer_))
        {	//��ʧ��
        }
        else
        {	//���������ļ�
            //std::string md5 = g_ComData.Conf.prikeymd5( );
            //if (DownListTable.prikeymd5( ).compare(md5) == 0)
            //{
            //    PairVec ListForUp;
            //    for (size_t index = 0; index < DownListTable.blocklistfordown_size( ); ++index)
            //    {
            //        const qiuwanli::BlockListForDown& DownList = DownListTable.blocklistfordown(index);
            //        std::pair<std::string, std::vector<unsigned long>> pairs;
            //        pairs.first = DownList.filesha512( );
            //        std::vector<unsigned long> vec;
            //        for (size_t index = 0; index < DownList.blocks_size( ); ++index)
            //        {
            //            const qiuwanli::Block& Block = DownList.blocks(index);
            //            //vec.emplace_back(Block.blockitem( ));
            //        }

            //        ListForUp.emplace_back(pairs);
            //    }

            //    //�����߳̽����ļ�����
            //    auto FailList = SendFileToClient(ListForUp);
            //    if (FailList.size( ) == 0)
            //    {
            //        optlog.log("Sent File Success!");
            //    }
            //    else
            //    {
            //        //�����ش�
            //        SendFileToClient(ListForUp);
            //    }

            //}
        }
    }

    //�����ļ��飬��������ʧ�ܵ��б�����
    PairVec& SendFileToClient(PairVec& DownList)
    {
        //�����ļ��б�������ɵĴ��б���ɾ��
        for (auto& FileBlocks : DownList)
        {
            std::string File512 = FileBlocks.first;
            for (auto Block : FileBlocks.second)
            {
                //�����ļ��飬���������ȡ�ļ���Ͷ�Ӧ���MD5�������͸��ͻ���

            }
        }

        return DownList;
    }


    //�����ܵ������ݿ飬����Ϊ�ļ���+�ļ�����
    void handle_file(const boost::system::error_code& error)
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
        //const char *msg_type = "";

        //��const char* �ָ�
        std::string str(base_name_msg);
        //std::vector<std::string>  vstr;
        boost::split(vstr, str, boost::is_any_of("+"), boost::token_compress_on);
        basename = vstr[0].c_str( );

        fopen_s(&fp_,basename, "wb");
        if (fp_ == NULL)
        {
            std::cerr << "Failed to open file to write\n";
            return;
        }

        receive_file_content( );
    }

    void receive_file_content( )
    {
        socket_.async_receive(boost::asio::buffer(buffer_, k_buffer_size)
                              , boost::bind(&Session::handle_write
                              , shared_from_this( )
                              , boost::asio::placeholders::error
                              , boost::asio::placeholders::bytes_transferred)
        );
    }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (error)
        {
            if (error != boost::asio::error::eof)
            {
                return print_asio_error(error);
            }

            DataBlockTypeInfo::Size_type filesize = file_info_.m_FileSize;
            if (total_bytes_writen_ != filesize)
            {
            }

            return;
        }
        total_bytes_writen_ += fwrite(buffer_, 1, bytes_transferred, fp_);

        receive_file_content( );
    }

    //�����ļ�
    void send_file_content_sender(const char*	ip_address
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

                                                                            //boost::asio::ip::tcp::socket socket(io);
                                                                            /* socket_.connect(boost::asio::ip::tcp::endpoint(
                                                                            boost::asio::ip::address_v4::from_string(ip_address)
                                                                            , port));*/

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

};


#endif // !SESSION_HPP
