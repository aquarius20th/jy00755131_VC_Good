// PortMonitoringDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "端口监视器.h"
#include "PortMonitoringDlg.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPortMonitoringDlg 对话框




CPortMonitoringDlg::CPortMonitoringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPortMonitoringDlg::IDD, pParent)
	, strFilePath("")
	, m_bStartSave(FALSE)
	, m_iSaveSize(1024)
	, m_strReceiveIP("")
	, m_strSendIP("")
	, m_csRcvPort1(_T(""))
	, m_csSendPort1(_T(""))
	, m_csRcvPort2(_T(""))
	, m_csSendPort2(_T(""))
	, m_csSaveFile( _T(""))
	, m_csSaveSendFile(_T(""))
	, m_csSaveReceiveFile(_T(""))
	, m_csEditShow( _T(""))
	, m_bStartSample(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CPortMonitoringDlg::~CPortMonitoringDlg(void)
{

}
void CPortMonitoringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SAVESIZE, m_iSaveSize);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPreceive);
	DDX_Control(pDX, IDC_IPADDRESS2, m_IPsend);
	DDX_Text(pDX, IDC_EDIT_AIMRCVPORT1, m_csRcvPort1);
	DDX_Text(pDX, IDC_EDIT_AIMSENDPORT1, m_csSendPort1);
	DDX_Text(pDX, IDC_EDIT_AIMRCVPORT2, m_csRcvPort2);
	DDX_Text(pDX, IDC_EDIT_AIMSENDPORT2, m_csSendPort2);
}

BEGIN_MESSAGE_MAP(CPortMonitoringDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILEPATH, &CPortMonitoringDlg::OnBnClickedButtonSavefilepath)
	ON_BN_CLICKED(IDC_BUTTON_STARTSAVE, &CPortMonitoringDlg::OnBnClickedButtonStartsave)
	ON_BN_CLICKED(IDC_BUTTON_STOPSAVE, &CPortMonitoringDlg::OnBnClickedButtonStopsave)
	ON_BN_CLICKED(IDC_BUTTON_CLEARSCREEN, &CPortMonitoringDlg::OnBnClickedButtonClearscreen)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CPortMonitoringDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CPortMonitoringDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CPortMonitoringDlg 消息处理程序

BOOL CPortMonitoringDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	pDlg = this;


	// 得到当前路径
	GetCurrentDirectory(MAX_PATH, cSaveToFilePath);
	// 创建保存信息的文件夹
	CreateDirectory(_T("数据备份"),NULL);
	strFilePath = cSaveToFilePath;
	strFilePath += _T("\\数据备份");

	m_IPreceive.SetAddress(192,168,0,12);
    m_IPsend.SetAddress(192,168,0,11);
// 	m_IPreceive.SetAddress(127,0,0,1);
// 	m_IPsend.SetAddress(127,0,0,1);


	m_csRcvPort1.Format("0x%04x", RcvPort1);
	m_csSendPort1.Format("0x%04x", SendPort1);
	m_csRcvPort2.Format("0x%04x", RcvPort2);
	m_csSendPort2.Format("0x%04x", SendPort2);
	UpdateData(FALSE);

	m_IPreceive.GetWindowText(m_strReceiveIP);
	m_IPsend.GetWindowText(m_strSendIP);

	m_oThreadManage.Init();
	CString str = "";
	str.Format("%d", 0);
	GetDlgItem(IDC_STATIC_SENDNUM)->SetWindowText(str);
	SetTimer(1, 1000, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPortMonitoringDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPortMonitoringDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPortMonitoringDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPortMonitoringDlg::OnBnClickedButtonSavefilepath()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	char szDir[MAX_PATH];

	BROWSEINFO bi;

	ITEMIDLIST *pidl;

	bi.hwndOwner = this->m_hWnd; // 指定父窗口，在对话框显示期间，父窗口将被禁用 


	bi.pidlRoot = NULL; // 如果指定NULL，就以“桌面”为根 

	bi.pszDisplayName = szDir; 

	bi.lpszTitle = "请选择数据存储目录"; // 这一行将显示在对话框的顶端 

	bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;

	bi.lpfn = NULL;

	bi.lParam = 0;

	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);

	if(pidl == NULL) 
	{
		return;
	}
	if(!SHGetPathFromIDList(pidl, szDir))
	{	
		return;
	}
	else
	{
		strFilePath = szDir;
	}
}

void CPortMonitoringDlg::OnBnClickedButtonStartsave()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_bStartSave = TRUE;
}

void CPortMonitoringDlg::OnBnClickedButtonStopsave()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_bStartSave = FALSE;
	CString strtemp = _T("");
	m_Sec.Lock();
	strtemp = m_csEditShow;
	m_Sec.Unlock();
	GetDlgItem(IDC_EDIT_RECEIVE)->SetWindowText(strtemp);
	OnSaveToFile();
}
// 保存接收数据
void CPortMonitoringDlg::OnSaveReceiveData(unsigned char* buf, int iRecLength)
{
	if (m_bStartSave == FALSE)
	{
		return;
	}

	CString str = _T("");
	CString strtemp = _T("");
	int icsSaveFileLength = 0;

	SYSTEMTIME  sysTime;
	GetLocalTime(&sysTime);

	str.Format(_T("%04d.%02d.%02d %02d:%02d:%02d:%04d 接收数据 数据包大小为 %d 数据为：\r\n"),sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, iRecLength);
	strtemp += str;
	
	for (int i=0; i<iRecLength; i++)
	{
		str.Format(_T("%02x "),buf[i]);
		strtemp += str;
	}

	strtemp += _T("\r\n");
	m_Sec.Lock();
	m_csSaveFile += strtemp;
	m_csSaveReceiveFile += strtemp;
	m_csEditShow += strtemp;
	icsSaveFileLength = m_csSaveFile.GetLength();
	m_Sec.Unlock();
//	GetDlgItem(IDC_EDIT_RECEIVE)->SetWindowText(m_cseditshow);
//	UpdateData(FALSE);
	if (icsSaveFileLength> (m_iSaveSize*1024))
	{	
		OnSaveToFile();
	}
}
// vivi 2010.12.09
// 保存发送数据
void CPortMonitoringDlg::OnSaveSendData(unsigned char* buf, int iSendLength)
{
	if (m_bStartSave == FALSE)
	{
		return;
	}
	CString str = _T("");
	CString strtemp = _T("");
	int icsSaveFileLength = 0;
	SYSTEMTIME  sysTime;
	GetLocalTime(&sysTime);

	str.Format(_T("%04d.%02d.%02d %02d:%02d:%02d:%04d 发送数据 数据包大小为 %d 数据为：\r\n"),sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, iSendLength);
	strtemp += str;
	
	for (int i=0; i<iSendLength; i++)
	{
		str.Format(_T("%02x "),buf[i]);
		strtemp += str;
		
	}
	
	strtemp += _T("\r\n");
	m_Sec.Lock();
	m_csSaveFile += strtemp;
	m_csSaveSendFile += strtemp;
	m_csEditShow += strtemp;
	icsSaveFileLength = m_csSaveFile.GetLength();
	m_Sec.Unlock();
//	GetDlgItem(IDC_EDIT_RECEIVE)->SetWindowText(m_cseditshow);
	// 用UpdateData(FALSE)快速反复刷新界面会占用大量资源，拖慢速度
//	UpdateData(FALSE);
	if (icsSaveFileLength > (m_iSaveSize*1024))
	{	
		OnSaveToFile();
	}
}
// vivi 2010.12.09
// 保存到文件中
void CPortMonitoringDlg::OnSaveToFile()
{
	CString csSaveFileTemp = "";
	CString csSaveReceiveFileTemp = "";
	CString csSaveSendFileTemp = "";
	CString strFileName = "";
	SYSTEMTIME  sysTime;
	m_Sec.Lock();
	csSaveFileTemp = m_csSaveFile;
	csSaveReceiveFileTemp = m_csSaveReceiveFile;
	csSaveSendFileTemp = m_csSaveSendFile;
	m_csSaveFile = "";
	m_csSaveReceiveFile = "";
	m_csSaveSendFile = "";
	m_Sec.Unlock();

	if (csSaveFileTemp.GetLength() == 0)
	{	
		return;
	}
	// 得到当前系统时间
	GetLocalTime(&sysTime);
	strFileName.Format("%s\\%04d%02d%02d%02d%02d%02d%04d.text", strFilePath,sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	
	errno_t err;
	//保存成ANSI格式的文件
	if((err = fopen_s(&m_file,strFileName,"w+"))==NULL)
	{
 		fprintf(m_file,_T("%s"),csSaveFileTemp); 
	}
	fclose(m_file);
	// 保存接收到的数据到文件
	if (csSaveReceiveFileTemp.GetLength() != 0)
	{
		strFileName.Format("%s\\Rec%04d%02d%02d%02d%02d%02d%04d.text", strFilePath,sysTime.wYear,sysTime.wMonth,sysTime.wDay,
			sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);

		errno_t err;
		//保存成ANSI格式的文件
		if((err = fopen_s(&m_file,strFileName,"w+"))==NULL)
		{
			fprintf(m_file,_T("%s"),csSaveReceiveFileTemp); 
		}
		fclose(m_file);
	}
	// 保存发送的数据到文件
	if (csSaveSendFileTemp.GetLength() != 0)
	{
		strFileName.Format("%s\\Send%04d%02d%02d%02d%02d%02d%04d.text", strFilePath,sysTime.wYear,sysTime.wMonth,sysTime.wDay,
			sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);

		errno_t err;
		//保存成ANSI格式的文件
		if((err = fopen_s(&m_file,strFileName,"w+"))==NULL)
		{
			fprintf(m_file,_T("%s"),csSaveSendFileTemp); 
		}
		fclose(m_file);
	}

}
void CPortMonitoringDlg::OnBnClickedButtonClearscreen()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_csEditShow = _T("");
	UpdateData(FALSE);
}

void CPortMonitoringDlg::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	m_oThreadManage.OnClose();
	CDialog::OnClose();
	
}
void CPortMonitoringDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	unsigned int uiADCFrameCount = 0;
	unsigned int uiSendADCRetransmissionFramNum = 0;
	unsigned int uiRcvFrameNum = 0;
	CString str = "";
	if (nIDEvent == 1)
	{
		KillTimer(1);
		m_Sec.Lock();
		uiADCFrameCount = m_oThreadManage.m_SendThread.m_uiADCFrameCount;
		uiSendADCRetransmissionFramNum = m_oThreadManage.m_SendThread.m_uiSendADCRetransmissionFramNum;
		uiRcvFrameNum = m_oThreadManage.m_SendThread.m_uiRcvByte;
		m_Sec.Unlock();
		str.Format("%d", uiADCFrameCount);
		GetDlgItem(IDC_STATIC_SENDNUM)->SetWindowText(str);

		str.Format("%d", uiSendADCRetransmissionFramNum);
		GetDlgItem(IDC_STATIC_ADCRETRANSMISSIONNUM)->SetWindowText(str);

		str.Format("%d", uiRcvFrameNum/256);
		GetDlgItem(IDC_STATIC_RCVFRAMECOUNT)->SetWindowText(str);

		SetTimer(1, 1000, NULL);
	}
	CDialog::OnTimer(nIDEvent);
}
void CPortMonitoringDlg::OnBnClickedButtonStart()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_Sec.Lock();
	m_oThreadManage.m_SendThread.m_uiADCFrameCount = 0;
	m_oThreadManage.m_SendThread.m_uiSendADCRetransmissionFramNum = 0;
	m_oThreadManage.m_SendThread.m_oADCLostMap.RemoveAll();
	m_oThreadManage.m_SendThread.m_oADCSendMap.RemoveAll();
	m_bStartSample = TRUE;
	m_Sec.Unlock();
}

void CPortMonitoringDlg::OnBnClickedButtonStop()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_Sec.Lock();
	m_bStartSample = FALSE;
	m_Sec.Unlock();
}
