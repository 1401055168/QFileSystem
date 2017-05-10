#ifndef PUBLIC_H__
#define PUBLIC_H__
#include <vector>
#include <iostream>
#include <string>

//�ļ����С//
const unsigned int BLOCK_SIZE = 1024 * 64;
const unsigned int FILE_MAX = ~0;
//k*1024
const unsigned int k_times = 32;

const unsigned long long k_buffer_size = 1024 * k_times;
//��������

const unsigned long DelayTime_Heart = 60 * 1000;
const unsigned long DelayTime_SendInfo = 15 * 1000;
const unsigned long DelayWriteToFile = DelayTime_Heart * 15;

const unsigned int Size_Kb = 1 << 10;
const unsigned int Size_Mb = 1 << 20;
const unsigned int Size_Gb = 1 << 30;


//��Ϣ���ͣ��Լ���Ϣ���Ƴ��ȣ��ļ����С
struct DataBlockTypeInfo
{
    using  Size_type = unsigned long long;
    char m_ReqiureType;
    Size_type m_FileSize;
    unsigned int m_FileNameLength;

    DataBlockTypeInfo( ) : m_FileSize(0), m_FileNameLength(0) ,m_ReqiureType(0){ }
};


//Require Type
char Type_f[ ] = {'a'       //������֤
                ,'b'        //��֤���
                ,'c'        //
                ,'d'        //
                ,'e'        //
                ,'f'        //
                ,'g'        //
                ,'h'        //
                ,'s'        //
                ,'j'        //
                ,'t'        //
                ,'x'        //
                
                ,'y'        //�ļ����������б�+��֤��Ϣ
                ,'z'        //�ļ���+MD5+filesha512
                ,'w'        //�ļ�����Ϣ+IP ���͵�Ŀ¼������
                ,'q'        //�ļ����ϴ�
                ,'p'};      //

using PairVec = std::vector<std::pair<std::string, std::vector<unsigned long>>>;
#endif // !PUBLIC_H__
