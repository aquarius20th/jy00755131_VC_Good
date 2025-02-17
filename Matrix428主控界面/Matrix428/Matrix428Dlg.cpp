// Matrix428Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Matrix428.h"
#include "Matrix428Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMatrix428Dlg dialog




CMatrix428Dlg::CMatrix428Dlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CMatrix428Dlg::IDD, pParent)
	, m_bShowDirection(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMatrix428Dlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMatrix428Dlg, CBCGPDialog)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_QUERYDRAGICON()

	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_MINMIZEBOX, &CMatrix428Dlg::OnBnClickedButtonMinmizebox)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEBOX, &CMatrix428Dlg::OnBnClickedButtonClosebox)
	ON_STN_DBLCLK(IDC_STATIC_E428, &CMatrix428Dlg::OnStnDblclickStaticE428)
	ON_BN_CLICKED(ID_BUTTON_LINECLIENT, &CMatrix428Dlg::OnBnClickedButtonLineClient)
	ON_BN_CLICKED(ID_BUTTON_LINESERVER, &CMatrix428Dlg::OnBnClickedButtonLineServer)
	ON_BN_CLICKED(ID_BUTTON_OPERATIONCLIENT, &CMatrix428Dlg::OnBnClickedButtonOperationClient)
	ON_BN_CLICKED(ID_BUTTON_OPERATIONSERVER, &CMatrix428Dlg::OnBnClickedButtonOperationServer)
	ON_BN_CLICKED(ID_BUTTON_SIM, &CMatrix428Dlg::OnBnClickedButtonSim)
END_MESSAGE_MAP()


// CMatrix428Dlg message handlers

BOOL CMatrix428Dlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	CRect dlgRect;
	dlgRect.left = 0;
	dlgRect.top= 0;
	dlgRect.right = 800;
	dlgRect.bottom = 76;
	CWnd::SetWindowPos(NULL,0,0,dlgRect.Width(),dlgRect.Height(), SWP_NOZORDER);

	// 设置控件的位置和尺寸
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_BUTTON_MINMIZEBOX );    // 获取控件指针，IDC_BUTTON_MINMIZEBOX为控件ID号
	pWnd->MoveWindow( CRect(2,2,32,32) );	// 在窗口左上角显示一个宽32、高32的按钮控件

	pWnd = GetDlgItem( IDC_BUTTON_CLOSEBOX );
	pWnd->MoveWindow( CRect(36,2,68,32) );	// 两个按钮之间间距为2

	pWnd = GetDlgItem( IDC_STATIC_E428 );
	pWnd->MoveWindow( CRect(2,36,68,68) );	// 设置静态控件位置

	GetCurrentDirectory(500, CurrentPath);	// 获取当前路径

	LoadToolBar();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMatrix428Dlg::OnPaint() 
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
HCURSOR CMatrix428Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 手动设置最小化按钮
void CMatrix428Dlg::OnBnClickedButtonMinmizebox()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	ShowWindow(SW_MINIMIZE ); 
}
// 手动设置关闭按钮
void CMatrix428Dlg::OnBnClickedButtonClosebox()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnCancel(); 

}
// 实现无标题栏对话框的拖动
LRESULT CMatrix428Dlg::OnNcHitTest(CPoint pt)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	return rc.PtInRect(pt) ? HTCAPTION : CBCGPDialog::OnNcHitTest(pt);
} 

// 鼠标左键双击STATIC文本框
void CMatrix428Dlg::OnStnDblclickStaticE428()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

 	CRect   pin; 
 	GetDlgItem(IDC_STATIC_E428)-> GetWindowRect(&pin); 
 	
	CPoint   pt; 
	GetCursorPos(&pt);
 
 	if(pin.PtInRect(pt)) 
	{
		if (m_bShowDirection == false)
		{
			m_bShowDirection = true;
			// 文本框纵向显示
			DlgVerticalShow();
			m_ToolBar.SetRows(5, TRUE, CRect(11,80,61,350));	// 纵向显示按键，CRect为工具条尺寸
		} 
		else
		{
			m_bShowDirection = false;
			// 文本框横向显示
			DlgHorizontalShow();
			m_ToolBar.SetRows(1, TRUE, CRect(80,11,350,61));	// 横向显示按键，CRect为工具条尺寸
		}

	}

}

void CMatrix428Dlg::DlgHorizontalShow(void)
{
	// 设置对话框尺寸
	CRect dlgRect,oldRect;
	dlgRect.left = 0;
	dlgRect.top= 0;
	dlgRect.right = 800;
	dlgRect.bottom = 76;
	GetWindowRect(oldRect);
	CWnd::SetWindowPos(NULL,oldRect.left,oldRect.top,dlgRect.Width(),dlgRect.Height(), SWP_NOZORDER);
	m_ToolBar.MoveWindow(CRect(80,11,350,61));

}

void CMatrix428Dlg::DlgVerticalShow(void)
{
	// 设置对话框尺寸
	CRect dlgRect,oldRect;
	dlgRect.left = 0;
	dlgRect.top= 0;
	dlgRect.right = 76;
	dlgRect.bottom = 800;
	GetWindowRect(oldRect);
	CWnd::SetWindowPos(NULL,oldRect.left,oldRect.top,dlgRect.Width(),dlgRect.Height(), SWP_NOZORDER);
	m_ToolBar.MoveWindow(CRect(11,80,61,350));
// 
// 	m_ToolBar.SetImageList(&m_ImageList);
// 	m_ToolBar.AddButtons(3,Buttons);
// 	m_ToolBar.AutoSize();
// 	m_ToolBar.RedrawWindow();
}


// 载入自定义工具栏
void CMatrix428Dlg::LoadToolBar(void)
{
	CBitmap bm;
	UINT Resource[5]={IDB_BITMAP1,IDB_BITMAP2,IDB_BITMAP3,IDB_BITMAP4,IDB_BITMAP5};          //位图ID数组
	m_ImageList.Create(48,48,ILC_COLOR24|ILC_MASK,0,0);              //创建Image List
	m_ToolBar.Create(TBSTYLE_FLAT |TBSTYLE_TOOLTIPS| CCS_NORESIZE | WS_CHILD | WS_VISIBLE | CCS_ADJUSTABLE,CRect(80,11,350,61),this,IDR_TOOLBAR);                    //创建Toolbar Control
	m_ToolBar.SetBitmapSize(CSize(48,48));

	int i =0;
	for(i=0;i<5;i++)
	{
		bm.LoadBitmap(Resource[i]);
		m_ImageList.Add(&bm,(CBitmap *)NULL);
		bm.Detach();
	}

	m_ToolBar.SetImageList(&m_ImageList);

	//定义TBBUTTON结构体数组
	CString str;

	for(i=0;i<5;i++)
	{
// 		str.LoadString(ID_BUTTON_LINE+i);                                               //在按键下方文字显示按键标签
// 		Buttons[i].iString=m_ToolBar.AddStrings(str);
		Buttons[i].dwData=0;
		Buttons[i].fsState=TBSTATE_ENABLED;
		Buttons[i].fsStyle=TBSTYLE_BUTTON;
		Buttons[i].iBitmap=i;
		Buttons[i].idCommand=ID_BUTTON_LINECLIENT+i;                                   //按钮命令响应
	}
	m_ToolBar.AddButtons(5,Buttons);
	m_ToolBar.AutoSize();

	m_ToolBar.ShowWindow(SW_SHOW);

}
BOOL CMatrix428Dlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class

	TOOLTIPTEXT*  ptt;

	ptt = (TOOLTIPTEXT*)lParam;

	CString Tip;

	switch(ptt->hdr.code)
	{
	case TTN_NEEDTEXT:

		switch(ptt->hdr.idFrom)
		{
		case ID_BUTTON_LINECLIENT:// ID_CURVE_OPEN为按钮ID

			Tip.LoadString(ID_BUTTON_LINECLIENT);

			break;
		case ID_BUTTON_LINESERVER:// ID_CURVE_OPEN为按钮ID

			Tip.LoadString(ID_BUTTON_LINESERVER);

			break;
		case ID_BUTTON_OPERATIONCLIENT:// ID_CURVE_OPEN为按钮ID

			Tip.LoadString(ID_BUTTON_OPERATIONCLIENT);

			break;
		case ID_BUTTON_OPERATIONSERVER:// ID_CURVE_OPEN为按钮ID

			Tip.LoadString(ID_BUTTON_OPERATIONSERVER);

			break;
		case ID_BUTTON_SIM:// ID_CURVE_OPEN为按钮ID

			Tip.LoadString(ID_BUTTON_SIM);

			break;
		}
		strcpy_s(ptt->szText,Tip);

		break;
	}

	return CBCGPDialog::OnNotify(wParam, lParam, pResult);

}
void CMatrix428Dlg::OnBnClickedButtonLineClient()
{
	char s1[500],s2[500],s3[1000];
	_tcscpy_s(s1,500, CurrentPath); //这个函数将string2 中的内容拷贝给string1。string2 可以是一个数组，一个指针，或一个字符串常量constant string。
	_stprintf_s(s2,500,_T("\\BCGPVisualStudioGUIDemo.exe"));
	_stprintf_s(s3,_T("%s%s"),s1,s2);
	// 	ShellExecute(this->m_hWnd,NULL,_T("C;\\Documents and Settings\\cxm\\桌面\\最终版光谱仪软件\\第二步\\SpectruMX\\release\\intelNirChem.exe"),NULL,NULL,SW_SHOWNORMAL);   

	//保存成ANSI格式的文件

	WinExec(s3,1);
}
void CMatrix428Dlg::OnBnClickedButtonLineServer()
{
	char s1[500],s2[500],s3[1000];
	_tcscpy_s(s1,500, CurrentPath); //这个函数将string2 中的内容拷贝给string1。string2 可以是一个数组，一个指针，或一个字符串常量constant string。
	_stprintf_s(s2,500,_T("\\TestSiteManage.exe"));
	_stprintf_s(s3,_T("%s%s"),s1,s2);
	// 	ShellExecute(this->m_hWnd,NULL,_T("C;\\Documents and Settings\\cxm\\桌面\\最终版光谱仪软件\\第二步\\SpectruMX\\release\\intelNirChem.exe"),NULL,NULL,SW_SHOWNORMAL);   

	//保存成ANSI格式的文件

	WinExec(s3,1);
}
void CMatrix428Dlg::OnBnClickedButtonOperationClient()
{
	char s1[500],s2[500],s3[1000];
	_tcscpy_s(s1,500, CurrentPath); //这个函数将string2 中的内容拷贝给string1。string2 可以是一个数组，一个指针，或一个字符串常量constant string。
	_stprintf_s(s2,500,_T("\\Operation.exe"));
	_stprintf_s(s3,_T("%s%s"),s1,s2);
	// 	ShellExecute(this->m_hWnd,NULL,_T("C;\\Documents and Settings\\cxm\\桌面\\最终版光谱仪软件\\第二步\\SpectruMX\\release\\intelNirChem.exe"),NULL,NULL,SW_SHOWNORMAL);   

	//保存成ANSI格式的文件

	WinExec(s3,1);
}
void CMatrix428Dlg::OnBnClickedButtonOperationServer()
{
	char s1[500],s2[500],s3[1000];
	_tcscpy_s(s1,500, CurrentPath); //这个函数将string2 中的内容拷贝给string1。string2 可以是一个数组，一个指针，或一个字符串常量constant string。
	_stprintf_s(s2,500,_T("\\OperationServer.exe"));
	_stprintf_s(s3,_T("%s%s"),s1,s2);
	// 	ShellExecute(this->m_hWnd,NULL,_T("C;\\Documents and Settings\\cxm\\桌面\\最终版光谱仪软件\\第二步\\SpectruMX\\release\\intelNirChem.exe"),NULL,NULL,SW_SHOWNORMAL);   

	//保存成ANSI格式的文件

	WinExec(s3,1);
}
void CMatrix428Dlg::OnBnClickedButtonSim()
{
	char s1[500],s2[500],s3[1000];
	_tcscpy_s(s1,500, CurrentPath); //这个函数将string2 中的内容拷贝给string1。string2 可以是一个数组，一个指针，或一个字符串常量constant string。
	_stprintf_s(s2,500,_T("\\SimInstrument.exe"));
	_stprintf_s(s3,_T("%s%s"),s1,s2);
	// 	ShellExecute(this->m_hWnd,NULL,_T("C;\\Documents and Settings\\cxm\\桌面\\最终版光谱仪软件\\第二步\\SpectruMX\\release\\intelNirChem.exe"),NULL,NULL,SW_SHOWNORMAL);   

	//保存成ANSI格式的文件

	WinExec(s3,1);
}