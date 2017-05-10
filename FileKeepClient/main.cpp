#define _CRT_SECURE_NO_WARNINGS

#include "Servers.hpp"
#include "SendFile.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <windows.h>

ComData g_ComData;

//�����ݽ��г�ʼ��
void initData( )
{
    //���������ļ�
    boost::filesystem::fstream OpFile("config", std::ios::in | std::ios::binary);
    if (!OpFile)
    {
        g_ComData.opplog.log("Conf ��ʧ�ܣ�");
        return;
    }

    if (!g_ComData.Conf.ParseFromIstream(&OpFile))
    {	//��ʧ��
        OpFile.close( );
        g_ComData.opplog.log("qiuwanli::ConfigFile ����ʧ�ܣ�");
        return;
    }

    OpFile.close( );

    //�����ļ�����Ϣ��
    boost::filesystem::fstream OpFileBlockInfo("BlockTable.db", std::ios::in | std::ios::binary);
    if (!OpFileBlockInfo)
    {
        g_ComData.opplog.log("BlockTable.db ��ʧ�ܣ�");
        return;
    }

    if (!g_ComData.BlockTable.ParseFromIstream(&OpFileBlockInfo))
    {	//��ʧ��
        OpFileBlockInfo.close( );
        g_ComData.opplog.log("qiuwanli::ConfigFile ����ʧ�ܣ�");
        return;
    }

    OpFileBlockInfo.close( );
}


//���ݵĸ��£��Լ���ʱ��
void doItNextTime( )
{
    auto CurPath = boost::filesystem::current_path( );
    boost::filesystem::space_info Space = boost::filesystem::space(CurPath);
    
    //�ȸ��±��ط��������
    g_ComData.Conf.set_totalsize((Space.capacity + Space.available) / Size_Mb);
    g_ComData.Conf.set_remainsize(Space.capacity / Size_Mb);
    
    //�ٽ�����ͬ����Ŀ¼����������������
    try
    {
        boost::asio::io_service io;
        SendFile senddata;
        std::string ip = g_ComData.Conf.serversip( );
        senddata.sender(io
                        , g_ComData.Conf.serversip( ).c_str()
                        , std::atoi(g_ComData.Conf.serversport( ).c_str())
                        , "config"
                        , 'a');
    }
    catch (std::exception e)
    {
        g_ComData.opplog.log("Heart Fail!\t");
    }

    //��ʱ60���ڷ�����������
    Sleep(DelayTime_Heart);

    doItNextTime( );
}

void sendBlockInfoToServers( )
{
    //�����15���ڽ��յ��Ŀ���Ϣ�����͸������
    if (g_ComData.BlockTableDiff.block_size( ))
    {
        try
        {
            boost::asio::io_service io;
            SendFile senddata;
            senddata.sender(io, g_ComData.Conf.serversip( ).c_str( )
                            , std::atoi(g_ComData.Conf.serversport( ).c_str( ))
                            , "BlockTableDiff"
                            , 'a');
        } catch (std::exception e)
        {
            g_ComData.opplog.log("Heart Fail!\t");
        }
    }

    //����ϲ�
    g_ComData.BlockTablePreDiff.MergeFrom(g_ComData.BlockTableDiff);
    
    //������ݣ������´κϲ�
    g_ComData.BlockTableDiff.Clear( );

    Sleep(DelayTime_SendInfo);
    
    sendBlockInfoToServers( );
}

//��ʱ��blockinfoд�뵽�ļ�
void WriteToFile( )
{
    if (g_ComData.OpFile.is_open( ))
    {
        g_ComData.OpFile.close( );
    }

    if (g_ComData.OpFileBlockInfo.is_open( ))
    {
        g_ComData.OpFileBlockInfo.close( );
    }
}

void Init( )
{
    //���ļ�
    g_ComData.OpFile.open("config", std::ios::out | std::ios::binary);
    if (!g_ComData.OpFile)
    {
        g_ComData.opplog.log("Conf ��ʧ�ܣ�");
        return;
    }

    //�����ļ�����Ϣ��
    g_ComData.OpFileBlockInfo.open("BlockTable.db", std::ios::out | std::ios::binary);
    if (!g_ComData.OpFileBlockInfo)
    {
        g_ComData.opplog.log("BlockTable.db ��ʧ�ܣ�");
        return;
    }
}

void DelayWirte( )
{
    if (!g_ComData.Conf.SerializePartialToOstream(&g_ComData.OpFile))
    {	//��ʧ��
        g_ComData.OpFile.close( );
        g_ComData.opplog.log("qiuwanli::ConfigFile ���л�ʧ�ܣ�");
        return;
    }

    g_ComData.BlockTable.MergeFrom(g_ComData.BlockTableDiff);

    if (!g_ComData.BlockTable.SerializePartialToOstream(&g_ComData.OpFileBlockInfo))
    {	//��ʧ��
        g_ComData.OpFileBlockInfo.close( );
        g_ComData.opplog.log("qiuwanli::ConfigFile ���л�ʧ�ܣ�");
        return;
    }
    Sleep(DelayWriteToFile);
    DelayWirte( );
}

int main (int argc, char* argv[])
{
    try
    {
        //�ȳ�ʼ������
        initData( );

        //��ʼ�������������Ϣ


        //��ִ��ѭ����������
        boost::thread HeartThread(doItNextTime);
        HeartThread.detach( );

        //��������ȥ���������л��ļ��ĸ���
        //WriteToFile FileManage;
        //boost::thread ManageFileUpdate{WriteToFile( )};
        //ManageFileUpdate.join( );
        boost::thread FileManage([&]( ){
            Init( );
            Sleep(DelayWriteToFile);
            DelayWirte( );
        });


		if (argc != 5)
		{
			std::cerr << "Usage: server <address> <port> <threads> <blocksize>\n";
			return 1;
		}

		boost::asio::ip::address address = boost::asio::ip::address::from_string (argv[1]);
		short port = std::atoi (argv[2]);
		int thread_count = std::atoi (argv[3]);
		size_t block_size = std::atoi (argv[4]);

		boost::asio::io_service ios;

		//server ssss (ios, port);
		server ssss (ios, boost::asio::ip::tcp::endpoint (address, port));

		// Threads not currently supported in this test.
		std::list<boost::thread*> threads;

		while (--thread_count > 0)
		{
			boost::thread* new_thread = new boost::thread (
				boost::bind (&boost::asio::io_service::run,&ios));
			threads.push_back (new_thread);
		}

		ios.run ();

		while (!threads.empty ())
		{
			threads.front ()->join ();

			delete threads.front ();

			threads.pop_front ();
		}

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what () << "\n";
	}

	return 0;
}