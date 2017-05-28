
// ThridDownClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "afxdialogex.h"

#include "ThridDownClient.h"
#include "ThridDownClientDlg.h"

#include "SendFile.hpp"
#include "Servers.hpp"

#include <openssl/evp.h>  
#include <boost/filesystem.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <boost/asio.hpp>
#include "public.h"

// CThridDownClientDlg �Ի���
ComData g_ComData;

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
    ON_WM_TIMER()
    ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CThridDownClientDlg::OnNMRClickListFile)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_ForDown, &CThridDownClientDlg::OnNMRClickListFordownMenu)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_SHARED, &CThridDownClientDlg::OnNMRClickListSharedMenu)
    ON_BN_CLICKED(IDOK, &CThridDownClientDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CThridDownClientDlg::OnBnClickedCancel)
    ON_COMMAND(ID_32773, &CThridDownClientDlg::DeleteFile)
    ON_COMMAND(ID_32774, &CThridDownClientDlg::OnUploadFile)
    ON_COMMAND(ID_32775, &CThridDownClientDlg::DownloadFile)
    ON_COMMAND(ID_32776, &CThridDownClientDlg::MakeShared)
    ON_COMMAND(ID_32779, &CThridDownClientDlg::StartDownload)
    ON_COMMAND(ID_32778, &CThridDownClientDlg::PauseDownload)
    ON_COMMAND(ID_32780, &CThridDownClientDlg::ReStart)
    ON_COMMAND(ID_32771, &CThridDownClientDlg::DeleteSharedUrl)
    ON_COMMAND(ID_32772, &CThridDownClientDlg::CopyUrl)
    ON_BN_CLICKED(IDC_DownLoadURL, &CThridDownClientDlg::OnBnClickedDownloadurl)
END_MESSAGE_MAP()

void CThridDownClientDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case 1:
              
        break;
    case 2:
        //�������ִ�ж�ʱ��2������ 
        break;
    default:
        break;
    }
    CDialogEx::OnTimer(nIDEvent);
}

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


    runRecive( );
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void runservers()
{
    try{
        std::cerr << "Usage: server <address> <port> <threads> <blocksize>\n";

        boost::asio::ip::address address = boost::asio::ip::address::from_string("127.0.0.1");
        short port = 8889;
        int thread_count = 4;
        size_t block_size = 4096;

        boost::asio::io_service ios;
        server ssss(ios, boost::asio::ip::tcp::endpoint(address, port));
        std::list<boost::thread*> threads;

        while (--thread_count > 0)
        {
            boost::thread* new_thread = new boost::thread(
                boost::bind(&boost::asio::io_service::run, &ios));
            threads.push_back(new_thread);
        }

        ios.run( );

        while (!threads.empty( ))
        {
            threads.front( )->join( );

            delete threads.front( );

            threads.pop_front( );
        }
    } catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what( ) << "\n";
    }
}

void CThridDownClientDlg::runRecive( )
{
    boost::thread sr(runservers);
    sr.detach( );
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
    pSubMenu = menu.GetSubMenu(1); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵�
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
    pSubMenu = menu.GetSubMenu(2); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵�
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
    m_SharedList->InsertItem(0, _T("234da434zz39n0v343un4374n2c9342bn364b82402n8340823402n34234023n94c29403498028234u8v3234bg254j234h3g"));
    m_SharedList->SetItemText(0, 1, _T("234"));
    m_SharedList->SetItemText(0, 2, _T("345"));
    m_SharedList->SetItemText(0, 3, _T("56"));
    m_SharedList->SetItemText(0, 4, _T("20160707234507"));

    m_SharedList->InsertItem(1, _T("234da434ttt9n0v343un4374n2c9342bn364b82402n8340823402n34234023n94c29403498028234v3234bg254j234h3grt"));
    m_SharedList->SetItemText(1, 1, _T("234"));
    m_SharedList->SetItemText(1, 2, _T("345"));
    m_SharedList->SetItemText(1, 3, _T("56"));
    m_SharedList->SetItemText(1, 4, _T("20160517234507"));

    m_SharedList->InsertItem(2, _T("234da434ggg9n0v343un4374n2c9342bn364b82402n8340823402n34234023n94c294034980282347hv3234bg254j234h3g"));
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


void CThridDownClientDlg::DeleteFile( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::OnUploadFile( )
{
    CString str;
    int nId;
    //���ȵõ������λ��
    POSITION pos = m_FileList->GetFirstSelectedItemPosition( );
    if (pos == NULL)
    {
        MessageBox(_T("������ѡ��һ��"), _T("��ʾ"), MB_ICONEXCLAMATION);
        return;
    }
    //�õ��кţ�ͨ��POSITIONת��
    nId = (int) m_FileList->GetNextSelectedItem(pos);

    //�õ����е����ݣ�0��ʾ��һ�У�ͬ��1,2,3...��ʾ�ڶ���������...�У�
    str = m_FileList->GetItemText(nId, 0);
    //MessageBox(str);

    //��ȡ�ļ���FilsSHA512ֵ

    std::string filename = CT2A(str);
    //m_VUpFileList.push_back(filename);
   
    std::string sha512;

    GetFileSHA512(filename, sha512);
    g_ComData.m_UploadFile.push_back(ComData::Vec3(filename,sha512,0));
    //g_ComData.curUploadFile.push_back(std::make_pair(filename, sha512));
    filename += '+';
    filename += sha512;
    filename += '+';
    filename += user.userid();
    filename += '+';
    filename += user.userps( );

    boost::asio::io_service io_ser; 
    SendFile sender;
    try
    {
        sender.senderLitter(io_ser, "127.0.0.1", 8089, filename.c_str( ), 'a');
    }
    catch (CException* e)
    {
    }
    
}


void CThridDownClientDlg::DownloadFile( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::MakeShared( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::StartDownload( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::PauseDownload( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::ReStart( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::DeleteSharedUrl( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::CopyUrl( )
{
    // TODO: �ڴ���������������
}


void CThridDownClientDlg::OnBnClickedDownloadurl( )
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CThridDownClientDlg::GetFileSHA512(std::string& fileName, std::string& FileSHA512)
{
    EVP_MD_CTX mdctx;
    const EVP_MD *md = NULL;
    char buffer[256];
    char FFFF[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    unsigned char mdValue[EVP_MAX_MD_SIZE] = {0};
    unsigned int mdLen = 0;

    OpenSSL_add_all_digests( );
    md = EVP_get_digestbyname("sha512");

    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);

    boost::filesystem::ifstream inFile(fileName, std::ios::in | std::ios::binary);

    unsigned long endPos = inFile.tellg( );
    inFile.seekg(0, std::ios::beg);
    unsigned long inPos = 0;

    while ((endPos - inPos) > 256)
    {
        inPos = inFile.tellg( );
        inFile.read(buffer, 256);
        EVP_DigestUpdate(&mdctx, buffer, 256);
    }

    inFile.read(buffer, endPos - inPos);
    EVP_DigestUpdate(&mdctx, buffer, endPos - inPos);

    EVP_DigestFinal_ex(&mdctx, mdValue, &mdLen);
    EVP_MD_CTX_cleanup(&mdctx);

    int j = 0;
    for (j = 0; j < mdLen; j++)
    {
        printf("%s", mdValue[j]);
    }

    for (int ii = 0; ii < 64; ++ii)
    {
        int x = mdValue[ii];
        int xx = x & 15;
        int xxx = x & 240;
        xxx = xxx >> 4;
        FileSHA512 + FFFF[xxx] + FFFF[xx];
    }
    //FileSHA512
}

void CThridDownClientDlg::UpDataUI()
{
    SetTimer(1, 15000,NULL);

}
