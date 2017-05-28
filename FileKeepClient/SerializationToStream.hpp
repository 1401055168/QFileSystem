#ifndef SERIALIZATION_TO_STREAM_HPP
#define SERIALIZATION_TO_STREAM_HPP

//#include "FileDataStruct.pb.h"

#include "PublicStruct.pb.h"

namespace qiuwanli
{

class SerialToStream
{
public:
    SerialToStream( ) { }
    ~SerialToStream( ) { }

    /**
     * ��ȡ���������ļ�
     * @param Config       �����ļ������л�����
     * @param SaveClientID �洢��ID
     * @param SaveClientIP �洢��IP
     * @param Prikey       ˽Կ
     * @param PriKeyMD5    ˽Կ��MD5
     * @param TotalSize    �����ܿռ�
     * @param RemainSize   ʣ��ռ�
     */
    void MakeSerailToStream(qiuwanli::ConfigFile* Config
                            , const std::string& SaveClientID
                            , const std::string& SaveClientIP
                            , const std::string& Prikey
                            , const std::string& PriKeyMD5
                            , const unsigned long TotalSize
                            , const unsigned long RemainSize);

    /**
     * �����ļ���洢��Ϣ
     * @param Blocks          �ļ������
     * @param FileSHA512      �ļ���SHA512
     * @param BlockMd5        �ļ����MD5��֤��Ϣ
     * @param SaveFileName    ���洢���ļ���
     * @param BlockNum        �ļ�����
     * @param CurSize         �ļ��鵱ǰ��С
     * @param FileBlockOffSet �ļ��洢ƫ����
     */
    void MakeBlockInfo(qiuwanli::BlockInfo* Blocks
                       , const std::string& FileSHA512
                       , const std::string& BlockMd5
                       , const std::string& SaveFileName
                       , const unsigned long BlockNum
                       , const unsigned long CurSize
                       , const unsigned long FileBlockOffSet = 0);

    //���ͻ�����Ϣ������д�룬���������л�
    void MakeBlockInfoSend(qiuwanli::BlockInfo * Blocks
                           , const std::string & FileSHA512
                           , const std::string & BlockMd5
                           , const std::string & SaveFileName
                           , const unsigned long BlockNum
                           , const unsigned long CurSize);
    /**
     * [MakeBlock description]
     * @param Block     [description]
     * @param BlockItem [description]
     */
    //void MakeBlock(qiuwanli::Block* Block, const unsigned long BlockItem);
    /**
     * [MakeBlockListForDown description]
     * @param DownList      [description]
     * @param FileSHA512    [description]
     * @param SaveServersIP [description]
     * @param FileMd5       [description]
     * @param Block         [description]
     */
    //void MakeBlockListForDown(qiuwanli::BlockListForDown* DownList
    //                          , const std::string& FileSHA512
    //                          , const std::string& SaveServersIP
    //                          , const std::string& FileMd5
    //                          , const std::vector<unsigned long>& BlockList);
    /**
     * [MakeBlockCheck description]
     * @param BlockCheck [description]
     * @param BlockItem  [description]
     * @param BlockMd5   [description]
     */
    void MakeBlockCheck(qiuwanli::BlockCheck* BlockCheck
                        , const unsigned long BlockItem
                        , const std::string& BlockMd5);
    /**
     * [MakeBlockListForDown description]
     * @param DownList      [description]
     * @param FileSHA512    [description]
     * @param SaveServersIP [description]
     * @param FileMd5       [description]
     * @param BlockCheck    [description]
     */
    void MakeBlockListCheckForDown(qiuwanli::BlockListForDownCheck* DownList
                                   , const std::string& FileSHA512
                                   , const std::string& SaveServersIP
                                   , const std::string& FileMd5
                                   , const std::vector<std::pair<unsigned long, std::string> >& BlockList);


    //template<typename T>
    ///**
    // * ���ļ�������ɣ�������Ϣ���͵�Ŀ¼������
    // * @param BlockInfo  �����
    // * @param FileSHA512 �ļ���SHA512
    // * @param Blocks     ����Ϣ����
    // */
    //void MakeBlock2Server(qiuwanli::BlockInfo2Server* BlockInfo
    //                      , const std::string& FileSHA512
    //                      , const std::vector<T>& Blocks);

};


}   //! end namespace qiuwanli


#endif //! SERIALIZATION_TO_STREAM_HPP
