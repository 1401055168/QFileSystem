
// QFileSystemDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "QFileSystem.h"
#include "QFileSystemDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CQFileSystemDlg �Ի���

BEGIN_DHTML_EVENT_MAP(CQFileSystemDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


CQFileSystemDlg::CQFileSystemDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(IDD_QFILESYSTEM_DIALOG, IDR_HTML_QFILESYSTEM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQFileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQFileSystemDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CQFileSystemDlg::OnNMRClickFileList)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_Save_Servers, &CQFileSystemDlg::OnNMRClickClientList)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_Shared, &CQFileSystemDlg::OnNMRClickSharedList)
END_MESSAGE_MAP()


// CQFileSystemDlg ��Ϣ�������

BOOL CQFileSystemDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

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
    TCHAR rgtsz2[3][10] = {_T("�洢�˱��"),_T("IP"),_T("ʣ��ռ�")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn2;
    CRect rect2;
    m_ListSaveServer->GetWindowRect(&rect2);
    for (int i = 0; i < 3; i++)
    {
        lvcolumn2.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn2.fmt = LVCFMT_LEFT;
        lvcolumn2.pszText = rgtsz2[i];
        lvcolumn2.iSubItem = i;
        lvcolumn2.iOrder = i;
        lvcolumn2.cx = rect2.Width( ) / 3;
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



    //���ض������б�
    m_ListClient = (CListCtrl*) GetDlgItem(IDC_LIST_Client);
    DWORD dwStyle4 = GetWindowLong(m_ListClient->m_hWnd, GWL_STYLE);
    SetWindowLong(m_ListClient->m_hWnd, GWL_STYLE, dwStyle4 | LVS_REPORT);

    //����listctrl��������ѡ�����������
    DWORD styles4 = m_ListClient->GetExtendedStyle( );
    m_ListClient->SetExtendedStyle(styles4 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    //��listctrl����5��������
    TCHAR rgtsz4[2][10] = {_T("�û�ID"),_T("IP")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn4;
    CRect rect4;
    m_ListClient->GetWindowRect(&rect4);
    for (int i = 0; i < 2; i++)
    {
        lvcolumn4.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
            | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn4.fmt = LVCFMT_LEFT;
        lvcolumn4.pszText = rgtsz4[i];
        lvcolumn4.iSubItem = i;
        lvcolumn4.iOrder = i;
        lvcolumn4.cx = rect4.Width( ) / 2;
        m_ListClient->InsertColumn(i, &lvcolumn4);
    }



    //��־�б�
    m_ListLogs = (CListCtrl*) GetDlgItem(IDC_LIST_Logs);
    DWORD dwStyle5 = GetWindowLong(m_ListLogs->m_hWnd, GWL_STYLE);
    SetWindowLong(m_ListLogs->m_hWnd, GWL_STYLE, dwStyle5 | LVS_REPORT);

    //����listctrl��������ѡ�����������
    DWORD styles5 = m_ListLogs->GetExtendedStyle( );
    m_ListLogs->SetExtendedStyle(styles5 | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    //��listctrl����5��������
    TCHAR rgtsz5[4][10] = {_T("ʱ��"),_T("�¼�"),_T("�û�ID"),_T("����")};

    //�޸������С������ȷ����������û�����ȣ�����޸���������ݣ���ɫ���֣�ҲҪ���Ÿı�
    LV_COLUMN lvcolumn5;
    CRect rect5;
    m_ListLogs->GetWindowRect(&rect5);
    for (int i = 0; i < 4; i++)
    {
        lvcolumn5.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
                        | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn5.fmt = LVCFMT_LEFT;
        lvcolumn5.pszText = rgtsz5[i];
        lvcolumn5.iSubItem = i;
        lvcolumn5.iOrder = i;
        lvcolumn5.cx = rect5.Width( ) / 4;
        m_ListLogs->InsertColumn(i, &lvcolumn5);
    }

    InitData( );

	//�������ݺ��Ѻͷ�����������
	updateList ();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
		CDHtmlDialog::OnSysCommand(nID, lParam);
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
		CDHtmlDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CQFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CQFileSystemDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CQFileSystemDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}


void CQFileSystemDlg::updateList ()
{
	qiuwanli::ID2IPTable ips;
	qiuwanli::File2ClientServers client;

	std::fstream ipInfo ("ID2IP", std::ios::in | std::ios::binary);
	std::fstream clientInfo ("FileSharedLog", std::ios::in | std::ios::binary);

	if (!ipInfo)
		MessageBox (L"�����ļ���ʧ�ܣ�");

	if (!ips.ParseFromIstream (&ipInfo))
	{	//��ʧ��
		MessageBox (L"�����ļ�����ʧ�ܣ�");
		ipInfo.close ();
	}
	else
	{	//���������ļ�
		for (int i = 0; i < ips.ip_size(); ++i)
		{
			const qiuwanli::ID2IP& myfriend = ips.ip(i);

			m_ListSaveServer->InsertItem (i, StringToWstring (myfriend.cilentid()).c_str ());
            m_ListSaveServer->SetItemText (i, 1, StringToWstring (myfriend.ip ()).c_str ());
            m_ListSaveServer->SetItemText (i, 2, StringToWstring (myfriend.keymd5 ()).c_str ());
		}
	}
	ipInfo.close ();

	if (!clientInfo)
		MessageBox (L"sharedlist �����ļ���ʧ�ܣ�");

	if (!client.ParseFromIstream (&clientInfo))
	{	//��ʧ��
		MessageBox (L" sharedlist �����ļ�����ʧ�ܣ�");
		clientInfo.close ();
	}
	else
	{	//���������ļ�
		for (int i = 0; i < client.client_size(); ++i)
		{
			const qiuwanli::File2Cilent& shared = client.client (i);

			m_ListFile->InsertItem (i, StringToWstring (shared.filename ()).c_str ());
            m_ListFile->SetItemText (i, 1, StringToWstring (shared.sha512 ()).c_str ());
            m_ListFile->SetItemText (i, 2, StringToWstring (shared.cilentid ()).c_str ());
            m_ListFile->SetItemText (i, 3, StringToWstring (shared.createdate ()).c_str ());
		}
	}

	clientInfo.close ();
}

void CQFileSystemDlg::MakeFilesLog (qiuwanli::File2Cilent * file
                                    , std::string filename
                                    , std::string sha512
                                    , std::string client
                                    , std::string createtime)
{
	file->set_filename (filename);
	file->set_sha512 (sha512);
	file->set_cilentid (client);
	file->set_createdate (createtime);
}

void CQFileSystemDlg::MakeLogs (qiuwanli::Logs * Log
                                , std::string user_id
                                , std::string logdate
                                , std::string loginfo
                                , std::string logtype) 
{
	Log->set_user_id (user_id);
	Log->set_log_date (logdate);
	Log->set_log_info (loginfo);
	Log->set_log_type (logtype);
}
void CQFileSystemDlg::MakeLogs (qiuwanli::ID2IP * id2ip
                                , std::string clientid
                                , std::string ip
                                , std::string Prikey
                                , std::string KeyMd5
                                , std::string Others)
{
	id2ip->set_cilentid (clientid);
	id2ip->set_ip (ip);
	id2ip->set_prikey (Prikey);
	id2ip->set_keymd5 (KeyMd5);
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
    m_ListFile->InsertItem(0, L"dafs");
    m_ListFile->SetItemText(0, 1, L"dafs");
    m_ListFile->SetItemText(0, 2, L"dafs");
    m_ListFile->SetItemText(0, 3, L"dafs");
    m_ListFile->InsertItem(1, L"dafs");
    m_ListFile->SetItemText(1, 1, L"dafs");
    m_ListFile->SetItemText(1, 2, L"dafs");
    m_ListFile->SetItemText(1, 3, L"dafs");
    m_ListFile->InsertItem(2, L"dafs");
    m_ListFile->SetItemText(2, 1, L"dafs");
    m_ListFile->SetItemText(2, 2, L"dafs");
    m_ListFile->SetItemText(3, 3, L"dafs");

    m_ListSaveServer->InsertItem(0, L"dafs");
    m_ListSaveServer->SetItemText(0, 1, L"dafs");
    m_ListSaveServer->SetItemText(0, 2, L"dafs");
    m_ListSaveServer->SetItemText(0, 3, L"dafs");
    m_ListSaveServer->InsertItem(1, L"dafs");
    m_ListSaveServer->SetItemText(1, 1, L"dafs");
    m_ListSaveServer->SetItemText(1, 2, L"dafs");
    m_ListSaveServer->SetItemText(1, 3, L"dafs");
    m_ListSaveServer->InsertItem(2, L"dafs");
    m_ListSaveServer->SetItemText(2, 1, L"dafs");
    m_ListSaveServer->SetItemText(2, 2, L"dafs");
    m_ListSaveServer->SetItemText(3, 3, L"dafs");

    m_ListShared->InsertItem(0, L"dafs");
    m_ListShared->SetItemText(0, 1, L"dafs");
    m_ListShared->SetItemText(0, 2, L"dafs");
    m_ListShared->SetItemText(0, 3, L"dafs");
    m_ListShared->InsertItem(1, L"dafs");
    m_ListShared->SetItemText(1, 1, L"dafs");
    m_ListShared->SetItemText(1, 2, L"dafs");
    m_ListShared->SetItemText(1, 3, L"dafs");
    m_ListShared->InsertItem(2, L"dafs");
    m_ListShared->SetItemText(2, 1, L"dafs");
    m_ListShared->SetItemText(2, 2, L"dafs");
    m_ListShared->SetItemText(3, 3, L"dafs");

    m_ListClient->InsertItem(0, L"dafs");
    m_ListClient->SetItemText(0, 1, L"dafs");
    m_ListClient->SetItemText(0, 2, L"dafs");
    m_ListClient->SetItemText(0, 3, L"dafs");
    m_ListClient->InsertItem(1, L"dafs");
    m_ListClient->SetItemText(1, 1, L"dafs");
    m_ListClient->SetItemText(1, 2, L"dafs");
    m_ListClient->SetItemText(1, 3, L"dafs");
    m_ListClient->InsertItem(2, L"dafs");
    m_ListClient->SetItemText(2, 1, L"dafs");
    m_ListClient->SetItemText(2, 2, L"dafs");
    m_ListClient->SetItemText(3, 3, L"dafs");

    m_ListLogs->InsertItem(0, L"dafs");
    m_ListLogs->SetItemText(0, 1, L"dafs");
    m_ListLogs->SetItemText(0, 2, L"dafs");
    m_ListLogs->SetItemText(0, 3, L"dafs");
    m_ListLogs->InsertItem(1, L"dafs");
    m_ListLogs->SetItemText(1, 1, L"dafs");
    m_ListLogs->SetItemText(1, 2, L"dafs");
    m_ListLogs->SetItemText(1, 3, L"dafs");
    m_ListLogs->InsertItem(2, L"dafs");
    m_ListLogs->SetItemText(2, 1, L"dafs");
    m_ListLogs->SetItemText(2, 2, L"dafs");
    m_ListLogs->SetItemText(3, 3, L"dafs");

}
