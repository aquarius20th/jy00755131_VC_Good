// PortMonitoring.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabPortMonitoring.h"


// CPortMonitoring 对话框

IMPLEMENT_DYNAMIC(CTabPortMonitoring, CDialog)

CTabPortMonitoring::CTabPortMonitoring(CWnd* pParent /*=NULL*/)
	: CDialog(CTabPortMonitoring::IDD, pParent)
	, m_uiRcvPort1(0)
	, m_uiSendPort1(0)
	, m_uiRcvPort2(0)
	, m_uiSendPort2(0)
	, m_csIPRec(_T("127.0.0.1"))
	, m_csIPSend(_T("127.0.0.1"))
	, m_iSaveSize(1024)
	, m_bPortDistribution(FALSE)
//	, m_bStart(FALSE)
, m_bPortMonitoringStart(FALSE)
{

}

CTabPortMonitoring::~CTabPortMonitoring()
{
}

void CTabPortMonitoring::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PORTMONITORING, m_cTabCtrlItems);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPctrlRec);
	DDX_Control(pDX, IDC_IPADDRESS2, m_IPctrlSend);
	DDX_Check(pDX, IDC_CHECK_PORTDISTRIBUTION, m_bPortDistribution);
}


BEGIN_MESSAGE_MAP(CTabPortMonitoring, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PORTMONITORING, &CTabPortMonitoring::OnTcnSelchangeTabPortmonitoring)
	ON_BN_CLICKED(IDC_BUTTON_PORTMONITORING_OPEN, &CTabPortMonitoring::OnBnClickedButtonPortmonitoringOpen)
	ON_BN_CLICKED(IDC_BUTTON_PORTMONITORING_CLOSE, &CTabPortMonitoring::OnBnClickedButtonPortmonitoringClose)
	ON_BN_CLICKED(IDC_BUTTON_STARTSAVE, &CTabPortMonitoring::OnBnClickedButtonStartsave)
	ON_BN_CLICKED(IDC_BUTTON_STOPSAVE, &CTabPortMonitoring::OnBnClickedButtonStopsave)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILEPATH, &CTabPortMonitoring::OnBnClickedButtonSavefilepath)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CTabPortMonitoring::OnBnClickedButtonReset)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// 初始化选项卡控件

void CTabPortMonitoring::InitTabControlItems(void)
{
	m_oPortMonitorRec.Create(IDD_DIALOG_PORTMONITORING_REC, GetDlgItem(IDC_TAB_PORTMONITORING));
	m_oPortMonitorSend.Create(IDD_DIALOG_PORTMONITORING_SEND,GetDlgItem(IDC_TAB_PORTMONITORING));
	

	//获得IDC_TABTEST客户区大小
	CRect rs;
	m_cTabCtrlItems.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置
	rs.top+=20; 

	//设置子对话框尺寸并移动到指定位置
	m_oPortMonitorRec.MoveWindow(&rs);
	m_oPortMonitorSend.MoveWindow(&rs);

	m_oPortMonitorRec.ScreenToClient(&rs);
	m_oPortMonitorSend.ScreenToClient(&rs);

	//分别设置隐藏和显示
	m_oPortMonitorRec.ShowWindow(TRUE);
	m_oPortMonitorSend.ShowWindow(FALSE);
	
	//设置默认的选项卡
	m_cTabCtrlItems.SetCurSel(0); 
	CString str;

	str=_T("接收帧");
	m_cTabCtrlItems.InsertItem(0,str);  //添加参数一选项卡
	str=_T("发送帧");
	m_cTabCtrlItems.InsertItem(1,str);  //添加参数二选项卡
}

void CTabPortMonitoring::OnTcnSelchangeTabPortmonitoring(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	int CurSel = m_cTabCtrlItems.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_oPortMonitorRec.ShowWindow(TRUE); 
		m_oPortMonitorSend.ShowWindow(FALSE);
		Invalidate(TRUE);
		break;
	case 1:
		m_oPortMonitorRec.ShowWindow(FALSE);
		m_oPortMonitorSend.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	default:
		;
	}    
	*pResult = 0;
}

void CTabPortMonitoring::OnBnClickedButtonPortmonitoringOpen()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	CString str;
	int flag = 0;

	m_IPctrlRec.GetWindowText(m_csIPRec);
	m_IPctrlSend.GetWindowText(m_csIPSend);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->GetWindowText(str);			// 0x8202
	sscanf_s(str,"%x", &m_uiRcvPort1);
	m_oThreadRec.m_iRecPort = m_uiRcvPort1;
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->GetWindowText(str);			// 0x9002
	sscanf_s(str,"%x", &m_uiSendPort1);
	m_oThreadRec.m_iSendPort = m_uiSendPort1;
	m_oThreadRec.m_csIP = m_csIPRec;
	
	m_oThreadRec.ResumeThread();
	m_oThreadRec.OnOpen();

	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->GetWindowText(str);			// 0x9001
	sscanf_s(str,"%x", &m_uiRcvPort2);
	m_oThreadSend.m_iRecPort = m_uiRcvPort2;
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->GetWindowText(str);			// 0x8201
	sscanf_s(str,"%x", &m_uiSendPort2);
	m_oThreadSend.m_iSendPort = m_uiSendPort2;
	m_oThreadSend.m_csIP = m_csIPSend;

	m_oThreadSend.m_bPortDistribution = m_bPortDistribution;

	m_oThreadSend.ResumeThread();
	m_oThreadSend.OnOpen();
	
	GetDlgItem(IDC_EDIT_SAVESIZE)->GetWindowText(str);
	m_iSaveSize = atoi(str);
	m_oSaveFile.m_iSaveSize = m_iSaveSize;
	
	m_IPctrlRec.EnableWindow(FALSE);
	m_IPctrlSend.EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_PORTDISTRIBUTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SAVESIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->EnableWindow(FALSE);
	SetTimer(1, 1000, NULL);
	m_bPortMonitoringStart = TRUE;
}

void CTabPortMonitoring::OnBnClickedButtonPortmonitoringClose()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	m_oThreadRec.SuspendThread();
	m_oThreadRec.OnStop();

	m_oThreadSend.SuspendThread();
	m_oThreadSend.OnStop();

	m_IPctrlRec.EnableWindow(TRUE);
	m_IPctrlSend.EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_PORTDISTRIBUTION)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SAVESIZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->EnableWindow(TRUE);
	KillTimer(1);
	m_bPortMonitoringStart = FALSE;
}

void CTabPortMonitoring::OnBnClickedButtonStartsave()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oSaveFile.OnSaveStart();
	
}

void CTabPortMonitoring::OnBnClickedButtonStopsave()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oSaveFile.OnSaveStop();
}

void CTabPortMonitoring::OnBnClickedButtonSavefilepath()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oSaveFile.m_hWnd = this->m_hWnd;
	m_oSaveFile.OnSelectSaveFilePath();
}

void CTabPortMonitoring::OnBnClickedButtonReset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oSaveFile.OnReset();
	m_oThreadRec.OnReset();
	m_oThreadSend.OnReset();
	UpdateData(FALSE);
}

// 初始化
void CTabPortMonitoring::OnInit(void)
{
	InitTabControlItems();

	m_oSaveFile.m_pWndTab = this;
	m_oSaveFile.OnInit();

	// 串口调试环境
	m_IPctrlRec.SetWindowText(m_csIPRec);
	m_IPctrlSend.SetWindowText(m_csIPSend);

	CString str = "";
	str.Format("0x%04x", m_uiRcvPort1);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->SetWindowText(str);

	str.Format("0x%04x", m_uiSendPort1);
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->SetWindowText(str);

	str.Format("0x%04x", m_uiRcvPort2);
	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->SetWindowText(str);

	str.Format("0x%04x", m_uiSendPort2);
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->SetWindowText(str);

	str.Format("%d", m_iSaveSize);
	GetDlgItem(IDC_EDIT_SAVESIZE)->SetWindowText(str);

	m_bPortDistribution = TRUE;
	UpdateData(FALSE);

	m_oPortMonitorSend.m_pRecThread = &m_oThreadRec;
	m_oPortMonitorRec.m_pSendThread = &m_oThreadSend;

	m_oThreadRec.m_pSaveFile = &m_oSaveFile;
	m_oThreadSend.m_pSaveFile = &m_oSaveFile;

	m_oThreadSend.OnInit();
	m_oThreadSend.CreateThread();
	m_oThreadSend.SuspendThread();

	m_oThreadRec.OnInit();
	m_oThreadRec.CreateThread();
	m_oThreadRec.SuspendThread();
	
}

void CTabPortMonitoring::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	int iResult = 0;
	m_oThreadSend.OnClose();
	if (m_bPortMonitoringStart == TRUE)
	{
		iResult = ::WaitForSingleObject(m_oThreadSend.m_hPortMonitoringSendThreadClose,550);
		if (iResult != WAIT_OBJECT_0)
		{
			AfxMessageBox(_T("端口监视发送线程在550ms内未能正常结束！"));
		}
	}
	m_oThreadRec.OnClose();
	if (m_bPortMonitoringStart == TRUE)
	{
		iResult = ::WaitForSingleObject(m_oThreadRec.m_hPortMonitoringRecThreadClose,550);
		if (iResult != WAIT_OBJECT_0)
		{
			AfxMessageBox(_T("端口监视接收线程在550ms内未能正常结束！"));
		}
	}
}
// 防止程序在循环体中运行无法响应消息
void CTabPortMonitoring::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

void CTabPortMonitoring::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		CString strTemp = _T("");
		unsigned int uiSendFrameNum = 0;
		unsigned int uiRecFrameNum = 0;
	
		m_Sec.Lock();
		uiSendFrameNum = m_oThreadRec.m_uiSendFrameNum;
		m_Sec.Unlock();

		strTemp.Format(_T("%d"),uiSendFrameNum);
		GetDlgItem(IDC_STATIC_SENDFRAMENUM)->SetWindowText(strTemp);

		m_Sec.Lock();
		uiRecFrameNum = m_oThreadSend.m_uiRecFrameNum;
		m_Sec.Unlock();


		strTemp.Format(_T("%d"),uiRecFrameNum);
		GetDlgItem(IDC_STATIC_RECFRAMENUM)->SetWindowText(strTemp);
	}

	CDialog::OnTimer(nIDEvent);
}
