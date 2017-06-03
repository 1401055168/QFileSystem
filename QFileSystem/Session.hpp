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
std::mutex g_mutex;
std::mutex g_mutex1;
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

        std::cout << "\n Thread ID: " << boost::this_thread::get_id( ) << std::endl;

        //���ļ��������ʱ�����ļ��������ļ�����
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

    void dof( )
    {
        //�������ݿ����         
        std::mutex some_mutex;
        std::vector<std::string> vtmp;
        vtmp.push_back(vstr[0]);
        vtmp.push_back(vstr[2]);
        vtmp.push_back(vstr[1]);
        vtmp.push_back(vstr[3]);

        {
            std::lock_guard<std::mutex> guard(g_mutex);
            g_ComData.g_AddBlockTmp.push_back(vtmp);
        }
        {
            std::lock_guard<std::mutex> guard(g_mutex1);
            PublicData.DoBlockList4DownTable(g_ComData.m_BlockToFileTableTmp.add_blocklistfordown( )
                                             , vstr[0]
                                             , std::stoll(vstr[2])
                                             , vstr[1]
                                             , vstr[3]);
        }

        //����״̬λ
        g_ComData.m_UIChange |= size_t(4);
    }

    bool DoBlockList4DownTable(qiuwanli::BlockList4Down* BlockList
                               , const std::string& FileSHA512
                               , unsigned long long BlockNumer
                               , const std::string& BlockMD5
                               , const std::string& SaveServersIP)
    {
        BlockList->set_filesha512(FileSHA512);
        BlockList->set_blocknumer(BlockNumer);
        BlockList->set_blockmd5(BlockMD5);
        BlockList->set_saveserversip(SaveServersIP);

        return true;
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

        //�ļ���+SHA512+UserID+UsPS
        //����Ŀ¼�������֤�ļ��Ƿ��Ѵ�����
        if (file_info_.m_ReqiureType == 'a')
        {
            std::cout << "����Ŀ¼�������֤�ļ��Ƿ��Ѵ�����" << std::endl;
            //�����û������룬��֤
            bool isok = false;
            {
                std::lock_guard<std::mutex> guard(g_mutex);
                for (int index = 0; index < g_ComData.m_UserhasFile.user_size( ); ++index)
                {
                    if (g_ComData.m_UserhasFile.user(index).userid( ).c_str( ) == vstr[2] &&
                        g_ComData.m_UserhasFile.user(index).userps( ).c_str( ) == vstr[3])
                    {
                        isok = true;
                    }
                }
            }
            if (!isok)
            {
                //socket_.shutdown(boost::asio::ip::tcp::socket::shutdown);
                socket_.shutdown_both;
                return;
            }

            //�����ļ�SHA512
            {
                std::lock_guard<std::mutex> guard(g_mutex);

                for (int i = 0; i < g_ComData.m_FileListTable.filelist_size( ); ++i)
                {
                    if (g_ComData.m_FileListTable.filelist(i).filesha512( ).compare(vstr[1]) == 0)
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
            }

            //�ļ������ڣ����ҵ�ǰ�ɴ洢�ķ���ˣ�������
            qiuwanli::ClientConfigFileTable ClientInfo;

            {
                std::lock_guard<std::mutex> guard(g_mutex);
                for (int i = 0; i < g_ComData.m_ClientConfigFile.clientinfo_size( ); ++i)
                {

                    const qiuwanli::ClientConfigFile& conf = g_ComData.m_ClientConfigFile.clientinfo(i);
                    PublicData.DoClientConfigFileTable(ClientInfo.add_clientinfo( )
                                                       , conf.cilentid( )
                                                       , conf.saveip( )
                                                       , ""                 //conf.prikey( )
                                                       , conf.keymd5( )
                                                       , 0                  //conf.totalsize( )
                                                       , conf.remainsize( ));
                }
            }

            //����ʱ�ļ������ڴ���б��ļ�
            boost::filesystem::fstream Op("ClientConfigFileTmp", std::ios::out | std::ios::binary | std::ios::trunc);

            if (!ClientInfo.SerializePartialToOstream(&Op))
            {
                return;
            }
            Op.close( );

            std::string  filename = "ClientConfigFileTmp";
            filename += "+";
            filename += vstr[1];

            boost::asio::io_service io_ser;
            SendFile sender;
            try
            {
                sender.sender(io_ser, "127.0.0.1", 8089, filename.c_str( ), 'c');
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
            std::cout << "�����Ѿ��ϴ����洢����˵��ļ����Ӧ����Ϣ" << std::endl;
            dof( );
        }

        //�ļ�������ɣ����п�ͳ��
        if (file_info_.m_ReqiureType == 'g')
        {
            size_t countTimes = std::stoi(vstr[4]);
            Sleep(10 * countTimes);
            //����ɾѡ����
            std::cout << "�ļ�������ɣ����п�ͳ��" << std::endl;
            qiuwanli::BlockList4DownTable TmpList;
            int countBlock = 0;
            {
                std::lock_guard<std::mutex> guard(g_mutex);
                for (int index = 0; index < g_ComData.m_BlockToFileTableTmp.blocklistfordown_size( ); ++index)
                {
                    auto item = g_ComData.m_BlockToFileTableTmp.mutable_blocklistfordown(index);
                    std::string itemT = item->filesha512( );

                    std::vector<std::string> strss;
                    boost::split(strss, itemT, boost::is_any_of("-"), boost::token_compress_on);

                    if (strss[0].compare(vstr[1]) == 0)
                    {
                        ++countBlock;
                    }

                    if (item->filesha512( ).compare(vstr[1]) == 0)
                    {
                        PublicData.DoBlockList4DownTable(TmpList.add_blocklistfordown( )
                                                         , item->filesha512( )
                                                         , item->blocknumer( )
                                                         , item->blockmd5( )
                                                         , item->saveserversip( ));
                        item->Clear( );
                    }

                }
            }

            size_t count = std::stoi(vstr[4]);
            std::string  filenamed = vstr[0];
            filenamed += "+";
            //���������ֽ��кϲ��������л����ļ�
            if (countBlock == count)
            {
                std::lock_guard<std::mutex> guard(g_mutex1);
                g_ComData.m_BlockToFileTable.MergeFrom(TmpList);

                //������Ӧ����������UI
                g_ComData.m_UIChange |= (size_t) 1;

                //������Ϣ��ʱ��ű�д�뵽����Ϣ�б�
                g_ComData.NotifyFileList.push_back(ComData::VecStr5(vstr[0], vstr[1], vstr[5], vstr[3], "��"));

                //����������Ϣ��ӵ��ļ��б�
                PublicData.DoFileInfoListTable(g_ComData.m_FileListTable.add_filelist( )
                                               , vstr[1]
                                               , vstr[2]
                                               , vstr[0]
                                               , vstr[3]
                                               , "��"
                                               , atoll(vstr[4].c_str( ))
                                               , atoll(vstr[5].c_str( ))
                                               , "");
            }
            else
            {
                filenamed += "false";
                std::lock_guard<std::mutex> guard(g_mutex);
                //�����ݲ������򽫣�Ը���ݿ���Ϣ���뵽����Ϣ��ʱ��ű�
                g_ComData.m_BlockToFileTableTmp.MergeFrom(TmpList);
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
        if (file_info_.m_ReqiureType == 'i')
        {
            std::lock_guard<std::mutex> guard(g_mutex);
            g_ComData.m_UIChange |= size_t(2);
            g_ComData.m_HeartList.push_back(ComData::HeartLink(vstr[0]
                                            , atol(vstr[1].c_str( ))
                                            , atol(vstr[2].c_str( ))
                                            , vstr[3]));
        }

        //�ļ���������
        if (file_info_.m_ReqiureType == 'A')
        {
            std::cout << "�ļ���������" << std::endl;
            //�û������룬��֤
            bool isok = false;
            {
                std::lock_guard<std::mutex> guard(g_mutex);
                for (int index = 0; index < g_ComData.m_UserhasFile.user_size( ); ++index)
                {
                    if (g_ComData.m_UserhasFile.user(index).userid( ) == vstr[2] &&
                        g_ComData.m_UserhasFile.user(index).userps( ) == vstr[3])
                    {
                        isok = true;
                    }
                }
            }
            if (!isok)
            {
                socket_.shutdown_both;
                return;
            }

            //����ļ��Ƿ���ں�����
            unsigned long long CountBlock;

            //��ǰ���ߵĴ洢�������б�
            qiuwanli::FileInfoList FileInfo;
            qiuwanli::ClientConfigFileTable StorageServiceIPList;
            //qiuwanli::BlockList4Down WaitingForDownloadedFileBlockList;
            qiuwanli::BlockList4DownTable BlocksList;

            if (g_ComData.m_FileListTable.filelist_size( ) > 0 &&
                g_ComData.m_UserhasFile.user_size( ) > 0)
            {
                //����ļ��б����Ƿ����
                for (int index = 0; index < g_ComData.m_FileListTable.filelist_size( ); ++index)
                {
                    if (g_ComData.m_FileListTable.filelist(index).filesha512( ).compare(vstr[1]))
                    {
                        //�����ļ������ص���Ϣ�������ļ���ԭ����֤
                        FileInfo.set_filesha512(g_ComData.m_FileListTable.filelist(index).filesha512( ));
                        FileInfo.set_filemd5(g_ComData.m_FileListTable.filelist(index).filemd5( ));
                        FileInfo.set_filename(g_ComData.m_FileListTable.filelist(index).filename( ));
                        FileInfo.set_filecreatedate(g_ComData.m_FileListTable.filelist(index).filecreatedate( ));
                        FileInfo.set_isshared(g_ComData.m_FileListTable.filelist(index).isshared( ));
                        FileInfo.set_fileallblock(g_ComData.m_FileListTable.filelist(index).fileallblock( ));
                        FileInfo.set_filetotalsize(g_ComData.m_FileListTable.filelist(index).filetotalsize( ));
                        FileInfo.set_tag(g_ComData.m_FileListTable.filelist(index).tag( ));

                        //������ǰ���ߵĴ洢�������������ڵ�IP��ַ���浽�������б���
                        for (int index3 = 0; index3 < g_ComData.m_ClientConfigFile.clientinfo_size( ); ++index3)
                        {
                            const qiuwanli::ClientConfigFile item = g_ComData.m_ClientConfigFile.clientinfo(index3);
                            
                            //���˵������ߴ洢��            
                            if (item.online( ).compare("ON") )
                                //��ʣ��ռ��С����1GB��Server
                                //&& ( item.remainsize()>(unsigned long long)(1024 * 1024)))
                            {
                                PublicData.DoClientConfigFileTable(StorageServiceIPList.add_clientinfo( )
                                                                   , item.cilentid()
                                                                   , item.saveip()
                                                                   , ""             //������˽Կ
                                                                   ,item.keymd5()
                                                                   ,item.totalsize()
                                                                   ,item.remainsize() );
                            }
                        }

                        //���ļ���Ӧ�Ĵ洢λ����Ϣ���˳���
                        CountBlock = g_ComData.m_FileListTable.filelist(index).fileallblock( );
                        for (int index2 = 0; index2 < g_ComData.m_BlockToFileTable.blocklistfordown_size( ); ++index2)
                        {
                            const qiuwanli::BlockList4Down blockItem = g_ComData.m_BlockToFileTable.blocklistfordown(index2);
                            if (blockItem.filesha512( ).compare(vstr[1]))
                            {
                                //�޳������ߵĴ洢���������洢���ļ�����Ϣ
                                for (int index4 = 0; index < StorageServiceIPList.clientinfo_size( ); ++index4)
                                {
                                    if (StorageServiceIPList.clientinfo(index4).saveip( ).compare(blockItem.saveserversip()))
                                    {
                                        DoBlockList4DownTable(BlocksList.add_blocklistfordown( )
                                                              , blockItem.filesha512( )
                                                              , blockItem.blocknumer( )
                                                              , blockItem.blockmd5( )
                                                              , blockItem.saveserversip( ));
                                    }
                                }
                            }
                        }

                        break;
                    }
                }
            }

            //�������Ӷ���
            boost::asio::io_service io_ser;
            SendFile send;

            //�ļ��Ƿ����//�ļ��Ƿ�����
            if ((BlocksList.blocklistfordown_size( ) > 0) && (BlocksList.blocklistfordown_size( ) == CountBlock))
            {
                ////�����ߵĴ洢�������б�д�뵽�ļ�
                //std::string ClientListKeepFile = "OnLineStorageServiceIPList";
                //boost::filesystem::ofstream ClientOutFile(ClientListKeepFile, std::ios::trunc | std::ios::out | std::ios::binary);
                //if (!ClientOutFile.is_open( ))
                //    return;
                //if (!StorageServiceIPList.SerializePartialToOstream(&ClientOutFile))
                //    return;
                //ClientOutFile.close( );


                //���ļ���洢�б����л����ݵ��ļ�
                std::string BlocksListKepFile = BlocksList.blocklistfordown(0).filesha512( ) + "-BlockInfoList";
                boost::filesystem::ofstream BlockOutFile(BlocksListKepFile, std::ios::out | std::ios::trunc | std::ios::binary);
                if (!BlockOutFile.is_open( ))
                    return;
                if (!BlocksList.SerializePartialToOstream(&BlockOutFile))
                    return;
                BlockOutFile.close( );
                BlocksList.Clear( );

                //try
                //{
                //    //���ļ��洢���Լ����ߵĴ洢���б��Ƚ��ļ����͸��ͻ���
                //    send.sender(io_ser, "127.0.0.1", 8089, ClientListKeepFile.c_str( ), 'Z');
                //    send.sender(io_ser, "127.0.0.1", 8089, BlocksListKepFile.c_str( ), 'Z');
                //} catch (const std::exception& e)
                //{
                //    std::cout << e.what( ) << std::endl;
                //    return;
                //}

                //�ٷ����ļ���������
                try
                {
                    //����Ϣ���洢���ļ���+�ļ�SHA512+�ļ�MD5+�ļ���+�ļ�����+�ļ���С
                    std::string filename12 = BlocksListKepFile;
                    filename12 += "+";
                    filename12 += FileInfo.filesha512( );
                    filename12 += "+";
                    filename12 += FileInfo.filemd5( );
                    filename12 += "+";
                    filename12 += FileInfo.filename( );
                    filename12 += "+";
                    filename12 += std::to_string(FileInfo.fileallblock( ));
                    filename12 += "+";
                    filename12 += std::to_string(FileInfo.filetotalsize( ));

                    send.sender(io_ser, "127.0.0.1", 8089, filename12.c_str( ), 'C');
                } 
                catch (std::exception& e)
                {
                    std::cout << e.what( ) << std::endl;
                }
            }
            else
            {
                //�����ڣ��򷵻ش�����Ϣ
                try
                {
                    std::string filename12 = vstr[0];
                    filename12 += "+";
                    filename12 += vstr[1];

                    send.senderLitter(io_ser, "127.0.0.1", 8089, filename12.c_str( ), 'B');
                } catch (std::exception& e)
                {
                    std::cout << e.what( ) << std::endl;
                }
            }
        }

        //�����ļ���������
        if (file_info_.m_ReqiureType == 'G')
        {
            std::cout << "�����ļ���������" << std::endl;
            //����û����ڣ������ļ�������ļ��Ƿ����
            //�����ڣ�������һ��8λ��������������ļ�SHA512��
            //�ļ���С���û�ID���һ�������ʶ�����͸��ͻ���
            boost::asio::io_service io_ser;
            SendFile send;

            if (CheckKey(vstr[3], vstr[4]))
            {
                for (int index = 0; index < g_ComData.m_SharedTable.sharedinfo_size( ); ++index)
                {
                    if (g_ComData.m_SharedTable.sharedinfo(index).sha512( ).compare(vstr[1]) == 0)
                    {
                        char randStr[9];
                        rand_str(randStr, 8);
                        std::string msg = vstr[0];
                        msg += "+";
                        msg += vstr[1];
                        msg += "+";
                        msg += vstr[2];
                        msg += "+";
                        msg += vstr[3];
                        msg += "+";
                        msg += randStr;

                        try
                        {
                            send.senderLitter(io_ser, "127.0.0.1", 8089, msg.c_str( ), 'I');
                        } catch (std::exception& e)
                        {
                            std::cout << e.what( ) << std::endl;
                        }
                    }
                }
            }
            else
            {
                try
                {
                    send.senderLitter(io_ser, "127.0.0.1", 8089, "�ļ�����ʧ��", 'H');
                } catch (std::exception& e)
                {
                    std::cout << e.what( ) << std::endl;
                }
            }
        }

        //����ɾ������
        if (file_info_.m_ReqiureType == 'J')
        {
            std::cout << "����ɾ������" << std::endl;
            //����û����ڣ����������ļ��������������Ƿ����
            //�����ڣ�����ɾ�����������ڣ��򷵻ش�����Ϣ
            std::string done = "ɾ��ʧ��";
            if (CheckKey(vstr[2],vstr[3]))
            {
                for (int index = g_ComData.m_SharedTable.sharedinfo_size( ); index >= 0; --index)
                {
                    auto xx = g_ComData.m_SharedTable.sharedinfo(index);
                    if (xx.sha512().compare(vstr[0])&& 
                        xx.verificationcode().compare(vstr[1]) )
                    {
                        xx.Clear( );
                        done.clear( );
                        done = "ɾ���ɹ�";
                    }
                }
            }

            std::string name = vstr[0];
            name += "+";
            name += done;

            boost::asio::io_service io_ser;
            SendFile send;
            try
            {
                send.senderLitter(io_ser, "127.0.0.1", 8089, name.c_str(), 'H');
            } catch (std::exception& e)
            {
                std::cout << e.what( ) << std::endl;
            }
        }

        //ɾ���ļ���������
        if (file_info_.m_ReqiureType == 'N')
        {
            std::cout << "ɾ���ļ���������" << std::endl;
            if (CheckKey( "","" ))
            {

            }
            else
            {

            }
        }

        //����������������
        if (file_info_.m_ReqiureType == 'X')
        {
            std::cout << "����������������" << std::endl;
            if (CheckKey(vstr[4],vstr[5]))
            {
                for (int index = 0; index < g_ComData.m_SharedTable.sharedinfo_size( ); ++index)
                {
                    if (g_ComData.m_SharedTable.sharedinfo(index).sha512( ).compare(vstr[0]) &&
                        std::to_string(g_ComData.m_SharedTable.sharedinfo(index).filesize( )).compare(vstr[1]) &&
                        g_ComData.m_SharedTable.sharedinfo(index).verificationcode( ).compare(vstr[2]) &&
                        g_ComData.m_SharedTable.sharedinfo(index).userid( ).compare(vstr[3]))
                    {
                     //�����ļ����б�����洢�б����͸��ͻ���
                     //����ļ��Ƿ���ں�����
                        qiuwanli::BlockList4DownTable BlocksList;
                        unsigned long long CountBlock;
                        qiuwanli::FileInfoList FileInfo;
                        if (g_ComData.m_FileListTable.filelist_size( ) > 0 &&
                            g_ComData.m_UserhasFile.user_size( ) > 0)
                        {
                            for (int index = 0; index < g_ComData.m_FileListTable.filelist_size( ); ++index)
                            {
                                if (g_ComData.m_FileListTable.filelist(index).filesha512( ).compare(vstr[1]))
                                {
                                    FileInfo.set_filesha512(g_ComData.m_FileListTable.filelist(index).filesha512( ));
                                    FileInfo.set_filemd5(g_ComData.m_FileListTable.filelist(index).filemd5( ));
                                    FileInfo.set_filename(g_ComData.m_FileListTable.filelist(index).filename( ));
                                    FileInfo.set_filecreatedate(g_ComData.m_FileListTable.filelist(index).filecreatedate( ));
                                    FileInfo.set_isshared(g_ComData.m_FileListTable.filelist(index).isshared( ));
                                    FileInfo.set_fileallblock(g_ComData.m_FileListTable.filelist(index).fileallblock( ));
                                    FileInfo.set_filetotalsize(g_ComData.m_FileListTable.filelist(index).filetotalsize( ));
                                    FileInfo.set_tag(g_ComData.m_FileListTable.filelist(index).tag( ));

                                    CountBlock = g_ComData.m_FileListTable.filelist(index).fileallblock( );
                                    for (int index2 = 0; index < g_ComData.m_BlockToFileTable.blocklistfordown_size( ); ++index2)
                                    {
                                        if (g_ComData.m_BlockToFileTable.blocklistfordown(index2).filesha512( ).compare(vstr[1]))
                                        {
                                            DoBlockList4DownTable(BlocksList.add_blocklistfordown( )
                                                                  , g_ComData.m_BlockToFileTable.blocklistfordown(index2).filesha512( )
                                                                  , g_ComData.m_BlockToFileTable.blocklistfordown(index2).blocknumer( )
                                                                  , g_ComData.m_BlockToFileTable.blocklistfordown(index2).blockmd5( )
                                                                  , g_ComData.m_BlockToFileTable.blocklistfordown(index2).saveserversip( )
                                            );
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                        //�ļ��Ƿ����//�ļ��Ƿ�����
                        if ((BlocksList.blocklistfordown_size( ) <= 0) && (BlocksList.blocklistfordown_size( ) != CountBlock))
                        {
                            //�����ڣ��򷵻ش�����Ϣ
                            try
                            {
                                std::string filename12 = vstr[0];
                                filename12 += "+";
                                filename12 += vstr[1];

                                boost::asio::io_service io_ser;
                                SendFile send;
                                send.senderLitter(io_ser, "127.0.0.1", 8089, filename12.c_str( ), 'B');
                            } catch (std::exception& e)
                            {
                                std::cout << e.what( ) << std::endl;
                            }
                        }
                        else
                        {
                            //���л����ݵ��ļ�
                            boost::filesystem::ofstream outfile(BlocksList.blocklistfordown(0).filesha512( )
                                                                , std::ios::out | std::ios::trunc | std::ios::binary);
                            //�����ڣ����ļ��洢���͸��ͻ���
                            try
                            {
                                //�ļ�SHA512+�ļ�MD5+�ļ���+�ļ�����+�ļ���С
                                std::string filename12 = FileInfo.filesha512( );
                                filename12 += "+";
                                filename12 += FileInfo.filemd5( );
                                filename12 += "+";
                                filename12 += FileInfo.filename( );
                                filename12 += "+";
                                filename12 += std::to_string(FileInfo.fileallblock( ));
                                filename12 += "+";
                                filename12 += std::to_string(FileInfo.filetotalsize( ));

                                boost::asio::io_service io_ser;
                                SendFile send;
                                send.senderLitter(io_ser, "127.0.0.1", 8089, filename12.c_str( ), 'C');
                            } catch (std::exception& e)
                            {
                                std::cout << e.what( ) << std::endl;
                            }

                            BlocksList.Clear( );
                        }
                    }
                }
            }
            else
            {
                try
                {
                    std::string name="�ļ������ڻ��������ʧЧ\r\n�볢�������������ӣ�";
                    boost::asio::io_service io_ser;
                    SendFile send;
                    send.senderLitter(io_ser, "127.0.0.1", 8089, name.c_str( ), 'C');
                } catch (std::exception& e)
                {

                }
            }
        }

    }
    
    //��������ַ���
    char *rand_str(char *str, const int len)
    {
        int i;
        for (i = 0; i < len; ++i)
            str[i] = 'A' + rand( ) % 26;
        str[++i] = '\0';
        return str;
    }

    //���ͻ���Я����PirateKey��MD5�Ƿ���ȷ��ʧ����ر�����
    bool CheckKey(std::string id,std::string ps)
    {
        //�û������룬��֤
        std::lock_guard<std::mutex> guard(g_mutex);
        for (int index = 0; index < g_ComData.m_UserhasFile.user_size( ); ++index)
        {
            if (g_ComData.m_UserhasFile.user(index).userid( ) == id &&
                g_ComData.m_UserhasFile.user(index).userps( ) == ps)
            {
                return true;
            }
        }
        return false;
    }

    ////�����ļ��飬��������ʧ�ܵ��б�����
    //PairVec& SendFileToClient(PairVec& DownList)
    //{
    //    //�����ļ��б�������ɵĴ��б���ɾ��
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
