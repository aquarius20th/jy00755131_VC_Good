// UsersView.cpp : 实现文件
//

#include "stdafx.h"
#include "Admin.h"
#include "UsersView.h"
#include "DlgNewUser.h"

// CUsersView

IMPLEMENT_DYNCREATE(CUsersView, CBCGPFormView)

CUsersView::CUsersView()
	: CBCGPFormView(CUsersView::IDD)
	, m_bAutoDelete(FALSE)
	, m_nExpiredMode(0)
	, m_nLastSelectedItem(-1)
{
	EnableVisualManagerStyle();
}	

CUsersView::~CUsersView()
{
}

void CUsersView::DoDataExchange(CDataExchange* pDX)
{
	CBCGPFormView::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECKAUTODELETE, m_bAutoDelete);
	DDX_Control(pDX, IDC_COMBOROLE, m_ctrlUserRole);
	DDX_Control(pDX, IDC_BTNEXPIRYDATE, m_ctrlExpiryDate);
	DDX_Control(pDX, IDC_LISTUSERS, m_ctrlUsersList);
	DDX_Radio(pDX, IDC_RADIOPASSWORDNEVER, m_nExpiredMode);
}

BEGIN_MESSAGE_MAP(CUsersView, CBCGPFormView)
	ON_NOTIFY(NM_CLICK, IDC_LISTUSERS, &CUsersView::OnNMClickListusers)
	ON_NOTIFY(NM_RCLICK, IDC_LISTUSERS, &CUsersView::OnNMRClickListusers)
	ON_COMMAND(ID_USER_MODIFY, &CUsersView::OnUserModify)
	ON_BN_CLICKED(IDC_BTNSAVE, &CUsersView::OnBnClickedBtnsave)
	ON_COMMAND(ID_USER_DELETE, &CUsersView::OnUserDelete)
	ON_COMMAND(ID_USER_NEW, &CUsersView::OnUserNew)
END_MESSAGE_MAP()


// CUsersView 诊断

#ifdef _DEBUG
void CUsersView::AssertValid() const
{
	CBCGPFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CUsersView::Dump(CDumpContext& dc) const
{
	CBCGPFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUsersView 消息处理程序

void CUsersView::OnInitialUpdate()
{
	CBCGPFormView::OnInitialUpdate();
	// 初始化控件的显示状态
	UINT dwTemp = 0;
	const UINT stateMask = 
		CBCGPDateTimeCtrl::DTM_SPIN |
		CBCGPDateTimeCtrl::DTM_DROPCALENDAR | 
		CBCGPDateTimeCtrl::DTM_DATE |
		CBCGPDateTimeCtrl::DTM_TIME24H |
		CBCGPDateTimeCtrl::DTM_CHECKBOX |
		CBCGPDateTimeCtrl::DTM_TIME | 
		CBCGPDateTimeCtrl::DTM_TIME24HBYLOCALE;

	dwTemp |= CBCGPDateTimeCtrl::DTM_DROPCALENDAR;
	dwTemp |= CBCGPDateTimeCtrl::DTM_DATE;
	dwTemp |= CBCGPDateTimeCtrl::DTM_TIME24HBYLOCALE;
	m_ctrlExpiryDate.SetState(dwTemp,stateMask);
	m_ctrlExpiryDate.SizeToContent();
	
	m_ImageList.Create(IDB_BITMAPUSER,20,0,RGB(128,128,128));
	m_ctrlUsersList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_ctrlUsersList.InsertColumn(0,_T(""),LVCFMT_LEFT,80);
}
/**
 * @brief 禁止或者使能界面控件
 * @note  禁止或者使能界面控件；处于修改状态时使能界面控件。
 * @param BOOL bEnable，禁止或者使能
 * @return void
 */
void    CUsersView::EnableModify(BOOL bEnable)
{
	GetDlgItem(IDC_RADIOPASSWORDNEVER)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIOPASSWORDAT)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECKAUTODELETE)->EnableWindow(bEnable);
	m_ctrlUserRole.EnableWindow(bEnable);
	m_ctrlExpiryDate.EnableWindow(bEnable);	
}
/**
 * @brief 加载某个用户详细信息
 * @note  加载某个用户详细信息，并在右侧显示用户的具体信息
 * @param CUserInfo* pUserInfo，需要显示的用户信息
 * @return void
 */
void  CUsersView::LoadSingleUserInfo(CUserInfo* pUserInfo)
{
	if(!pUserInfo)
		return;
	// 密码失效模式
	m_nExpiredMode = pUserInfo->m_nExpiredMode;
	// 是否自动删除
	m_bAutoDelete = pUserInfo->m_bAutoDelete;
	// 角色
	m_ctrlUserRole.SetCurSel(pUserInfo->m_nRole);
	COleDateTime	oleDate(pUserInfo->m_tmExpiryDate);
	// 失效日期
	m_ctrlExpiryDate.SetDate(oleDate);
	UpdateData(FALSE);

}
/**
 * @brief 保存某个用户详细信息
 * @note  保存某个用户详细信息，并在右侧显示用户的具体信息
 * @param CUserInfo* pUserInfo，需要显示的用户信息
 * @return void
 */
void    CUsersView::SaveSingleUserInfo(int  nItem,CUserInfo* pUserInfo)
{
	if(!pUserInfo)
		return;
	UpdateData(TRUE);
	// 密码失效模式
	pUserInfo->m_nExpiredMode = m_nExpiredMode;
	// 是否自动删除
	pUserInfo->m_bAutoDelete = m_bAutoDelete;
	// 角色
	pUserInfo->m_nRole = m_ctrlUserRole.GetCurSel();
	COleDateTime	oleDate;
	// 失效日期
	oleDate=m_ctrlExpiryDate.GetDate();
	oleDate.GetAsSystemTime(pUserInfo->m_tmExpiryDate);
}
/**
 * @brief 加载所有用户信息
 * @note  加载所有的用户对象，并显示第一个用户的具体信息
 * @param void
 * @param void
 * @return void
 */
void CUsersView::LoadAllUserInfo(void)
{
	int nUseCount = theApp.m_AllUserInfo.GetCount();
	CUserInfo* pUserInfo = NULL;
	// 将用户名加入列表框
	for (int i=0;i<nUseCount;i++)
	{
		pUserInfo = theApp.m_AllUserInfo.GetUserInfo(i);
		m_ctrlUsersList.InsertItem(i,pUserInfo->m_strUserName,1);
		// 设置条目的数据：0表示没有修改，1表示修改
		m_ctrlUsersList.SetItemData(i,0);
	}
	// 默认选择第一个用户，并加载第一个用户属性
	m_ctrlUsersList.SetHotItem(0);
	pUserInfo = theApp.m_AllUserInfo.GetUserInfo(0);
	LoadSingleUserInfo(pUserInfo);
}

/**
 * @brief 响应NM_CLICK消息，选择不同用户时显示用户详细信息
 * @note  响应NM_CLICK消息，选择不同用户时显示用户详细信息
 * @param NMHDR *pNMHDR, 
 * @param LRESULT *pResult
 * @return void
 */
void CUsersView::OnNMClickListusers(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW *pNMListView=(NM_LISTVIEW *)pNMHDR;
	CUserInfo* pUserInfo = NULL;
	int nItem;
	
	// 得到当前选择的用户
	nItem=pNMListView->iItem;
	if(m_nLastSelectedItem == nItem)
		return;
	if(m_nLastSelectedItem>=0 && m_nLastSelectedItem<m_ctrlUsersList.GetItemCount())
	{ 
		// 已经修改条目，则保存到对象中，点击Save时保存到服务器
		if(m_ctrlUsersList.GetItemData(m_nLastSelectedItem))
		{
			pUserInfo = theApp.m_AllUserInfo.GetUserInfo(m_nLastSelectedItem);
			SaveSingleUserInfo(m_nLastSelectedItem,pUserInfo);
		}		
	}
	// 显示当前选择的用户属性
	m_nLastSelectedItem = nItem;
	if (nItem>=0)
	{
		EnableModify(m_ctrlUsersList.GetItemData(nItem));
		pUserInfo = theApp.m_AllUserInfo.GetUserInfo(nItem);
		LoadSingleUserInfo(pUserInfo);
	}
	*pResult = 0;
}

void CUsersView::OnNMRClickListusers(NMHDR *pNMHDR, LRESULT *pResult)
{
	// LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW *pNMListView=(NM_LISTVIEW *)pNMHDR;
	CMenu menu;
	VERIFY(menu.LoadMenu (IDR_POPUP_USERADMIN));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	/*
	if(pNMListView->iItem<0)
	{
		pPopup->EnableMenuItem(ID_USER_MODIFY,MF_BYCOMMAND | MF_DISABLED);
		pPopup->EnableMenuItem(ID_USER_DELETE,MF_BYCOMMAND | MF_DISABLED);
	}
	else
	{
		pPopup->EnableMenuItem(ID_USER_MODIFY,MF_BYCOMMAND | MF_ENABLED);
		pPopup->EnableMenuItem(ID_USER_DELETE,MF_BYCOMMAND | MF_ENABLED);
	}*/
	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
	// 显示菜单，由主框架响应菜单 
	CPoint   pt=pNMListView->ptAction;
	ClientToScreen(&pt);
	pPopupMenu->Create (this, pt.x ,pt.y,pPopup->Detach(),FALSE,TRUE); //pPopup->m_hMenu 
	*pResult = 0;
}
/**
 * @brief 允许修改的用户信息
 * @note  允许修改的用户信息，使能右侧的详细信息，允许编辑。
 * @param void
 * @return void
 */
void CUsersView::OnUserModify()
{
	// 找到当前选择的条目
	POSITION pos = m_ctrlUsersList.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return;
	}
	int nItem = m_ctrlUsersList.GetNextSelectedItem(pos);
	if(nItem<0)
		return;
	// 设置条目允许修改标志
	m_ctrlUsersList.SetItemData(nItem,1);
	EnableModify(TRUE);
}
/**
 * @brief 保存修改的用户信息
 * @note  保存修改的用户信息，将右侧的详细信息禁止
 * @param void
 * @return void
 */
void CUsersView::OnBnClickedBtnsave()
{
	// 先找出当前被选择的用户条目
	CUserInfo*  pUserInfo = NULL;
	POSITION pos = m_ctrlUsersList.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return;
	}
	int nItem = m_ctrlUsersList.GetNextSelectedItem(pos);
	// 如果该条目被修改，则先保存该条目对象
	if(nItem>=0 && m_ctrlUsersList.GetItemData(nItem))
	{
		pUserInfo = theApp.m_AllUserInfo.GetUserInfo(nItem);
		SaveSingleUserInfo(nItem,pUserInfo);
	}
	// 清除所有条目对象的修改标志
	nItem = m_ctrlUsersList.GetItemCount();
	for (int i=0;i<nItem;i++)
	{
		m_ctrlUsersList.SetItemData(i,0);
	}
	// 禁止界面修改
	EnableModify(FALSE);
	// 通知服务器，更新所有用户信息
	theApp.m_sockUserAdmin.SendFrm_SendAllUserInfo();
}
/**
 * @brief 删除当前选择的用户信息
 * @note  删除当前选择的用户信息
 * @param void
 * @return void
 */
void CUsersView::OnUserDelete()
{
	// 找到当前选择的条目
	POSITION pos = m_ctrlUsersList.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return;
	}
	int nItem = m_ctrlUsersList.GetNextSelectedItem(pos);
	if(nItem<0)
		return;
	// 删除对应的条目
	m_ctrlUsersList.DeleteItem(nItem);
	// 删除用户对象
	theApp.m_AllUserInfo.RemoveAt(nItem);	
}
/**
 * @brief 添加新的用户
 * @note  新增加一个用户
 * @param void
 * @return void
 */
void CUsersView::OnUserNew()
{
	CDlgNewUser		Dlg;
	if(Dlg.DoModal()==IDCANCEL)
		return;
	// 创建新用户
	CUserInfo*   pUserInfo = new CUserInfo;
	if( !pUserInfo)
		return;
	// 保存用户及密码
	pUserInfo->m_strUserName = Dlg.m_strUserName;
	pUserInfo->m_strPassword = Dlg.m_strPassword;
	theApp.m_AllUserInfo.Add(pUserInfo);
	
	// 增加用户列表
	 int  nCount = m_ctrlUsersList.GetItemCount();
	 m_ctrlUsersList.InsertItem(nCount,Dlg.m_strUserName,1);
	 LoadSingleUserInfo(pUserInfo);
	 m_ctrlUsersList.SetItemData(nCount,1);
	 m_ctrlUsersList.SetItemState(nCount,LVIS_SELECTED,LVIS_SELECTED );
	 EnableModify(TRUE);
}
