// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Admin.h"
#include "MainFrm.h"
#include "Admindoc.h"
#include "AdminView.h"
#include "UsersView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame
//添加的标识只运行一次的属性名, 自己定义一个属性值
extern CString	 g_strProgName;
extern HANDLE    g_hProgValue;
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_GETALLUSERINFO, OnGetAllUserInfo)

	ON_COMMAND(ID_SETUP_USERS, &CMainFrame::OnSetupUsers)
	ON_COMMAND(ID_SETUP_SESSION, &CMainFrame::OnSetupSession)
	ON_COMMAND(ID_SETUP_SERVER, &CMainFrame::OnSetupServer)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}
/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
*/
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	// m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);	
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);
	// DockControlBar(&m_wndToolBar);

	// 设置程序属性，确保程序仅允许一次	
	SetProp(m_hWnd,g_strProgName,g_hProgValue);	
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
LRESULT CMainFrame::OnGetAllUserInfo(WPARAM wParam, LPARAM lParam)
{
	// 得到当前激活的视图：可能是TabWnd封装的任意视图类
	CView*	pView0 =GetActiveView();
	if(!pView0)
		return 0;
	// 得到CBCGPTabWnd类指针
	CWnd* pWnd=pView0->GetParent();
	if(!pWnd)
		return 0;
	// 得到主视图类CAdminView的指针
	CAdminView*	pAdminView =(CAdminView*)(pWnd->GetParent());
	if(pAdminView)
		pAdminView->LoadAllUserInfo();
	return 0;
}
// 激活Users窗口
void CMainFrame::OnSetupUsers()
{
	// 得到当前激活的视图：可能是TabWnd封装的任意视图类
	CView*	pView0 =GetActiveView();
	if(!pView0)
		return;
	// 得到CBCGPTabWnd类指针
	CWnd* pWnd=pView0->GetParent();
	if(!pWnd)
		return;
	// 得到主视图类CAdminView的指针
	CAdminView*	pAdminView =(CAdminView*)(pWnd->GetParent());
	if(pAdminView)
		pAdminView->SetActiveView(0);
}
// 激活Session窗口
void CMainFrame::OnSetupSession()
{
	// 得到当前激活的视图：可能是TabWnd封装的任意视图类
	CView*	pView0 =GetActiveView();
	if(!pView0)
		return;
	// 得到CBCGPTabWnd类指针
	CWnd* pWnd=pView0->GetParent();
	if(!pWnd)
		return;
	// 得到主视图类CAdminView的指针
	CAdminView*	pAdminView =(CAdminView*)(pWnd->GetParent());
	if(pAdminView)
		pAdminView->SetActiveView(1);
}
// 激活Server窗口
void CMainFrame::OnSetupServer()
{
	// 得到当前激活的视图：可能是TabWnd封装的任意视图类
	CView*	pView0 =GetActiveView();
	if(!pView0)
		return;
	// 得到CBCGPTabWnd类指针
	CWnd* pWnd=pView0->GetParent();
	if(!pWnd)
		return;
	// 得到主视图类CAdminView的指针
	CAdminView*	pAdminView =(CAdminView*)(pWnd->GetParent());
	if(pAdminView)
		pAdminView->SetActiveView(2);
}
