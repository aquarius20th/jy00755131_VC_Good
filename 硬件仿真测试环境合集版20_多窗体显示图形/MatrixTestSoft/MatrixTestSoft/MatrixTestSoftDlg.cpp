// MatrixTestSoftDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "MatrixTestSoftDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	LoadXmlFile(XMLFileName);
	InitTabControlItems();
	m_oTabPortMonitoring.OnInit();
	m_oTabUartToUdp.OnInit();
	m_oTabSample.OnInit();

	m_oTabADCDataShow.m_pADCDataShow = m_oTabSample.m_oThreadManage.m_oADCDataRecThread.m_dADCDataShow;
	m_oTabSample.m_oADCSet.m_pTabADCSettings = &m_oTabADCSettings;

	
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
void CMatrixTestSoftDlg::InitTabControlItems(void)
{
	m_oTabUartToUdp.Create(IDD_DIALOG_UARTTOUDP,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabPortMonitoring.Create(IDD_DIALOG_PORTMONITORING,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabSample.Create(IDD_DIALOG_SAMPLE,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabADCSettings.Create(IDD_DIALOG_ADCSETTINGS,GetDlgItem(IDC_TAB_ITEMS));
	m_oTabADCDataShow.Create(IDD_DIALOG_ADCDATASHOW,GetDlgItem(IDC_TAB_ITEMS));

	//获得IDC_TABTEST客户区大小
	CRect rs;
	m_cTabCtrlItems.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置
	rs.top+=20; 
	// 	rs.bottom-=1; 
	// 	rs.left+=1; 
	// 	rs.right-=2; 

	//设置子对话框尺寸并移动到指定位置
	m_oTabUartToUdp.MoveWindow(&rs);
	m_oTabPortMonitoring.MoveWindow(&rs);
	m_oTabSample.MoveWindow(&rs);
	m_oTabADCSettings.MoveWindow(&rs);
	m_oTabADCDataShow.MoveWindow(&rs);

	m_oTabUartToUdp.ScreenToClient(&rs);
	m_oTabPortMonitoring.ScreenToClient(&rs);
	m_oTabSample.ScreenToClient(&rs);
	m_oTabADCSettings.ScreenToClient(&rs);
	m_oTabADCDataShow.ScreenToClient(&rs);

	//分别设置隐藏和显示
	m_oTabUartToUdp.ShowWindow(FALSE);
	m_oTabPortMonitoring.ShowWindow(FALSE);
	m_oTabSample.ShowWindow(TRUE);
	m_oTabADCSettings.ShowWindow(FALSE);
	m_oTabADCDataShow.ShowWindow(FALSE);

	//设置默认的选项卡
	m_cTabCtrlItems.SetCurSel(0); 
	CString str;

	str=_T("数据采集");
	m_cTabCtrlItems.InsertItem(0,str);  //添加参数一选项卡
	str=_T("ADC参数设置");
	m_cTabCtrlItems.InsertItem(1,str);	//添加第二选项卡
	str=_T("ADC数据显示");
	m_cTabCtrlItems.InsertItem(2,str);	//添加参数三选项卡
	str=_T("串口转UDP");
	m_cTabCtrlItems.InsertItem(3,str);  //添加参数四选项卡
	str=_T("端口监视");
	m_cTabCtrlItems.InsertItem(4,str);	//添加第五选项卡
	
}

void CMatrixTestSoftDlg::OnTcnSelchangeTabItems(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	int CurSel = m_cTabCtrlItems.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_oTabUartToUdp.ShowWindow(FALSE); 
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(TRUE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	case 1:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(TRUE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	case 2:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	case 3:
		m_oTabUartToUdp.ShowWindow(TRUE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	case 4:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(TRUE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		m_oTabADCDataShow.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	default:
		;
	}    
	*pResult = 0;
}

void CMatrixTestSoftDlg::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	m_oTabUartToUdp.OnClose();
	m_oTabPortMonitoring.OnClose();
	m_oTabSample.OnClose();

	// 保存配置文件
	SaveXmlFile();

	CDialog::OnClose();
}

// 载入XML配置文件
void CMatrixTestSoftDlg::LoadXmlFile(CString csXmlFileName)
{
	OpenXmlFile(csXmlFileName);

	LoadUartToUDPSetup();
	LoadUDPPortMonitoringSetup();
	LoadSampleSetup();
	LoadADCSettingsSetup();

	CloseXmlFile();
}

// 打开XML配置文件
void CMatrixTestSoftDlg::OpenXmlFile(CString csXmlFileName)
{
		// 初始化COM库
	CoInitialize(NULL);

	CString strOLEObject;
	COleException oError;
	COleVariant oVariant;

	strOLEObject = "msxml2.domdocument";
	BOOL bData = m_oXMLDOMDocument.CreateDispatch(strOLEObject, &oError);

	oVariant = csXmlFileName;
	bData = m_oXMLDOMDocument.load(oVariant);
}

// 载入串口转UDP配置
bool CMatrixTestSoftDlg::LoadUartToUDPSetup(void)
{
	CString strKey;
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
bool CMatrixTestSoftDlg::LoadUDPPortMonitoringSetup(void)
{
	CString strKey;
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
bool CMatrixTestSoftDlg::LoadSampleSetup(void)
{
	CString strKey;
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
bool CMatrixTestSoftDlg::LoadADCSettingsSetup(void)
{
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;	
	unsigned int uitemp = 0;
	unsigned short int ustemp = 0;
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
		ustemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_usHpfLow = ustemp;

		strKey =_T("HPFHigh");
		ustemp = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		m_oTabADCSettings.m_usHpfHigh = ustemp;


		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 关闭程序配置文件
void CMatrixTestSoftDlg::CloseXmlFile(void)
{
	m_oXMLDOMDocument.DetachDispatch();
	// 释放COM库
	CoUninitialize();
}

// 保存串口转UDP配置
bool CMatrixTestSoftDlg::SaveUartToUDPSetup(void)
{
	CString strKey;
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
bool CMatrixTestSoftDlg::SaveUDPPortMonitoringSetup(void)
{
	CString strKey;
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
bool CMatrixTestSoftDlg::SaveSampleSetup(void)
{
	CString strKey;
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
bool CMatrixTestSoftDlg::SaveADCSettingsSetup(void)
{
	CString strKey;
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
		oVariant = (long)m_oTabADCSettings.m_usHpfLow;
		oElement.setAttribute(strKey, oVariant);

		strKey =_T("HPFHigh");
		oVariant = (long)m_oTabADCSettings.m_usHpfHigh;
		oElement.setAttribute(strKey, oVariant);

		return true;
	}
	catch (CException* )
	{
		return false;
	}
}

// 保存配置文件
void CMatrixTestSoftDlg::SaveXmlFile(void)
{
	COleVariant oVariant;

	OpenXmlFile(XMLFileName);

	SaveUartToUDPSetup();
	SaveUDPPortMonitoringSetup();
	SaveSampleSetup();
	SaveADCSettingsSetup();
	oVariant = XMLFileName;
	m_oXMLDOMDocument.save(oVariant);

	CloseXmlFile();
}
