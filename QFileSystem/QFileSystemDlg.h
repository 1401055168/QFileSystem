
// QFileSystemDlg.h : ͷ�ļ�
//

#pragma once
#include "constdate.hpp"

// CQFileSystemDlg �Ի���
class CQFileSystemDlg : public CDHtmlDialog
{
// ����
public:
	CQFileSystemDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QFILESYSTEM_DIALOG, IDH = IDR_HTML_QFILESYSTEM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

    CListCtrl*  m_ListFile;
    CListCtrl*  m_ListSaveServer;
    CListCtrl*  m_ListShared;
    CListCtrl*  m_ListClient;
    CListCtrl*  m_ListLogs;

	void updateList ();
	void MakeFilesLog (qiuwanli::File2Cilent * file
                       , std::string filename
                       , std::string sha512
                       , std::string client
                       , std::string createtime);
	void MakeLogs (qiuwanli::Logs * Log
                   , std::string user_id
                   , std::string logdate
                   , std::string loginfo
                   , std::string logtype);
	void MakeLogs (qiuwanli::ID2IP * id2ip
                   , std::string clientid
                   , std::string id
                   , std::string Prikey
                   , std::string KeyMd5
                   , std::string Others);
	void sender (boost::asio::io_service &io
                 , const char*	ip_address
                 , unsigned	port
                 , const char* filename
                 , const char* msg_type);

	//���ͷ��ļ�
	void sender (boost::asio::io_service &io
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

	std::string& WStringToUTF8String (const std::wstring &wstr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		std::string u8str = "";
		u8str = conv.to_bytes (wstr);
		return std::move (u8str);
	}

public:
	afx_msg void OnNMRClickFileList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickClientList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickSharedList(NMHDR *pNMHDR, LRESULT *pResult);
};

