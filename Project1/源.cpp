//#include <string>
//#include <iostream>
//#include <boost/filesystem.hpp>
//#include <windows.h>
//#include <thread>
//
////char* ReadFileBlock(char* fileBlock, const size_t OffSet, const size_t CurSize)
////{
////    //�����ļ�����
////    if (OffSet > (m_FileBlockConut - 1))
////    {
////        return nullptr;
////    }
////
////    //�����ڴ棬�洢�ļ�
////    fileBlock = new char[BLOCK_SIZE];
////
////    //���ļ�ָ���ƶ���ָ��λ��
////    m_FileInOut->seekg(OffSet*BLOCK_SIZE);
////    m_FileInOut->read(fileBlock, CurSize);
////
////    return fileBlock;
////}
////
//////д������
////bool WriteFileBlock(char* WriteBlock, const size_t CurSize, const size_t OffSet)
////{
////    if (OffSet == FILE_MAX)
////    {
////        // return false;
////    }
////
////    m_FileInOut->seekp(0);
////    m_FileInOut->write(WriteBlock, CurSize);
////
////    return false;
////}
////
////size_t Getfilesize(const boost::filesystem::path& FileName)
////{
////    return FileName.size( );
////}
//
//
//void doItNextTime( )
//{
//    //�ȸ��±��ط��������
//
//    //�ٽ�����ͬ����Ŀ¼������
//
//    Sleep(3000);
//
//    std::cout << "Time:" << std::endl;
//    doItNextTime( );
//}
//
//
//void doItNextTime1( )
//{
//    //�ȸ��±��ط��������
//
//    //�ٽ�����ͬ����Ŀ¼������
//
//    Sleep(300);
//
//    std::cout << "3333333:" << std::endl;
//    doItNextTime1( );
//}
//#include "qq.h"
//#include "qqq.h"
//
//void ff( )
//{
//    std::cout << fff << "  " << &fff << std::endl;
//    fff = 23450;
//    std::cout << fff << "  " << &fff << std::endl;
//    int fff = 34560;
//    std::cout << fff << "  " << &fff << std::endl;
//}
//
//
//int main( )
//{
//
//    f( );
//
//    ff( );
//
//    f( );
//
//
///*
//    std::thread t1(doItNextTime);
//
//    t1.detach( );
//    doItNextTime1( );*/
//    //std::string FileName="ttt.txt";
//    //boost::filesystem::path FilePath(FileName);
//    //
//    //boost::filesystem::fstream m_FileInOut(FileName, std::ios::out | std::ios::in | std::ios::binary);
//    //if (!m_FileInOut.is_open( ))
//    //{
//    //    std::cout << "�ļ���ʧ��" << std::endl;
//    //    return 0;
//    //}
//
//    //std::cout << "File Name: " << FileName << "\nfilesize:" << FileName.size( )
//    //    << std::endl;
//    ////char* BlocksFile = new char[BLOCK_SIZE];
//    //std::string BlocksFile = "ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCDABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD              ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD ABCDABCDABCDABCD";
//
//    //m_FileInOut.seekp(0);
//    //m_FileInOut.write(BlocksFile.c_str(), BlocksFile.size());
//    //m_FileInOut.close( );
//
//    return 0;
//}

#include <boost/filesystem.hpp> 
#include <iostream> 

int main( )
{
    boost::filesystem::path p("photo.jpg");
    std::cout << p.stem( ) << std::endl;
    std::cout << p.extension( ) << std::endl;
}