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

	InitTabControlItems();
	m_oTabPortMonitoring.OnInit();
	m_oTabUartToUdp.OnInit();
	m_oTabSample.OnInit();

	m_oTabSample.m_oADCSet.m_pWnd = &m_oTabADCSettings;
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

	m_oTabUartToUdp.ScreenToClient(&rs);
	m_oTabPortMonitoring.ScreenToClient(&rs);
	m_oTabSample.ScreenToClient(&rs);
	m_oTabADCSettings.ScreenToClient(&rs);

	//分别设置隐藏和显示
	m_oTabUartToUdp.ShowWindow(FALSE);
	m_oTabPortMonitoring.ShowWindow(FALSE);
	m_oTabSample.ShowWindow(TRUE);
	m_oTabADCSettings.ShowWindow(FALSE);

	//设置默认的选项卡
	m_cTabCtrlItems.SetCurSel(0); 
	CString str;

	str=_T("数据采集");
	m_cTabCtrlItems.InsertItem(0,str);  //添加参数一选项卡
	str=_T("ADC参数设置");
	m_cTabCtrlItems.InsertItem(1,str);    //添加第二选项卡 
	str=_T("串口转UDP");
	m_cTabCtrlItems.InsertItem(2,str);  //添加参数三选项卡
	str=_T("端口监视");
	m_cTabCtrlItems.InsertItem(3,str);    //添加第四选项卡 
	
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
		Invalidate(TRUE);
		break;
	case 1:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	case 2:
		m_oTabUartToUdp.ShowWindow(TRUE);
		m_oTabPortMonitoring.ShowWindow(FALSE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	case 3:
		m_oTabUartToUdp.ShowWindow(FALSE);
		m_oTabPortMonitoring.ShowWindow(TRUE);
		m_oTabSample.ShowWindow(FALSE);
		m_oTabADCSettings.ShowWindow(FALSE);
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

	CDialog::OnClose();
}
