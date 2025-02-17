// TabADCDataShow.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabADCDataShow.h"
#include "Parameter.h"

// CTabADCDataShow 对话框

IMPLEMENT_DYNAMIC(CTabADCDataShow, CDialog)

CTabADCDataShow::CTabADCDataShow(CWnd* pParent /*=NULL*/)
	: CDialog(CTabADCDataShow::IDD, pParent)
	, m_pADCDataShow(NULL)
{

}

CTabADCDataShow::~CTabADCDataShow()
{
	if (m_pADCDataShow != NULL)
	{
		m_pADCDataShow = NULL;
		delete m_pADCDataShow;
	}
}

void CTabADCDataShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabADCDataShow, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTabADCDataShow 消息处理程序
//************************************
// Method:    OnInitDialog
// FullName:  CTabADCDataShow::OnInitDialog
// Access:    virtual protected 
// Returns:   BOOL
// Qualifier:
//************************************
BOOL CTabADCDataShow::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetTimer(TabADCDataShowADCTimerNb, TabADCDataShowADCTimerSet, NULL);
	return TRUE;
}
void CTabADCDataShow::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	if (nIDEvent == TabADCDataShowADCTimerNb)
	{
		CString str = _T("");
		CString strshow = _T("");
		double dbTemp = 0.0;
		if (m_pADCDataShow != NULL)
		{	
			for (int i=1; i<=InstrumentNum; i++)
			{
				ProcessMessages();
				m_Sec_ADCDataShow.Lock();
				dbTemp = m_pADCDataShow[i-1];
				m_Sec_ADCDataShow.Unlock();
				str.Format(_T("ADC%d = %2.*lf"), i, DecimalPlaces, dbTemp);
				strshow += str;
				if(i % ADCDataShowPerLineNum == 0)
				{
					strshow += _T("\r\n");
				}
				else
				{
					strshow += _T("\t");
				}	
			}
		}
		GetDlgItem(IDC_EDIT_ADCDATA)->SetWindowText(strshow);
	}
	CDialog::OnTimer(nIDEvent);
}

// 防止程序在循环中运行无法响应消息
void CTabADCDataShow::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
