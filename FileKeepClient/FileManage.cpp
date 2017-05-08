#include "FileManage.hpp"
#include "public.h"

FileManage::FileManage( ) 
    : m_FileName("")
    , m_FilePtrCurPos(0)
    , m_FileBlockConut(0)
    //, m_FileBlockBuf(nullptr)
{ 

}

FileManage::~FileManage( )
{
    /*if (m_FileBlockBuf!=nullptr)
    {
        delete [ ]m_FileBlockBuf;
        m_FileBlockBuf = nullptr;
    }*/

    if (m_FileInOut.is_open( ))
    {
        m_FileInOut.close( );
    }
}

//��Դ�ļ��Ѵﵽ���ֵ���򴴽����ļ�
bool FileManage::CreateFile( std::string& File)
{

    return true;
}

bool FileManage::open(const std::string& FileName)
{
    if (0 == FileName.compare(""))
    {
        return false;
    }

    m_FileName = FileName;
    m_FileInOut.open(FileName, std::ios::out | std::ios::in | std::ios::binary);
    if (!m_FileInOut.is_open( ))
    {
        return false;
    }

    //�����ļ��ܳ���
    m_FileInOut.seekp(0, std::ios_base::end);
    m_FileBlockConut = m_FileInOut.tellp( )/BLOCK_SIZE+1;

    //m_FileBlockBuf = new char[BLOCK_SIZE];

    return true;
}

char* FileManage::ReadFileBlock(char* fileBlock, const size_t OffSet, const size_t CurSize)
{
    //�����ļ�����
    if (OffSet > (m_FileBlockConut-1))
    {
        return nullptr;
    }

    //�����ڴ棬�洢�ļ�
    fileBlock = new char[BLOCK_SIZE];

    //���ļ�ָ���ƶ���ָ��λ��
    m_FileInOut.seekg(OffSet*BLOCK_SIZE);
    m_FileInOut.read(fileBlock, CurSize);

    return fileBlock;
}

//д������
bool FileManage::WriteFileBlock(char* WriteBlock, const size_t CurSize,const size_t OffSet )
{
    if (OffSet == FILE_MAX)
    {
        return false;
    }

    m_FileInOut.seekp(OffSet*BLOCK_SIZE);
    m_FileInOut.write(WriteBlock, CurSize);

    delete []WriteBlock;

    return false;
}

size_t FileManage::Getfilesize(const boost::filesystem::path& FileName)
{
    return FileName.size( );
}
