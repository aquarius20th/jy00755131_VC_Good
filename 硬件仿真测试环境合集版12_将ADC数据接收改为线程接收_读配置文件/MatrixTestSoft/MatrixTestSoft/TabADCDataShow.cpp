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
}

void CTabADCDataShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabADCDataShow, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTabADCDataShow 消息处理程序
BOOL CTabADCDataShow::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetTimer(1, 3000, NULL);
	return TRUE;
}
void CTabADCDataShow::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	if (nIDEvent == 1)
	{
		CString str = _T("");
		CString strshow = _T("");
		KillTimer(1);

		for (int i=1; i<=GraphViewNum; i++)
		{
			str.Format("ADC%d = %.9f", i, m_pADCDataShow[i-1]);
			strshow += str;
			if(i%6 == 0)
			{
				strshow += _T("\r\n");
			}
			else
			{
				strshow += _T("\t");
			}	
		}
		GetDlgItem(IDC_EDIT_ADCDATA)->SetWindowText(strshow);
		SetTimer(1, 1000, NULL);
	}

	CDialog::OnTimer(nIDEvent);
}
