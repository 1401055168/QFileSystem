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

#include "public.h"
//#include "PublicStruct.pb.h"
//std::mutex g_mutex;
ComData g_ComData;
CInterface PublicData;
qiuwanli::BlockList4DownTable m_BlockToFileTableTmp;
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

    //::InitializeCriticalSection(&g_ComData);
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
    TCHAR rgtsz2[5][10] = {_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�"),_T("�ܿռ�"),_T("״̬")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn2;
    CRect rect2;
    m_ListSaveServer->GetWindowRect(&rect2);
    for (int i = 0; i < 5; i++)
    {
        lvcolumn2.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn2.fmt = LVCFMT_LEFT;
        lvcolumn2.pszText = rgtsz2[i];
        lvcolumn2.iSubItem = i;
        lvcolumn2.iOrder = i;
        lvcolumn2.cx = rect2.Width( ) / 5;
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
    TCHAR rgtsz3[4][10] = {_T("�ļ���ʶ"),_T("��֤��Ϣ"),_T("�û�ID"),_T("�ļ���С")};

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


    //�����б�
    m_ListBlockInfo = (CListCtrl*) GetDlgItem(IDC_LIST_BLOCKINFO);
    DWORD dwStyle4 = GetWindowLong(m_ListBlockInfo->m_hWnd, GWL_STYLE);
    SetWindowLong(m_ListBlockInfo->m_hWnd, GWL_STYLE, dwStyle4 | LVS_REPORT);

    //����listctrl��������ѡ�����������
    DWORD styles4 = m_ListBlockInfo->GetExtendedStyle( );
    m_ListBlockInfo->SetExtendedStyle(styles4 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    //��listctrl����5��������
    TCHAR rgtsz4[4][10] = {_T("�ļ�SHA512"),_T("�ļ����"),_T("���MD5"),_T("�洢λ��")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn4;
    CRect rect4;
    m_ListBlockInfo->GetWindowRect(&rect4);
    for (int i = 0; i < 4; i++)
    {
        lvcolumn4.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn4.fmt = LVCFMT_LEFT;
        lvcolumn4.pszText = rgtsz4[i];
        lvcolumn4.iSubItem = i;
        lvcolumn4.iOrder = i;
        lvcolumn4.cx = rect4.Width( ) / 4;
        m_ListBlockInfo->InsertColumn(i, &lvcolumn4);
    }

    InitData( );

	//�������ݺ��Ѻͷ�����������
	updateList ();

    runServer( );

    SetTimer(1, 1000, NULL);
    SetTimer(2, 30000, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


//��ʱ��
void CQFileSystemDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case 1: //��������
    {
        std::lock_guard<std::mutex> guard(g_mutex);
        //����UI�ļ��б�
        if (((size_t(1)&g_ComData.m_UIChange) == size_t(1)) && (g_ComData.NotifyFileList.size( )))
        {
            for (auto xx :g_ComData.NotifyFileList)
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
           
           //д���ļ�
           WriteFileInfoList( );
        }

        //�������ߵĴ洢��
        if (((size_t(2)&g_ComData.m_UIChange) == size_t(2)) && (g_ComData.m_HeartList.size( ) > 0))
        {
            //����UI��Heart�ļ��б�
            bool nee = false;

            for (auto xx : g_ComData.m_HeartList)
            {
                //std::lock_guard<std::mutex> guard(g_mutex);
                for (int index = 0; index < g_ComData.m_ClientConfigFile.clientinfo_size( ); ++index)
                {
                    if (g_ComData.m_ClientConfigFile.clientinfo(index).cilentid( ) == xx.ClientID)
                    {
                        nee = true;
                        auto item = g_ComData.m_ClientConfigFile.mutable_clientinfo(index);
                        item->set_online("ON");
                    }
                }

                //��̬����б�
                if (nee == false)
                {
                    std::string IPList[10] = {"127.0.0.1","192.4.2.9","127.0.0.1","212.4.45.9","147.0.7.1","192.34.65.9","127.53.54.1","192.4.45.9","246.23.54.23"};
                    std::string IPs;
                    std::srand(unsigned(time(0)));
                    int pos = std::rand( );
                    if (pos < 10)
                    {
                        IPs = IPList[pos];
                    }

                    PublicData.DoClientConfigFileTable(g_ComData.m_ClientConfigFile.add_clientinfo( ), xx.ClientID, IPs, "213dfsefgser", xx.Prikeymd5, xx.Remain, xx.Total);
                }
            }

           
           g_ComData.m_HeartList.clear( );

            //����״̬λ
            size_t tmp = size_t(2);
            tmp = ~tmp;
           g_ComData.m_UIChange &= tmp;

            //���½���
            UpDateUISaveServerList( );
            UpdateUIFileInfoList( );
        }

        //��ʱ������ʾ���Ѿ����ܵ������ݿ�
        if (((size_t(4)&g_ComData.m_UIChange) == size_t(4)) && (g_ComData.g_AddBlockTmp.size( ) > 0))
        {
            //std::lock_guard<std::mutex> guard(g_mutex);
            for (auto xxxx : g_ComData.g_AddBlockTmp)
            {
                if (xxxx.size( ) == 4)
                {
                    CString Data1;
                    auto xx = xxxx.begin( );
                    Data1 = xx->c_str( );
                    m_ListBlockInfo->InsertItem(0, Data1);

                    ++xx;
                    CString Data3;
                    std::string ss = xx->c_str( );
                    Data3 = ss.c_str( );
                    m_ListBlockInfo->SetItemText(0, 1, Data3);

                    ++xx;
                    CString Data2;
                    Data2 = xx->c_str( );
                    m_ListBlockInfo->SetItemText(0, 2, Data2);

                    ++xx;
                    CString Data4;
                    Data4 = xx->c_str( );
                    m_ListBlockInfo->SetItemText(0, 3, Data4);
                }
            }
           g_ComData.g_AddBlockTmp.clear( );

            //����״̬λ
            size_t tmp = size_t(4);
            tmp = ~tmp;
           g_ComData.m_UIChange &= tmp;

           //�����ݿ�д�뵽�ļ�
           //�ϲ�����д���ļ�
           g_ComData.m_BlockToFileTable.MergeFrom(g_ComData.m_BlockToFileTableTmp);
           WriteBlockInfo( );
        }

        //��ʱ������ʾ���ļ��������ʱ���н������
        if (((size_t(8)&g_ComData.m_UIChange) == size_t(8)) && (g_ComData.g_AddBlockTmp.size( ) > 0))
        {
            
            for (auto xxxx : g_ComData.g_AddBlockTmp)
            {
             
            }
            g_ComData.g_AddBlockTmp.clear( );

            //����״̬λ
            size_t tmp = size_t(8);
            tmp = ~tmp;
            g_ComData.m_UIChange &= tmp;
        }

    }
    break;
    case 2:
    {
        std::lock_guard<std::mutex> guard(g_mutex);
        //��ʱ���ݵ������ļ�
        WriteBlockInfo( );
        WriteFileInfoList( );
        WriteSaveServerList( );
        WriteUserInfo( );
        //WriteSaveServerList( );
        WriteSharedList( );
        //

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
        int thread_count = 16;
        size_t block_size = k_buffer_size;

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


//���ݵĳ�ʼ��
void CQFileSystemDlg::InitData( )
{
    //��ʼ���б�
    ReadBlockInfo( );
    //ReadFileInfoList( );
    ReadSaveServerList( );
    ReadSharedList( );
    readUserInfo( );

    m_ListFile->InsertItem(0, _T("VC++��׼��.pdf"));
    m_ListFile->SetItemText(0, 1, _T("5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"));
    m_ListFile->SetItemText(0, 2, _T("3265235"));
    m_ListFile->SetItemText(0, 3, _T("2017-1-3 8:34:44"));
    m_ListFile->SetItemText(0, 4, _T("�ѷ���"));

    m_ListFile->InsertItem(0, _T("BOOST��׼��.pdf"));
    m_ListFile->SetItemText(0, 1, _T("4ABD812CF05FE5D4925D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"));
    m_ListFile->SetItemText(0, 2, _T("6874654"));
    m_ListFile->SetItemText(0, 3, _T("2017-5-3 8:34:44"));
    m_ListFile->SetItemText(0, 4, _T(""));

    m_ListFile->InsertItem(0, _T("�ֻز�ֹ.pdf"));
    m_ListFile->SetItemText(0, 1, _T("543DD5DCA59C9E4ABD815D492643DD5DCA59C9E4ABD812CF05FE5D4922CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"));
    m_ListFile->SetItemText(0, 2, _T("68765456"));
    m_ListFile->SetItemText(0, 3, _T("2017-3-3 18:35:44"));
    m_ListFile->SetItemText(0, 4, _T("�ѷ���"));

    m_ListFile->InsertItem(0, _T("���ķ�ʽ.pdf"));
    m_ListFile->SetItemText(0, 1, _T("HD492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"));
    m_ListFile->SetItemText(0, 2, _T("796785543"));
    m_ListFile->SetItemText(0, 3, _T("2017-1-3 8:34:44"));
    m_ListFile->SetItemText(0, 4, _T(""));

    //qiuwanli::FileInfoListTable ssss;
    g_ComData.m_FileListTable.Clear( );
    PublicData.DoFileInfoListTable(g_ComData.m_FileListTable.add_filelist( )
                                   , "5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"
                                   , "D492643DD5DCA59C9E4ABD812CF05FE"
                                   , "���ķ�ʽ.pdf"
                                   , "2017-1-3 8:34:44"
                                   , ""
                                   , 5345
                                   , 4546464
                                   , "");

    PublicData.DoFileInfoListTable(g_ComData.m_FileListTable.add_filelist( )
                                   , "HD492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"
                                   , "D492643DD5DCA59C9E4ABD812CF05FE"
                                   , "VC++��׼��.pdf"
                                   , "2017-1-3 8:34:44"
                                   , "�ѷ���"
                                   , 345
                                   , 4564645
                                   , "");

    PublicData.DoFileInfoListTable(g_ComData.m_FileListTable.add_filelist( )
                                   , "4ABD812CF05FE5D4925D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"
                                   , "92643DD5DCA59C9E4ABD812C43DD5DC"
                                   , "BOOST��׼��.pdf"
                                   , "2017-11-3 8:34:44"
                                   , ""
                                   , 355
                                   , 47464
                                   , "");

    PublicData.DoFileInfoListTable(g_ComData.m_FileListTable.add_filelist( )
                                   , "543DD5DCA59C9E4ABD815D492643DD5DCA59C9E4ABD812CF05FE5D4922CF05FE5D492643DD5DCA59C9E4ABD812CF05FE5D492643DD5DCA59C9E4ABD812CF05FE"
                                   , "D492643DD5DCA59C9E4ABD812CF05FE"
                                   , "���ķ�ʽ.pdf"
                                   , "2017-3-3 18:35:44"
                                   , ""
                                   , 345
                                   , 4456464
                                   , "");

    WriteFileInfoList( );

}

//�����ļ��洢���Ӧ���ļ��б�
void CQFileSystemDlg::ReadBlockInfo( )
{
    boost::filesystem::fstream readBlockInfoFile("ServerBlockInfoFiles", std::ios::in | std::ios::binary);
    if (readBlockInfoFile.is_open( ))
    {
        if (!g_ComData.m_BlockToFileTable.ParseFromIstream(&readBlockInfoFile))
            MessageBox(_T("ServerBlockInfoFiles �ļ�����ʧ�ܣ�"));
    }
    else
    {
        MessageBox(_T("ServerBlockInfoFiles �ļ���ʧ�ܣ�"));
    }

    for (int index = 0; index <g_ComData.m_BlockToFileTable.blocklistfordown_size( ); ++index)
    {
        //_T("�ļ���"), _T("SHA512"), _T("�ļ���С"), _T("����ʱ��"), _T("�Ƿ����")
        CString Data1;
        Data1 =g_ComData.m_BlockToFileTable.blocklistfordown(index).filesha512( ).c_str( );
        m_ListBlockInfo->InsertItem(0, Data1);

        CString Data3;
        std::string ss = std::to_string(g_ComData.m_BlockToFileTable.blocklistfordown(index).blocknumer( ));
        Data3 = ss.c_str( );
        m_ListBlockInfo->SetItemText(0, 1, Data3);

        CString Data2;
        Data2 =g_ComData.m_BlockToFileTable.blocklistfordown(index).blockmd5( ).c_str( );
        m_ListBlockInfo->SetItemText(0, 2, Data2);

        CString Data4;
        Data4 =g_ComData.m_BlockToFileTable.blocklistfordown(index).saveserversip( ).c_str( );
        m_ListBlockInfo->SetItemText(0, 3, Data4);
    }

    readBlockInfoFile.close( );
}
void CQFileSystemDlg::UpdateUIBlockInfo( )
{
    m_ListBlockInfo->DeleteAllItems( );

    for (int index = 0; index <g_ComData.m_BlockToFileTable.blocklistfordown_size( ); ++index)
    {
        CString Data1;
        Data1 =g_ComData.m_BlockToFileTable.blocklistfordown(index).filesha512( ).c_str( );
        m_ListBlockInfo->InsertItem(0, Data1);

        CString Data3;
        std::string ss = std::to_string(g_ComData.m_BlockToFileTable.blocklistfordown(index).blocknumer( ));
        Data3 = ss.c_str( );
        m_ListBlockInfo->SetItemText(0, 1, Data3);

        CString Data2;
        Data2 =g_ComData.m_BlockToFileTable.blocklistfordown(index).blockmd5( ).c_str( );
        m_ListBlockInfo->SetItemText(0, 2, Data2);

        CString Data4;
        Data4 =g_ComData.m_BlockToFileTable.blocklistfordown(index).saveserversip( ).c_str( );
        m_ListBlockInfo->SetItemText(0, 3, Data4);
    }
}
void  CQFileSystemDlg::WriteBlockInfo( )
{
    boost::filesystem::fstream readBlockInfoFile("ServerBlockInfoFiles", std::ios::trunc | std::ios::out | std::ios::binary);
    if (!readBlockInfoFile.is_open( ))
        MessageBox(_T("ServerBlockInfoFiles �ļ���ʧ�ܣ�"));
    if (!g_ComData.m_BlockToFileTable.ParseFromIstream(&readBlockInfoFile))
        MessageBox(_T("ServerBlockInfoFiles �ļ�����ʧ�ܣ�"));

    readBlockInfoFile.close( );
}

//�����ļ��б�
void CQFileSystemDlg::ReadFileInfoList( )
{ 
    boost::filesystem::fstream readFile("FileInfoList", std::ios::in | std::ios::binary);
    if (!readFile.is_open( ))
        return;

    if (!g_ComData.m_FileListTable.ParsePartialFromIstream(&readFile));

    readFile.close( );

    for (int index = 0; index <g_ComData.m_FileListTable.filelist_size( ); ++index)
    {
        //_T("�ļ���"), _T("SHA512"), _T("�ļ���С"), _T("����ʱ��"), _T("�Ƿ����")
        CString Data1;
        Data1 =g_ComData.m_FileListTable.filelist(index).filename( ).c_str( );
        m_ListFile->InsertItem(0, Data1);

        CString Data2;
        Data2 =g_ComData.m_FileListTable.filelist(index).filesha512( ).c_str( );
        m_ListFile->SetItemText(0, 1, Data2);

        CString Data3;
        std::string ss = std::to_string(g_ComData.m_FileListTable.filelist(index).filetotalsize( ));
        Data3 = ss.c_str( );
        m_ListFile->SetItemText(0, 2, Data3);

        CString Data4;
        Data4 =g_ComData.m_FileListTable.filelist(index).filecreatedate( ).c_str( );
        m_ListFile->SetItemText(0, 3, Data4);

        CString Data5;
        Data5 =g_ComData.m_FileListTable.filelist(index).isshared( ).c_str( );
        m_ListFile->SetItemText(0, 4, Data5);

    }
}
void CQFileSystemDlg::UpdateUIFileInfoList( )
{ 
    m_ListFile->DeleteAllItems( );

    for (int index = 0; index <g_ComData.m_FileListTable.filelist_size( ); ++index)
    {
        //_T("�ļ���"), _T("SHA512"), _T("�ļ���С"), _T("����ʱ��"), _T("�Ƿ����")
        CString Data1;
        Data1 =g_ComData.m_FileListTable.filelist(index).filename( ).c_str( );
        m_ListFile->InsertItem(0, Data1);

        CString Data2;
        Data2 =g_ComData.m_FileListTable.filelist(index).filesha512( ).c_str( );
        m_ListFile->SetItemText(0, 1, Data2);

        CString Data3;
        std::string ss = std::to_string(g_ComData.m_FileListTable.filelist(index).filetotalsize( ));
        Data3 = ss.c_str( );
        m_ListFile->SetItemText(0, 2, Data3);

        CString Data4;
        Data4 =g_ComData.m_FileListTable.filelist(index).filecreatedate( ).c_str( );
        m_ListFile->SetItemText(0, 3, Data4);

        CString Data5;
        Data5 =g_ComData.m_FileListTable.filelist(index).isshared( ).c_str( );
        m_ListFile->SetItemText(0, 4, Data5);

    }
}
void CQFileSystemDlg::WriteFileInfoList( )
{ 
    boost::filesystem::fstream readFile("FileInfoList", std::ios::trunc | std::ios::out | std::ios::binary);
    if (!readFile.is_open( ))
        return;

    if (!g_ComData.m_FileListTable.SerializePartialToOstream(&readFile));

    readFile.close( );
}

//�����б�
void CQFileSystemDlg::ReadSharedList( )
{ 
    boost::filesystem::fstream readFile("SharedInfoList", std::ios::in | std::ios::binary);
    if (!readFile.is_open( ))
        return;

    if (!g_ComData.m_SharedTable.ParsePartialFromIstream(&readFile));

    for (int index = 0; index <g_ComData.m_SharedTable.sharedinfo_size( ); ++index)
    {
        //_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�"),_T("�ܿռ�")
        CString Data1;
        Data1 =g_ComData.m_SharedTable.sharedinfo(index).sha512( ).c_str( );
        m_ListShared->InsertItem(0, Data1);

        CString Data2;
        Data2 =g_ComData.m_SharedTable.sharedinfo(index).verificationcode( ).c_str( );
        m_ListShared->SetItemText(0, 1, Data2);

        CString Data3;
        Data3 =g_ComData.m_SharedTable.sharedinfo(index).userid( ).c_str( );
        m_ListShared->SetItemText(0, 2, Data3);

        CString Data4;
        std::string ss1 = std::to_string(g_ComData.m_SharedTable.sharedinfo(index).filesize( ));
        Data4 = ss1.c_str( );
        m_ListShared->SetItemText(0, 3, Data4);
    }

    readFile.close( );
}
void CQFileSystemDlg::UpdateUISharedList( )
{ 
    m_ListShared->DeleteAllItems( );

    for (int index = 0; index <g_ComData.m_SharedTable.sharedinfo_size( ); ++index)
    {
        //_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�"),_T("�ܿռ�")
        CString Data1;
        Data1 =g_ComData.m_SharedTable.sharedinfo(index).sha512( ).c_str( );
        m_ListShared->InsertItem(0, Data1);

        CString Data2;
        Data2 =g_ComData.m_SharedTable.sharedinfo(index).verificationcode( ).c_str( );
        m_ListShared->SetItemText(0, 1, Data2);

        CString Data3;
        Data3 =g_ComData.m_SharedTable.sharedinfo(index).userid( ).c_str( );
        m_ListShared->SetItemText(0, 2, Data3);

        CString Data4;
        std::string ss1 = std::to_string(g_ComData.m_SharedTable.sharedinfo(index).filesize( ));
        Data4 = ss1.c_str( );
        m_ListShared->SetItemText(0, 3, Data4);
    }
}
void CQFileSystemDlg::WriteSharedList( )
{
    boost::filesystem::fstream readFile("SharedInfoList", std::ios::trunc | std::ios::out | std::ios::binary);
    if (!readFile.is_open( ))
        return;

    if (!g_ComData.m_SharedTable.SerializePartialToOstream(&readFile))
        ;

    readFile.close( );
}

//�����洢����Ϣ
void CQFileSystemDlg::ReadSaveServerList( )
{ 
    boost::filesystem::fstream readClientFile("ClientTable", std::ios::in | std::ios::binary);
    if (!readClientFile.is_open( ))
        return;

    if (!g_ComData.m_ClientConfigFile.ParsePartialFromIstream(&readClientFile))
        ;

    readClientFile.close( );

    for (int index = 0; index <g_ComData.m_ClientConfigFile.clientinfo_size( ); ++index)
    {
        //_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�"),_T("�ܿռ�")
        CString Data1;
        Data1 =g_ComData.m_ClientConfigFile.clientinfo(index).cilentid( ).c_str( );
        m_ListSaveServer->InsertItem(0, Data1);

        CString Data2;
        Data2 =g_ComData.m_ClientConfigFile.clientinfo(index).saveip( ).c_str( );
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
        auto tt =g_ComData.m_ClientConfigFile.mutable_clientinfo(index);
        tt->set_online("OFF");          //����Protoc����
        m_ListSaveServer->SetItemText(0, 4, _T("OFF"));
    }
}
void CQFileSystemDlg::UpDateUISaveServerList( )
{
    m_ListSaveServer->DeleteAllItems( );

    for (int index = 0; index <g_ComData.m_ClientConfigFile.clientinfo_size( ); ++index)
    {
        //_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�"),_T("�ܿռ�")
        CString Data1;
        Data1 =g_ComData.m_ClientConfigFile.clientinfo(index).cilentid( ).c_str( );
        m_ListSaveServer->InsertItem(0, Data1);

        CString Data2;
        Data2 =g_ComData.m_ClientConfigFile.clientinfo(index).saveip( ).c_str( );
        m_ListSaveServer->SetItemText(0, 1, Data2);

        CString Data3;
        std::string ss = std::to_string(g_ComData.m_ClientConfigFile.clientinfo(index).remainsize( ));
        Data3 = ss.c_str( );
        m_ListSaveServer->SetItemText(0, 2, Data3);

        CString Data4;
        std::string ss1 = std::to_string(g_ComData.m_ClientConfigFile.clientinfo(index).totalsize( ));
        Data4 = ss1.c_str( );
        m_ListSaveServer->SetItemText(0, 3, Data4);

        CString Data5;
        Data5 =g_ComData.m_ClientConfigFile.clientinfo(index).online( ).c_str( );
        m_ListSaveServer->SetItemText(0, 4, Data5);
    }
}
void CQFileSystemDlg::WriteSaveServerList( )
{ 
    boost::filesystem::fstream readClientFile("ClientTable", std::ios::trunc | std::ios::out | std::ios::binary);
    if (!readClientFile.is_open( ))
        return;

    if (!g_ComData.m_ClientConfigFile.SerializePartialToOstream(&readClientFile))
        ;

    readClientFile.close( );
}

//�����û��б�
void CQFileSystemDlg::readUserInfo( )
{
    boost::filesystem::fstream readUserInfoFile("UserTable", std::ios::in | std::ios::binary);
    if (!readUserInfoFile.is_open( ))//return;
        MessageBox(_T("UserTable �ļ���ʧ�ܣ�"));

    if (!g_ComData.m_UserhasFile.ParseFromIstream(&readUserInfoFile))
        MessageBox(_T("UserTable �ļ�����ʧ�ܣ�"));

    readUserInfoFile.close( );
}
void CQFileSystemDlg::WriteUserInfo( )
{
    boost::filesystem::fstream readUserInfoFile("UserTable", std::ios::trunc | std::ios::out | std::ios::binary);
    if (!readUserInfoFile.is_open( ))//return;
        MessageBox(_T("UserTable �ļ���ʧ�ܣ�"));

    if (!g_ComData.m_UserhasFile.SerializePartialToOstream(&readUserInfoFile))
        MessageBox(_T("UserTable �ļ�����ʧ�ܣ�"));

    readUserInfoFile.close( );
}

