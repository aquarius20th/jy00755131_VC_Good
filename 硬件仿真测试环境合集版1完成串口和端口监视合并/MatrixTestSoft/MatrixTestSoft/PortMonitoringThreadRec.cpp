// PortMonitoringThreadRec.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "PortMonitoringThreadRec.h"


// CPortMonitoringThreadRec

IMPLEMENT_DYNCREATE(CPortMonitoringThreadRec, CWinThread)

CPortMonitoringThreadRec::CPortMonitoringThreadRec()
: m_bClose(false)
, m_usudp_count(0)
, m_iRecPort(0)
, m_iSendPort(0)
, m_csIP(_T(""))
, m_pwnd(NULL)
, m_pWndTab(NULL)
, m_pSaveFile(NULL)
, m_uiHeartBeatNum(0)
, m_uiIPSetNum(0)
, m_uiTailTimeNum(0)
, m_uiDelayTimeSetNum(0)
, m_uiADCSetNum(0)
, m_uiSendFrameNum(0)
{
}

CPortMonitoringThreadRec::~CPortMonitoringThreadRec()
{
}

BOOL CPortMonitoringThreadRec::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CPortMonitoringThreadRec::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPortMonitoringThreadRec, CWinThread)
END_MESSAGE_MAP()


// 线程初始化

void CPortMonitoringThreadRec::OnInit(void)
{
	m_bClose = false;
	m_hThreadRecClose = ::CreateEvent(false, false, NULL, NULL);	// 创建事件对象
	::ResetEvent(m_hThreadRecClose);	// 设置事件对象为无信号状态
}

// 线程关闭
void CPortMonitoringThreadRec::OnClose(void)
{
	m_bClose = true;
}

int CPortMonitoringThreadRec::Run()
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	DWORD dwFrameCount = 0;
	int icount = 0;
	unsigned short uiPort = 0;
	// 循环，直到关闭标志为真
	while(true)
	{
		if(m_bClose == true)
		{
			break;
		}

		// 得到网络接收缓冲区数据字节数
		m_hSocketRec.IOCtl(FIONREAD, &dwFrameCount);
		dwFrameCount = dwFrameCount/128;
		if(0 != dwFrameCount)	
		{
			for(unsigned int i = 0; i < dwFrameCount; i++)	// 依次检查每个仪器IP设置应答帧
			{
				icount = m_hSocketRec.Receive(m_ucudp_buf[m_usudp_count],128);
				if (icount == 128)
				{
					CString strTemp = _T("");
					unsigned int uiCommand = 0;
					// 通过端口识别功能
					memcpy(&uiPort, &m_ucudp_buf[m_usudp_count][24], 2);
					if (uiPort == HeartBeatRec)
					{
						m_uiHeartBeatNum ++;
						strTemp.Format(_T("%d"),m_uiHeartBeatNum);
						m_pwnd->GetDlgItem(IDC_STATIC_HEARTBEATFRAMENUM)->SetWindowText(strTemp);
					}
					else if (uiPort == IPSetPort)
					{
						m_uiIPSetNum ++;
						strTemp.Format(_T("%d"),m_uiIPSetNum);
						m_pwnd->GetDlgItem(IDC_STATIC_IPSETNUM)->SetWindowText(strTemp);
					}
					else if (uiPort == TailTimeFramePort)
					{
						m_uiTailTimeNum ++;
						strTemp.Format(_T("%d"),m_uiTailTimeNum);
						m_pwnd->GetDlgItem(IDC_STATIC_TAILTIMESETNUM)->SetWindowText(strTemp);
					}
					else if (uiPort == TimeSetPort)
					{
						m_uiDelayTimeSetNum ++;
						strTemp.Format(_T("%d"),m_uiDelayTimeSetNum);
						m_pwnd->GetDlgItem(IDC_STATIC_DELAYTIMESETNUM)->SetWindowText(strTemp);
					}
					else if (uiPort == ADSetReturnPort)
					{
						m_uiADCSetNum ++;
						strTemp.Format(_T("%d"),m_uiADCSetNum);
						m_pwnd->GetDlgItem(IDC_STATIC_ADCSETNUM)->SetWindowText(strTemp);
					}

					m_uiSendFrameNum ++;
					strTemp.Format(_T("%d"),m_uiSendFrameNum);
					m_pWndTab->GetDlgItem(IDC_STATIC_SENDFRAMENUM)->SetWindowText(strTemp);

					m_hSocketRec.SendTo(m_ucudp_buf[m_usudp_count],128,m_iSendPort,m_csIP);
					m_pSaveFile->OnSaveReceiveData(m_ucudp_buf[m_usudp_count],128);
		
					m_usudp_count +=1;
					m_usudp_count = m_usudp_count%8;
				}
				else
				{
					break;
				}
			}
		}

		if (m_bClose == true)
		{
			break;
		}
		Sleep(50);
	}
	::SetEvent(m_hThreadRecClose);	// 设置事件对象为有信号状态

	return CWinThread::Run();
}
// 打开
void CPortMonitoringThreadRec::OnOpen(void)
{
	int flag =  m_hSocketRec.Create(m_iRecPort,SOCK_DGRAM);
	m_hSocketRec.SetSockOpt(NULL,&m_hSocketRec,65536,SOL_SOCKET);
}

// 停止
void CPortMonitoringThreadRec::OnStop(void)
{
	m_hSocketRec.ShutDown(2);
	m_hSocketRec.Close();
}

// 重置界面
void CPortMonitoringThreadRec::OnReset(void)
{
	CString strTemp;
	m_uiHeartBeatNum = 0;
	strTemp.Format(_T("%d"),m_uiHeartBeatNum);
	m_pwnd->GetDlgItem(IDC_STATIC_HEARTBEATFRAMENUM)->SetWindowText(strTemp);

	m_uiIPSetNum = 0;
	strTemp.Format(_T("%d"),m_uiIPSetNum);
	m_pwnd->GetDlgItem(IDC_STATIC_IPSETNUM)->SetWindowText(strTemp);

	m_uiTailTimeNum = 0;
	strTemp.Format(_T("%d"),m_uiTailTimeNum);
	m_pwnd->GetDlgItem(IDC_STATIC_TAILTIMESETNUM)->SetWindowText(strTemp);
	
	m_uiDelayTimeSetNum = 0;
	strTemp.Format(_T("%d"),m_uiDelayTimeSetNum);
	m_pwnd->GetDlgItem(IDC_STATIC_DELAYTIMESETNUM)->SetWindowText(strTemp);
	
	m_uiADCSetNum = 0;
	strTemp.Format(_T("%d"),m_uiADCSetNum);
	m_pwnd->GetDlgItem(IDC_STATIC_ADCSETNUM)->SetWindowText(strTemp);
	
	m_uiSendFrameNum = 0;
	strTemp.Format(_T("%d"),m_uiSendFrameNum);
	m_pWndTab->GetDlgItem(IDC_STATIC_SENDFRAMENUM)->SetWindowText(strTemp);
}
