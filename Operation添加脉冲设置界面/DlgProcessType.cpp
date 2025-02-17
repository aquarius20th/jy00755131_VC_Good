// DlgProcessType.cpp : 实现文件
//
// vivi 2010.11.8
#include "stdafx.h"
#include "Operation.h"
#include "OperaStruct.h"
#include "DlgProcessType.h"



// CDlgProcessType 对话框

IMPLEMENT_DYNAMIC(CDlgProcessType, CBCGPDialog)

CDlgProcessType::CDlgProcessType(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgProcessType::IDD, pParent)
	, m_cbindexProcessType(PROCESS_IMPULSIVE)
	, m_radioProcessSetup(PROCESS_STANDARD)
{

}

CDlgProcessType::~CDlgProcessType()
{
}

void CDlgProcessType::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_PROCESSTYPESELECT, m_cbindexProcessType);
	DDX_Radio(pDX, IDC_RADIO_STANDARD, m_radioProcessSetup);
}


BEGIN_MESSAGE_MAP(CDlgProcessType, CBCGPDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_PROCESSTYPESELECT, &CDlgProcessType::OnCbnSelchangeProcessType)
	ON_BN_CLICKED(IDC_RADIO_STANDARD, &CDlgProcessType::OnBnClickedRadioStandard)
	ON_BN_CLICKED(IDC_RADIO_ADVANCED, &CDlgProcessType::OnBnClickedRadioAdvanced)
END_MESSAGE_MAP()


// CDlgProcessType 消息处理程序

BOOL CDlgProcessType::OnInitDialog(void)
{
	CBCGPDialog::OnInitDialog();
// 	设置默认选项为Advanced
// 	CButton* pRadio = (CButton*)GetDlgItem (IDC_RADIO_ADVANCED);
// 	pRadio->SetCheck (1);

	
	return TRUE;
}
void CDlgProcessType::OnCbnSelchangeProcessType()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	// 更新m_combovalueprocesstype的值
	UpdateData ();
	CWnd *pWnd = this->GetDlgItem(IDC_STATICPICTURE);
	HWND hwnd = m_dlgImpulsive.m_hWnd;
	HWND hwnd2 = m_dlgStackImpulsive.m_hWnd;

	// 如果IMPULSIVE子窗口句柄不为空，则销毁子窗口
	if (hwnd != NULL)
	{
		m_dlgImpulsive.DestroyWindow();
	}
	// 如果STACKIMPULSIVE子窗口句柄不为空，则销毁子窗口
	if (hwnd2 != NULL)
	{
		m_dlgStackImpulsive.DestroyWindow();
	}
	switch(m_cbindexProcessType)
	{
	case 0:
		pWnd->GetClientRect(&m_dlgImpulsive.dlgRect);
		if ( m_dlgImpulsive.Create(IDD_IMPULSIVE, pWnd) == false )
		{
			AfxMessageBox(_T("Create Impulsive Dialog Error!"));
		}

		break;
	case 1:
		pWnd->GetClientRect(&m_dlgStackImpulsive.dlgRect);
		if ( m_dlgStackImpulsive.Create(IDD_STACKIMPULSIVE, pWnd) == false )
		{
			AfxMessageBox(_T("Create StackImpulsive Dialog Error!"));
		}
		break;
	}
	// 刷新界面
	Invalidate(TRUE);
}

void CDlgProcessType::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBCGPDialog::OnShowWindow(bShow, nStatus);
	
	// 设置DlgImpulsive为初始化界面显示在IDC_STATICPICTURE上
	CWnd *pWnd = this->GetDlgItem(IDC_STATICPICTURE);
	pWnd->GetClientRect(&m_dlgImpulsive.dlgRect);
	if ( m_dlgImpulsive.Create(IDD_IMPULSIVE, pWnd) == false )
	{
		AfxMessageBox(_T("Create Impulsive Dialog Error!"));
	}
}

void CDlgProcessType::OnBnClickedRadioStandard()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_radioProcessSetup = PROCESS_STANDARD;
}

void CDlgProcessType::OnBnClickedRadioAdvanced()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_radioProcessSetup = PROCESS_ADVANCE;
}
