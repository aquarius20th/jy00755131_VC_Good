// DlgAcquisitionControl.cpp : 实现文件
//

#include "stdafx.h"
#include "Operation.h"
#include "DlgAcquisitionControl.h"


// CDlgAcquisitionControl 对话框

IMPLEMENT_DYNAMIC(CDlgAcquisitionControl, CBCGPDialog)

CDlgAcquisitionControl::CDlgAcquisitionControl(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgAcquisitionControl::IDD, pParent)
{
	m_nSelectedButton = -1;
}

CDlgAcquisitionControl::~CDlgAcquisitionControl()
{
}

void CDlgAcquisitionControl::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAcquisitionControl, CBCGPDialog)
	ON_BN_CLICKED(ID_BTNEND, &CDlgAcquisitionControl::OnBnClickedBtnend)
	ON_BN_CLICKED(IDCANCEL, &CDlgAcquisitionControl::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgAcquisitionControl::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAcquisitionControl 消息处理程序

// 按下”GO“按钮
void CDlgAcquisitionControl::OnBnClickedOk()
{
	m_nSelectedButton = 1;
	OnOK();
}
// 按下Cancel按钮
void CDlgAcquisitionControl::OnBnClickedCancel()
{
	m_nSelectedButton = 2;
	OnOK();
}
// 按下End按钮
void CDlgAcquisitionControl::OnBnClickedBtnend()
{
	m_nSelectedButton = 3;
	OnOK();
}
// 用户直接关闭该对话框
void CDlgAcquisitionControl::OnCancel()
{
	m_nSelectedButton = -1;
	CBCGPDialog::OnCancel();
}
