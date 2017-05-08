#define _CRT_SECURE_NO_WARNINGS

#include "Servers.hpp"
#include "FileDataStruct.pb.h"
#include "SendFile.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <windows.h>

qiuwanli::ConfigFile Conf;
qiuwanli::BlockInfoTable BlockTable;
OptLog opplog;

//�����ݽ��г�ʼ��
void initData( )
{
    //���������ļ�
    boost::filesystem::fstream OpFile("config", std::ios::in | std::ios::binary);
    if (!OpFile)
    {
        opplog.log("Conf ��ʧ�ܣ�");
        return;
    }

    if (!Conf.ParseFromIstream(&OpFile))
    {	//��ʧ��
        OpFile.close( );
        opplog.log("qiuwanli::ConfigFile ����ʧ�ܣ�");
        return;
    }

    OpFile.close( );

    //�����ļ�����Ϣ��
    boost::filesystem::fstream OpFileBlockInfo("BlockTable.db", std::ios::in | std::ios::binary);
    if (!OpFileBlockInfo)
    {
        opplog.log("BlockTable.db ��ʧ�ܣ�");
        return;
    }

    if (!BlockTable.ParseFromIstream(&OpFileBlockInfo))
    {	//��ʧ��
        OpFileBlockInfo.close( );
        opplog.log("qiuwanli::ConfigFile ����ʧ�ܣ�");
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
    Conf.set_totalsize((Space.capacity + Space.available) / Size_Mb);
    Conf.set_remainsize(Space.capacity / Size_Mb);
    
    //�ٽ�����ͬ����Ŀ¼����������������
    try
    {
        boost::asio::io_service io;
        SendFile senddata;
        senddata.sender(io, Conf.serversip, Conf.serversport, "config", 'a');
    }
    catch (std::exception& e)
    {
        opplog.log("Heart Fail!\t" + e.what);
    }

    //��ʱ60���ڷ�����������
    Sleep(DelayTime_Heart);

    doItNextTime( );
}


//��ʱ��blockinfoд�뵽�ļ�
class WriteToFile
{
public:
    WriteToFile( )
    {
        Init( );
    }

    ~WriteToFile( )
    {
        if (OpFile.is_open( ))
        {
            OpFile.close( );
        }

        if (OpFileBlockInfo.is_open( ))
        {
            OpFileBlockInfo.close( );
        }
    }

    void Init()
    {
        //���ļ�
        OpFile.open("config", std::ios::out | std::ios::binary);
        if (!OpFile)
        {
            opplog.log("Conf ��ʧ�ܣ�");
            return;
        }

        //�����ļ�����Ϣ��
        boost::filesystem::ofstream OpFileBlockInfo("BlockTable.db", std::ios::out | std::ios::binary);
        if (!OpFileBlockInfo)
        {
            opplog.log("BlockTable.db ��ʧ�ܣ�");
            return;
        }

        Sleep(DelayWriteToFile);
        DelayWirte( );
    }

    void DelayWirte( )
    {
        if (!Conf.SerializePartialToOstream(&OpFile))
        {	//��ʧ��
            OpFile.close( );
            opplog.log("qiuwanli::ConfigFile ���л�ʧ�ܣ�");
            return;
        }

        if (!BlockTable.SerializePartialToOstream(&OpFileBlockInfo))
        {	//��ʧ��
            OpFileBlockInfo.close( );
            opplog.log("qiuwanli::ConfigFile ���л�ʧ�ܣ�");
            return;
        }

        Sleep(DelayWriteToFile);
        DelayWirte( );
    }

private:
    boost::filesystem::ofstream OpFile;
    boost::filesystem::ofstream OpFileBlockInfo;
};

int main (int argc, char* argv[])
{
	try
	{
        //�ȳ�ʼ������
        initData( );

        //��ִ��ѭ����������
        boost::thread HeartThread(doItNextTime);
        HeartThread.detach( );

        //��������ȥ���������л��ļ��ĸ���
        WriteToFile FileManage;
        boost::thread ManageFileUpdate(FileManage);
        ManageFileUpdate.detach( );


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