// AdminView.cpp : CAdminView 类的实现
//

#include "stdafx.h"
#include "Admin.h"
#include "AdminDoc.h"
#include "AdminView.h"
#include "MainFrm.h"
#include "UsersView.h"
#include "SessionsView.h"
#include "ServerView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAdminView

IMPLEMENT_DYNCREATE(CAdminView, CBCGPTabView)

BEGIN_MESSAGE_MAP(CAdminView, CBCGPTabView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CBCGPTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CBCGPTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CBCGPTabView::OnFilePrintPreview)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CAdminView 构造/析构

CAdminView::CAdminView()
{
	// TODO: 在此处添加构造代码

}

CAdminView::~CAdminView()
{
}

BOOL CAdminView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CBCGPTabView::PreCreateWindow(cs);
}

// CAdminView 绘制

void CAdminView::OnDraw(CDC* /*pDC*/)
{
	CAdminDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CAdminView 打印

BOOL CAdminView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CAdminView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CAdminView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CAdminView 诊断

#ifdef _DEBUG
void CAdminView::AssertValid() const
{
	CBCGPTabView::AssertValid();
}

void CAdminView::Dump(CDumpContext& dc) const
{
	CBCGPTabView::Dump(dc);
}

CAdminDoc* CAdminView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAdminDoc)));
	return (CAdminDoc*)m_pDocument;
}
#endif //_DEBUG


// CAdminView 消息处理程序

void CAdminView::OnInitialUpdate()
{
	CBCGPTabView::OnInitialUpdate();

	CBCGPTabWnd& 	Tabwnd = GetTabControl();
	// 设置 Tab 视图的外观：标签在上端，不显示标签按钮，禁止tab页面位置交换
	Tabwnd.SetLocation(CBCGPBaseTabWnd::LOCATION_TOP);
	Tabwnd.SetButtonsVisible(FALSE);
	Tabwnd.EnableTabSwap(FALSE);
	Tabwnd.RecalcLayout();
	// 请求所有用户信息
	// CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	theApp.m_sockUserAdmin.SendFrm_ReqUsersInfo();
}

int CAdminView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 增加所需的视图	
	AddView(RUNTIME_CLASS( CUsersView ),_T("Users"),0);
	AddView(RUNTIME_CLASS( CSessionsView ),_T("Sessions"),1);
	AddView(RUNTIME_CLASS( CServerView ),_T("Server"),2);
	return 0;
}

void CAdminView::LoadAllUserInfo(void)
{
	CBCGPTabWnd&  TabWnd = GetTabControl();	
	CUsersView* pUserView = (CUsersView*)TabWnd.GetTabWnd(0);
	if(pUserView)
		pUserView->LoadAllUserInfo();
	CSessionsView* pSessionView = (CSessionsView*)TabWnd.GetTabWnd(1);
	if(pSessionView)
		pSessionView->LoadAliveUserInfo();
}