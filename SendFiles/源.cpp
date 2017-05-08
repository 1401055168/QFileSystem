#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

struct File_info 
{
    unsigned char   m_RequireType;
	size_t          m_FileSize;
	size_t          m_FileNameLength;
	File_info () : m_FileSize(0), m_FileNameLength(0) {}
};

void sender (boost::asio::io_service &io
             , const char*	ip_address
             , unsigned	port
             , const char* filename
             , const char* msg_type)
{
	FILE *fp=nullptr;

	fopen_s (&fp, filename, "rb");
	if (fp == NULL) 
    {
		std::cerr << "cannot open file\n";
		return;
	}

	//ʹ������ָ�룬��ֹ��������쳣ʱ��fcloseδ�����á�
	boost::shared_ptr<FILE> file_ptr (fp, fclose);

	clock_t cost_time = clock ();

	const size_t k_buffer_size = 32 * 1024;
	char buffer[k_buffer_size]{};
	File_info file_info;

	char buf[32 * 1024]{};
	
	strcat_s (buf, filename);
	strcat_s (buf, "+");
	strcat_s (buf, msg_type);
	filename = (const char*)buf;

	//MessageBox ();
	//const char* filename_msg = filename + msg_type;

	size_t filename_size = strlen (filename) + 1;
	size_t file_info_size = sizeof (file_info);
	size_t total_size = file_info_size + filename_size;

	if (total_size > k_buffer_size) 
    {
		std::cerr << "File name is too long";
		return;
	}
	file_info.m_FileNameLength = filename_size;
	fseek (fp, 0, SEEK_END);
	file_info.m_FileSize = ftell (fp);
	rewind (fp);

    memcpy(buffer, &file_info, file_info_size);							//�ļ���Ϣ
    memcpy(buffer + file_info_size, filename, filename_size);			//�ļ���/��Ϣ����

	boost::asio::ip::tcp::socket socket (io);
	socket.connect (boost::asio::ip::tcp::endpoint (boost::asio::ip::address_v4::from_string (ip_address), port));

	std::cout << "Sending file : " << filename << " MsgType:" << msg_type << std::endl;
	size_t len = total_size;

	unsigned long long total_bytes_read = 0;
	while (true) 
    {
		//�ȷ����ļ�ͷ��֮����data
		socket.send (boost::asio::buffer (buffer, len), 0);

        if (feof(fp))
        {
            break;
        }

        len = fread (buffer, 1, k_buffer_size, fp);
		total_bytes_read += len;
	}

	//����ʱ�䡢��С���ٶ�//
	cost_time = clock () - cost_time;
	if (cost_time == 0) cost_time = 1;
    double speed = total_bytes_read * (CLOCKS_PER_SEC / 1024.0 / 1024.0) / cost_time;
    std::cout << "cost time: " << cost_time / (double) CLOCKS_PER_SEC
        << " s " << "  transferred_bytes: " << total_bytes_read
        << " bytes\n" << "speed: " << speed << " MB/s\n\n";
}

int main ()
{
	boost::asio::io_service io_ser;
	try {
        sender(io_ser, "127.0.0.1", 9999, "login - ���� (2)", "1001");
        sender(io_ser, "127.0.0.1", 9999, "login - ���� (3)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (4)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (5)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (6)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (7)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (8)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (9)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (10)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (11)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (12)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (13)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (14)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (15)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (16)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (17)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (18)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (19)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (20)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (21)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (22)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (23)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (24)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (25)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (26)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (27)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (28)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (29)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (30)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (31)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (32)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (33)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (34)", "1001");
		sender (io_ser, "127.0.0.1", 9999, "login - ���� (35)", "1001");
	}
	catch (std::exception& err) 
    {
		std::cerr << err.what () << "\n";
	}

	return 0;
}