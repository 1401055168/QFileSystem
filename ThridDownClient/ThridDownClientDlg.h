
// ThridDownClientDlg.h : ͷ�ļ�
//

#pragma once
#include <iostream>
#include <vector>

// CThridDownClientDlg �Ի���
class CThridDownClientDlg : public CDialogEx
{
// ����
public:
	CThridDownClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THRIDDOWNCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickListFordownMenu(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickListSharedMenu(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedOk( );
    afx_msg void OnBnClickedCancel( );

    void FillData( );

    CListCtrl* m_FileList;
    CListCtrl* m_DownList;
    CListCtrl* m_SharedList;
    afx_msg void DeleteFile( );
    afx_msg void On32774( );
    afx_msg void DownloadFile( );
    afx_msg void MakeShared( );
    afx_msg void StartDownload( );
    afx_msg void PauseDownload( );
    afx_msg void ReStart( );
    afx_msg void DeleteSharedUrl( );
    afx_msg void CopyUrl( );
    afx_msg void OnBnClickedDownloadurl( );
};
