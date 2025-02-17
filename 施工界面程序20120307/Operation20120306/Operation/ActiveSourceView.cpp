// ActiveSourceView.cpp : 实现文件
//

#include "stdafx.h"
#include "Operation.h"
#include "ActiveSourceView.h"
#include "MainFrm.h"
#include "DlgAcquisitionControl.h"

////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
/////////////////////////CActiveSourceGridCtrl  //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////// 

IMPLEMENT_DYNAMIC(CActiveSourceGridCtrl, CBCGPGridCtrl)

CActiveSourceGridCtrl::CActiveSourceGridCtrl()
{
	//EnableVisualManagerStyle();
}

CActiveSourceGridCtrl::~CActiveSourceGridCtrl()
{
}


BEGIN_MESSAGE_MAP(CActiveSourceGridCtrl, CBCGPGridCtrl)
END_MESSAGE_MAP()

void CActiveSourceGridCtrl::SetRowHeight()
{
	CBCGPGridCtrl::SetRowHeight();

	m_nRowHeight = m_nRowHeight + 5;
	m_nLargeRowHeight = m_nRowHeight;
}
/**
 * @brief 继承父类网格条码选择改变
 * @note  网格中被选中的条目发生变化时，系统会调用该函数通知用户。
 * @note  根据当前所选择的震源的状态，来使能或者禁止“GO”、“Stop”、“Abort”三个按钮。
 * @param CBCGPGridRow* pNewRow, 当前选中的震源行指针
 * @param CBCGPGridRow* pOldRow，前一次被选中的震源行指针
 * @return void
 */
void CActiveSourceGridCtrl::OnChangeSelection(CBCGPGridRow* pNewRow,CBCGPGridRow* pOldRow)
{
	int nSourceState;
	CActiveSourceView *pView = (CActiveSourceView*)GetParent();
	if(pNewRow!=NULL)
	{
		// 刷新显示
		pView->RefreshBlasterState();
		// 当前选择的不是空行
		nSourceState = pNewRow->GetItem(0)->GetImage();
		if(nSourceState==SHOTSOURCE_READY)	// 所有按钮使能
		{
			if (   pView->m_btnGo.GetSafeHwnd()
				&& pView->m_btnStop.GetSafeHwnd()
				&& pView->m_btnAbort.GetSafeHwnd())
			{	
				pView->m_btnGo.EnableWindow(TRUE);
				pView->m_btnStop.EnableWindow(TRUE);
				pView->m_btnAbort.EnableWindow(TRUE);
				return;
			}			
		}
		else if(nSourceState==SHOTSOURCE_FOCUS)	// GO按钮禁止，其他按钮使能
		{
			if (   pView->m_btnGo.GetSafeHwnd()
				&& pView->m_btnStop.GetSafeHwnd()
				&& pView->m_btnAbort.GetSafeHwnd())
			{	
				// cxm 2012.3.8
//				pView->m_btnGo.EnableWindow(TRUE);
				pView->m_btnGo.EnableWindow(FALSE);
				pView->m_btnStop.EnableWindow(TRUE);
				pView->m_btnAbort.EnableWindow(TRUE);
				return;
			}
		}
	}
	// 没有选中或者选中的震源没有准备好，则禁止按钮
	if (   pView->m_btnGo.GetSafeHwnd()
		&& pView->m_btnStop.GetSafeHwnd()
		&& pView->m_btnAbort.GetSafeHwnd())
	{
		// cxm 2012.3.8
// 		pView->m_btnGo.EnableWindow(TRUE);
// 		pView->m_btnStop.EnableWindow(TRUE);
// 		pView->m_btnAbort.EnableWindow(TRUE);

		pView->m_btnGo.EnableWindow(FALSE);
		pView->m_btnStop.EnableWindow(FALSE);
		pView->m_btnAbort.EnableWindow(FALSE);
	}
	return CBCGPGridCtrl::OnChangeSelection(pNewRow,pOldRow);
}

////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
/////////////////////////   CActiveSourceView  //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
IMPLEMENT_DYNCREATE(CActiveSourceView, CBCGPFormView)

CActiveSourceView::CActiveSourceView()
	: CBCGPFormView(CActiveSourceView::IDD)
{
	m_nBlasterState = 0;
	m_bShowBlasterCtrl = FALSE;
	EnableVisualManagerStyle ();
}

CActiveSourceView::~CActiveSourceView()
{
}

void CActiveSourceView::DoDataExchange(CDataExchange* pDX)
{
	CBCGPFormView::DoDataExchange(pDX);
	// DDX_Control(pDX, IDC_STATICGRID, m_wndGridLocation);
	DDX_Control(pDX, IDC_BUTTONGO, m_btnGo);
	DDX_Control(pDX, IDC_BUTTONSTOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTONABORT, m_btnAbort);
	DDX_Control(pDX, IDC_STATICBLASTER, m_ctrlBlasterText);
	DDX_Control(pDX, IDC_STATICUPHOLEMS, m_ctrlUpholeTxt);
	DDX_Control(pDX, IDC_STATICTBMS, m_ctrlTBText);
	DDX_Control(pDX, IDC_EDITUPHOLE, m_ctrlUpholeValue);
	DDX_Control(pDX, IDC_EDITTBMS, m_ctrlTBValue);
}

BEGIN_MESSAGE_MAP(CActiveSourceView, CBCGPFormView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	ON_BN_CLICKED(IDC_BUTTONGO, &CActiveSourceView::OnBnClickedButtongo)
	ON_BN_CLICKED(IDC_BUTTONSTOP, &CActiveSourceView::OnBnClickedButtonstop)
	ON_BN_CLICKED(IDC_BUTTONABORT, &CActiveSourceView::OnBnClickedButtonabort)
END_MESSAGE_MAP()


// CActiveSourceView 诊断

#ifdef _DEBUG
void CActiveSourceView::AssertValid() const
{
	CBCGPFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CActiveSourceView::Dump(CDumpContext& dc) const
{
	CBCGPFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CActiveSourceView 消息处理程序
BOOL CActiveSourceView::CreateView(CWnd* pParent, CCreateContext* pContext)
{	
	CRect rect(0,0,300,300);

	if (!Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		rect, pParent, AFX_IDW_PANE_FIRST, pContext))
	{
		TRACE0("Warning: couldn't create CMyFormView!\n");
		return FALSE;
	}
	CRect rectGrid;
	m_ImageList.Create (IDB_SOURCESTATE,16,0,RGB (255, 0, 255));
	m_wndGrid.SetImageList (&m_ImageList);
	GetClientRect(&rectGrid);
	rectGrid.bottom = rectGrid.bottom -80;
	m_wndGrid.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	// 禁止列排序
	m_wndGrid.EnableHeader (TRUE, BCGP_GRID_HEADER_HIDE_ITEMS);
	m_wndGrid.EnableInvertSelOnCtrl (FALSE);
		// 设置窗口位置
	m_wndGrid.SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	BCGP_GRID_COLOR_DATA clrData;
	clrData = m_wndGrid.GetColorTheme();
	clrData.m_clrBackground = globalData.clrBarFace;		// ;::GetSysColor(COLOR_3DFACE)	
	clrData.m_EvenColors.m_clrBackground = RGB(255,255,255);
	clrData.m_OddColors.m_clrBackground = RGB(250,253,253);

	m_wndGrid.SetColorTheme(clrData);
	m_wndGrid.SetReadOnly ();	
	m_wndGrid.SetSingleSel(TRUE);
	m_wndGrid.SetWholeRowSel(TRUE);
	m_wndGrid.EnableDragSelection(FALSE);

	SetGridHead();
	//m_wndGrid.LoadState (_T("ActiveSourceGrid"));
	return TRUE;
}
void CActiveSourceView::OnInitialUpdate()
{
	CBCGPFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CRect rc;
	GetWindowRect(&rc);
	SendMessage(WM_SIZE,SIZE_RESTORED,MAKELPARAM(rc.Width(),rc.Height()) );

}
void CActiveSourceView::OnDraw(CDC* pDC)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(m_bShowBlasterCtrl)
	{
		switch(m_nBlasterState)
		{
		case 0:		// 红色
			pDC->FillSolidRect(m_rcBlaster,RGB(255,0,0));
			break;
		case 1:		// 绿色
			pDC->FillSolidRect(m_rcBlaster,RGB(0,255,0));
			break;
		default:	
			pDC->FillSolidRect(m_rcBlaster,RGB(255,128,10));
			break;
		}
	}	
}

void CActiveSourceView::OnSize(UINT nType, int cx, int cy)
{
	CBCGPFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_wndGrid.GetSafeHwnd())
	{
		m_wndGrid.SetWindowPos (NULL, -1, -1, cx, cy-60, SWP_NOZORDER | SWP_NOACTIVATE); // SWP_NOMOVE | 
	}
	// 放炮控制按钮位置：Stop按钮的右边位置为中心线
 	if(m_btnGo.GetSafeHwnd())
 	{
 		m_btnGo.SetWindowPos (NULL, cx/2-220, cy-50, 100, 40, SWP_NOZORDER  |SWP_DRAWFRAME);
 	}
	if(m_btnStop.GetSafeHwnd())
	{
		m_btnStop.SetWindowPos (NULL, cx/2-100, cy-50, 100, 40, SWP_NOZORDER |SWP_DRAWFRAME);
	}
	if(m_btnAbort.GetSafeHwnd())
	{
		m_btnAbort.SetWindowPos (NULL, cx/2+20, cy-50, 100, 40, SWP_NOZORDER|SWP_DRAWFRAME );
	}
	// 爆炸机状态，包括TB、井口时间、爆炸机状态等，该组控件离窗口的右边距为20
	// TB
	if(m_ctrlTBValue.GetSafeHwnd())
	{
		m_ctrlTBValue.SetWindowPos (NULL, cx-100, cy-30, 80, 25, SWP_NOZORDER );
	}
	if(m_ctrlTBText.GetSafeHwnd())
	{
		m_ctrlTBText.SetWindowPos (NULL, cx-100, cy-55, 80, 25, SWP_NOZORDER );
	}
	// 井口时间
	if(m_ctrlUpholeValue.GetSafeHwnd())
	{
		m_ctrlUpholeValue.SetWindowPos (NULL, cx-200, cy-30, 80, 25, SWP_NOZORDER );
	}
	if(m_ctrlUpholeTxt.GetSafeHwnd())
	{
		m_ctrlUpholeTxt.SetWindowPos (NULL, cx-200, cy-55, 80, 25, SWP_NOZORDER );
	}
	// 状态
	if(m_ctrlBlasterText.GetSafeHwnd())
	{
		m_ctrlBlasterText.SetWindowPos (NULL, cx-300, cy-55, 80, 25, SWP_NOZORDER );
		m_rcBlaster.left = cx-300+30;
		m_rcBlaster.top = cy-28;
		m_rcBlaster.right = m_rcBlaster.left + 20;
		m_rcBlaster.bottom = m_rcBlaster.top + 20;
		Invalidate();
	}

}
void CActiveSourceView::OnDestroy()
{
	CBCGPFormView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (m_wndGrid.GetSafeHwnd())
	{
		// m_wndGrid.SaveState(_T("ActiveSourceGrid"));
		m_wndGrid.RemoveAll();
	}
}
bool CActiveSourceView::SetGridHead(void)
{
	if(!m_wndGrid.GetSafeHwnd())
		return false;	
	
	m_wndGrid.InsertColumn (0, _T("口"), 40);
	m_wndGrid.InsertColumn (1, _T("Rdy"), 40);
	m_wndGrid.InsertColumn (2, _T("Rdy VP"), 60);
	m_wndGrid.InsertColumn (3, _T("Source Name"), 120);
	m_wndGrid.InsertColumn (4, _T("Source Index"), 120);
	m_wndGrid.InsertColumn (5, _T("Shot Nb"), 80);
	m_wndGrid.InsertColumn (6, _T("Dist min"), 80);
	m_wndGrid.InsertColumn (7, _T("Current Stack"), 120);
	m_wndGrid.InsertColumn (8, _T("SourcePoint Line"), 150);
	m_wndGrid.InsertColumn (9, _T("SourcePoint Number"), 160);
	m_wndGrid.InsertColumn (10, _T("SourcePoint Index"), 160);
	m_wndGrid.InsertColumn (11, _T("Sfl"), 60);
	m_wndGrid.InsertColumn (12, _T("Sfn"), 60);
	m_wndGrid.InsertColumn (13, _T("Spread#"), 80);
	m_wndGrid.InsertColumn (14, _T("SuperSpread"), 100);
	m_wndGrid.InsertColumn (15, _T("ProcessType"), 100);
	m_wndGrid.InsertColumn (16, _T("Swath"), 60);
	//m_wndGrid.SetGridHeaderHeight(100);
	//m_wndGrid.AddRow();
	return true;
}
int CActiveSourceView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
/**
*/
void   CActiveSourceView::ShowBlasterCtrl(void)
{
	m_ctrlBlasterText.ShowWindow(m_bShowBlasterCtrl);
	m_ctrlUpholeTxt.ShowWindow(m_bShowBlasterCtrl);
	m_ctrlTBText.ShowWindow(m_bShowBlasterCtrl);
	m_ctrlUpholeValue.ShowWindow(m_bShowBlasterCtrl);
	m_ctrlTBValue.ShowWindow(m_bShowBlasterCtrl);	
	InvalidateRect(&m_rcBlaster);
}
/************************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能：     用户按下“GO”按钮，开始放炮。
	               服务器响应“GO按钮”，执行三个动作：
				   
				   1、服务器程序通知服务器采集线程开始接收数据采集；
				   2、通知测网开始数据采集；
				   3、发送命令通知爆炸机点火。

    修改历史：

*************************************************************************************/
void CActiveSourceView::OnBnClickedButtongo()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	CBCGPGridRow* pRow = m_wndGrid.GetCurSel();
	if(!pRow)
	{
		// pFrm->MessageBox(_T("Select shot source!"),_T("Alarm"),MB_OK);
		// cxm 2012.3.7
//		theApp.WriteLog(_T("CActiveSourceView::OnBnClickedButtongo"),_T("Please first select shot source !"),LOGTYPE_WARNING,TRUE);
		return;
	}
	// 得到网格当前选择的行号
	int nRow=pRow->GetRowId();
	if(nRow>=pFrm->m_AllShotSources.GetCount())
	{
		return;
	}
	// 找到震源
	CShotSource* pSource = pFrm->m_AllShotSources.GetShotSource(nRow);	
	// 查找激活的炮点和震源对象CActiveShot，调用放炮函数
	nRow = pFrm->m_AllActiveShots.FindFirstActiveShotBySource(pSource);
	CActiveShot* pActiveShot = pFrm->m_AllActiveShots.GetActiveShot(nRow);
	if(pActiveShot!=NULL)
	{
		m_btnGo.EnableWindow(FALSE);
		pActiveShot->SendGeneralCMD(OPERATION_CMD_SHOTGO);
	}	
}
/**
 * @brief  按下爆炸机窗口的Stop按钮
 * @note  按下“Stop”按钮，在该爆炸机关联的炮点完成采集后，在 Impulsive 模式中，
 您可将数据转储到记录过程。在其它模式中，您可使用所提示的三个下压按钮（Go、 Cancel、End）
 来选择是继续该顺序，还是再进行一次采集，还是停止该顺序。

详细说明： 
 （一）按下Stop按钮
 按下Stop按钮后，继续完成当前放炮采集，如果是 Impulsive 模式，则保存采集数据。
 在其他模式下，弹出三个下压按钮（Go、 Cancel、End） 来选择。
 （1）Go按钮：继续该采样顺序，开始后续采集，后续采集可能是继续本次采集，
              也可能是下一次采集（根据本次采样的状态来决定，如果本次采样结束，则根据震源步进属性计算的下一次炮点编号）；
 
 （2）Cancel按钮：终止本次采集，丢掉后续未采集的数据。但本次采集仍然保存选中。用户按下“Go”按钮，可以重新开始本次采集；

 （3）End按钮：将记录无保持采集的未完成震源点。然后点击 Go 将开始下一个震源点。
   
 * @param void
 * @return void
 */
void CActiveSourceView::OnBnClickedButtonstop()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	// 得到当前选择网格行
	CBCGPGridRow* pRow = m_wndGrid.GetCurSel();
	if(!pRow)
	{
		// pFrm->MessageBox(_T("Select shot source!"),_T("Alarm"),MB_OK);
		// cxm 2012.3.7
//		theApp.WriteLog(_T("CActiveSourceView::OnBnClickedButtonstop"),_T("Please first select shot source !"),LOGTYPE_WARNING,TRUE);
		return;
	}
	// 得到网格当前选择的行号
	int nRow=pRow->GetRowId();
	if(nRow>=pFrm->m_AllShotSources.GetCount())
	{
		return;
	}
	// 找到震源
	CShotSource* pSource = pFrm->m_AllShotSources.GetShotSource(nRow);	
	// 查找激活的炮点和震源对象CActiveShot，调用放炮函数
	nRow = pFrm->m_AllActiveShots.FindFirstActiveShotBySource(pSource);
	CActiveShot* pActiveShot = pFrm->m_AllActiveShots.GetActiveShot(nRow);
	if(pActiveShot==NULL)
	{
		// pActiveShot->SendGeneralCMD(OPERATION_CMD_SHOTGO);
		// cxm 2012.3.7
//		theApp.WriteLog(_T("CActiveSourceView::OnBnClickedButtonstop"),_T("The Shot Source is not activated!"),LOGTYPE_WARNING,TRUE);
		return;
	}
	CDlgAcquisitionControl		DlgCtrl;
	DlgCtrl.DoModal();
	switch(DlgCtrl.m_nSelectedButton)
	{
	case 1:			// 按下Go
		m_btnGo.EnableWindow(FALSE);
		break;
	case  2:		// Cancel
		m_btnGo.EnableWindow(TRUE);
		pActiveShot->SendGeneralCMD(OPERATION_CMD_AGAIN);
		break;
	case  3:		// End
		{
			// 调用主框架处理函数，通知服务器停止放炮，删除放炮管理对象
			m_btnGo.EnableWindow(TRUE);
			pFrm->OnDisassociatevp(pSource->m_dwSourceNb);
		}
		
		break;
	default:
		break;
	}
}
/**
 * @brief  按下爆炸机窗口的Stop按钮
 * @note   按下Abort按钮
 
 按下Abort按钮：将在当前采集完成后中断当前震源点。弹出对话框（OK or Cancel），
                 
				(1)选择OK则继续则当前采集被记录到 SEGD 文件。然后点击Go 可使该顺序继续进行。

  			    (2)如果选中Cancel，则则当前采集没有被记录。系统会提示	 Go、Cancel、End 按钮，与按下Stop按钮
				   后弹出的对话框含义相同。

 * @param void
 * @return void
 */
void CActiveSourceView::OnBnClickedButtonabort()
{
	//  
	if(MessageBox(_T("Do you want to abort?"),_T("Warning"),MB_OKCANCEL|MB_ICONWARNING)==IDOK)
	{
		// ????????????????????继续本次采样，完成后施工模式变为手动
		return;
	}
	OnBnClickedButtonstop();
}

/**
 * @brief  刷新界面上爆炸机状态显示
 * @note  将确认TB、井口时间、爆炸机状态显示在对应的控件中。
 * @param CActiveShot* pActiveShot，震源对象指针
 * @return void
 */
void  CActiveSourceView::RefreshBlasterState()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	CBCGPGridRow* pRow = m_wndGrid.GetCurSel();
	m_bShowBlasterCtrl = FALSE;
	if(!pRow)
	{		
		ShowBlasterCtrl();
		return;
	}
	// 得到网格当前选择的行号
	int nRow=pRow->GetRowId();
	if(nRow>=pFrm->m_AllShotSources.GetCount())
	{
		ShowBlasterCtrl();
		return;
	}
	// 找到震源
	CShotSource* pSource = pFrm->m_AllShotSources.GetShotSource(nRow);	
	// 查找激活的炮点和震源对象CActiveShot，调用放炮函数
	nRow = pFrm->m_AllActiveShots.FindFirstActiveShotBySource(pSource);
	if(nRow<0)
	{
		ShowBlasterCtrl();
		return;
	}
	CActiveShot* pActiveShot = pFrm->m_AllActiveShots.GetActiveShot(nRow);
	if(!pActiveShot || !pActiveShot->m_bNewBlaster)
	{
		ShowBlasterCtrl();
		return;
	}
	// 收到新的爆炸机状态参数，显示确认TB、井口时间
	m_bShowBlasterCtrl = TRUE;
	ShowBlasterCtrl();
	CString    strTemp;
	if (pActiveShot->m_bITBErr)
	{
		m_ctrlBlasterText.SetWindowText(_T("ITB"));		
	}
	else
	{
		m_ctrlBlasterText.SetWindowText(_T("Blaster"));		
	}
	// 爆炸机状态
	m_nBlasterState = pActiveShot->m_byBlasterState;
	strTemp.Format(_T("%.2f"),pActiveShot->m_fConfirmedTB);
	m_ctrlTBValue.SetWindowText(strTemp);
	strTemp.Format(_T("%.2f"),pActiveShot->m_fUpholeTime);
	m_ctrlUpholeValue.SetWindowText(strTemp);
	Invalidate();
}

////////////////////////////////////////////////////////////////////////////////////////////////// | SWP_NOACTIVATE
////////////////////////////////////////////////////////////////////////////////////////////////// SWP_NOMOVE | 
////////////////////////////CActiveSourceWnd///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////// | SWP_NOACTIVATE
////////////////////////////////////////////////////////////////////////////////////////////////// | SWP_NOACTIVATE
// CActiveSourceWnd

BEGIN_MESSAGE_MAP(CActiveSourceWnd, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CActiveSourceWnd)
	ON_WM_CREATE()	
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP	
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ACTIVESOURCE_DISASSOCIATEVP, &CActiveSourceWnd::OnActivesourceDisassociatevp)
	ON_COMMAND(ID_ACTIVESOURCE_ASSOCIATEVP, &CActiveSourceWnd::OnActivesourceAssociatevp)
	ON_COMMAND(ID_ACTIVESOURCE_ASSOCIATESPLSPN, &CActiveSourceWnd::OnActivesourceAssociatesplspn)
	ON_COMMAND(ID_ACTIVESOURCE_CLEANREADY, &CActiveSourceWnd::OnActivesourceCleanready)
END_MESSAGE_MAP()

CActiveSourceWnd::CActiveSourceWnd(void):m_pContext(NULL)
{
	CRuntimeClass* pFactory = RUNTIME_CLASS(CActiveSourceView);
	m_pFormView = (CActiveSourceView *)(pFactory->CreateObject());	
}

CActiveSourceWnd::~CActiveSourceWnd(void)
{
}
/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CActiveSourceWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_pFormView != NULL)
	{
		m_pFormView->CreateView(this, m_pContext);
	}
	LoadState (_T("ActiveSourceWnd"));
	return 0;
}

void CActiveSourceWnd::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);
	
	if (m_pFormView->GetSafeHwnd ())
	{
		m_pFormView->SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}	
}


void CActiveSourceWnd::OnDestroy()
{
	CBCGPDockingControlBar::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	 SaveState(_T("ActiveSourceWnd"));
}
BOOL CActiveSourceWnd::Create(LPCTSTR lpszCaption,CWnd*pParentWnd,const RECT&rect,BOOL bHasGripper,UINT nID,DWORD dwStyle,DWORD dwTabbedStyle,DWORD dwBCGStyle,CCreateContext*pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	BOOL bFlag=CBCGPDockingControlBar::Create( lpszCaption,pParentWnd,rect, bHasGripper, nID, dwStyle, dwTabbedStyle, dwBCGStyle,pContext);

	SetTabbedControlBarRTC (RUNTIME_CLASS (CBCGPTabbedControlBar));
	// 删除Caption按钮，在标题栏上下三角符号
	// RemoveCaptionButtons ();
	return bFlag;
}


/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能：

    修改历史：

*******************************************************************************/
CBCGPGridCtrl* CActiveSourceWnd::GetGridCtrl(void)
{
	if(NULL==m_pFormView)
		return NULL;
	return &(m_pFormView->m_wndGrid);
}
/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 将震源表加载到网格中

    修改历史：

*******************************************************************************/
void CActiveSourceWnd::LoadShotSources(CShotSources* pShotSrcs)
{
	if(NULL==pShotSrcs)
		return;
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return;
	int nCount = pShotSrcs->GetCount();
	pGridCtrl->RemoveAll();
	CShotSource* pSrc = NULL;
	for (int nRow = 0; nRow < nCount; nRow++)
	{
		pSrc=pShotSrcs->GetShotSource(nRow);
		if(!pSrc )
			continue;
		LoadShotSource(pSrc);
	}
	pGridCtrl->AdjustLayout ();
	return;
}

/**
 * @brief  将一条震源加载到网格中
 * @note  将一个震源对象属性添加到网格的对应列中，如果nRowNum小于0，则表示向网格中新增加一行；
 如果nRowNum大于0，则表示向指定行输出震源对象。
 * @param CShotSource* pShotSrc，震源对象指针
 * @param int  nRowNum，行号
 * @return void
 */
void CActiveSourceWnd::LoadShotSource(CShotSource* pShotSrc,int  nRowNum)
{
	if(NULL==pShotSrc)
		return ;
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return;
	CBCGPGridRow* pRow=NULL;
	if(nRowNum<0)
	{
		pRow = pGridCtrl->CreateRow (pGridCtrl->GetColumnCount());
		pGridCtrl->AddRow(pRow);
	}	
	else
	{
		pRow = pGridCtrl->GetRow(nRowNum);
	}
	if(!pRow)
		return;
	// 震源属性
	pRow->GetItem (0)->SetImage(pShotSrc->m_bySourceState);
	pRow->GetItem (2)->SetValue (_T("-"));
	pRow->GetItem (3)->SetValue (LPCTSTR(pShotSrc->m_strLabel));
	pRow->GetItem (4)->SetValue (pShotSrc->m_dwSourceIndex);
	// 空的炮点
	pRow->GetItem (5)->SetValue (_T(""));
	pRow->GetItem (6)->SetValue (_T("-"));
	pRow->GetItem (7)->SetValue (_T(""));
	pRow->GetItem (8)->SetValue (_T(""));
	pRow->GetItem (9)->SetValue (_T(""));
	pRow->GetItem (10)->SetValue (_T(""));
	pRow->GetItem (11)->SetValue (_T(""));
	pRow->GetItem (12)->SetValue (_T(""));
	pRow->GetItem (13)->SetValue (_T(""));
	pRow->GetItem (14)->SetValue (_T(""));
	pRow->GetItem (15)->SetValue (_T(""));
	pRow->GetItem (16)->SetValue (_T(""));
	// 设置行的属性数据为震源的编号
	pRow->SetData(pShotSrc->m_dwSourceNb);	
}


/******************************************************************************
    函    数：
    
    输    入：
    
    输    出：
  
    返 回 值：
    
    功    能： 用户选择震源、炮号准备放炮时，将炮号的信息（CShotPoint）写到表格中对应的震源行内

    修改历史：

*******************************************************************************/
int CActiveSourceWnd::LoadShotPointBySource(CShotSource* pSource, CShotPoint* pShotPoint)
{
	if(!pSource || !pShotPoint)
		return -1;
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return -1;
	CBCGPGridRow *pRow=NULL;
	int nRowCount = pGridCtrl->GetRowCount();
	COleVariant oVariant;
	for(int i=0;i<nRowCount;i++)
	{
		pRow = pGridCtrl->GetRow(i);		
		// 判断每行的属性数据与震源的编号是否相等
		if(pRow->GetData()==pSource->m_dwSourceNb)
		{
			// 装载下一次要放炮的炮点编号
			oVariant = (long)pShotPoint->m_dwShotNb + pSource->m_lStep;
			oVariant.ChangeType(VT_BSTR);			
			pRow->GetItem (2)->SetValue (oVariant);

			// 装载炮点的数据，编号
			oVariant = (long)pShotPoint->m_dwShotNb;
			oVariant.ChangeType(VT_BSTR);			
			pRow->GetItem (5)->SetValue (oVariant);
			// Dist min
			pRow->GetItem (6)->SetValue (_T("-"));
			// Current Stack
			pRow->GetItem (7)->SetValue (_T("0"));
			// SourcePoint Line			
			oVariant = pShotPoint->m_fSourceLine;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (8)->SetValue (oVariant);
			// SourcePoint Number		
			oVariant = pShotPoint->m_fSourceNb;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (9)->SetValue (oVariant);
			// SourcePoint Index
			oVariant = (long)pShotPoint->m_dwSourcePointIndex;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (10)->SetValue (oVariant);
			// SFL
			oVariant = (long)pShotPoint->m_dwSpreadSFL;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (11)->SetValue (oVariant);
			// SFN			
			oVariant = (long)pShotPoint->m_dwSpreadSFN;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (12)->SetValue (oVariant);
			// Spread#			
			oVariant = (long)pShotPoint->m_dwSpreadNb;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (13)->SetValue (oVariant);
			// SuperSpread			
			oVariant = (long)pShotPoint->m_dwSuperSpread;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (14)->SetValue (oVariant);
			// ProcessType			
			oVariant = (long)pShotPoint->m_dwProcessNb;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (15)->SetValue (oVariant);
			// Swath			
			oVariant = (long)pShotPoint->m_dwSwathNb;
			oVariant.ChangeType(VT_BSTR);
			pRow->GetItem (16)->SetValue (oVariant);
			return 1;
		}
	}
	
	return 1;

}
/**
 * @brief 刷新震源的状态图标
 * @note  通过每行设定的数据（等于震源的Nb）找到CShotSource* pSource对应的行，
 并刷新震源的状态图标，图标位于网格中第一列
 * @param  CShotSource* pSource，需要刷新显示状态的震源指针
 * @return void
 */
void  CActiveSourceWnd::RefrestShotSourceState(CShotSource* pSource)
{
	if(!pSource)
		return ;
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return ;
	CBCGPGridRow *pRow=NULL;
	int nRowCount = pGridCtrl->GetRowCount();	
	for(int i=0;i<nRowCount;i++)
	{
		pRow = pGridCtrl->GetRow(i);		
		// 判断每行的属性数据与震源的编号是否相等
		if(pRow->GetData()==pSource->m_dwSourceNb)
		{
			pRow->GetItem (0)->SetImage(pSource->m_bySourceState);
		}
	}
}
void	CActiveSourceWnd::RefreshBlasterState()
{
	if(m_pFormView)
		m_pFormView->RefreshBlasterState();
}
/**
 * @brief 响应鼠标在震源网格中右键消息，显示右键菜单
 * @note  如果选中某一个震源，则响应消息，显示右键菜单。菜单内容包括停止某一震源、为震源分配炮点编号、
 为某震源通过震源点测线号及道号来确定炮点、清除Ready状态等
 * @param CWnd* pWnd，窗口指针
 * @param CPoint point，鼠标坐标
 * @return void
 */
void CActiveSourceWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return;
	// 如果没有选择震源或者网格中某一行，则不弹出菜单
	CBCGPGridRow* pRow= pGridCtrl->GetCurSel();
	if(NULL==pRow)
		return;
	CMenu menu;
	// 加载右击菜单
	VERIFY(menu.LoadMenu (IDR_POPUP_ACTIVESOURCE));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);	
	// CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	CString  strTemp;
	// 每一行的数据等于震源的编号
	strTemp.Format(_T("Source %d"),pRow->GetData());
	
	// pPopup->ModifyMenu(MF_BYCOMMAND,MF_STRING,ID_ACTIVESOURCE_SOURCE,strTemp);
	// pPopup->ModifyMenu(MF_BYPOSITION,MF_STRING,0,_T("AAAAA"));
	// 更改第一个菜单的标题，显示为“Source”+震源编号
	MENUITEMINFO info;
	info.cbSize = sizeof (MENUITEMINFO);		// must fill up this field
	info.fMask = MIIM_STRING;					// get the state of the menu item
	info.dwTypeData = strTemp.GetBuffer();
	info.cch = strTemp.GetLength();
	VERIFY(pPopup->SetMenuItemInfo(ID_ACTIVESOURCE_SOURCE, &info,FALSE));
	strTemp.ReleaseBuffer();
	/*
	int  nSourceState = pRow->GetItem(0)->GetImage();
	if(nSourceState==SHOTSOURCE_FOCUS)
	{	
		info.cbSize = sizeof (MENUITEMINFO);		// must fill up this field
		info.fMask = MIIM_STATE;
		info.fState = MFS_DISABLED;
		// 如果正处于采集过程中，禁止更改放炮点
		pPopup->SetMenuItemInfo(ID_ACTIVESOURCE_DISASSOCIATEVP, &info,FALSE);
		pPopup->SetMenuItemInfo(ID_ACTIVESOURCE_ASSOCIATEVP, &info,FALSE);
		pPopup->SetMenuItemInfo(ID_ACTIVESOURCE_ASSOCIATESPLSPN, &info,FALSE);
		
		// pPopup->EnableMenuItem(ID_ACTIVESOURCE_DISASSOCIATEVP,MF_BYCOMMAND|MF_DISABLED);
		// pPopup->EnableMenuItem(ID_ACTIVESOURCE_ASSOCIATEVP,MF_BYCOMMAND|MF_DISABLED);
		// pPopup->EnableMenuItem(ID_ACTIVESOURCE_ASSOCIATESPLSPN,MF_BYCOMMAND|MF_DISABLED);
	}*/
	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
	pPopupMenu->Create(this, point.x, point.y,pPopup->Detach(),FALSE,TRUE);

}
/**
 * @brief "Disassociate VP",暂停震源使用
 * @note  不再希望继续使用某一震源，或想要将其置入待命状态，或想要手工分配给它另一
 个 VP，则在Active Source窗口右键菜单中选择Disassociate VP，暂停该震源的施工。
 在滑动扫描等模式下以前分配给该未用震源的剩余 VP 将被忽略。
 * @param  void
 * @return void
 */
void CActiveSourceWnd::OnActivesourceDisassociatevp()
{
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return;
	// 如果没有选择震源或者网格中某一行，则不处理
	CBCGPGridRow* pRow= pGridCtrl->GetCurSel();
	if(NULL==pRow)
		return;

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	// 调用主框架处理函数，通知服务器停止放炮，删除放炮管理对象
	CShotSource* pSource = pFrm->OnDisassociatevp(pRow->GetData());
	// 重新加载震源，并删除对应的炮点属性
	if(pSource)
		LoadShotSource(pSource,pRow->GetRowId());
}
/**
 * @brief "Associate VP",为震源分配炮点
 * @note  打开一个可指定要（通过输入其 Shot Number）分配给该震源的 VP 的对话框。这与用鼠标右键点击放炮表内
部所调出的“Start Seismonitor with Vib Source”命令效果基本相同。
 * @param  void
 * @return void
 */
void CActiveSourceWnd::OnActivesourceAssociatevp()
{
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return;
	// 如果没有选择震源或者网格中某一行，则不处理
	CBCGPGridRow* pRow= pGridCtrl->GetCurSel();
	if(NULL==pRow)
		return;

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->OnAssociatevp(pRow->GetData());
}
/**
 * @brief "Associate SPL & SPN",为震源分配炮点
 * @note  与 Associate VP 命令效果基本相同，但您不必输入要分配给该震源的 VP Shot Number，而是必须输
 入其 SPL （震源点测线） 和 SPN （震源点号）。
 * @param  void
 * @return void
 */
void CActiveSourceWnd::OnActivesourceAssociatesplspn()
{
	CBCGPGridCtrl* pGridCtrl = GetGridCtrl();
	if(NULL==pGridCtrl)
		return;
	// 如果没有选择震源或者网格中某一行，则不处理
	CBCGPGridRow* pRow= pGridCtrl->GetCurSel();
	if(NULL==pRow)
		return;

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	// 调用主框架处理函数，通过SPL和SPN找到响应的炮点
	pFrm->OnAssociateSPLSPN(pRow->GetData());
}
/**
 * @brief "Clean Ready",解除Ready信号
 * @note  用于解除该下压按钮中所暂停和记录的“准备就绪”状态，例如，如果您不想进行（显示在“Rdy VP”
 一列中的）与其相关的 VP。这样，您需要请爆炸工或可控震源领队再次发送“准备就绪”状态。
 * @param  void
 * @return void
 */
void CActiveSourceWnd::OnActivesourceCleanready()
{
	// TODO: 在此添加命令处理程序代码
}

