// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Operation.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CBCGPFrameWnd)

const int  iMaxUserToolbars		= 10;
const UINT uiFirstUserToolBarId	= AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId	= uiFirstUserToolBarId + iMaxUserToolbars - 1;

/** 将标题栏设置在居中位置，在前面添加空格*/
void MoveTitleToCenter(CWnd* pWnd)
{
	CString str;
	CString strOut = _T("");
	CDC* pDC = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(&rect);
	pWnd->GetWindowText(str);
	int iLength = str.GetLength();
	int iSpaceWidth = pDC->GetTextExtent(_T(" ")).cx;
	int iSpaceNum = rect.Width() / iSpaceWidth - iLength;
	iSpaceNum /= 2;
	for (int i=0; i<iSpaceNum; i++)
	{
		strOut += _T(" ");
	}
	strOut += str;
	pWnd->SetWindowText(strOut);
}

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_COMMAND_RANGE(ID_VPSHOT_FROM, ID_VPSHOT_FROM + ActiveSourceNumLimit * 3 - 1, &CMainFrame::OnSelectActiveSource)
	ON_MESSAGE(WM_MSG_CLIENT, &CMainFrame::OnMsgClient)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CONNECT, &CMainFrame::OnConnectServer)
	ON_UPDATE_COMMAND_UI(IDS_CONNECTSERVER_ICON, &CMainFrame::OnConnectServer)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, &CMainFrame::OnToolbarReset)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CONNECT,
	IDS_CONNECTSERVER_ICON,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bServerConnected = false;
	m_uiServerPort = ServerListenPort;
	m_strServerIP = ServerIP;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Menu will not take the focus on activation:
	CBCGPPopupMenu::SetForceMenuFocus (FALSE);

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}
	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	UINT uiToolbarHotID = bIsHighColor ? IDB_TOOLBAR256 : 0;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBarSetup.SetLockedSizes(CSize(16,16),CSize(16,16));
	uiToolbarHotID = bIsHighColor ? IDB_BITMAP_SETUPBAR256 : 0;
	if (!m_wndToolBarSetup.Create(this,
		WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D,
		ID_TOOLBAR_SETUP) ||
		!m_wndToolBarSetup.LoadToolBar (IDR_SETUPBAR, 0, 0, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	uiToolbarHotID = bIsHighColor ? IDB_BITMAP_VIEWBAR256 : 0;
	if (!m_wndToolBarView.Create(this,
		WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D,
		ID_TOOLBAR_VIEW) ||
		!m_wndToolBarView.LoadToolBar (IDR_VIEWBAR, 0, 0, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CBCGPToolBar::AddToolBarForImageCollection (IDR_VPVIEWBAR, 
		bIsHighColor ? IDB_BITMAP_VPVIEW256 : 0);


	if (!CreateStatusBar ())
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_imagesBarView.Create (IDB_BITMAP_VPVIEW256, 16, 0, RGB (255, 0, 255));

	CRect rectClient;
	GetClientRect(rectClient);
	if (!m_wndActiveSource.Create (_T("Active Source"), this, CRect (0, 0, rectClient.right / 2, rectClient.bottom / 5),
		TRUE, ID_VIEW_ACTIVESOURCEBAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Active Source bar\n");
		return -1;      // fail to create
	}
	m_wndActiveSource.SetIcon (m_imagesBarView.ExtractIcon (3), FALSE);
	m_wndActiveSource.LoadActiveSources();

	if (!m_wndActiveAcq.Create (_T("Active Acquisition"), this, CRect (rectClient.right / 2, 0, rectClient.right - 1, rectClient.bottom / 5),
		TRUE, ID_VIEW_ACTIVEACQBAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create All VP bar\n");
		return -1;      // fail to create
	}
	m_wndActiveAcq.SetIcon (m_imagesBarView.ExtractIcon (4), FALSE);
	m_wndActiveAcq.LoadAcqInfos();

	if (!m_wndVPToDo.Create (_T("VP To Do"), this, CRect (0, rectClient.bottom / 5, rectClient.right / 2, rectClient.bottom * 2 / 5),
		TRUE, ID_VIEW_VPTODOBAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create VP To Do bar\n");
		return -1;      // fail to create
	}
	m_wndVPToDo.SetIcon (m_imagesBarView.ExtractIcon (1), FALSE);
	m_wndVPToDo.LoadShotPoints();

	if (!m_wndVPDone.Create (_T("VP Done"), this, CRect (rectClient.right / 2, rectClient.bottom / 5, rectClient.right - 1, rectClient.bottom * 2 / 5),
		TRUE, ID_VIEW_VPDONEBAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create VP Done bar\n");
		return -1;      // fail to create
	}
	m_wndVPDone.SetIcon (m_imagesBarView.ExtractIcon (2), FALSE);
	m_wndVPDone.LoadShotPoints();

	if (!m_wndAllVP.Create (_T("All VP"), this, CRect (0, rectClient.bottom * 2 / 5, rectClient.right - 1, rectClient.bottom * 11 / 15),
		TRUE, ID_VIEW_ALLVPBAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create All VP bar\n");
		return -1;      // fail to create
	}
	m_wndAllVP.SetIcon (m_imagesBarView.ExtractIcon (0), FALSE);
	m_wndAllVP.LoadShotPoints();

	if (!m_wndOutput.Create (_T("Status Mail"), this, CSize (rectClient.right-1, rectClient.bottom/5),
		TRUE /* Has gripper */, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create Status Mail bar\n");
		return -1;      // fail to create
	}
	
	CString strToolbarTitle;
	strToolbarTitle.LoadString (IDS_MAIN_TOOLBAR);
	m_wndToolBar.SetWindowText (strToolbarTitle);

	strToolbarTitle.LoadString (IDS_SETUP_TOOLBAR);
	m_wndToolBarSetup.SetWindowText (strToolbarTitle);

	strToolbarTitle.LoadString (IDS_VIEW_TOOLBAR);
	m_wndToolBarView.SetWindowText (strToolbarTitle);

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarSetup.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndActiveSource.EnableDocking(CBRS_ALIGN_ANY);
	m_wndActiveAcq.EnableDocking(CBRS_ALIGN_ANY);
	m_wndAllVP.EnableDocking(CBRS_ALIGN_ANY);
	m_wndVPDone.EnableDocking(CBRS_ALIGN_ANY);
	m_wndVPToDo.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBarSetup);
	DockControlBarLeftOf(&m_wndToolBarView,&m_wndToolBarSetup);
	DockControlBarLeftOf(&m_wndToolBar,&m_wndToolBarView);
	DockControlBar(&m_wndActiveSource);
	DockControlBar(&m_wndVPToDo);
	DockControlBar(&m_wndAllVP);
 	m_wndActiveAcq.DockToWindow (&m_wndActiveSource, CBRS_ALIGN_RIGHT);
  	m_wndVPDone.DockToWindow (&m_wndVPToDo, CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndOutput);
	// 添加自定义工具栏
	m_wndToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	m_wndToolBarView.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	m_wndToolBarSetup.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	
	// Allow user-defined toolbars operations:
	InitUserToobars (NULL,
					uiFirstUserToolBarId,
					uiLastUserToolBarId);

	// Enable control bar context menu (list of bars + customize command):
	EnableControlBarMenu (	
		TRUE,				// Enable
		ID_VIEW_CUSTOMIZE, 	// Customize command ID
		_T("Customize..."),	// Customize command text
		ID_VIEW_TOOLBARS);	// Menu items with this ID will be replaced by
							// toolbars menu

	// enable VS 2010 look:
	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2010));
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
	// VISUAL_MANAGER
	RecalcLayout();
	ShowControlBar (&m_wndActiveAcq, FALSE, FALSE, TRUE);
	ShowControlBar (&m_wndVPToDo, FALSE, FALSE, TRUE);
	ShowControlBar (&m_wndVPDone, FALSE, FALSE, TRUE);
	// @@@端口和IP的命令行参数解析

	m_oComDll.OnInit(_T("MatrixCommDll.dll"), m_uiServerPort, m_strServerIP, this->m_hWnd);
	m_oComDll.m_oXMLDllOpt.OnInit(_T("MatrixServerDll.dll"));
	m_wndActiveAcq.m_pOptSetupData = m_oComDll.m_oXMLDllOpt.m_pOptSetupData;
	m_oComDll.m_hWnd = m_hWnd;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a customize toolbars dialog:
	//------------------------------------
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */);
	
	// 在自定义对话框命令中添加User
// 	pDlgCust->AddButton (_T("User"), CBCGPToolbarButton (ID_VIEW_USER_TOOLBAR1, 1, _T("User Tool 1"), TRUE));
// 	pDlgCust->AddButton (_T("User"), CBCGPToolbarButton (ID_VIEW_USER_TOOLBAR2, 2, _T("User Tool 2"), TRUE));
// 	pDlgCust->AddButton (_T("User"), CBCGPToolbarButton (ID_VIEW_USER_TOOLBAR3, 3, _T("User Tool 3"), TRUE));
// 
// 	pDlgCust->SetUserCategory (_T("User"));

	pDlgCust->EnableUserDefinedToolbars ();
	pDlgCust->Create ();
}

afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM wp,LPARAM)
{
	// TODO: reset toolbar with id = (UINT) wp to its initial state:

	UINT uiToolBarId = (UINT) wp;
	if (uiToolBarId != IDR_VIEWBAR)
	{
		return 0;
	}
	CMenu menuViews;
	menuViews.LoadMenu (IDR_POPUP_VPVIEW);
	m_wndToolBarView.ReplaceButton (ID_VIEW_VPSOURCEBAR,
		CBCGPToolbarMenuButton (ID_VIEW_VPSOURCEBAR, menuViews, 
		CImageHash::GetImageOfCommand (ID_VIEW_VPSOURCEBAR), NULL));
	return 0;
}
 // RIBBON_APP

BOOL CMainFrame::CreateStatusBar ()
{
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return FALSE;      // fail to create
	}
	if (m_bmpIconConnected.GetSafeHandle () == NULL)
	{
		HBITMAP hbmp = (HBITMAP) ::LoadImage (
			AfxGetResourceHandle (),
			MAKEINTRESOURCE(IDB_SERVER_CONNECTED),
			IMAGE_BITMAP,
			0, 0,
			LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
		m_bmpIconConnected.Attach (hbmp);
	}
	if (m_bmpIconDisConnected.GetSafeHandle () == NULL)
	{
		HBITMAP hbmp = (HBITMAP) ::LoadImage (
			AfxGetResourceHandle (),
			MAKEINTRESOURCE(IDB_SERVER_DISCONNECTED),
			IMAGE_BITMAP,
			0, 0,
			LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
		m_bmpIconDisConnected.Attach (hbmp);
	}
	return TRUE;
}

void CMainFrame::OnSelectActiveSource(UINT nMenuItemID)
{	
	int	nIndex;
	// 从VPAll的窗口右键菜单选择
	if(nMenuItemID >= ID_VPSHOT_FROM && nMenuItemID < (ID_VPSHOT_FROM + ActiveSourceNumLimit))
	{
		// 计算所选择的震源在网格中位置
		nIndex = nMenuItemID - ID_VPSHOT_FROM;
		return;
	}
	// 从VPToDo的窗口右键菜单选择
	if(nMenuItemID >= (ID_VPSHOT_FROM + ActiveSourceNumLimit) && nMenuItemID< (ID_VPSHOT_FROM + ActiveSourceNumLimit * 2))
	{
		// 计算所选择的震源在网格中位置
		nIndex = nMenuItemID - (ID_VPSHOT_FROM + ActiveSourceNumLimit);		
		return;
	}
	// 从VPDone的窗口右键菜单选择
	if(nMenuItemID >= (ID_VPSHOT_FROM + ActiveSourceNumLimit * 2) && nMenuItemID < (ID_VPSHOT_FROM + ActiveSourceNumLimit * 3))
	{
		// 计算所选择的震源在网格中位置
		nIndex = nMenuItemID - (ID_VPSHOT_FROM + ActiveSourceNumLimit * 2);		
		return;
	}
}
void CMainFrame::OnConnectServer(CCmdUI* pCmdUI)
{
	CClientDC dc (this);
	CString str = _T("");
	CString strTemp = _T("");
	CSize sz;
	int index = m_wndStatusBar.CommandToIndex(pCmdUI->m_nID);
	pCmdUI->Enable(TRUE);
	switch (pCmdUI->m_nID)
	{
	case ID_INDICATOR_CONNECT:
		str = _T("Connected to server ");
		str += m_strServerIP;
		strTemp.Format(_T(":%d"), m_uiServerPort);
		str += strTemp;
		//设置一个CClientDC对象来获取str的长度
		sz = dc.GetTextExtent(str, 33);
		//设置状态栏宽度
		m_wndStatusBar.SetPaneInfo(index, ID_INDICATOR_CONNECT, SBPS_NORMAL, sz.cx);
		//设置ID_INDICATOR_CONNECT字符串
		m_wndStatusBar.SetPaneText(index, str);
		break;
	case IDS_CONNECTSERVER_ICON:
		m_wndStatusBar.SetPaneInfo(index, IDS_CONNECTSERVER_ICON, SBPS_POPOUT, 0);
		if (m_bServerConnected)
		{
			m_wndStatusBar.SetPaneIcon (index, m_bmpIconConnected);
		}
		else
		{
			m_wndStatusBar.SetPaneIcon (index, m_bmpIconDisConnected);
		}
		break;
	default:
		break;
	}
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 关闭与客户端通讯连接
	m_oComDll.m_oXMLDllOpt.OnClose();
	m_oComDll.OnClose();
	CBCGPFrameWnd::OnClose();
}

LRESULT CMainFrame::OnMsgClient(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case CmdClientConnect:
		m_bServerConnected = true;
		m_wndOutput.AppendLog(_T("Connect to Server!"));
		// 查询 OperationDelay XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryDelayOptXMLInfo, NULL, 0);
		// 查询 炮表 XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQuerySourceShotOptXMLInfo, NULL, 0);
		// 查询 Explo震源类型 XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryExploOptXMLInfo, NULL, 0);
		// 查询 Vibro震源类型 XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryVibroOptXMLInfo, NULL, 0);
		// 查询 ProcessRecord XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryProcessRecordOptXMLInfo, NULL, 0);
		// 查询 ProcessAux XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryProcessAuxOptXMLInfo, NULL, 0);
		// 查询 ProcessAcq XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryProcessAcqOptXMLInfo, NULL, 0);
		// 查询 ProcessType XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryProcessTypeOptXMLInfo, NULL, 0);
		// 查询 注释 XML文件信息（帧内容为空）
		m_oComDll.m_pCommClient->m_oSndFrame.MakeSetFrame(CmdQueryCommentsOptXMLInfo, NULL, 0);
		break;
	case CmdSetDelayOptXMLInfo:
		m_wndOutput.AppendLog(_T("Receive the information of delay setup !"));
		break;
	case CmdSetSourceShotOptXMLInfo:
		m_wndOutput.AppendLog(_T("Receive the information of source shot setup !"));
		break;
	case CmdSetExploOptXMLInfo:
		m_wndOutput.AppendLog(_T("Receive the information of explo setup !"));
		break;
	case CmdSetVibroOptXMLInfo:
		m_wndOutput.AppendLog(_T("Receive the information of vibro setup !"));
		break;
	case CmdSetProcessRecordOptXMLInfo:
		break;
	case CmdSetProcessAuxOptXMLInfo:
		break;
	case CmdSetProcessAcqOptXMLInfo:
		m_wndActiveAcq.m_bReload = true;
		break;
	case CmdSetProcessTypeOptXMLInfo:
		m_wndOutput.AppendLog(_T("Receive the information of process setup !"));
		break;
	case CmdSetCommentsOptXMLInfo:
		m_wndOutput.AppendLog(_T("Receive the information of comments setup !"));
		break;
	default:
		break;
	}

	return TRUE;
}