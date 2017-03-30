
// QFileSystemDlg.h : ͷ�ļ�
//

#pragma once
#include "date.pb.h"

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

	CListCtrl*  m_ListControl = nullptr;
	CListCtrl*  m_ListContro2 = nullptr;
	void updateList ();
	void MakeFilesLog (qiuwanli::File2Cilent * downingLog, std::string filename,
		std::string sha512, std::string client, std::string createtime)
};
