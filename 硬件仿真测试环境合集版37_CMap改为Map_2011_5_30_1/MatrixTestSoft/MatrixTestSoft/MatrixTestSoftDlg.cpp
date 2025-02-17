// MatrixTestSoftDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "MatrixTestSoftDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static unsigned short CRC_TABLE[256]=
{
	0x0000,0x8005,0x800F,0x000A,0x801B,0x001E,0x0014,0x8011,
	0x8033,0x0036,0x003C,0x8039,0x0028,0x802D,0x8027,0x0022,
	0x8063,0x0066,0x006C,0x8069,0x0078,0x807D,0x8077,0x0072,
	0x0050,0x8055,0x805F,0x005A,0x804B,0x004E,0x0044,0x8041,
	0x80C3,0x00C6,0x00CC,0x80C9,0x00D8,0x80DD,0x80D7,0x00D2,
	0x00F0,0x80F5,0x80FF,0x00FA,0x80EB,0x00EE,0x00E4,0x80E1,
	0x00A0,0x80A5,0x80AF,0x00AA,0x80BB,0x00BE,0x00B4,0x80B1,
	0x8093,0x0096,0x009C,0x8099,0x0088,0x808D,0x8087,0x0082,
	0x8183,0x0186,0x018C,0x8189,0x0198,0x819D,0x8197,0x0192,
	0x01B0,0x81B5,0x81BF,0x01BA,0x81AB,0x01AE,0x01A4,0x81A1,
	0x01E0,0x81E5,0x81EF,0x01EA,0x81FB,0x01FE,0x01F4,0x81F1,
	0x81D3,0x01D6,0x01DC,0x81D9,0x01C8,0x81CD,0x81C7,0x01C2,
	0x0140,0x8145,0x814F,0x014A,0x815B,0x015E,0x0154,0x8151,
	0x8173,0x0176,0x017C,0x8179,0x0168,0x816D,0x8167,0x0162,
	0x8123,0x0126,0x012C,0x8129,0x0138,0x813D,0x8137,0x0132,
	0x0110,0x8115,0x811F,0x011A,0x810B,0x010E,0x0104,0x8101,
	0x8303,0x0306,0x030C,0x8309,0x0318,0x831D,0x8317,0x0312,
	0x0330,0x8335,0x833F,0x033A,0x832B,0x032E,0x0324,0x8321,
	0x0360,0x8365,0x836F,0x036A,0x837B,0x037E,0x0374,0x8371,
	0x8353,0x0356,0x035C,0x8359,0x0348,0x834D,0x8347,0x0342,
	0x03C0,0x83C5,0x83CF,0x03CA,0x83DB,0x03DE,0x03D4,0x83D1,
	0x83F3,0x03F6,0x03FC,0x83F9,0x03E8,0x83ED,0x83E7,0x03E2,
	0x83A3,0x03A6,0x03AC,0x83A9,0x03B8,0x83BD,0x83B7,0x03B2,
	0x0390,0x8395,0x839F,0x039A,0x838B,0x038E,0x0384,0x8381,
	0x0280,0x8285,0x828F,0x028A,0x829B,0x029E,0x0294,0x8291,
	0x82B3,0x02B6,0x02BC,0x82B9,0x02A8,0x82AD,0x82A7,0x02A2,
	0x82E3,0x02E6,0x02EC,0x82E9,0x02F8,0x82FD,0x82F7,0x02F2,
	0x02D0,0x82D5,0x82DF,0x02DA,0x82CB,0x02CE,0x02C4,0x82C1,
	0x8243,0x0246,0x024C,0x8249,0x0258,0x825D,0x8257,0x0252,
	0x0270,0x8275,0x827F,0x027A,0x826B,0x026E,0x0264,0x8261,
	0x0220,0x8225,0x822F,0x022A,0x823B,0x023E,0x0234,0x8231,
	0x8213,0x0216,0x021C,0x8219,0x0208,0x820D,0x8207,0x0202
};

/*
函数名称：unsigned short get_crc_16_private(const char* buf, int buf_size)
功能描述：通过输入指定长度的字符串生成CRC校验码
返回值：CRC16编码
参数：
buf		cnost char*			传入字符串指针
buf_size	int				字符串长度
注：
*/
unsigned short get_crc_16(unsigned char* buf, int buf_size)
{
	unsigned short crc_16 = 0;
	unsigned char tmp = 0;
	unsigned char* ptr = (unsigned char*)buf;

	while(buf_size--){

		tmp = crc_16 >> 8;
		crc_16 <<= 8;
		crc_16 ^= CRC_TABLE[tmp ^ *ptr++];
	}
	return crc_16;
}
// 宽字节字符串转换为多字节字符串
string WideCharToMultiChar(wstring str)
{
	string return_value;
	//获取缓冲区的大小，并申请空间，缓冲区大小是按字节计算的
	LPCWSTR lpWideCharStr = str.c_str();
	int cchWideChar = (int)str.size();
	int len = WideCharToMultiByte(CP_ACP, 0, lpWideCharStr, cchWideChar, NULL, 0, NULL, NULL);
	char *buffer = new char[len+1];
	WideCharToMultiByte(CP_ACP, 0, lpWideCharStr, cchWideChar, buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值
	return_value.append(buffer);
	delete []buffer;
	return return_value;
}
// 将含有中文的字符串写入文件
void WriteCHToCFile(CArchive *ar, CString str)
{
	if (ar == NULL)
	{
		return;
	}
	//因为需要保存的内容包含中文，所以需要如下的转换过程
	// 为了少显示一个空格，故在计算长度时-1
// 	int ansiCount = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL) - 1;
// 	char * pTempChar = (char*)malloc(ansiCount*sizeof(char));
// 	memset(pTempChar, 0, ansiCount);
// 	WideCharToMultiByte(CP_ACP, 0, str, -1, pTempChar, ansiCount, NULL, NULL);
// 	file->Write(pTempChar, ansiCount);
// 	free(pTempChar);
	ar->WriteString(str);
}
// void WriteCHToCFile(CFile *file, CString str)
// {
// 	if (file->m_hFile == CFile::hFileNull)
// 	{
// 		return;
// 	}
// 	//因为需要保存的内容包含中文，所以需要如下的转换过程
// 	// 为了少显示一个空格，故在计算长度时-1
// 	int ansiCount = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL) - 1;
// 	char* pTempChar = (char*)malloc(ansiCount*sizeof(char));
// 	memset(pTempChar, 0, ansiCount);
// 	WideCharToMultiByte(CP_ACP, 0, str, -1, pTempChar, ansiCount, NULL, NULL);
// 	file->Write(pTempChar, ansiCount);
// 	free(pTempChar);
// }
// CString转换为const char*
const char* ConvertCStringToConstCharPointer(CString str)
{
	const char* pChar = NULL;
	char pach[100];
	CStringW strw;
	wstring wstr;
	strw = str;
	wstr = strw;
	string mstring = WideCharToMultiChar(wstr);
	strcpy_s( pach, sizeof(pach), mstring.c_str());
	pChar = pach;
	return pChar;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CMatrixTestSoftDlg dialog




CMatrixTestSoftDlg::CMatrixTestSoftDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMatrixTestSoftDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMatrixTestSoftDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ITEMS, m_cTabCtrlItems);
}

BEGIN_MESSAGE_MAP(CMatrixTestSoftDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ITEMS, &CMatrixTestSoftDlg::OnTcnSelchangeTabItems)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMatrixTestSoftDlg message handlers

BOOL CMatrixTestSoftDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	STARTUPINFO startup_info;
	wchar_t cmd_buf[1000] = {_T("")};
	PROCESS_INFORMATION proc_info;
	HWND window_handler = NULL;
	window_handler = ::FindWindow(NULL, _T("Rev: 1.2.33.202.2011.08.12.20MHZ.FIFO.SERIAL.NO"));
	if (window_handler != NULL)
	{
		::SendMessage(window_handler, WM_COMMAND, 1103, NULL);//结束中转
		::SendMessage(window_handler, WM_CLOSE, NULL, NULL); //关闭程序
		Sleep(1000);
	}

	ZeroMemory(&startup_info, sizeof(startup_info));
	ZeroMemory(&proc_info, sizeof(proc_info));
	startup_info.cb = sizeof(startup_info);
	startup_info.dwFlags = STARTF_USESHOWWINDOW;
	startup_info.wShowWindow = SW_HIDE;

	CreateProcess(_T("pcid.exe"), cmd_buf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, 
		&startup_info, &proc_info);

	Sleep(1000);
	window_handler = ::FindWindow(NULL, _T("Rev: 1.2.33.202.2011.08.12.20MHZ.FIFO.SERIAL.NO"));
	::SendMessage(window_handler, WM_COMMAND, 1103, NULL);//开始中转


	// 得到当前路径
	wchar_t cSaveToFilePath[MAX_PATH];
//	char cSaveToFilePath[MAX_PATH];
	SYSTEMTIME  sysTime;
	CString csSaveFilePath = _T("");
	CString str = _T("");
	// 得到当前路径
	GetCurrentDirectory(MAX_PATH, cSaveToFilePath);
	// 得到当前时间
	GetLocalTime(&sysTime);
	csSaveFilePath = cSaveToFilePath;
	csSaveFilePath += _T("\\程序运行日志");
	str.Format(_T("_%04d_%02d_%02d_%02d_%02d_%02d_%03d"), sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	csSaveFilePath += str;
	// 创建程序运行日志文件夹
	CreateDirectory(csSaveFilePath,NULL);
	m_oLogFile.m_csSaveLogFilePath = csSaveFilePath;
	m_oTabPortMonitoring.m_oSaveFile.m_csSaveFilePath = csSaveFilePath;
	m_oTabSample.m_csSaveFilePath = csSaveFilePath;
	
	// 创建并打开程序运行日志文件
	m_oLogFile.OnOpenLogFile();
	m_oTabPortMonitoring.m_pLogFile = &m_oLogFile;
	m_oTabSample.m_pLogFile = &m_oLogFile;
	m_oTabUartToUdp.m_pLogFile = &m_oLogFile;

	// 载入配置文件
	if (FALSE == LoadXmlFile(XMLFileName))
	{
		CloseXmlFile();
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::LoadXmlFile"), _T("载入配置文件出错！"), ErrorStatus);
	}
	else
	{
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::LoadXmlFile"), _T("载入配置文件成功！"), SuccessStatus);
	}

	WSADATA data;									// 定义结构体变量
	WORD w = MAKEWORD(2, 2);			// 初始化套接字版本号
	int err = WSAStartup(w, &data);							// 初始化套接字库
	if (err == 0)
	{
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::OnInitDialog"), _T("初始化套接字库成功！"), SuccessStatus);
	}
	else
	{
		str.Format(_T("初始化套接字库失败，错误码为%d！"), err);
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::OnInitDialog"), str, ErrorStatus);
	}

	// 设置选项卡控件
	InitTabControlItems();
	// 各选项卡进行初始化设置
	m_oTabPortMonitoring.OnInit();
	m_oTabSample.OnInit();
	m_oTabUartToUdp.OnInit();

	m_oTabADCDataShow.m_pADCDataShow = m_oTabSample.m_oThreadManage.m_oADCDataRecThread.m_uiADCDataShow;
	m_oTabSample.m_oThreadManage.m_oADCSet.m_pTabADCSettings = &m_oTabADCSettings;
	m_oTabEepromUpdata.m_pInstrumentList = &m_oTabSample.m_oThreadManage.m_oInstrumentList;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMatrixTestSoftDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMatrixTestSoftDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMatrixTestSoftDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 初始化选项卡控件
//************************************
// Method:    InitTabControlItems
// FullName:  CMatrixTestSoftDlg::InitTabControlItems
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CMatrixTestSoftDlg::InitTabControlItems(void)
{
	CRect rs;
	CString str = _T("");
	// 创建选项卡
	m_oTabUartToUdp.Create(IDD_DIALOG_UARTTOUDP,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabPortMonitoring.Create(IDD_DIALOG_PORTMONITORING,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabSample.Create(IDD_DIALOG_SAMPLE,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabADCSettings.Create(IDD_DIALOG_ADCSETTINGS,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabADCDataShow.Create(IDD_DIALOG_ADCDATASHOW,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabEepromUpdata.Create(IDD_DIALOG_EEPROM, GetDlgItem(IDC_TAB_ITEMS));

	//获得IDC_TABTEST客户区大小
	m_cTabCtrlItems.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置
	rs.top += 20; 

	//设置子对话框尺寸并移动到指定位置
	m_oTabUartToUdp.MoveWindow(&rs);
	m_oTabPortMonitoring.MoveWindow(&rs);
	m_oTabSample.MoveWindow(&rs);
	m_oTabADCSettings.MoveWindow(&rs);
	m_oTabADCDataShow.MoveWindow(&rs);
	m_oTabEepromUpdata.MoveWindow(&rs);

	m_oTabUartToUdp.ScreenToClient(&rs);
	m_oTabPortMonitoring.ScreenToClient(&rs);
	m_oTabSample.ScreenToClient(&rs);
	m_oTabADCSettings.ScreenToClient(&rs);
	m_oTabADCDataShow.ScreenToClient(&rs);
	m_oTabEepromUpdata.ScreenToClient(&rs);

	//分别设置隐藏和显示
	m_oTabUartToUdp.ShowWindow(FALSE);
	m_oTabPortMonitoring.ShowWindow(FALSE);
	m_oTabSample.ShowWindow(TRUE);
	m_oTabADCSettings.ShowWindow(FALSE);
	m_oTabADCDataShow.ShowWindow(FALSE);
	m_oTabEepromUpdata.ShowWindow(FALSE);

	//设置默认的选项卡
	m_cTabCtrlItems.SetCurSel(TabSampleCtl); 

	str=_T("数据采集");
	m_cTabCtrlItems.InsertItem(TabSampleCtl, str);		// 添加参数一选项卡
	str=_T("ADC参数设置");
	m_cTabCtrlItems.InsertItem(TabADCSettingsCtl, str);		// 添加第二选项卡
	// @@@@@@@@ cxm 2011.11.25 提供给用户暂不显示以下4个界面
// 	str=_T("ADC数据显示");
// 	m_cTabCtrlItems.InsertItem(TabADCDataShowCtl, str);		// 添加参数三选项卡
// 	str=_T("串口转UDP");
// 	m_cTabCtrlItems.InsertItem(TabUartToUdpCtl, str);				// 添加参数四选项卡
// 	str=_T("端口监视");
// 	m_cTabCtrlItems.InsertItem(TabPortMonitoringCtl, str);		// 添加第五选项卡
// 	str=_T("程序更新");
// 	m_cTabCtrlItems.InsertItem(TabEepromCtl, str);					// 添加第六选项卡
}

void CMatrixTestSoftDlg::OnTcnSelchangeTabItems(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	int CurSel = m_cTabCtrlItems.GetCurSel();
	switch(CurSel)
	{
	case TabSampleCtl:
		m_oTabUartToUdp.ShowWindow(FALSE); 
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		m_oTabEepromUpdata.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	case TabADCSettingsCtl:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		m_oTabEepromUpdata.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	case TabADCDataShowCtl:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabEepromUpdata.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	case TabUartToUdpCtl:
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		m_oTabEepromUpdata.ShowWindow(FALSE);
		m_oTabUartToUdp.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	case TabPortMonitoringCtl:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		m_oTabEepromUpdata.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	case TabEepromCtl:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabEepromUpdata.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	default:
		;
	}    
	*pResult = 0;
}

//************************************
// Method:    OnClose
// FullName:  CMatrixTestSoftDlg::OnClose
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void CMatrixTestSoftDlg::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	m_oTabUartToUdp.OnClose();
	m_oTabPortMonitoring.OnClose();
	m_oTabSample.OnClose();
	m_oTabEepromUpdata.OnClose();
	// 保存配置文件
	if (FALSE == SaveXmlFile())
	{
		CloseXmlFile();
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::SaveXmlFile"), _T("保存配置文件出错！"), ErrorStatus);
	}
	else
	{
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::SaveXmlFile"), _T("保存配置文件成功！"), SuccessStatus);
	}

	// 释放套接字库
	int err = WSACleanup();	
	if (err == 0)
	{
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::OnClose"), _T("释放套接字库成功！"), SuccessStatus);
	}
	else
	{
		CString str = _T("");
		str.Format(_T("释放套接字库失败，错误码为%d！"), err);
		m_oLogFile.OnWriteLogFile(_T("CMatrixTestSoftDlg::OnClose"), str, ErrorStatus);
	}

	// 关闭日志文件
	m_oLogFile.OnCloseLogFile();
	HWND window_handler = NULL;
	window_handler = ::FindWindow(NULL, _T("Rev: 1.2.33.202.2011.08.12.20MHZ.FIFO.SERIAL.NO"));
	if (window_handler != NULL)
	{
		::SendMessage(window_handler, WM_COMMAND, 1103, NULL);//结束中转
		::SendMessage(window_handler, WM_CLOSE, NULL, NULL); //关闭程序
	}
	CDialog::OnClose();
}

// 载入XML配置文件
//************************************
// Method:    LoadXmlFile
// FullName:  CMatrixTestSoftDlg::LoadXmlFile
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: CString csXmlFileName
//************************************
BOOL CMatrixTestSoftDlg::LoadXmlFile(CString csXmlFileName)
{
	if (FALSE == OpenXmlFile(csXmlFileName))
	{
		return FALSE;
	}
	if (false == LoadUartToUDPSetup())
	{
		return FALSE;
	}
	if (false == LoadUDPPortMonitoringSetup())
	{
		return FALSE;
	}
	if (false == LoadSampleSetup())
	{
		return FALSE;
	}
	if (false == LoadADCSettingsSetup())
	{
		return FALSE;
	}
	CloseXmlFile();
	return TRUE;
}

// 打开XML配置文件
//************************************
// Method:    OpenXmlFile
// FullName:  CMatrixTestSoftDlg::OpenXmlFile
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: CString csXmlFileName
//************************************
BOOL CMatrixTestSoftDlg::OpenXmlFile(CString csXmlFileName)
{
	BOOL bReturn = FALSE;
		// 初始化COM库
	CoInitialize(NULL);

	CString strOLEObject;
	COleException oError;
	COleVariant oVariant;

	strOLEObject = "msxml2.domdocument";
	bReturn = m_oXMLDOMDocument.CreateDispatch(strOLEObject, &oError);
	if (bReturn == FALSE)
	{
		return bReturn;
	}
	oVariant = csXmlFileName;
	bReturn = m_oXMLDOMDocument.load(oVariant);

	return bReturn;
}

// 载入串口转UDP配置
//************************************
// Method:    LoadUartToUDPSetup
// FullName:  CMatrixTestSoftDlg::LoadUartToUDPSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::LoadUartToUDPSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;	
	unsigned int uitemp = 0;
	CString cstemp = _T("");
	try
	{
		// 找到UartToUDPHost
		strKey =_T("UartToUDPHost");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到串口端口
		strKey =_T("Com");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabUartToUdp.m_uiSerialPortComCurSel = uitemp;

		// 得到串口波特率
		strKey =_T("Baud");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabUartToUdp.m_uiSerialPortBaudCurSel = uitemp;

		// 得到接收端口
		strKey =_T("HostPort");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabUartToUdp.m_uiRecPort = uitemp;

		// 找到UartToUDPServer
		strKey =_T("UartToUDPServer");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到服务器IP地址
		strKey =_T("ServerIP");
		cstemp = CXMLDOMTool::GetElementAttributeString(&oElement, strKey);
		m_oTabUartToUdp.m_csIPAddress = cstemp;

		// 得到服务器接收端口
		strKey =_T("ServerPort");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabUartToUdp.m_uiSendPort = uitemp;
	
		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 载入UDP端口监视配置
//************************************
// Method:    LoadUDPPortMonitoringSetup
// FullName:  CMatrixTestSoftDlg::LoadUDPPortMonitoringSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::LoadUDPPortMonitoringSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;	
	unsigned int uitemp = 0;
	CString cstemp = _T("");
	try
	{
		// 找到UDPPortMonitoringHost
		strKey =_T("UDPPortMonitoringHost");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到客户端IP地址
		strKey =_T("HostIP");
		cstemp = CXMLDOMTool::GetElementAttributeString(&oElement, strKey);
		m_oTabPortMonitoring.m_csIPSend = cstemp;

		// 得到客户端接收端口
		strKey =_T("HostRecPort");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabPortMonitoring.m_uiRcvPort2 = uitemp;

		// 得到客户端发送端口
		strKey =_T("HostSendPort");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabPortMonitoring.m_uiSendPort2 = uitemp;

		// 找到UDPPortMonitoringServer
		strKey =_T("UDPPortMonitoringServer");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到服务器IP地址
		strKey =_T("ServerIP");
		cstemp = CXMLDOMTool::GetElementAttributeString(&oElement, strKey);
		m_oTabPortMonitoring.m_csIPRec = cstemp;

		// 得到服务器接收端口
		strKey =_T("ServerRecPort");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabPortMonitoring.m_uiRcvPort1 = uitemp;

		// 得到服务器发送端口
		strKey =_T("ServerSendPort");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabPortMonitoring.m_uiSendPort1 = uitemp;

		// 找到UDPPortMonitoringSetup 
		strKey =_T("UDPPortMonitoringSetup");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到自动存储文件的尺寸
		strKey =_T("AutoSaveLength");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabPortMonitoring.m_iSaveSize = uitemp;

		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 载入采样配置
//************************************
// Method:    LoadSampleSetup
// FullName:  CMatrixTestSoftDlg::LoadSampleSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::LoadSampleSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;	
	unsigned int uitemp = 0;
	try
	{
		// 找到SampleSetup
		strKey =_T("SampleSetup");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到ADC数据自动存储帧数
		strKey =_T("SaveADCFrameNum");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabSample.m_uiADCFileLength = uitemp;

		// 得到测网数据发送端口
		strKey =_T("SendPort");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabSample.m_uiSendPort = uitemp;

		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 载入ADC参数设置配置
//************************************
// Method:    LoadADCSettingsSetup
// FullName:  CMatrixTestSoftDlg::LoadADCSettingsSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::LoadADCSettingsSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;	
	unsigned int uitemp = 0;
	byte uctemp = 0;
	try
	{
		// 找到ADCControl
		strKey =_T("ADCControl");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey =_T("ADC_SYNC");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCSync = uitemp;

		strKey =_T("ADC_MODE");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCMode = uitemp;

		strKey =_T("ADC_SPS");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCSps = uitemp;

		strKey =_T("ADC_PHS");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCPhs = uitemp;

		strKey =_T("ADC_FILTER");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCFilter = uitemp;

		strKey =_T("ADC_MUX");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCMux = uitemp;

		strKey =_T("ADC_CHOP");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCChop= uitemp;

		strKey =_T("ADC_PGA");
		uitemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_uiADCPga = uitemp;

		// 找到HPFSettings
		strKey =_T("HPFSettings");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey =_T("HPFLow");
		uctemp = CXMLDOMTool::GetElementAttributeByte(&oElement, strKey);
		m_oTabADCSettings.m_ucHpfLow = uctemp;

		strKey =_T("HPFHigh");
		uctemp = CXMLDOMTool::GetElementAttributeByte(&oElement, strKey);
		m_oTabADCSettings.m_ucHpfHigh = uctemp;


		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 关闭程序配置文件
//************************************
// Method:    CloseXmlFile
// FullName:  CMatrixTestSoftDlg::CloseXmlFile
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CMatrixTestSoftDlg::CloseXmlFile(void)
{
	m_oXMLDOMDocument.DetachDispatch();
	// 释放COM库
	CoUninitialize();
}

// 保存串口转UDP配置
//************************************
// Method:    SaveUartToUDPSetup
// FullName:  CMatrixTestSoftDlg::SaveUartToUDPSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::SaveUartToUDPSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	COleVariant oVariant;

	try
	{
		// 找到UartToUDPHost设置区
		strKey = "UartToUDPHost";
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "Com";
		oVariant = (long)m_oTabUartToUdp.m_uiSerialPortComCurSel;
		oElement.setAttribute(strKey, oVariant);
		strKey = "Baud";
		oVariant = (long)m_oTabUartToUdp.m_uiSerialPortBaudCurSel;
		oElement.setAttribute(strKey, oVariant);
		strKey = "HostPort";
		oVariant = (long)m_oTabUartToUdp.m_uiRecPort;
		oElement.setAttribute(strKey, oVariant);

		// 找到UartToUDPServer设置区
		strKey = "UartToUDPServer";
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "ServerIP";
		oVariant = m_oTabUartToUdp.m_csIPAddress;
		oElement.setAttribute(strKey, oVariant);
		strKey = "ServerPort";
		oVariant = (long)m_oTabUartToUdp.m_uiSendPort;
		oElement.setAttribute(strKey, oVariant);

		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 保存UDP端口监视配置
//************************************
// Method:    SaveUDPPortMonitoringSetup
// FullName:  CMatrixTestSoftDlg::SaveUDPPortMonitoringSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::SaveUDPPortMonitoringSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	COleVariant oVariant;

	try
	{
		// 找到UDPPortMonitoringHost
		strKey =_T("UDPPortMonitoringHost");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到客户端IP地址
		strKey =_T("HostIP");
		oVariant = m_oTabPortMonitoring.m_csIPSend;
		oElement.setAttribute(strKey, oVariant);

		// 得到客户端接收端口
		strKey =_T("HostRecPort");
		oVariant = (long)m_oTabPortMonitoring.m_uiRcvPort2;
		oElement.setAttribute(strKey, oVariant);

		// 得到客户端发送端口
		strKey =_T("HostSendPort");
		oVariant = (long)m_oTabPortMonitoring.m_uiSendPort2;
		oElement.setAttribute(strKey, oVariant);

		// 找到UDPPortMonitoringServer
		strKey =_T("UDPPortMonitoringServer");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到服务器IP地址
		strKey =_T("ServerIP");
		oVariant = m_oTabPortMonitoring.m_csIPRec;
		oElement.setAttribute(strKey, oVariant);

		// 得到服务器接收端口
		strKey =_T("ServerRecPort");
		oVariant = (long)m_oTabPortMonitoring.m_uiRcvPort1;
		oElement.setAttribute(strKey, oVariant);

		// 得到服务器发送端口
		strKey =_T("ServerSendPort");
		oVariant = (long)m_oTabPortMonitoring.m_uiSendPort1;
		oElement.setAttribute(strKey, oVariant);

		// 找到UDPPortMonitoringSetup 
		strKey =_T("UDPPortMonitoringSetup");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到自动存储文件的尺寸
		strKey =_T("AutoSaveLength");
		oVariant = (long)m_oTabPortMonitoring.m_iSaveSize;
		oElement.setAttribute(strKey, oVariant);

		return true;
	}
	catch (CException* )
	{
		return false;
	}
}


// 保存采样设置
//************************************
// Method:    SaveSampleSetup
// FullName:  CMatrixTestSoftDlg::SaveSampleSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::SaveSampleSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	COleVariant oVariant;

	try
	{
		// 找到SampleSetup
		strKey =_T("SampleSetup");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		// 得到ADC数据自动存储帧数
		strKey =_T("SaveADCFrameNum");
		oVariant = (long)m_oTabSample.m_uiADCFileLength;
		oElement.setAttribute(strKey, oVariant);

		// 得到测网数据发送端口
		strKey =_T("SendPort");
		oVariant = (long)m_oTabSample.m_uiSendPort;
		oElement.setAttribute(strKey, oVariant);

		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 保存ADC参数设置配置
//************************************
// Method:    SaveADCSettingsSetup
// FullName:  CMatrixTestSoftDlg::SaveADCSettingsSetup
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CMatrixTestSoftDlg::SaveADCSettingsSetup(void)
{
	CString strKey = _T("");
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	COleVariant oVariant;

	try
	{
		// 找到ADCControl
		strKey =_T("ADCControl");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey =_T("ADC_SYNC");
		oVariant = (long)m_oTabADCSettings.m_uiADCSync;
		oElement.setAttribute(strKey, oVariant);

		strKey =_T("ADC_MODE");
		oVariant = (long)m_oTabADCSettings.m_uiADCMode;
		oElement.setAttribute(strKey, oVariant);

		strKey =_T("ADC_SPS");
		oVariant = (long)m_oTabADCSettings.m_uiADCSps;
		oElement.setAttribute(strKey, oVariant);

		strKey =_T("ADC_PHS");
		oVariant = (long)m_oTabADCSettings.m_uiADCPhs;
		oElement.setAttribute(strKey, oVariant);

		strKey =_T("ADC_FILTER");
		oVariant = (long)m_oTabADCSettings.m_uiADCFilter;
		oElement.setAttribute(strKey, oVariant);
	
		strKey =_T("ADC_MUX");
		oVariant = (long)m_oTabADCSettings.m_uiADCMux;
		oElement.setAttribute(strKey, oVariant);

		strKey =_T("ADC_CHOP");
		oVariant = (long)m_oTabADCSettings.m_uiADCChop;
		oElement.setAttribute(strKey, oVariant);
	
		strKey =_T("ADC_PGA");
		oVariant = (long)m_oTabADCSettings.m_uiADCPga;
		oElement.setAttribute(strKey, oVariant);

		// 找到HPFSettings
		strKey =_T("HPFSettings");
		lpDispatch = m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey =_T("HPFLow");
		oVariant = m_oTabADCSettings.m_ucHpfLow;
		oElement.setAttribute(strKey, oVariant);

		strKey =_T("HPFHigh");
		oVariant = m_oTabADCSettings.m_ucHpfHigh;
		oElement.setAttribute(strKey, oVariant);

		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 保存配置文件
//************************************
// Method:    SaveXmlFile
// FullName:  CMatrixTestSoftDlg::SaveXmlFile
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
BOOL CMatrixTestSoftDlg::SaveXmlFile(void)
{
	COleVariant oVariant;

	if (FALSE == OpenXmlFile(XMLFileName))
	{
		return FALSE;
	}
	if (false == SaveUartToUDPSetup())
	{
		return FALSE;
	}
	if (false == SaveUDPPortMonitoringSetup())
	{
		return FALSE;
	}
	if (false == SaveSampleSetup())
	{
		return FALSE;
	}
	if (false == SaveADCSettingsSetup())
	{
		return FALSE;
	}
	oVariant = XMLFileName;
	m_oXMLDOMDocument.save(oVariant);

	CloseXmlFile();
	return TRUE;
}
