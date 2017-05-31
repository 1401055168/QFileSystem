
// QFileSystemDlg.h : ͷ�ļ�
//

#pragma once
//#include "constdate.hpp"

#include "public.h"
#include "PublicStruct.pb.h"


// CQFileSystemDlg �Ի���
class CQFileSystemDlg : public CDialogEx
{
// ����
public:
	CQFileSystemDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QFILESYSTEM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
    virtual void OnTimer(UINT_PTR nIDEvent);
    void runServer( );
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	//DECLARE_DHTML_EVENT_MAP()

    CListCtrl*  m_ListFile;
    CListCtrl*  m_ListSaveServer;
    CListCtrl*  m_ListShared;
    CListCtrl*  m_ListBlockInfo;

	void updateList ();

	//���ͷ��ļ�
    void sender(boost::asio::io_service &io
                , const char*	ip_address
                , unsigned	port
                , const char* filename
                , const char* msg_type);
	//�˶��Ƿ����
	bool CQFileSystemDlg::checkItem (std::string item);

	std::wstring StringToWstring (const std::string& str)
	{
		int size_needed = MultiByteToWideChar (CP_UTF8, 0, &str[0]
                                               , (int)str.size (), NULL, 0);
		std::wstring wstrTo (size_needed, 0);
		MultiByteToWideChar (CP_UTF8, 0, &str[0], (int)str.size ()
                             , &wstrTo[0], size_needed);
		return wstrTo;
	}

public:
	afx_msg void OnNMRClickFileList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickClientList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickSharedList(NMHDR *pNMHDR, LRESULT *pResult);
    
    //���ݵĳ�ʼ��
    void InitData( );
    void LoadFileList( );
    void LoadSaveServerList( );
    void LoadSharedList( );

public:
    qiuwanli::FileInfoList      m_CFileList;
    //qiuwanli::File2ClientTable  m_CFileToClient;
    qiuwanli::SharedTable       m_CSharedTable;
    qiuwanli::UserInfoTable     m_CUserInfo;

    qiuwanli::Heart        m_CHeart;
    qiuwanli::BlockListForDownCheckTable     m_CBlockDownList;
};

