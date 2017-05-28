#ifndef PUBLIC_H__
#define PUBLIC_H__
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <queue>
#include "PublicStruct.pb.h"

//�ļ����С//
const unsigned int BLOCK_SIZE = 1024 * 64;
const unsigned int FILE_MAX = ~0;
//k*1024
const unsigned int k_times = 32;

const unsigned long k_buffer_size = 1024 * k_times;
//��������

const unsigned long DelayTime_Heart = 60 * 1000;
const unsigned long DelayTime_SendInfo = 15 * 1000;
const unsigned long DelayWriteToFile = DelayTime_Heart * 15;

const unsigned int Size_Kb = 1 << 10;
const unsigned int Size_Mb = 1 << 20;
const unsigned int Size_Gb = 1 << 30;

using PairVec = std::vector<std::pair<std::string, std::vector<unsigned long>>>;

//��Ϣ���ͣ��Լ���Ϣ���Ƴ��ȣ��ļ����С
struct DataBlockTypeInfo
{
    using  Size_type = unsigned long long;
    unsigned char m_ReqiureType;
    Size_type m_FileSize;
    size_t m_FileNameLength;
    //char m_UserID[11];
    DataBlockTypeInfo( ) : m_FileSize(0), m_FileNameLength(0), m_ReqiureType(0) { }
};

struct ComData
{
    qiuwanli::ConfigFile Conf;
    qiuwanli::BlockInfoTable BlockTable;
    qiuwanli::BlockInfoTable BlockTableDiff;
    qiuwanli::BlockInfoTable BlockTablePreDiff;
    qiuwanli::FileInfoListTable FileInfoList;
    qiuwanli::ClientConfigFileTable ClientConfigFile;
    qiuwanli::UserInfo user;

    boost::filesystem::ofstream OpFile;
    boost::filesystem::ofstream OpFileBlockInfo;
    
    std::vector<std::pair<std::string, std::string>> curUploadFile;
    struct Vec3
    {
        std::string m_FileName;
        std::string m_FileSHA512;
        int m_type;
        Vec3(std::string& s1, std::string& s2, int type) 
            :m_FileName(s1), m_FileSHA512(s2), m_type(type)
        {}
    };
    std::vector<Vec3> m_UploadFile;
    std::vector<std::pair<std::string, std::string>> DoneUploadFile;
    size_t DateChage = 0;
};

extern ComData g_ComData;

#endif // !PUBLIC_H__
