// PortMonitoringThreadSend.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "PortMonitoringThreadSend.h"


// CPortMonitoringThreadSend

IMPLEMENT_DYNCREATE(CPortMonitoringThreadSend, CWinThread)

CPortMonitoringThreadSend::CPortMonitoringThreadSend()
: m_bClose(false)
, m_usudp_count(0)
, m_iRecPort(0)
, m_iSendPort(0)
, m_csIP(_T(""))
, m_uiHeadFrameNum(0)
, m_uiIPSetReturnNum(0)
, m_uiTailFrameNum(0)
, m_uiTailTimeReturnNum(0)
, m_uiDelayTimeReturnNum(0)
, m_uiADCSetReturnNum(0)
, m_uiADCDataRecNum(0)
, m_csHeadFrameShow(_T(""))
, m_csIPSetReturnShow(_T(""))
, m_csTailFrameShow(_T(""))
, m_csTailTimeReturnShow(_T(""))
, m_csDelayTimeReturnShow(_T(""))
, m_csADCDataRecShow(_T(""))
, m_uiRecFrameNum(0)
, m_usADCLastDataPoint(0)
, m_pwnd(NULL)
, m_pWndTab(NULL)
, m_pSaveFile(NULL)
{
}

CPortMonitoringThreadSend::~CPortMonitoringThreadSend()
{
}

BOOL CPortMonitoringThreadSend::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CPortMonitoringThreadSend::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPortMonitoringThreadSend, CWinThread)
END_MESSAGE_MAP()


// 线程初始化

void CPortMonitoringThreadSend::OnInit(void)
{
	m_bClose = false;
	m_hThreadSendClose = ::CreateEvent(false, false, NULL, NULL);	// 创建事件对象
	::ResetEvent(m_hThreadSendClose);	// 设置事件对象为无信号状态
}

// 线程关闭
void CPortMonitoringThreadSend::OnClose(void)
{
	m_bClose = true;
}

int CPortMonitoringThreadSend::Run()
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà

	unsigned short uiPort = 0;
	DWORD dwFrameCount = 0;
	int icount = 0;
	unsigned short usADCDataPoint = 0;
	unsigned short usADCDataPointMove = 0;

	// 循环，直到关闭标志为真
	while(true)
	{
		if (m_bClose == true)
		{
			break;
		}

		dwFrameCount = 0;

		// 得到网络接收缓冲区数据字节数
		m_hSocketSend.IOCtl(FIONREAD, &dwFrameCount);
		dwFrameCount = dwFrameCount/256;
		if(0 != dwFrameCount)	
		{
			for(unsigned int i = 0; i < dwFrameCount; i++)	// 依次检查每个仪器IP设置应答帧
			{

				icount = m_hSocketSend.Receive(m_ucudp_buf[m_usudp_count],256);
				if (icount == 256)
				{
					CString strTemp = _T("");
					unsigned int uiCommand = 0;
					// 加入端口分发功能
					memcpy(&uiPort, &m_ucudp_buf[m_usudp_count][24], 2);
					if (uiPort == HeadFramePort)
					{
						memcpy(&uiCommand, &m_ucudp_buf[m_usudp_count][28], 1);
						if (uiCommand == 0x01)
						{
							// 显示SN
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][29]);
							m_csHeadFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][30]);
							m_csHeadFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][31]);
							m_csHeadFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][32]);
							m_csHeadFrameShow += strTemp;
							m_csHeadFrameShow += _T("\t");
							// 显示首包时刻
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][34]);
							m_csHeadFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][35]);
							m_csHeadFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][36]);
							m_csHeadFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][37]);
							m_csHeadFrameShow += strTemp;
							m_csHeadFrameShow += _T("\r\n");
				
 							m_pwnd->GetDlgItem(IDC_EDIT_HEADFRAMESHOW)->SetWindowText(m_csHeadFrameShow);
							m_uiHeadFrameNum++;
							strTemp.Format(_T("%d"),m_uiHeadFrameNum);
							m_pwnd->GetDlgItem(IDC_STATIC_HEADFRAMENUM)->SetWindowText(strTemp);
						}
					}
					else if (uiPort == IPSetPort)
					{
						memcpy(&uiCommand, &m_ucudp_buf[m_usudp_count][28], 1);
						if (uiCommand == 0x01)
						{
							// 显示SN
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][29]);
							m_csIPSetReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][30]);
							m_csIPSetReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][31]);
							m_csIPSetReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][32]);
							m_csIPSetReturnShow += strTemp;
							m_csIPSetReturnShow += _T("\t");
							// 显示IP
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][34]);
							m_csIPSetReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][35]);
							m_csIPSetReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][36]);
							m_csIPSetReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][37]);
							m_csIPSetReturnShow += strTemp;
							m_csIPSetReturnShow += _T("\r\n");
							m_pwnd->GetDlgItem(IDC_EDIT_IPSETRETURNSHOW)->SetWindowText(m_csIPSetReturnShow);
							m_uiIPSetReturnNum++;
							strTemp.Format(_T("%d"),m_uiIPSetReturnNum);
							m_pwnd->GetDlgItem(IDC_STATIC_IPSETRETURNNUM)->SetWindowText(strTemp);
						}
					}
					else if (uiPort == TailFramePort)
					{
						memcpy(&uiCommand, &m_ucudp_buf[m_usudp_count][33], 1);
						if (uiCommand == 0x01)
						{
							// 显示SN
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][34]);
							m_csTailFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][35]);
							m_csTailFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][36]);
							m_csTailFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][37]);
							m_csTailFrameShow += strTemp;
							m_csTailFrameShow += _T("\t");

							// 显示IP
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][29]);
							m_csTailFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][30]);
							m_csTailFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][31]);
							m_csTailFrameShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][32]);
							m_csTailFrameShow += strTemp;
							m_csTailFrameShow += _T("\r\n");

							m_pwnd->GetDlgItem(IDC_EDIT_TAILFRAMESHOW)->SetWindowText(m_csTailFrameShow);
							m_uiTailFrameNum++;
							strTemp.Format(_T("%d"),m_uiTailFrameNum);
							m_pwnd->GetDlgItem(IDC_STATIC_TAILFRAMENUM)->SetWindowText(strTemp);
						}
					}
					else if (uiPort == TailTimeFramePort)
					{
						memcpy(&uiCommand, &m_ucudp_buf[m_usudp_count][28], 1);
						if (uiCommand == 0x01)
						{	// 显示SN
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][29]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][30]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][31]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][32]);
							m_csTailTimeReturnShow += strTemp;
							m_csTailTimeReturnShow += _T(" ");
							// 显示网络时间
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][34]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][35]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][36]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][37]);
							m_csTailTimeReturnShow += strTemp;
							m_csTailTimeReturnShow += _T(" ");
							// 显示本地时间
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][39]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][40]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][41]);
							m_csTailTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][42]);
							m_csTailTimeReturnShow += strTemp;
//							m_csTailTimeReturnShow += _T(" ");
// 							// 显示尾包接收发送时刻（前14位有效）
// 							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][44]);
// 							m_csTailTimeReturnShow += strTemp;
// 							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][45]);
// 							m_csTailTimeReturnShow += strTemp;
// 							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][46]);
// 							m_csTailTimeReturnShow += strTemp;
// 							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][47]);
// 							m_csTailTimeReturnShow += strTemp;
							m_csTailTimeReturnShow += _T("\r\n");
							m_pwnd->GetDlgItem(IDC_EDIT_TAILTIMERETURNSHOW)->SetWindowText(m_csTailTimeReturnShow);
							m_uiTailTimeReturnNum++;
							strTemp.Format(_T("%d"),m_uiTailTimeReturnNum);
							m_pwnd->GetDlgItem(IDC_STATIC_TAILTIMERETURNNUM)->SetWindowText(strTemp);
						}
					}
					else if (uiPort == TimeSetPort)
					{
						memcpy(&uiCommand, &m_ucudp_buf[m_usudp_count][28], 1);
						if (uiCommand == 0x05)
						{
							// 显示IP
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][16]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][17]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][18]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][19]);
							m_csDelayTimeReturnShow += strTemp;
							m_csDelayTimeReturnShow += _T(" ");

							// 显示延时修正高位
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][29]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][30]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][31]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][32]);
							m_csDelayTimeReturnShow += strTemp;
							m_csDelayTimeReturnShow += _T(" ");

							// 显示延时修正低位
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][34]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][35]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][36]);
							m_csDelayTimeReturnShow += strTemp;
							strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][37]);
							m_csDelayTimeReturnShow += strTemp;
							m_csDelayTimeReturnShow += _T("\r\n");

							m_pwnd->GetDlgItem(IDC_EDIT_DELAYTIMERETURNSHOW)->SetWindowText(m_csDelayTimeReturnShow);
							m_uiDelayTimeReturnNum++;
							strTemp.Format(_T("%d"),m_uiDelayTimeReturnNum);
							m_pwnd->GetDlgItem(IDC_STATIC_DELAYTIMERETURNNUM)->SetWindowText(strTemp);
						}
					}
					else if (uiPort == ADSetReturnPort)
					{
						// 每次采集只有一个ADC设置应答帧
						m_uiADCSetReturnNum++;
						strTemp.Format(_T("%d"),m_uiADCSetReturnNum);
						m_pwnd->GetDlgItem(IDC_STATIC_ADCSETRETURNNUM)->SetWindowText(strTemp);
					}
					else if (uiPort == ADRecPort)
					{
						// 显示IP
						strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][16]);
						m_csADCDataRecShow += strTemp;
						strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][17]);
						m_csADCDataRecShow += strTemp;
						strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][18]);
						m_csADCDataRecShow += strTemp;
						strTemp.Format(_T("%02x "),m_ucudp_buf[m_usudp_count][19]);
						m_csADCDataRecShow += strTemp;
						m_csADCDataRecShow += _T(" ");

						// 显示ADC数据采样帧个数
						m_uiADCDataRecNum++;
						strTemp.Format(_T("%d"),m_uiADCDataRecNum);
						m_pwnd->GetDlgItem(IDC_STATIC_ADCDATARECNUM)->SetWindowText(strTemp);
						m_csADCDataRecShow += strTemp;
						m_csADCDataRecShow += _T("\t");

						memcpy(&usADCDataPoint, &m_ucudp_buf[m_usudp_count][28], 2);
						strTemp.Format(_T("%d"),usADCDataPoint);
						m_csADCDataRecShow += strTemp;
						m_csADCDataRecShow += _T("\t");

						usADCDataPointMove = usADCDataPoint - m_usADCLastDataPoint;
						strTemp.Format(_T("%d"),usADCDataPointMove);
						m_csADCDataRecShow += strTemp;
						m_csADCDataRecShow += _T("\r\n");
						m_pwnd->GetDlgItem(IDC_EDIT_ADCDATARECSHOW)->SetWindowText(m_csADCDataRecShow);
						m_usADCLastDataPoint = usADCDataPoint;
					}
					else
					{
						
					}
					m_uiRecFrameNum++;
					strTemp.Format(_T("%d"),m_uiRecFrameNum);
					m_pWndTab->GetDlgItem(IDC_STATIC_RECFRAMENUM)->SetWindowText(strTemp);

 					m_hSocketSend.SendTo(m_ucudp_buf[m_usudp_count],256,uiPort,m_csIP);
					m_pSaveFile->OnSaveSendData(m_ucudp_buf[m_usudp_count],256);
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
	::SetEvent(m_hThreadSendClose);	// 设置事件对象为有信号状态

	return CWinThread::Run();
}
// 打开
void CPortMonitoringThreadSend::OnOpen(void)
{
	int flag =  m_hSocketSend.Create(m_iRecPort,SOCK_DGRAM);
	m_hSocketSend.SetSockOpt(NULL,&m_hSocketSend,65536,SOL_SOCKET);
}

// 停止
void CPortMonitoringThreadSend::OnStop(void)
{
	m_hSocketSend.ShutDown(2);
	m_hSocketSend.Close();
}

// 重置界面
void CPortMonitoringThreadSend::OnReset(void)
{
	CString strTemp;
	// 首包计数
	m_uiHeadFrameNum = 0;
	strTemp.Format(_T("%d"),m_uiHeadFrameNum);
	
	m_pwnd->GetDlgItem(IDC_STATIC_HEADFRAMENUM)->SetWindowText(strTemp);

	// IP地址设置计数
	m_uiIPSetReturnNum = 0;
	strTemp.Format(_T("%d"),m_uiIPSetReturnNum);
	m_pwnd->GetDlgItem(IDC_STATIC_IPSETRETURNNUM)->SetWindowText(strTemp);

	// 尾包计数
	m_uiTailFrameNum = 0;
	strTemp.Format(_T("%d"),m_uiTailFrameNum);
	m_pwnd->GetDlgItem(IDC_STATIC_TAILFRAMENUM)->SetWindowText(strTemp);

	// 尾包时刻查询应答计数
	m_uiTailTimeReturnNum = 0;
	strTemp.Format(_T("%d"),m_uiTailTimeReturnNum);
	m_pwnd->GetDlgItem(IDC_STATIC_TAILTIMERETURNNUM)->SetWindowText(strTemp);

	// 时延设置应答计数
	m_uiDelayTimeReturnNum = 0;
	strTemp.Format(_T("%d"),m_uiDelayTimeReturnNum);
	m_pwnd->GetDlgItem(IDC_STATIC_DELAYTIMERETURNNUM)->SetWindowText(strTemp);

	// ADC设置应答计数
	m_uiADCSetReturnNum = 0;
	strTemp.Format(_T("%d"),m_uiADCSetReturnNum);
	m_pwnd->GetDlgItem(IDC_STATIC_ADCSETRETURNNUM)->SetWindowText(strTemp);

	// ADC数据采集帧个数
	m_uiADCDataRecNum = 0;
	strTemp.Format(_T("%d"),m_uiADCDataRecNum);
	m_pwnd->GetDlgItem(IDC_STATIC_ADCDATARECNUM)->SetWindowText(strTemp);

	// ADC数据采集上一帧的数据指针偏移量
	m_usADCLastDataPoint = 0;

	// 显示首包帧，包含SN和首包时刻
	m_csHeadFrameShow = _T("");
	m_pwnd->GetDlgItem(IDC_EDIT_HEADFRAMESHOW)->SetWindowText(_T(""));

	// 显示IP地址设置应答帧，包含SN和设置的IP地址
	m_csIPSetReturnShow = _T("");
	m_pwnd->GetDlgItem(IDC_EDIT_IPSETRETURNSHOW)->SetWindowText(_T(""));

	// 显示尾包帧，包含SN
	m_csTailFrameShow = _T("");
	m_pwnd->GetDlgItem(IDC_EDIT_TAILFRAMESHOW)->SetWindowText(_T(""));

	// 显示尾包时刻查询应答帧，包含IP地址和查询结果
	m_csTailTimeReturnShow = _T("");
	m_pwnd->GetDlgItem(IDC_EDIT_TAILTIMERETURNSHOW)->SetWindowText(_T(""));

	// 显示时延设置应答帧，包含IP地址和设置内容
	m_csDelayTimeReturnShow = _T("");
	m_pwnd->GetDlgItem(IDC_EDIT_DELAYTIMERETURNSHOW)->SetWindowText(_T(""));

	// 显示ADC采样数据帧，包含IP地址、帧计数和采样数据帧个数
	m_csADCDataRecShow = _T("");
	m_pwnd->GetDlgItem(IDC_EDIT_ADCDATARECSHOW)->SetWindowText(_T(""));

	// 接收帧数
	m_uiRecFrameNum = 0;
	strTemp.Format(_T("%d"),m_uiRecFrameNum);
	m_pWndTab->GetDlgItem(IDC_STATIC_RECFRAMENUM)->SetWindowText(strTemp);

}
