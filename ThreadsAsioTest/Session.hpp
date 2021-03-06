#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <algorithm>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include <boost/thread.hpp>
#include "handler_allocator.hpp"
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <cstdio>
#include <mutex>
#include <boost/filesystem.hpp>

class OptLog
{
private:
	std::mutex m_mutex;
	boost::filesystem::fstream fout;
public:

	void log (std::string ss)
	{
		std::lock_guard<std::mutex> locker_m (m_mutex);
		fout << ss << std::endl;
	}

	OptLog () 
    { 
        fout.open ("log.txt",std::ios::app); 
    }

	~OptLog () 
	{
		if (fout.is_open())
		{
			fout.close ();
		}
	}
};

struct File_info 
{
	typedef unsigned long long Size_type;
    unsigned char  m_RequireType;
	Size_type filesize;
	size_t filename_size;
	File_info () : filesize (0), filename_size (0),m_RequireType(0) { }
};

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

	boost::asio::ip::tcp::socket& socket () { return socket_; }

	~Session ()
	{
		std::cout << "\n Thread ID: " << boost::this_thread::get_id () << std::endl;
		if (fp_) fclose (fp_);
		clock_ = clock () - clock_;
		File_info::Size_type bytes_writen = total_bytes_writen_;
		if (clock_ == 0) clock_ = 1;
		double speed = bytes_writen * (CLOCKS_PER_SEC / 1024.0 / 1024.0) / clock_;
		//std::cout << "cost time: " << clock_ / (double)CLOCKS_PER_SEC << " s  "
		//	<< "bytes_writen: " << bytes_writen << " bytes\n"
		//	<< "speed: " << speed << " MB/s\n\n";
		optlog.log ("cost time: " +std::to_string(clock_ / (double)CLOCKS_PER_SEC)+ " s  "
			+ "bytes_writen: " + std::to_string (bytes_writen) + " bytes\n"
			+ "speed: " + std::to_string (speed) + " MB/s\n\n");
		//在文件接收完成时，打开文件，解析文件参数
		/*qiuwanli::user u;
		std::fstream input ("login", std::ios::in | std::ios::binary);
		u.ParseFromIstream (&input);
		std::cout << u.user_id () << std::endl << u.user_type () << std::endl <<
			u.user_password_md5 () << std::endl << u.login_code () << std::endl <<
			u.user_name () << std::endl << u.user_client_uuid () << std::endl;*/
	}

	void start ()
	{
		clock_ = clock ();
		std::cout << "client: " << socket_.remote_endpoint ().address () << "\n";
        socket_.async_receive(
                    boost::asio::buffer(reinterpret_cast<char*>(&file_info_)
                                , sizeof(file_info_))
                    , boost::bind(&Session::handle_header, shared_from_this( )
                                , boost::asio::placeholders::error));
	}

private:
	Session (boost::asio::io_service& io) :
		socket_ (io), fp_ (NULL), total_bytes_writen_ (0)
	{ }

	void handle_header (const boost::system::error_code& error)
	{
        if (error)
        {
            return print_asio_error (error);
        }

        size_t filename_size = file_info_.filename_size;
		if ((filename_size) > k_buffer_size) 
        {
			std::cerr << "Path name is too long!\n";
			return;
		}
		
        //得用async_read, 不能用async_read_some，防止路径名超长时，一次接收不完
		boost::asio::async_read (socket_
                                 , boost::asio::buffer (buffer_
                                                        , file_info_.filename_size)
                                 , boost::bind (&Session::handle_file
                                                , shared_from_this ()
                                                , boost::asio::placeholders::error)
                                );
	}

	//将接受到的数据块，解析为文件名+文件数据
	void handle_file (const boost::system::error_code& error)
	{
        if (error)
        {
			return print_asio_error (error);
        }

		const char *base_name_msg = buffer_ + file_info_.filename_size - 1;
        while (base_name_msg >= buffer_
               && (*base_name_msg != '\\'
               && *base_name_msg != '/'))
        {
			--base_name_msg;
        }
		++base_name_msg;

		const char *basename = "";
		//const char *msg_type = "";

		//将const char* 分割
		std::string str (base_name_msg);
		std::vector<std::string>  vstr;
		boost::split (vstr, str, boost::is_any_of ("+"), boost::token_compress_on);
		basename = vstr[0].c_str ();
		//msg_type = vstr[1].c_str ();;

		//std::cout << "base_name_msg:" << base_name_msg << std::endl;
		//std::cout << "basename:" << basename << std::endl;
		//std::cout << "msg_type:" << vstr[1] << std::endl;

		//std::cout << "Open file: " << basename << " (" << buffer_ << ")\n";
		//std::cout << "FileType:" << std::stoi (vstr[1]) << std::endl;
        optlog.log("base_name_msg:" + str + "basename:" + vstr[0] + "msg_type:"
                   + vstr[1] + "File Size:" + buffer_);

		fp_ = fopen (basename, "wb");
		if (fp_ == NULL) 
        {
			std::cerr << "Failed to open file to write\n";
			return;
		}

        if (file_info_.m_RequireType=='a')
        {
            std::cout << std::endl <<"@@@@@@@@@@@@@@@@@@@@"<< std::endl;
        }
		receive_file_content ();
	}

	void receive_file_content ()
	{
        socket_.async_receive(boost::asio::buffer(buffer_, k_buffer_size)
                              , boost::bind(&Session::handle_write, shared_from_this( )
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

			File_info::Size_type filesize = file_info_.filesize;
            if (total_bytes_writen_ != filesize)
            {
				optlog.log ("filesize not matched! " + std::to_string (total_bytes_writen_) 
                            + "/" + std::to_string (filesize) + "\n");
            }
				//std::cerr << "filesize not matched! " << total_bytes_writen_<< "/" << filesize << "\n";
			return;
		}

		total_bytes_writen_ += fwrite (buffer_, 1, bytes_transferred, fp_);

		receive_file_content ();
	}

	//发送文件
	void send_file_content ()
	{

	}

private:
	clock_t clock_;
	boost::asio::ip::tcp::socket socket_;
	FILE *fp_;
	File_info file_info_;
	File_info::Size_type total_bytes_writen_;
	static const unsigned k_buffer_size = 1024 * 32;
	char buffer_[k_buffer_size];
	OptLog optlog;
};


#endif // !SESSION_HPP
