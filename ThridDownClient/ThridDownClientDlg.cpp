
// ThridDownClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ThridDownClient.h"
#include "ThridDownClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThridDownClientDlg �Ի���


CThridDownClientDlg::CThridDownClientDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_THRIDDOWNCLIENT_DIALOG, pParent)
    //, m_DownList(nullptr)
    //, m_FileList(nullptr)
    //, m_SharedList(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CThridDownClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CThridDownClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CThridDownClientDlg::OnNMRClickListFile)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_ForDown, &CThridDownClientDlg::OnNMRClickListFordownMenu)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_SHARED, &CThridDownClientDlg::OnNMRClickListSharedMenu)
    ON_BN_CLICKED(IDOK, &CThridDownClientDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CThridDownClientDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CThridDownClientDlg ��Ϣ�������

BOOL CThridDownClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    m_FileList = (CListCtrl*) GetDlgItem(IDC_LIST_FILE);
    DWORD dwStyle1 = GetWindowLong(m_FileList->m_hWnd, GWL_STYLE);
    SetWindowLong(m_FileList->m_hWnd, GWL_STYLE, dwStyle1 | LVS_REPORT);

    //����m_FileList��������ѡ�����������
    DWORD styles1 = m_FileList->GetExtendedStyle( );
    m_FileList->SetExtendedStyle(styles1 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    //��m_FileList����5��������
    TCHAR rgtsz1[4][10] = {_T("�ļ���"),_T("��С"),_T("����"),_T("�ļ�״̬")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn1;
    CRect rect1;
    m_FileList->GetWindowRect(&rect1);
    for (int i = 0; i < 4; i++)
    {
        lvcolumn1.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn1.fmt = LVCFMT_LEFT;
        lvcolumn1.pszText = rgtsz1[i];
        lvcolumn1.iSubItem = i;
        lvcolumn1.iOrder = i;
        lvcolumn1.cx = rect1.Width( ) / 4;
        m_FileList->InsertColumn(i, &lvcolumn1);
    }

    //! 
    m_DownList = (CListCtrl*) GetDlgItem(IDC_LIST_ForDown);
    DWORD dwStyle2 = GetWindowLong(m_DownList->m_hWnd, GWL_STYLE);
    SetWindowLong(m_DownList->m_hWnd, GWL_STYLE, dwStyle2 | LVS_REPORT);

    //����m_DownList��������ѡ�����������
    DWORD styles2 = m_DownList->GetExtendedStyle( );
    m_DownList->SetExtendedStyle(styles2 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    //��m_DownList����4��������
    TCHAR rgtsz2[4][10] = {_T("�ļ���"),_T("�ļ���С"),_T("����ʱ��"),_T("����״̬")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn2;
    CRect rect2;
    m_DownList->GetWindowRect(&rect2);
    for (int i = 0; i < 4; i++)
    {
        lvcolumn2.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn2.fmt = LVCFMT_LEFT;
        lvcolumn2.pszText = rgtsz2[i];
        lvcolumn2.iSubItem = i;
        lvcolumn2.iOrder = i;
        lvcolumn2.cx = rect2.Width( ) / 4;
        m_DownList->InsertColumn(i, &lvcolumn2);
    }


    m_SharedList = (CListCtrl*) GetDlgItem(IDC_LIST_SHARED);
    DWORD dwStyle3 = GetWindowLong(m_SharedList->m_hWnd, GWL_STYLE);
    SetWindowLong(m_SharedList->m_hWnd, GWL_STYLE, dwStyle3 | LVS_REPORT);

    //����m_SharedList��������ѡ�����������
    DWORD styles3 = m_SharedList->GetExtendedStyle( );
    m_SharedList->SetExtendedStyle(styles3 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    //��m_SharedList����5��������
    TCHAR rgtsz3[5][10] = {_T("��������"),_T("�ļ���"),_T("�ļ���С"),_T("��֤��Ϣ"),_T("����ʱ��")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn3;
    CRect rect3;
    m_SharedList->GetWindowRect(&rect3);
    for (int i = 0; i < 5; i++)
    {
        lvcolumn3.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn3.fmt = LVCFMT_LEFT;
        lvcolumn3.pszText = rgtsz3[i];
        lvcolumn3.iSubItem = i;
        lvcolumn3.iOrder = i;
        lvcolumn3.cx = rect3.Width( ) / 5;
        m_SharedList->InsertColumn(i, &lvcolumn3);
    }


    FillData( );

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CThridDownClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CThridDownClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//�ļ��б��Ҽ��˵���Ӧ�¼�
void CThridDownClientDlg::OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������

    std::vector<CString> kSelectedItem; //���ѡ���
    CMenu menu, *pSubMenu; //��������Ҫ�õ���cmenu����
    menu.LoadMenu(IDR_MENU1); //װ���Զ�����Ҽ��˵�
    pSubMenu = menu.GetSubMenu(0); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵�
    CPoint oPoint; //����һ������ȷ�����λ�õ�λ��
    GetCursorPos(&oPoint); //��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����

    m_FileList->SetExtendedStyle(m_FileList->GetExtendedStyle( ) | LVS_EX_FULLROWSELECT);

    POSITION sSelectedPos = m_FileList->GetFirstSelectedItemPosition( );  //��ȡ��ѡ����λ��
    if (sSelectedPos == NULL)
    {
        return;
    }
    //ѡ�е�����
    //ѡ�е�1����������
    while (sSelectedPos)//��ȡ��ѡ����λ��
    {
        int nSelItem = -1;
        nSelItem = m_FileList->GetNextSelectedItem(sSelectedPos);//��ȡѡ���е�������ͨ��POSITIONת�� 

        if (nSelItem >= 0 && nSelItem < m_FileList->GetItemCount( ))
        {
            CString sFullPath = m_FileList->GetItemText(nSelItem, 1);
            //MessageBox(sFullPath ); //��ʾ��ǰѡ�е�·��
            kSelectedItem.push_back(sFullPath);
        }

    }

    //��ָ��λ����ʾ�����˵�
    pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);

    *pResult = 0;
}


//�����б��Ҽ��˵���Ӧ�¼�
void CThridDownClientDlg::OnNMRClickListFordownMenu(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    std::vector<CString> kSelectedItem; //���ѡ���
    CMenu menu, *pSubMenu; //��������Ҫ�õ���cmenu����
    menu.LoadMenu(IDR_MENU1); //װ���Զ�����Ҽ��˵�
    pSubMenu = menu.GetSubMenu(2); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵�
    CPoint oPoint; //����һ������ȷ�����λ�õ�λ��
    GetCursorPos(&oPoint); //��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����

    m_SharedList->SetExtendedStyle(m_SharedList->GetExtendedStyle( ) | LVS_EX_FULLROWSELECT);

    POSITION sSelectedPos = m_SharedList->GetFirstSelectedItemPosition( );  //��ȡ��ѡ����λ��
    if (sSelectedPos == NULL)
    {
        return;
    }
    //ѡ�е�����
    //ѡ�е�1����������
    while (sSelectedPos)//��ȡ��ѡ����λ��
    {
        int nSelItem = -1;
        nSelItem = m_SharedList->GetNextSelectedItem(sSelectedPos);//��ȡѡ���е�������ͨ��POSITIONת�� 

        if (nSelItem >= 0 && nSelItem < m_SharedList->GetItemCount( ))
        {
            CString sFullPath = m_SharedList->GetItemText(nSelItem, 1);
            //MessageBox(sFullPath ); //��ʾ��ǰѡ�е�·��
            kSelectedItem.push_back(sFullPath);
        }

    }

    //��ָ��λ����ʾ�����˵�
    pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
    *pResult = 0;
}


//�����б���Ӧ�¼�
void CThridDownClientDlg::OnNMRClickListSharedMenu(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    std::vector<CString> kSelectedItem; //���ѡ���
    CMenu menu, *pSubMenu; //��������Ҫ�õ���cmenu����
    menu.LoadMenu(IDR_MENU1); //װ���Զ�����Ҽ��˵�
    pSubMenu = menu.GetSubMenu(1); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵�
    CPoint oPoint; //����һ������ȷ�����λ�õ�λ��
    GetCursorPos(&oPoint); //��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����

    m_DownList->SetExtendedStyle(m_DownList->GetExtendedStyle( ) | LVS_EX_FULLROWSELECT);

    POSITION sSelectedPos = m_DownList->GetFirstSelectedItemPosition( );  //��ȡ��ѡ����λ��
    if (sSelectedPos == NULL)
    {
        return;
    }
    //ѡ�е�����
    //ѡ�е�1����������
    while (sSelectedPos)//��ȡ��ѡ����λ��
    {
        int nSelItem = -1;
        nSelItem = m_DownList->GetNextSelectedItem(sSelectedPos);//��ȡѡ���е�������ͨ��POSITIONת�� 

        if (nSelItem >= 0 && nSelItem < m_DownList->GetItemCount( ))
        {
            CString sFullPath = m_DownList->GetItemText(nSelItem, 1);
            //MessageBox(sFullPath ); //��ʾ��ǰѡ�е�·��
            kSelectedItem.push_back(sFullPath);
        }

    }

    //��ָ��λ����ʾ�����˵�
    pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
    *pResult = 0;
}


void CThridDownClientDlg::OnBnClickedOk( )
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CDialogEx::OnOK( );
}


void CThridDownClientDlg::OnBnClickedCancel( )
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CDialogEx::OnCancel( );
}


void CThridDownClientDlg::FillData( )
{
    //////////////////////////////////////
    m_FileList->InsertItem(0, _T("123"));
    m_FileList->SetItemText(0, 1, _T("234"));
    m_FileList->SetItemText(0, 2, _T("345"));
    m_FileList->SetItemText(0, 3, _T("�ѷ���"));

    m_FileList->InsertItem(1, _T("123"));
    m_FileList->SetItemText(1, 1, _T("234"));
    m_FileList->SetItemText(1, 2, _T("345"));
    m_FileList->SetItemText(1, 3, _T("���ϴ�"));

    m_FileList->InsertItem(2, _T("123"));
    m_FileList->SetItemText(2, 1, _T("234"));
    m_FileList->SetItemText(2, 2, _T("345"));
    m_FileList->SetItemText(2, 3, _T("����"));

    m_FileList->InsertItem(3, _T("123"));
    m_FileList->SetItemText(3, 1, _T("234"));
    m_FileList->SetItemText(3, 2, _T("345"));
    m_FileList->SetItemText(3, 3, _T("����"));

    /////////////////////////////////////////
    m_DownList->InsertItem(0, _T("123"));
    m_DownList->SetItemText(0, 1, _T("234"));
    m_DownList->SetItemText(0, 2, _T("345"));
    m_DownList->SetItemText(0, 3, _T("������"));

    m_DownList->InsertItem(1, _T("123"));
    m_DownList->SetItemText(1, 1, _T("234"));
    m_DownList->SetItemText(1, 2, _T("345"));
    m_DownList->SetItemText(1, 3, _T("��������"));

    m_DownList->InsertItem(2, _T("123"));
    m_DownList->SetItemText(2, 1, _T("234"));
    m_DownList->SetItemText(2, 2, _T("345"));
    m_DownList->SetItemText(2, 3, _T("δ����"));

    m_DownList->InsertItem(3, _T("123"));
    m_DownList->SetItemText(3, 1, _T("234"));
    m_DownList->SetItemText(3, 2, _T("345"));
    m_DownList->SetItemText(3, 3, _T("��ͣ"));

    ////////////////////////////////////////
    m_SharedList->InsertItem(0, _T("234da434zz39n0v343un4374n2c9342bn364b82402n8340823402n3423-4023n94c29403498028234 v3234bg254j234h3g"));
    m_SharedList->SetItemText(0, 1, _T("234"));
    m_SharedList->SetItemText(0, 2, _T("345"));
    m_SharedList->SetItemText(0, 3, _T("56"));
    m_SharedList->SetItemText(0, 4, _T("20160707234507"));

    m_SharedList->InsertItem(1, _T("234da434ttt9n0v343un4374n2c9342bn364b82402n8340823402n3423-4023n94c29403498028234 v3234bg254j234h3g"));
    m_SharedList->SetItemText(1, 1, _T("234"));
    m_SharedList->SetItemText(1, 2, _T("345"));
    m_SharedList->SetItemText(1, 3, _T("56"));
    m_SharedList->SetItemText(1, 4, _T("20160517234507"));

    m_SharedList->InsertItem(2, _T("234da434ggg9n0v343un4374n2c9342bn364b82402n8340823402n3423-4023n94c29403498028234 v3234bg254j234h3g"));
    m_SharedList->SetItemText(2, 1, _T("234"));
    m_SharedList->SetItemText(2, 2, _T("345"));
    m_SharedList->SetItemText(2, 3, _T("56"));
    m_SharedList->SetItemText(2, 4, _T("20161207234507"));

    m_SharedList->InsertItem(3, _T("234da434xxx9n0v343un4374n2c9342bn364b82402n8340823402n3423-4023n94c29403498028234 v3234bg254j234h3g"));
    m_SharedList->SetItemText(3, 1, _T("234"));
    m_SharedList->SetItemText(3, 2, _T("345"));
    m_SharedList->SetItemText(3, 3, _T("56"));
    m_SharedList->SetItemText(3, 4, _T("20170427034507"));
}
