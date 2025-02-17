// TabControlTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TabControlTest.h"
#include "TabControlTestDlg.h"

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


// CTabControlTestDlg dialog




CTabControlTestDlg::CTabControlTestDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CTabControlTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTabControlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CTabControlTestDlg, CBCGPDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTabControlTestDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CTabControlTestDlg message handlers

BOOL CTabControlTestDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

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
	m_tab1.Create(IDD_DIALOG1,GetDlgItem(IDC_TAB1));
	m_tab2.Create(IDD_DIALOG2,GetDlgItem(IDC_TAB1));
	m_tab3.Create(IDD_DIALOG3,GetDlgItem(IDC_TAB1));

	//获得IDC_TABTEST客户区大小
	CRect rs;
	m_tab.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置
	rs.top+=20; 
	// 	rs.bottom-=1; 
	// 	rs.left+=1; 
	// 	rs.right-=2; 

	//设置子对话框尺寸并移动到指定位置
	m_tab1.MoveWindow(&rs);
	m_tab2.MoveWindow(&rs);
	m_tab3.MoveWindow(&rs);

	m_tab1.ScreenToClient(&rs);
	m_tab2.ScreenToClient(&rs);
	m_tab3.ScreenToClient(&rs);



	//分别设置隐藏和显示
	m_tab1.ShowWindow(TRUE);
	m_tab2.ShowWindow(FALSE);
	m_tab3.ShowWindow(FALSE);

	//设置默认的选项卡
	m_tab.SetCurSel(0); 
	CString str;

	str=_T("选项卡1");
	m_tab.InsertItem(0,str);  //添加参数一选项卡
	str=_T("选项卡2");
	m_tab.InsertItem(1,str);  //添加参数二选项卡
	str=_T("选项卡3");
	m_tab.InsertItem(2,str);    //添加第三选项卡 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTabControlTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CBCGPDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTabControlTestDlg::OnPaint() 
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
		CBCGPDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTabControlTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTabControlTestDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	int CurSel = m_tab.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_tab1.ShowWindow(TRUE); 
		m_tab1.m_tab4.ShowWindow(TRUE);
		m_tab2.ShowWindow(FALSE);
		m_tab3.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	case 1:
		m_tab1.ShowWindow(FALSE);
		m_tab1.m_tab4.ShowWindow(FALSE);
		m_tab1.m_tab5.ShowWindow(FALSE);
		m_tab2.ShowWindow(TRUE);
		m_tab3.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	case 2:
		m_tab1.ShowWindow(FALSE);
		m_tab1.m_tab4.ShowWindow(FALSE);
		m_tab1.m_tab5.ShowWindow(FALSE);
		m_tab2.ShowWindow(FALSE);
		m_tab3.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	default:
		;
	}    

	*pResult = 0;
}
