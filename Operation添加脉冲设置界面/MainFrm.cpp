// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Operation.h"
#include "MainFrm.h"
#include "OperationParseXML.h"
#include "OperationDoc.h"
#include "OperationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_REGISTERED_MESSAGE(BCGM_TOOLBARMENU, OnToolbarContextMenu)
	ON_COMMAND(ID_VIEW_WORKSPACE, OnViewWorkspace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, OnUpdateViewWorkspace)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_CARBON, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_CARBON, OnUpdateAppLook)
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_ACTIVESOURCE, &CMainFrame::OnViewActivesource)
	ON_COMMAND(ID_VIEW_ALLVP, &CMainFrame::OnViewAllvp)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALLVP, &CMainFrame::OnUpdateViewAllvp)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ACTIVESOURCE, &CMainFrame::OnUpdateViewActivesource)
	ON_COMMAND(ID_VIEW_ALLDONE, &CMainFrame::OnViewAlldone)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALLDONE, &CMainFrame::OnUpdateViewAlldone)
	ON_COMMAND(ID_VIEW_ALLTODO, &CMainFrame::OnViewAlltodo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALLTODO, &CMainFrame::OnUpdateViewAlltodo)		
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_VPSHOT_ONE, ID_VPDONESHOT_TEN, OnShotPoint)
	ON_COMMAND(ID_SETUP_SOURCE, &CMainFrame::OnSetupSource)
	ON_COMMAND(ID_SETUP_OPERATION, &CMainFrame::OnSetupOperation)
	ON_COMMAND(ID_SETUP_COMMENTS, &CMainFrame::OnSetupComments)
	ON_COMMAND(ID_SETUP_PROCESSTYPE, &CMainFrame::OnSetupProcesstype)// vivi 2010.11.8
END_MESSAGE_MAP()

//ON_MESSAGE(WM_MSG_SHOT,OnStartShot)
static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
: m_pOperationSvrSock(NULL)
{
	// TODO: 在此添加成员初始化代码
	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_2007_1);
	
}

CMainFrame::~CMainFrame()
{
	/*
	int i,nCount = m_pwndActiveSource.GetCount();
	for (i=0;i<nCount;i++)
	{
		delete m_pwndActiveSource[i];
	}
	m_pwndActiveSource.RemoveAll();
	
	int i,nCount = m_arrActiveShot.GetCount();
	for (i=0;i<nCount;i++)
	{
		delete m_arrActiveShot[i];
		m_arrActiveShot[i] = NULL;
	}
	m_arrActiveShot.RemoveAll();
	*/
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	OnAppLook (m_nAppLook);

	CBCGPToolBar::EnableQuickCustomization ();
	CList<UINT, UINT>	lstBasicCommands;

	lstBasicCommands.AddTail (ID_VIEW_TOOLBARS);
	lstBasicCommands.AddTail (ID_FILE_NEW);
	lstBasicCommands.AddTail (ID_FILE_OPEN);
	lstBasicCommands.AddTail (ID_FILE_SAVE);
	lstBasicCommands.AddTail (ID_FILE_PRINT);
	lstBasicCommands.AddTail (ID_APP_EXIT);
	lstBasicCommands.AddTail (ID_EDIT_CUT);
	lstBasicCommands.AddTail (ID_EDIT_PASTE);
	lstBasicCommands.AddTail (ID_EDIT_UNDO);
	lstBasicCommands.AddTail (ID_APP_ABOUT);
	lstBasicCommands.AddTail (ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail (ID_VIEW_CUSTOMIZE);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2000);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_XP);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2003);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2005);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2008);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_WIN_XP);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_1);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_2);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_3);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_4);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_CARBON);

	CBCGPToolBar::SetBasicCommands (lstBasicCommands);

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}
	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);


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
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndActiveSource.EnableDocking(CBRS_ALIGN_ANY);//CBRS_ALIGN_BOTTOM	
	m_wndVPAll.EnableDocking(CBRS_ALIGN_ANY);
	m_wndVPToDo.EnableDocking(CBRS_ALIGN_ANY);
	m_wndVPDone.EnableDocking(CBRS_ALIGN_ANY);
	
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndActiveSource);
	DockControlBar(&m_wndVPAll);
	DockControlBar(&m_wndVPToDo);
	DockControlBar(&m_wndVPDone);
	
	//DockControlBar(&m_wndActiveSource,AFX_IDW_DOCKBAR_BOTTOM);

	/*m_wndVPAll.DockToWindow (this, CBRS_ALIGN_TOP);*/


	/*

	CBCGPTabbedControlBar* pTabbedBar = NULL; 
	m_wndActiveSource.AttachToTabWnd (&m_wndVPAll, BCGP_DM_SHOW, TRUE, 
		(CBCGPDockingControlBar**) &pTabbedBar);

// 	m_wndVPAll.AttachToTabWnd (pTabbedBar, BCGP_DM_SHOW, TRUE, 
// 		(CBCGPDockingControlBar**) &pTabbedBar);

	pTabbedBar->GetUnderlinedWindow ()->EnableTabDetach (1, FALSE);
*/
	// 创建客户端socket
	CreateShotSvrSocket();
	ParseXMLFile();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序
void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a customize toolbars dialog:
	//------------------------------------
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */
		);

	pDlgCust->Create ();
}

LRESULT CMainFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
{
	CPoint point (BCG_GET_X_LPARAM(lp), BCG_GET_Y_LPARAM(lp));

	CMenu menu;
	VERIFY(menu.LoadMenu (IDR_POPUP_TOOLBAR));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
	pPopupMenu->Create (this, point.x, point.y, pPopup->Detach ());

	return 0;
}

afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM wp, LPARAM)
{
	UINT uiToolBarId = (UINT) wp;
	if (uiToolBarId == IDR_MAINFRAME)
	{
//		CBCGPToolbarButton buttonUpdate (ID_VIEW_UPDATEALLVIEWS, -1, _T("&Update All Views"));
//		m_wndToolBar.InsertButton (buttonUpdate, 3);
	}

	return 0;
}

BOOL CMainFrame::OnShowPopupMenu (CBCGPPopupMenu* pMenuPopup)
{
	//---------------------------------------------------------
	// Replace ID_VIEW_TOOLBARS menu item to the toolbars list:
	//---------------------------------------------------------

	CFrameWnd::OnShowPopupMenu (pMenuPopup);

	if (pMenuPopup != NULL &&
		pMenuPopup->GetMenuBar ()->CommandToIndex (ID_VIEW_TOOLBARS) >= 0)
	{
		if (CBCGPToolBar::IsCustomizeMode ())
		{
			//----------------------------------------------------
			// Don't show toolbars list in the cuztomization mode!
			//----------------------------------------------------
			return FALSE;
		}

		pMenuPopup->RemoveAllItems ();

		CMenu menu;
		VERIFY(menu.LoadMenu (IDR_POPUP_TOOLBAR));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		pMenuPopup->GetMenuBar ()->ImportFromMenu (*pPopup, TRUE);
	}
/**/
	return TRUE;
}

void CMainFrame::OnViewWorkspace() 
{
	/*
	ShowControlBar (&m_wndWorkSpace,
		!(m_wndWorkSpace.IsVisible ()),
		FALSE, TRUE);
	ShowControlBar (&m_wndGridWorkSpace,
		!(m_wndGridWorkSpace.IsVisible ()),
		FALSE, TRUE);
	RecalcLayout ();
	*/
}

void CMainFrame::OnUpdateViewWorkspace(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetCheck (m_wndWorkSpace.IsVisible ());
}

void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	m_nAppLook = id;

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	switch (m_nAppLook)
	{
	case ID_VIEW_APPLOOK_2000:
		// enable Office 2000 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager));
		break;

	case ID_VIEW_APPLOOK_XP:
		// enable Office XP look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		// enable Windows XP look (in other OS Office XP look will be used):
		CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case ID_VIEW_APPLOOK_2003:
		// enable Office 2003 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2005:
		// enable VS 2005 Look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));
		CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D_ROUNDED;
		CBCGPVisualManager::GetInstance ();
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2008:
		// enable VS 2008 Look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));
		CBCGPVisualManager::GetInstance ();
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_1:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_2:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_4:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_CARBON:
		// enable Carbon Look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerCarbon));
		CBCGPVisualManager::GetInstance ();
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	default:
		ASSERT (FALSE);
	}

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (!CFrameWnd::OnCreateClient(lpcs, pContext))
	{
		return FALSE;
	}
/*
	m_wndWorkSpace.SetContext (pContext);

	if (!m_wndWorkSpace.Create (_T("View  1"), this, CRect (0, 0, 300, 300),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar\n");
		return FALSE;      // fail to create
	}
*/
	m_wndActiveSource.SetContext (pContext);
	if (!m_wndActiveSource.Create (_T("Active Source"), this, CRect (0, 0, 1024, 200),
		TRUE, ID_VIEW_WORKSPACE+10,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create ActiveSource Workspace bar\n");
		return FALSE;      // fail to create
	}

	m_wndVPAll.SetContext (pContext);
	if (!m_wndVPAll.Create (_T("All VP"), this, CRect (0, 201, 1000, 400),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP  | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create VPAll Workspace bar\n");
		return FALSE;      // fail to create
	}

	m_wndVPToDo.SetContext (pContext);
	if (!m_wndVPToDo.Create (_T("VP To Do"), this, CRect (0, 401, 1024, 600),
		TRUE, ID_VIEW_WORKSPACE+2,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP  | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create VP To Do Workspace bar\n");
		return FALSE;      // fail to create
	}

	m_wndVPDone.SetContext (pContext);
	if (!m_wndVPDone.Create (_T("VP Done"), this, CRect (0, 601, 1024, 800),
		TRUE, ID_VIEW_WORKSPACE+3,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP  | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create VP Done Workspace bar\n");
		return FALSE;      // fail to create
	}
	
	

	return TRUE;
}

void CMainFrame::OnDestroy()
{
	CBCGPFrameWnd::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if(m_pOperationSvrSock)
	{
		m_pOperationSvrSock->Close();
		delete m_pOperationSvrSock;
		m_pOperationSvrSock = NULL;
	}
	// 释放所有的采集道对象
	m_ChannelList.OnClose();

}
/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 写日志信息 ，

    修改历史：

*******************************************************************************/
void CMainFrame::WriteLog(LPCTSTR strLog, int nType)
{
	COperationView *pView = (COperationView*)GetActiveView();
	pView->m_OperationLog.WriteLog(strLog, nType);
}

/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能：  解析XML配置文件，读入放炮表、震源、处理类型、采集排列
 
    修改历史：

*******************************************************************************/
bool CMainFrame::ParseXMLFile(void)
{
	COperationParseXML		OperaXML;
	// 解析系统配置：IP地址、端口等
	OperaXML.ParseClientSetup(_T("Matrix_Operation.xml"));
	// 采集站配置信息
	OperaXML.ParseFDUConfig(_T("MatrixConfiguration.xml"),&m_FDUConfig);
	// 解析放炮表
	OperaXML.ParseOperationTable(_T("Matrix_Operation.xml"),&m_AllShotPoints);	
	m_wndVPAll.LoadShotPoints(&m_AllShotPoints);
	m_wndVPToDo.LoadShotPoints(&m_AllShotPoints);
	m_wndVPDone.LoadShotPoints(&m_AllShotPoints);
	// 解析震源
	OperaXML.ParseSourceType(_T("Matrix_Operation.xml"),&m_AllShotSrcs);
	m_wndActiveSource.LoadShotSources(&m_AllShotSrcs);
	// 解析采集排列
	// OperaXML.ParseShotSpread(_T("Matrix_Operation.xml"),&m_AllSpreads);
	// 解析处理类型
	OperaXML.ParseProcessType(_T("Matrix_Operation.xml"),&m_AllProcesses);
	// 读取采集排列
	m_ChannelList.OnInit();
	m_ChannelList.LoadSiteData();
	OperaXML.ParseShotSpread(_T("Matrix_Operation.xml"),&m_AllSpreads,&m_ChannelList);
	return false;
}

/******************************************************************************
    函    数：CreateShotSvrSocket
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 创建与施工服务器通信的socket

    修改历史：

*******************************************************************************/
bool CMainFrame::CreateShotSvrSocket(void)
{
	if(m_pOperationSvrSock)
	{
		m_pOperationSvrSock->Close();
		delete m_pOperationSvrSock;
		m_pOperationSvrSock = NULL;
	}
	m_pOperationSvrSock = new CShotServerSocket;
	//m_pOperationSvrSock->m_pWnd = this;
	// 创建socket
	if(!m_pOperationSvrSock->Create(OPERATION_CLIENTPORT,SOCK_DGRAM,theApp.m_strHostIP))
	{
		delete m_pOperationSvrSock;
		m_pOperationSvrSock =NULL;
		WriteLog(_T("Create Client Socket Error!"));
		return false;
	}
	// 设置接收缓冲区大小
	int RevBuf = 2048*1024;
	if (m_pOperationSvrSock->SetSockOpt( SO_RCVBUF, (char*)&RevBuf, sizeof(RevBuf)) == SOCKET_ERROR) 
	{
		m_pOperationSvrSock->Close();
		delete m_pOperationSvrSock;
		m_pOperationSvrSock = NULL;
		return false;
	}
	// 设置发送缓冲区大小
	if (m_pOperationSvrSock->SetSockOpt(SO_SNDBUF, (char*)&RevBuf, sizeof(RevBuf)) == SOCKET_ERROR) 
	{
		m_pOperationSvrSock->Close();
		delete m_pOperationSvrSock;
		m_pOperationSvrSock = NULL;
		return false;
	}
	// 设置允许接受广播信息
	RevBuf = 1;
	if (m_pOperationSvrSock->SetSockOpt(SO_BROADCAST, (char*)&RevBuf, sizeof(RevBuf)) == SOCKET_ERROR) 
	{
		m_pOperationSvrSock->Close();
		delete m_pOperationSvrSock;
		m_pOperationSvrSock = NULL;
		return false;
	}
	return true;
}
/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 增加一个

    修改历史：

*******************************************************************************/

void CMainFrame::OnViewActivesource()
{
	// TODO: 在此添加命令处理程序代码
	/*
	CActiveSourceWnd *pWnd = new CActiveSourceWnd;
	if(!pWnd)
		return;
	if (!pWnd->Create (_T("Active Source"), this, CRect (0, 600, 900, 900),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create ActiveSource Workspace bar\n");
		return ;      // fail to create
	}

	pWnd->EnableDocking(CBRS_ALIGN_ANY);	
	DockControlBar(pWnd);	
	m_pwndActiveSource.Add(pWnd);*/
	
	ShowControlBar (&m_wndActiveSource,	!(m_wndActiveSource.IsVisible ()),	FALSE, TRUE);
}
void CMainFrame::OnUpdateViewActivesource(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_wndActiveSource.IsVisible ());
}

void CMainFrame::OnViewAllvp()
{
	// TODO: 在此添加命令处理程序代码
	ShowControlBar (&m_wndVPAll, !(m_wndVPAll.IsVisible ()),	FALSE, TRUE);
}

void CMainFrame::OnUpdateViewAllvp(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_wndVPAll.IsVisible ());
}

void CMainFrame::OnViewAlldone()
{
	// TODO: 在此添加命令处理程序代码
	ShowControlBar (&m_wndVPDone, !(m_wndVPDone.IsVisible ()),	FALSE, TRUE);
}

void CMainFrame::OnUpdateViewAlldone(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_wndVPDone.IsVisible ());
}

void CMainFrame::OnViewAlltodo()
{
	// TODO: 在此添加命令处理程序代码
	ShowControlBar (&m_wndVPToDo, !(m_wndVPToDo.IsVisible ()),	FALSE, TRUE);
	/*
	CActiveShot *pShot = new CActiveShot;
	pShot->SetShotPoint(m_AllShotPoints.GetShotPoint(0),m_AllSpreads.GetShotSpread(0),m_AllProcesses.GetProcessType(0));
	pShot->SetShotSource( m_AllShotSrcs.GetShotSource(0));
	
	m_arrActiveShot.Add(pShot);
	pShot->m_pSvrSocket= m_pOperationSvrSock;
	pShot->m_dwServerIP = inet_addr(("192.168.0.255"));
	pShot->m_wServerPort = OPERATION_SVRPORT;
	pShot->SendRequestShotService();
	*/
}

void CMainFrame::OnUpdateViewAlltodo(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_wndVPToDo.IsVisible ());
}
/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 设置炮点的状态，需要更新的状态包括：
	           1、m_AllShotPoints中保存的对象；
			   2、m_wndVPAll中网格显示；



    修改历史：

*******************************************************************************/
int CMainFrame::SetShotPointStatus(CShotPoint* pShotPoint,unsigned char byVPState)
{	
	// int i,nCount;
	if(pShotPoint)
	{
		pShotPoint->m_VPState = byVPState;
	}
	// 更新All VP 窗口
	m_wndVPAll.SetShotPointState(pShotPoint,byVPState);
	// 判断是否已经放炮完成
	if(byVPState == VP_DONE)
	{
		m_wndVPDone.SetShotPointState(pShotPoint,VP_DONE);
	}
	return 0;
}
int CMainFrame::SetShotSourceStatus(CShotSource* pShotSource,unsigned char byVPState)
{
	return 0;
}
/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 通过服务器ID和线程ID号查找当前激活的放炮对象。

    修改历史：

*******************************************************************************/
int   CMainFrame::LookforActiveShotByID(DWORD dwServerID,DWORD dwThreadID)
{
	CActiveShot *pShot=NULL;
	int i,nCount;
	nCount = m_AllActiveShots.GetCount();
	for (i=0;i<nCount;i++)
	{
		pShot = m_AllActiveShots.GetActiveShot(i);
		if(pShot->m_dwServerID==dwServerID && pShot->m_dwThreadID==dwThreadID)
			return i;
	}
	return -1;
}

/**
 * @brief 通过编号寻找激活的放炮对象
 * @note  通过炮点编号和震源编号寻找激活的放炮对象
 * @param DWORD dwShotNb，炮点编号
 * @param DWORD dwSourceNb，震源编号
 * @return 成功返回放炮对象指针，失败则返回NULL
 */
CActiveShot*  CMainFrame::LookforActiveShotByNb(DWORD dwShotNb,DWORD dwSourceNb)
{
	CActiveShot *pShot=NULL;
	int i,nCount;
	nCount = m_AllActiveShots.GetCount();
	for (i=0;i<nCount;i++)
	{
		pShot = m_AllActiveShots.GetActiveShot(i);
		if(pShot->IsActiveShot(dwShotNb,dwSourceNb))
			return pShot;
	}
	return NULL;
}
/******************************************************************************
    函    数： AnalysisProtocolOfServerSock
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 解析从服务器接受的数据,被CShotServerSocket::OnReceive函数调用

			数据帧格式：
			帧头（2Byte）＋帧长度（2Byte）  ＋帧计数（4Byte）＋帧类型（1Byte）			   
			＋命令(2Byte)＋数据个数（2Byte）＋数据区（变长） ＋校验位（1Byte）＋帧尾（2Byte）


    修改历史：

*******************************************************************************/
void CMainFrame::AnalysisProtocolOfServerSock(void)
{
	unsigned char	pBuf[2048];
	WORD		wFrmLen;			// 帧长
	WORD		wFrmCmdCode;		// 命令码
	DWORD		dwFrmIndex;			// 帧计数
	DWORD		dwNb1;				// 炮号
	DWORD		dwNb2;				// 震源号
	int			nSize,nReadSize;
	CString		strTemp;

	if(!m_pOperationSvrSock)
		return;

	sockaddr_in sockShotServer;
	sockShotServer.sin_family = AF_INET;
	nSize = sizeof(sockShotServer);
	// 接收数据,nReadSize为接收到数据字节数
	nReadSize = m_pOperationSvrSock->ReceiveFrom( pBuf, 2048, (SOCKADDR*)&sockShotServer , &nSize );
	switch (nReadSize)
	{
	case 0:		// 没有收到数据 
		return;
		break;
	case SOCKET_ERROR:	// 发生错误，记录日志
		if (GetLastError() != WSAEWOULDBLOCK) 
		{  //发生socket错误 
			return;
		}
		break;
	default:
		break;
	}
	// -----------解析帧格式---------------------------------------------------------
	// 判断帧头是否0xEB, 0x90
	if (pBuf[0]!=0xEB || pBuf[1]!=0x90)
	{
		WriteLog(_T("Error Frame Head!"),3);
		return;
	}
	// 计算帧长度
	wFrmLen = MAKEWORD(pBuf[2],pBuf[3]);
	if(wFrmLen>2048)
	{	// 帧长度超长，发生错误记录日志	
		WriteLog(_T("Error Frame Length!"),3);
		return;
	}
	// 计算帧计数
	memcpy_s(&dwFrmIndex,sizeof(dwFrmIndex),pBuf+4 , 4);	
	// 帧尾
	if(pBuf[wFrmLen-2]!=0x14 || pBuf[wFrmLen-1]!=0x6F)
	{
		// ??????帧尾错误，需要重新定位帧头
		WriteLog(_T("Error Frame Tail!"),3);
		return;
	}
	// 命令码
	wFrmCmdCode=MAKEWORD(pBuf[9],pBuf[10]);
	// 可能是炮号和震源号，也可能是通信ID和线程ID
	memcpy_s(&dwNb1,4,pBuf+13,4);
	memcpy_s(&dwNb2,4,pBuf+17,4);
	// 当前激活的放炮对象	
	CActiveShot *pActiveShot=NULL;
	switch(wFrmCmdCode)
	{
	case OPERATION_CMD_REQUEST:	// 收到放炮请求的应答
		{
			
			pActiveShot = LookforActiveShotByNb(dwNb1,dwNb2);// ||pActiveShot->m_dwServerIP != inet_addr(("255.255.255.255"))
			if(!pActiveShot )
			{	// 已经接收到服务器的应答，其他服务器不回令。
				strTemp.Format(_T("Error Shot Nb# %d or Source Nb# %d ."),dwNb1,dwNb2);
				WriteLog(strTemp);
				return;
			}
			if( pActiveShot->m_dwServerID)
			{
				strTemp.Format(_T("Error Shot Nb# %d ,Source Nb# %d : Thread ID# %d already selected."),dwNb1,dwNb2,pActiveShot->m_dwServerID);
				WriteLog(strTemp);
				return;
			}
			// 保存服务器的IP地址和端口，此时可能有多个服务器应答，只能保存最后一个
			pActiveShot->m_dwServerIP = sockShotServer.sin_addr.s_addr;
			pActiveShot->m_wServerPort = ntohs(sockShotServer.sin_port);
			KillTimer(pActiveShot->m_dwTimerID);
			// 发送确认命令
			pBuf[4] = 0x01;								// 帧计数
			pBuf[8] = 0x01;								// 帧类型
			pBuf[9] = LOBYTE(OPERATION_CMD_CONFIRM);	// 命令码低位
			pBuf[10]= HIBYTE(OPERATION_CMD_CONFIRM);	// 命令码高位
			pActiveShot->SendToServer(pBuf,nReadSize);
			strTemp.Format(_T("Request Acked! Shot Nb# %d , Source Nb# %d."),dwNb1,dwNb2);
			WriteLog(strTemp);			
			return;			
		}
		break;
	case OPERATION_CMD_CONFIRM: // 找到施工服务及线程，下发数据
		{	// 根据炮点编号和震源编号查找当前激活的放炮对象
			pActiveShot = LookforActiveShotByNb(dwNb1,dwNb2);
			if(!pActiveShot )
			{
				strTemp.Format(_T("Error Shot Nb# %d or Source Nb# %d ."),dwNb1,dwNb2);
				WriteLog(strTemp);
				return;
			}
			if( pActiveShot->m_dwServerID)
			{
				strTemp.Format(_T("Error Shot Nb# %d ,Source Nb# %d : Thread ID# %d already selected."),dwNb1,dwNb2,pActiveShot->m_dwServerID);
				WriteLog(strTemp);
				return;
			}
			// 服务器提供的通信ID
			memcpy_s(&pActiveShot->m_dwServerID,4,pBuf+25,4);		
			// 线程ID
			memcpy_s(&pActiveShot->m_dwThreadID,4,pBuf+29,4);
			// 保存服务器的IP地址和端口，此时保存第一个确认应答的服务器地址
			pActiveShot->m_dwServerIP = sockShotServer.sin_addr.s_addr;
			pActiveShot->m_wServerPort = ntohs(sockShotServer.sin_port);
			// 发送炮号及 震源
			pActiveShot->SendShotPointAndSource();
			// 发送处理类型对象
			pActiveShot->SendProcessType();
			// 发送采集站配置信息
			pActiveShot->SendFDUConfig(&m_FDUConfig);
			// 发送采集排列
			pActiveShot->SendShotSpreadLabel();
			pActiveShot->SendShotSpreadChannel();
			// 启动放炮
			pActiveShot->SendGeneralCMD(OPERATION_CMD_START);
			strTemp.Format(_T("Confirm Acked! Thread ID# %d. "),pActiveShot->m_dwThreadID);
			WriteLog(strTemp);
			return;			
		}
		break;
	case OPERATION_ACK_NAME:		// 收到采集站点名结果
		{
			// 根据通信ID（ServerID）和线程编号（ThreadID）查找当前激活的放炮对象
			nSize=LookforActiveShotByID(dwNb1,dwNb2);
			if(nSize<0)
			{
				WriteLog(_T("Error ServerID or Thread ID!"));
				return;
			}
			pActiveShot = m_AllActiveShots.GetActiveShot(nSize);
			// 解析出无应答的个数
			memcpy_s(&dwNb1,4,pBuf+21,4);
			// 采集排列总数
			dwNb2 = pActiveShot->m_pSpread->GetCount();			
			
			strTemp.Format(_T("Spread Count %d, No Acked Count %d"),dwNb2, dwNb1);
			WriteLog(strTemp);
			// 采集排列和应答的道数相等则直接刷新界面，准备放炮
			if(dwNb1 == 0)
			{
				// 设置状态，刷新显示界面
				m_wndActiveSource.LoadShotPointBySource(pActiveShot->m_pShotSource, pActiveShot->m_pShotPoint);
				return;
			}
			strTemp+=_T(",Continue？");
			// 采集排列和应答的道数不相等，则询问用户进行选择
			if(MessageBox(strTemp,_T("Caution"),MB_YESNO|MB_ICONQUESTION)==IDYES)
			{	// 确认放炮，将炮点属性添加到激活震源属性表中
				m_wndActiveSource.LoadShotPointBySource(pActiveShot->m_pShotSource, pActiveShot->m_pShotPoint);
			}
			else
			{
				// 通知服务器，取消此次放炮
				pActiveShot->SendGeneralCMD(OPERATION_CMD_OVER);
				// 删除激活的放炮对象
				delete pActiveShot;
				pActiveShot = NULL;
				m_AllActiveShots.RemoveAt(nSize);
				strTemp.Format(_T("Spread Count %d, No Acked Count %d, Cancel Shot !"),dwNb2, dwNb1);
				WriteLog(strTemp);
				return;
			}
		}
		break;
	case OPERATION_CMD_SRCRDY:
		{
			// 收到爆炸机控制器准备好信号: 应该先更新震源的图标，再判断该震源是否分配炮号，如果分配则更新炮号的图标。
			memcpy_s(&dwFrmIndex,4,pBuf+21,4);	// 震源编号

			/*
			nSize=LookforActiveShotByID(dwNb1,dwNb2);
			if(nSize<0)
			{
				WriteLog(_T("Error ServerID or Thread ID!"));
				return;
			}
			pActiveShot = m_arrActiveShot[nSize];
			*/

		}
		break;
	case OPERATION_CMD_RCVING:	// 开始数据采样
		{
			strTemp.Format(_T("Start Sampling：Server ID# %d, Thread ID# %d"),dwNb1,dwNb2);			
			WriteLog(strTemp);
		}
		break;
	case OPERATION_CMD_RCVED:   // 数据采样结束
		{
			strTemp.Format(_T("Sampling End：Server ID# %d, Thread ID# %d"),dwNb1,dwNb2);
			WriteLog(strTemp);
		 }
		break;
	case OPERATION_CMD_PROCING:	// 数据处理开始
		break;
	case OPERATION_CMD_PROCED:	// 数据处理结束
		break;
	case OPERATION_CMD_OUTING:	// 输出开始
		break;
	case OPERATION_CMD_OUTED:	// 输出结束
		break;
	default:	// 命令码错误
		{
			strTemp.Format(_T("Error Command Code: %d"),wFrmCmdCode);
			WriteLog(strTemp);
		}
		break;
	}
	
}
/**
 * @brief 响应鼠标右键放炮的消息
 * @note  用户在放炮表的窗口点击鼠标右键后，根据震源动态创建右击菜单的内容，用户选择震源进行放炮。
 * @note  在 VP All窗口， 对应的菜单 从 ID_VPSHOT_ONE 到 ID_VPSHOT_TEN
 * @note  在 VP TODO窗口，对应的菜单从 ID_VPTODOSHOT_ONE 到 ID_VPTODOSHOT_TEN  
 * @note  在 VP DONE窗口，对应的菜单从 ID_VPDONESHOT_ONE 到 ID_VPDONESHOT_TEN  
 * @param  UINT id, 菜单的ID号
 * @return void
 */
void CMainFrame::OnShotPoint(UINT nMenuItemID)
{	
	int				nIndex;
	DWORD			dwShotNb;
	CShotSource*	pShotSource = NULL;
	
	if(nMenuItemID>=ID_VPSHOT_ONE && nMenuItemID<=ID_VPSHOT_TEN)
	{
		if(!m_wndVPAll.GetActiveShotPointNb(&dwShotNb))
		{
			return;
		}
		// 计算所选择的震源在网格中位置
		nIndex = nMenuItemID-ID_VPSHOT_ONE;
		if(nIndex>m_AllShotSrcs.GetCount())
			return;
		// 获得震源编号
		pShotSource=m_AllShotSrcs.GetShotSource(nIndex);
		if(pShotSource==NULL)
			return;
		// 准备放炮
		OnStartShot(dwShotNb, pShotSource->m_dwSourceNb);				
		return;
	}
	if(nMenuItemID>=ID_VPTODOSHOT_ONE && nMenuItemID<=ID_VPTODOSHOT_ONE)
	{
		if(!m_wndVPToDo.GetActiveShotPointNb(&dwShotNb))
		{
			return;
		}
		// 计算所选择的震源在网格中位置
		nIndex = nMenuItemID-ID_VPTODOSHOT_ONE;
		if(nIndex>m_AllShotSrcs.GetCount())
			return;
		// 获得震源编号
		pShotSource = m_AllShotSrcs.GetShotSource(nIndex);
		if(pShotSource==NULL)
			return;
		// 准备放炮
		OnStartShot(dwShotNb, pShotSource->m_dwSourceNb);					
		return;
	}
	if(nMenuItemID>=ID_VPDONESHOT_ONE && nMenuItemID<=ID_VPDONESHOT_TEN)
	{
		if(!m_wndVPDone.GetActiveShotPointNb(&dwShotNb))
		{
			return;
		}
		// 计算所选择的震源在网格中位置
		nIndex = nMenuItemID-ID_VPDONESHOT_ONE;
		if(nIndex>m_AllShotSrcs.GetCount())
			return;
		// 获得震源编号
		pShotSource = m_AllShotSrcs.GetShotSource(nIndex);
		if(pShotSource==NULL)
			return;
		// 准备放炮
		OnStartShot(dwShotNb, pShotSource->m_dwSourceNb);					
		return;
	}
	
}
/******************************************************************************************
    函    数：
    
    输    入：DWORD dwShotNb, DWORD dwSourceNb
    
    输    出：
  
    返 回 值：
    
    功    能：

			   一般是用户在界面上点击右击菜单，选择某一炮号或者震源开始放炮，发送该消息；

			   或者在自动放炮模式下，由系统自动判断放炮。

    修改历史：

*******************************************************************************************/
bool CMainFrame::OnStartShot( DWORD dwShotNb, DWORD dwSourceNb)
{	
	// 查找炮点对象
	CShotPoint* pShotPoint = m_AllShotPoints.GetShotPointByNb(dwShotNb);
	// 查找震源对象
	CShotSource* pShotSrc = m_AllShotSrcs.GetShotSourceByNb(dwSourceNb); 
	if(!pShotPoint || !pShotSrc)
	{
		// 发生错误
		return false;
	}
	// 查找采集排列
	CShotSpread *pSpread = m_AllSpreads.GetShotSpreadBySN(pShotPoint->m_dwSpreadNb);
	// 查找处理类型对象
	CProcessType *pProcess = m_AllProcesses.GetProcessTypeByNb(pShotPoint->m_dwProcessNb);
	if(!pSpread || !pProcess)
	{
		// 发生错误
		return false;
	}
	CActiveShot *pActiveShot = new CActiveShot;
	pActiveShot->SetShotPoint(pShotPoint,pSpread,pProcess);
	pActiveShot->SetShotSource( pShotSrc);

	pActiveShot->m_dwTimerID = m_AllActiveShots.Add(pActiveShot)+1;
	pActiveShot->m_pSvrSocket= m_pOperationSvrSock;
	
	pActiveShot->m_dwServerIP = inet_addr(("255.255.255.255"));		// 初始化为全网广播地址
	pActiveShot->m_wServerPort = OPERATION_SVRPORT;
	// 向服务器发送请求放炮请求
	pActiveShot->SendRequestShotService();
	// 启动重发定时器
	SetTimer(pActiveShot->m_dwTimerID,1000,NULL);
	CString  strTemp;
	strTemp.Format(_T("Shot #: %d, Source#: %d, Request Shot! "),dwShotNb,dwSourceNb);
	WriteLog(strTemp);
	return true;
}
/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 定时器处理函数，

    修改历史：

*******************************************************************************/
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int i;
	int nCount = m_AllActiveShots.GetCount();
	CActiveShot* pActiveShot = NULL;
	for (i=0;i<nCount;i++)
	{
		pActiveShot=m_AllActiveShots.GetActiveShot(i);
		if(pActiveShot->m_dwTimerID !=nIDEvent )
			continue;
		// 重发计数器
		pActiveShot->m_dwRetransCount++;
		if(pActiveShot->m_dwRetransCount>2)
		{
			// 重发三次无应答，显示错误信息，关闭定时器
			WriteLog(_T("Operation Server No Answer!"),0);
			delete pActiveShot;
			m_AllActiveShots.RemoveAt(i);
		}
		else
		{
			// 重发请求
			pActiveShot->SendRequestShotService();
			WriteLog(_T("Operation Server No Answer, Request Shot !"),0);
		}
	}
	CBCGPFrameWnd::OnTimer(nIDEvent);
}
/**
 * @brief 设置震源
 * @note  响应Setup下的Source菜单，弹出震源设置对话框，添加删除震源并设置震源属性。
 * @param  void
 * @return void
 */
void CMainFrame::OnSetupSource()
{
	// TODO: 在此添加命令处理程序代码
	CDlgSourceType	dlg;
	dlg.DoModal();
}

void CMainFrame::OnSetupOperation()
{
	// TODO: 在此添加命令处理程序代码
	CDlgShotPoint dlg;
	dlg.DoModal();
}

void CMainFrame::OnSetupComments()
{
	CDlgCommentType  dlg;
	dlg.DoModal();
}
//加入脉冲类型设置界面
void CMainFrame::OnSetupProcesstype()
{
	// TODO: ÔÚ´ËÌí¼ÓÃüÁî´¦Àí³ÌÐò´úÂë
	CDlgProcessType  dlg;
	dlg.DoModal();
}
