// Tab1.cpp : 实现文件
//

#include "stdafx.h"
#include "TabControlTest.h"
#include "Tab1.h"


// CTab1 对话框

IMPLEMENT_DYNAMIC(CTab1, CDialog)

CTab1::CTab1(CWnd* pParent /*=NULL*/)
	: CDialog(CTab1::IDD, pParent)
{

}

CTab1::~CTab1()
{
}

void CTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2, m_tab_in);
}


BEGIN_MESSAGE_MAP(CTab1, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CTab1::OnTcnSelchangeTab2)
END_MESSAGE_MAP()


// CTab1 消息处理程序
BOOL CTab1::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_tab4.Create(IDD_DIALOG4,GetDlgItem(IDC_TAB1));
	m_tab5.Create(IDD_DIALOG5,GetDlgItem(IDC_TAB1));

	//获得IDC_TABTEST客户区大小
	CRect rs;
	m_tab_in.GetWindowRect(&rs);
	//调整子对话框在父窗口中的位置
	rs.top+=20; 
	// 	rs.bottom-=1; 
	// 	rs.left+=1; 
	// 	rs.right-=2; 

	//设置子对话框尺寸并移动到指定位置
	m_tab4.MoveWindow(&rs);
	m_tab5.MoveWindow(&rs);
	m_tab4.ScreenToClient(&rs);
	m_tab5.ScreenToClient(&rs);

	//分别设置隐藏和显示
	m_tab4.ShowWindow(TRUE);
	m_tab5.ShowWindow(FALSE);

	//设置默认的选项卡
	m_tab_in.SetCurSel(0); 
	CString str;

	str=_T("选项卡1");
	m_tab_in.InsertItem(0,str);  //添加参数一选项卡
	str=_T("选项卡2");
	m_tab_in.InsertItem(1,str);  //添加参数二选项卡


	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CTab1::OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	int CurSel = m_tab_in.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_tab4.ShowWindow(TRUE);
		m_tab5.ShowWindow(FALSE);

		break;
	case 1:
		m_tab4.ShowWindow(FALSE);
		m_tab5.ShowWindow(TRUE);
	
		break;
	default:
		break;
	}    
	*pResult = 0;
}