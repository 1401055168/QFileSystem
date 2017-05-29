// QFileSystemDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "SendFile.hpp"
#include "Servers.hpp"
#include "QFileSystem.h"
#include "QFileSystemDlg.h"

//#include "public.h"
//#include "PublicStruct.pb.h"

ComData g_ComData;
CInterface PublicData;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CQFileSystemDlg::CQFileSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_QFILESYSTEM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQFileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQFileSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
    ON_WM_TIMER()           //��ʱ��֪ͨ
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CQFileSystemDlg::OnNMRClickFileList)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_Save_Servers, &CQFileSystemDlg::OnNMRClickClientList)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_Shared, &CQFileSystemDlg::OnNMRClickSharedList)
END_MESSAGE_MAP()


// CQFileSystemDlg ��Ϣ�������

BOOL CQFileSystemDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	
	///////////////////////////////////
	//�ļ��б�
	m_ListFile = (CListCtrl*)GetDlgItem (IDC_LIST_FILE);
	DWORD dwStyle1 = GetWindowLong (m_ListFile->m_hWnd, GWL_STYLE);
	SetWindowLong (m_ListFile->m_hWnd, GWL_STYLE, dwStyle1 | LVS_REPORT);

	//����listctrl��������ѡ�����������
	DWORD styles1 = m_ListFile->GetExtendedStyle ();
    m_ListFile->SetExtendedStyle (styles1 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//��listctrl����5��������
    TCHAR rgtsz1[5][10] = {_T("�ļ���"), _T("SHA512"), _T("�ļ���С")
                            ,_T("����ʱ��"),_T("�Ƿ����")};

	//�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�

	LV_COLUMN lvcolumn1;
	CRect rect1;
    m_ListFile->GetWindowRect (&rect1);
	for (int i = 0; i < 5; i++)
	{
        lvcolumn1.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
        lvcolumn1.fmt = LVCFMT_LEFT;
        lvcolumn1.pszText = rgtsz1[i];
        lvcolumn1.iSubItem = i;
        lvcolumn1.iOrder = i;
        lvcolumn1.cx = rect1.Width () / 5;
        m_ListFile->InsertColumn (i, &lvcolumn1);
	}

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    //�����б�
    m_ListSaveServer = (CListCtrl*) GetDlgItem(IDC_LIST_Save_Servers);
    DWORD dwStyle2 = GetWindowLong(m_ListSaveServer->m_hWnd, GWL_STYLE);
    SetWindowLong(m_ListSaveServer->m_hWnd, GWL_STYLE, dwStyle2 | LVS_REPORT);

    //����listctrl��������ѡ�����������
    DWORD styles2 = m_ListSaveServer->GetExtendedStyle( );
    m_ListSaveServer->SetExtendedStyle(styles2 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    //��listctrl����5��������
    TCHAR rgtsz2[4][10] = {_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�"),_T("�ܿռ�")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn2;
    CRect rect2;
    m_ListSaveServer->GetWindowRect(&rect2);
    for (int i = 0; i < 4; i++)
    {
        lvcolumn2.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn2.fmt = LVCFMT_LEFT;
        lvcolumn2.pszText = rgtsz2[i];
        lvcolumn2.iSubItem = i;
        lvcolumn2.iOrder = i;
        lvcolumn2.cx = rect2.Width( ) / 4;
        m_ListSaveServer->InsertColumn(i, &lvcolumn2);
    }


    //�����б�
    m_ListShared = (CListCtrl*)GetDlgItem(IDC_LIST_Shared);
    DWORD dwStyle3 = GetWindowLong(m_ListShared->m_hWnd, GWL_STYLE);
    SetWindowLong(m_ListShared->m_hWnd, GWL_STYLE, dwStyle3 | LVS_REPORT);

    //����listctrl��������ѡ�����������
    DWORD styles3 = m_ListShared->GetExtendedStyle( );
    m_ListShared->SetExtendedStyle(styles3 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    //��listctrl����5��������
    TCHAR rgtsz3[4][10] = {_T("URL"),_T("�ļ���ʶ"),_T("��֤��Ϣ"),_T("�û�ID")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn3;
    CRect rect3;
    m_ListShared->GetWindowRect(&rect3);
    for (int i = 0; i < 4; i++)
    {
        lvcolumn3.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn3.fmt = LVCFMT_LEFT;
        lvcolumn3.pszText = rgtsz3[i];
        lvcolumn3.iSubItem = i;
        lvcolumn3.iOrder = i;
        lvcolumn3.cx = rect3.Width( ) / 4;
        m_ListShared->InsertColumn(i, &lvcolumn3);
    }

    InitData( );

	//�������ݺ��Ѻͷ�����������
	updateList ();

    runServer( );

    SetTimer(1, 300, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}



void CQFileSystemDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case 1: //��������
    {
        if ((size_t(1)&g_ComData.m_UIChange) == 0)
        {
            return;
        }

        //����UI�ļ��б�
        if ((size_t(1)&g_ComData.m_UIChange) == size_t(1))
        {
            for (auto xx : g_ComData.NotifyFileList)
            {
                CString x1;
                x1 = xx.v1.c_str( );
                m_ListFile->InsertItem(0, x1);

                CString x2;
                x2 = xx.v2.c_str( );
                m_ListFile->SetItemText(0, 1, x2);

                CString x3;
                x3 = xx.v3.c_str( );
                m_ListFile->SetItemText(0, 1, x3);

                CString x4;
                x4 = xx.v4.c_str( );
                m_ListFile->SetItemText(0, 1, x4);

                CString x5;
                x5 = xx.v5.c_str( );
                m_ListFile->SetItemText(0, 1, x5);
            }
            g_ComData.NotifyFileList.clear( );

            //����״̬λ
            size_t tmp = size_t(1);
            tmp = ~tmp;
            g_ComData.m_UIChange &= tmp;
        }

        //����UI�ļ��б�

        for (auto xx : g_ComData.m_HeartList)
        {
            CString x1;
            x1 = xx.ClientID.c_str( );
            m_ListFile->InsertItem(0, x1);

            CString x2;
            x2 = std::to_string(xx.Remain).c_str( );
            m_ListFile->SetItemText(0, 1, x2);

            CString x3;
            x3 = std::to_string(xx.Total).c_str( );
            m_ListFile->SetItemText(0, 1, x3);

            CString x4;
            x4 = xx.Prikeymd5.c_str( );
            m_ListFile->SetItemText(0, 1, x4);
        }
        g_ComData.m_HeartList.clear( );

    }
    break;
    default:
        break;
    }
    CDialogEx::OnTimer(nIDEvent);
}

void runServers( )
{
    try
    {
        std::cerr << "Usage: server <address> <port> <threads> <blocksize>\n";

        boost::asio::ip::address address = boost::asio::ip::address::from_string("127.0.0.1");
        short port = 8189;
        int thread_count = 6;
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
void CQFileSystemDlg::runServer( )
{
    boost::thread serverThread(runServers);
    serverThread.detach( );
}
void CQFileSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
        CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CQFileSystemDlg::OnPaint()
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
HCURSOR CQFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CQFileSystemDlg::updateList ()
{
	
}


void CQFileSystemDlg::sender (boost::asio::io_service &io
                              , const char*	ip_address
                              , unsigned	port
                              , const char* filename
                              , const char* msg_type)
{
	FILE *fp;
	fopen_s (&fp, filename, "rb");
	if (fp == NULL) 
    {
		std::cerr << "cannot open file\n";
		return;
	}

	//ʹ������ָ�룬��ֹ��������쳣ʱ��fcloseδ�����á�
	boost::shared_ptr<FILE> file_ptr (fp, fclose);

	clock_t cost_time = clock ();

	const size_t k_buffer_size = k_times * 1024;
	char buffer[k_buffer_size];
    DataBlockTypeInfo file_info;

	char buf[k_times * 1024];
	strcat_s (buf, filename);
	strcat_s (buf, "+");
	strcat_s (buf, msg_type);
	filename = (const char*)buf;

	//MessageBox ();
	//const char* filename_msg = filename + msg_type;

	int filename_size = strlen (filename) + 1;
	size_t file_info_size = sizeof (file_info);
	size_t total_size = file_info_size + filename_size;
	if (total_size > k_buffer_size) 
    {
		std::cerr << "File name is too long";
		return;
	}
	file_info.m_FileNameLength = filename_size;
	fseek (fp, 0, SEEK_END);
	file_info.m_FileSize = ftell (fp);
	rewind (fp);

	memcpy (buffer, &file_info, file_info_size);								//�ļ���Ϣ
	memcpy (buffer + file_info_size, filename, filename_size);					//�ļ���/��Ϣ����

	boost::asio::ip::tcp::socket socket (io);
	socket.connect (boost::asio::ip::tcp::endpoint (boost::asio::ip::address_v4::from_string (ip_address), port));

	std::cout << "Sending file : " << filename << " MsgType:" << msg_type << std::endl;
	size_t len = total_size;

	unsigned long long total_bytes_read = 0;
	while (true) 
    {
		//�ȷ����ļ�ͷ��֮����data
		socket.send (boost::asio::buffer (buffer, len), 0);
		if (feof (fp))
        {
            break;
        }

		len = fread (buffer, 1, k_buffer_size, fp);
		total_bytes_read += len;
	}

	//����ʱ�䡢��С���ٶ�//
	cost_time = clock () - cost_time;
	if (cost_time == 0) cost_time = 1;
	double speed = total_bytes_read * (CLOCKS_PER_SEC / 1024.0 / 1024.0) / cost_time;
	std::cout << "cost time: " << cost_time / (double)CLOCKS_PER_SEC << " s "
		<< "  transferred_bytes: " << total_bytes_read << " bytes\n"
		<< "speed: " << speed << " MB/s\n\n";
}

//ÿ��ֻ����ѡ��һ��
//�Ҽ���Ӧ�¼�
void CQFileSystemDlg::OnNMRClickFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ȡ���Item��
    m_ListFile->SetExtendedStyle (m_ListFile->GetExtendedStyle () | LVS_EX_FULLROWSELECT);
	POSITION sSelectedPos = m_ListFile->GetFirstSelectedItemPosition ();  //��ȡ��ѡ����λ��
	if (sSelectedPos == NULL)
	{
		return;
	}

	int nSelItem = -1;
	nSelItem = m_ListFile->GetNextSelectedItem (sSelectedPos);
	if (nSelItem >= 0 && nSelItem < m_ListFile->GetItemCount ())
	{
		CString sFullPath = m_ListFile->GetItemText (nSelItem, 1);
		//���ļ���SHA512���뵽Vector��
		std::string ss = CT2A (sFullPath);
		CMenu menu, *pSubMenu; //��������Ҫ�õ���cmenu����
		menu.LoadMenu (IDR_MENU1); //װ���Զ�����Ҽ��˵�
		CPoint oPoint; //����һ������ȷ�����λ�õ�λ��

		pSubMenu = menu.GetSubMenu ((int)(checkItem (ss) ? 1 : 0)); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵�

		GetCursorPos (&oPoint); //��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����
								//��ָ��λ����ʾ�����˵�
		pSubMenu->TrackPopupMenu (TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
	}
	*pResult = 0;
}

bool CQFileSystemDlg::checkItem (std::string item)
{
	return true;
}


void CQFileSystemDlg::OnNMRClickClientList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������


    *pResult = 0;
}


void CQFileSystemDlg::OnNMRClickSharedList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������


    *pResult = 0;
}


void CQFileSystemDlg::InitData( )
{
    //�����ļ��б�
    boost::filesystem::fstream readFile("FileInfoList", std::ios::in | std::ios::binary);
    if (!readFile.is_open( ))
        return;

    if (!g_ComData.m_FileListTable.ParsePartialFromIstream(&readFile));

    for (int index = 0; index < g_ComData.m_FileListTable.filelist_size( ); ++index)
    {
        //_T("�ļ���"), _T("SHA512"), _T("�ļ���С"), _T("����ʱ��"), _T("�Ƿ����")
        CString Data1;
        Data1 = g_ComData.m_FileListTable.filelist(index).filename().c_str( );
        m_ListFile->InsertItem(0, Data1);

        CString Data2;
        Data2 = g_ComData.m_FileListTable.filelist(index).filesha512( ).c_str( );
        m_ListFile->SetItemText(0, 1, Data2);

        CString Data3;
        std::string ss=std::to_string(g_ComData.m_FileListTable.filelist(index).filetotalsize( ));
        Data3 = ss.c_str( );
        m_ListFile->SetItemText(0, 2, Data3);

        CString Data4;
        Data4 = g_ComData.m_FileListTable.filelist(index).filecreatedate( ).c_str( );
        m_ListFile->SetItemText(0, 3, Data4);

        CString Data5;
        Data5 = g_ComData.m_FileListTable.filelist(index).isshared( ).c_str( );
        m_ListFile->SetItemText(0, 4, Data5);

    }

    //�����洢����Ϣ
    boost::filesystem::fstream readClientFile("ClientTable", std::ios::in | std::ios::binary);
    if (!readClientFile.is_open( ))
        return;

    if (!g_ComData.m_ClientConfigFile.ParsePartialFromIstream(&readClientFile));

    for (int index = 0; index < g_ComData.m_ClientConfigFile.clientinfo_size( ); ++index)
    {
        //_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�"),_T("�ܿռ�")
        CString Data1;
        Data1 = g_ComData.m_ClientConfigFile.clientinfo(index).cilentid( ).c_str( );
        m_ListSaveServer->InsertItem(0, Data1);

        CString Data2;
        Data2 = g_ComData.m_ClientConfigFile.clientinfo(index).saveip( ).c_str( );
        m_ListSaveServer->SetItemText(0, 1, Data2);

        CString Data3;
        std::string ss = std::to_string(g_ComData.m_ClientConfigFile.clientinfo(index).remainsize( ));
        Data3 = ss.c_str( );
        m_ListSaveServer->SetItemText(0, 2, Data3);

        CString Data4;
        std::string ss1 = std::to_string(g_ComData.m_ClientConfigFile.clientinfo(index).totalsize( ));
        Data4 = ss1.c_str( );
        m_ListSaveServer->SetItemText(0, 3, Data4);
        
        //Ĭ������Ϊ���ߣ��ͻ��˷���Heart�ǽ��б䶯
        const std::string fff = "OFF";
        auto tt = g_ComData.m_ClientConfigFile.mutable_clientinfo(index);
        tt->set_online("OFF");          //����Protoc����
        m_ListSaveServer->SetItemText(0, 4, _T("OFF"));
    }

    //�����ļ��洢���Ӧ���ļ��б�
    boost::filesystem::fstream readBlockInfoFile("BlockInfoTable", std::ios::in | std::ios::binary);
    if (readBlockInfoFile.is_open( ))
    {
        if (!g_ComData.m_BlockListForDownCheckTable.ParsePartialFromIstream(&readBlockInfoFile))
            MessageBox(_T("BlockInfoTable �ļ�����ʧ�ܣ�"));
    }
    else
    {
        MessageBox(_T("BlockInfoTable �ļ���ʧ�ܣ�"));
    }
    //for (int index = 0; index < g_ComData.m_BlockListForDownCheckTable.blocklistfordown_size( ); ++index)
    

    //�����û��б�
    boost::filesystem::fstream readUserInfoFile("UserTable", std::ios::in | std::ios::binary);
    if (!readUserInfoFile.is_open( ))//return;
        MessageBox(_T("UserTable �ļ���ʧ�ܣ�"));

    if (!g_ComData.m_UserhasFile.ParsePartialFromIstream(&readUserInfoFile))
        MessageBox(_T("UserTable �ļ�����ʧ�ܣ�"));



    m_ListFile->InsertItem(0, L"vc++������ȫ");
    m_ListFile->SetItemText(0, 1, L"df89cia9da7dasd80ad87as0da0sd");
    m_ListFile->SetItemText(0, 2, L"8764253");
    m_ListFile->SetItemText(0, 3, L"20160304133307");
    m_ListFile->InsertItem(1, L"Boost������ȫ");
    m_ListFile->SetItemText(1, 1, L"df89cia9da7dasd80ad87as0da0sd");
    m_ListFile->SetItemText(1, 2, L"432534");
    m_ListFile->SetItemText(1, 3, L"20160304134409");

    m_ListSaveServer->InsertItem(0, L"10004");
    m_ListSaveServer->SetItemText(0, 1, L"124.54.77.33");
    m_ListSaveServer->SetItemText(0, 2, L"23452623");
    m_ListSaveServer->SetItemText(0, 3, L"923452623");
    m_ListSaveServer->InsertItem(1, L"10070");
    m_ListSaveServer->SetItemText(1, 1, L"124.54.74.33");
    m_ListSaveServer->SetItemText(1, 2, L"86452623");
    m_ListSaveServer->SetItemText(1, 3, L"234452623");
    m_ListSaveServer->InsertItem(2, L"10008");

    m_ListShared->InsertItem(0, L"etynnh89adasdaa0nendjfaij09434f343");
    m_ListShared->SetItemText(0, 1, L"89adasdaa0nendjfaij09434f343");
    m_ListShared->SetItemText(0, 2, L"5efwg4");
    m_ListShared->SetItemText(0, 3, L"2054534");
    m_ListShared->InsertItem(1, L"bsgrts89adasdaa0nendjfaij09434f343");
    m_ListShared->SetItemText(1, 1, L"89adasdaa0nendjfaij09434f343");
    m_ListShared->SetItemText(1, 2, L"234534");
    m_ListShared->SetItemText(1, 3, L"2364565");


    //
    LoadFileList( );
    LoadSaveServerList( );
    LoadSharedList( );
}

void CQFileSystemDlg::LoadFileList( )
{

}

void CQFileSystemDlg::LoadSharedList( )
{

}

void CQFileSystemDlg::LoadSaveServerList( )
{

}
