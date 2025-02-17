// PortMonitoringSendThread.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "PortMonitoringSendThread.h"

// CPortMonitoringSendThread

IMPLEMENT_DYNCREATE(CPortMonitoringSendThread, CWinThread)

CPortMonitoringSendThread::CPortMonitoringSendThread()
: m_iRecPort(0)
, m_iSendPort(0)
, m_csIP(_T(""))
, m_uiHeadFrameNum(0)
, m_uiIPSetReturnNum(0)
, m_uiTailFrameNum(0)
, m_uiTailTimeReturnNum(0)
, m_uiDelayTimeReturnNum(0)
, m_uiADCSetReturnNum(0)
, m_uiErrorCodeReturnNum(0)
, m_uiCollectSysTimeReturnNum(0)
, m_uiADCRecNum(0)
, m_csHeadFrameShow(_T(""))
, m_csIPSetReturnShow(_T(""))
, m_csTailFrameShow(_T(""))
, m_csTailTimeReturnShow(_T(""))
, m_csDelayTimeReturnShow(_T(""))
, m_csErrorCodeReturnShow(_T(""))
, m_uiRecFrameNum(0)
, m_pSaveFile(NULL)
, m_bPortDistribution(FALSE)
, m_bclose(false)
, m_uiUdpCount(0)
, m_pLogFile(NULL)
{
	memset(m_uiDataErrorCount, SndFrameBufInit, InstrumentMaxCount * (sizeof(unsigned int)));
	memset(m_uiCmdErrorCount, SndFrameBufInit, InstrumentMaxCount * (sizeof(unsigned int)));
}

CPortMonitoringSendThread::~CPortMonitoringSendThread()
{
}

BOOL CPortMonitoringSendThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CPortMonitoringSendThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPortMonitoringSendThread, CWinThread)
END_MESSAGE_MAP()


// CPortMonitoringSendThread 消息处理程序

int CPortMonitoringSendThread::Run()
{
	// TODO: 在此添加专用代码和/或调用基类
	DWORD dwFrameCount = 0;
	int icount = 0;
	sockaddr_in SenderAddr;
	int n = sizeof(SenderAddr);
	int iError = 0;
	CString str = _T("");
	// 循环，直到关闭标志为真
	while(true)
	{
		if (m_bclose == true)
		{
			break;
		}
		// 得到网络接收缓冲区数据字节数
		// 可以解决粘包现象
		while(SOCKET_ERROR != ioctlsocket(m_SendSocket, FIONREAD, &dwFrameCount))
		{
			if(dwFrameCount > 0) 
			{
				icount = recvfrom(m_SendSocket, reinterpret_cast<char*>(&m_ucUdpBuf), sizeof(m_ucUdpBuf), 0, reinterpret_cast<sockaddr*>(&SenderAddr), &n);
				if (icount != SOCKET_ERROR)
				{
					OnProcess(icount);
				}
				else if(icount == PortMonitoringSendBufSize)
				{
					str.Format(_T("端口监视发送线程数据接收超过缓冲区大小，缓冲区大小为%d！"), PortMonitoringSendBufSize);
					m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::Run"), str, ErrorStatus);
				}
				else
				{
					iError = WSAGetLastError();
					str.Format(_T("端口监视发送的接收帧错误，错误号为%d！"), iError);
					m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::Run"), str, ErrorStatus);
				}
			}
			else
			{
				break;
			}
		}

		if (m_bclose == true)
		{
			break;
		}
		Sleep(PortMonitoringSendThreadSleepTime);
	}
	::SetEvent(m_hPortMonitoringSendThreadClose);	// 设置事件对象为有信号状态,释放等待线程后将事件置为无信号
	return CWinThread::Run();
}
// 数据处理
//************************************
// Method:    OnProcess
// FullName:  CPortMonitoringSendThread::OnProcess
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iCount
//************************************
void CPortMonitoringSendThread::OnProcess(int iCount)
{
	unsigned short usCRC16 = 0;
	for (int i=0; i<iCount; i++)
	{
//		ProcessMessages();
		switch(m_uiUdpCount)
		{
		case 0:
			if (m_ucUdpBuf[i] == FrameHeadCheck0)
			{
				m_ucudp_buf[0] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}

		case 1:
			if (m_ucUdpBuf[i] == FrameHeadCheck1)
			{
				m_ucudp_buf[1] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}
		case 2:
			if (m_ucUdpBuf[i] == FrameHeadCheck2)
			{
				m_ucudp_buf[2] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}
		case 3:
			if (m_ucUdpBuf[i] == FrameHeadCheck3)
			{
				m_ucudp_buf[3] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}
		default:
			if (m_uiUdpCount >=  RcvFrameSize)
			{
				m_uiUdpCount = 0;
				break;
			}
			m_ucudp_buf[m_uiUdpCount] = m_ucUdpBuf[i];
			m_uiUdpCount++;
			if (m_uiUdpCount == RcvFrameSize)
			{
				m_uiUdpCount = 0;

				memcpy(&usCRC16, &m_ucudp_buf[RcvFrameSize - CRCSize], CRCSize);
				if (usCRC16 != get_crc_16(&m_ucudp_buf[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
				{
					//	continue;
				}
				OnPortMonitoringProc();
			}
			break;
		}
	}
}
// 初始化
//************************************
// Method:    OnInit
// FullName:  CPortMonitoringSendThread::OnInit
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CPortMonitoringSendThread::OnInit(void)
{
	m_hPortMonitoringSendThreadClose = ::CreateEvent(false, false, NULL, NULL);	// 创建事件对象
	if ((m_hPortMonitoringSendThreadClose == NULL)||(GetLastError() == ERROR_ALREADY_EXISTS))
	{
		AfxMessageBox(_T("创建端口监视发送线程关闭事件出错！"));
	}
	else
	{
		::ResetEvent(m_hPortMonitoringSendThreadClose);	// 设置事件对象为无信号状态
	}
	for (int i=0; i<InstrumentMaxCount; i++)
	{
		m_uiErrorCodeReturnCount[i] = 0;
		memset(&m_ucErrorCodeReturn[i], SndFrameBufInit, QueryErrorCodeNum);
	}
}

// 打开
//************************************
// Method:    OnOpen
// FullName:  CPortMonitoringSendThread::OnOpen
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CPortMonitoringSendThread::OnOpen(void)
{
	CString str = _T("");
	m_SendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	m_RecvAddr.sin_family = AF_INET;											// 填充套接字地址结构
	m_RecvAddr.sin_port = htons(static_cast<unsigned short>(m_iRecPort));
	m_RecvAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	int iReturn = bind(m_SendSocket, reinterpret_cast<sockaddr*>(&m_RecvAddr), sizeof(m_RecvAddr));	// 绑定本地地址
	listen(m_SendSocket, 2);
	if (iReturn == SOCKET_ERROR)
	{
		str = _T("端口监视程序的发送端口创建失败！");
		AfxMessageBox(str);
		m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnOpen"), str, ErrorStatus);
	}
	else
	{
		int nSendBuf = PortMonitoringSendBufSize;
		iReturn = setsockopt(m_SendSocket, SOL_SOCKET, SO_SNDBUF,  reinterpret_cast<const char *>(&nSendBuf), sizeof(int));
		if (iReturn == SOCKET_ERROR)
		{
			str = _T("端口监视程序的接收端口发送缓冲区设置失败！");
			AfxMessageBox(str);
			m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnOpen"), str, ErrorStatus);
		}
		int nRecvBuf = PortMonitoringSendBufSize;
		iReturn = setsockopt(m_SendSocket, SOL_SOCKET, SO_RCVBUF,  reinterpret_cast<const char *>(&nRecvBuf), sizeof(int));
		if (iReturn == SOCKET_ERROR)
		{
			str = _T("端口监视程序的发送端口接收缓冲区设置失败！");
			AfxMessageBox(str);
			m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnOpen"), str, ErrorStatus);
		}
		//设置广播模式
		int iOptlen = sizeof(int);
		int iOptval = 1;
		iReturn = setsockopt(m_SendSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char *>(&iOptval), iOptlen);
		if (iReturn == SOCKET_ERROR)
		{
			str = _T("端口监视程序的发送端口设置为广播端口失败！");
			AfxMessageBox(str);
			m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnOpen"), str, ErrorStatus);
		}
		// 避免端口阻塞
		OnAvoidIOBlock(m_SendSocket);
	}
}
// 避免端口阻塞
//************************************
// Method:    OnAvoidIOBlock
// FullName:  CPortMonitoringSendThread::OnAvoidIOBlock
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: SOCKET socket
//************************************
void CPortMonitoringSendThread::OnAvoidIOBlock(SOCKET socket)
{
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	DWORD status = 0;
	status = WSAIoctl(socket, SIO_UDP_CONNRESET ,
		&bNewBehavior, sizeof(bNewBehavior),
		NULL, 0, &dwBytesReturned,
		NULL, NULL);
}
// 停止
//************************************
// Method:    OnStop
// FullName:  CPortMonitoringSendThread::OnStop
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CPortMonitoringSendThread::OnStop(void)
{
	OnCloseUDP();
	// 硬件设备错误查询应答帧个数
	m_uiErrorCodeReturnNum = 0;
	for (int i=0; i<InstrumentMaxCount; i++)
	{
		m_uiErrorCodeReturnCount[i] = 0;
		memset(&m_ucErrorCodeReturn[i], SndFrameBufInit, QueryErrorCodeNum);
	}
}

// 关闭
//************************************
// Method:    OnClose
// FullName:  CPortMonitoringSendThread::OnClose
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CPortMonitoringSendThread::OnClose(void)
{
	OnCloseUDP();
	m_bclose = true;
}
// 重置界面
//************************************
// Method:    OnReset
// FullName:  CPortMonitoringSendThread::OnReset
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CPortMonitoringSendThread::OnReset(void)
{
	// 首包计数
	m_uiHeadFrameNum = 0;
	// IP地址设置计数
	m_uiIPSetReturnNum = 0;
	// 尾包计数
	m_uiTailFrameNum = 0;
	// 尾包时刻查询应答计数
	m_uiTailTimeReturnNum = 0;
	// 时延设置应答计数
	m_uiDelayTimeReturnNum = 0;
	// ADC设置应答计数
	m_uiADCSetReturnNum = 0;
	// 硬件设备错误查询应答帧个数
	m_uiErrorCodeReturnNum = 0;
	// 查询得到的本地时间帧数
	m_uiCollectSysTimeReturnNum = 0;
	// 接收得到的ADC数据帧数
	m_uiADCRecNum = 0;
	// 显示首包帧，包含SN和首包时刻
	m_csHeadFrameShow.Empty();
	// 显示IP地址设置应答帧，包含SN和设置的IP地址
	m_csIPSetReturnShow.Empty();
	// 显示尾包帧，包含SN
	m_csTailFrameShow.Empty();
	// 显示尾包时刻查询应答帧，包含IP地址和查询结果
	m_csTailTimeReturnShow.Empty();
	// 显示时延设置应答帧，包含IP地址和设置内容
	m_csDelayTimeReturnShow.Empty();
	// 显示硬件错误码和码差
	m_csErrorCodeReturnShow.Empty();
	// 接收帧数
	m_uiRecFrameNum = 0;
	// 数据错误计数
	memset(m_uiDataErrorCount, SndFrameBufInit, InstrumentMaxCount * (sizeof(unsigned int)));
	// 命令错误计数
	memset(m_uiCmdErrorCount, SndFrameBufInit, InstrumentMaxCount * (sizeof(unsigned int)));

}
// 防止程序在循环中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CPortMonitoringSendThread::ProcessMessages
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CPortMonitoringSendThread::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

// 端口监视处理函数
//************************************
// Method:    OnPortMonitoringProc
// FullName:  CPortMonitoringSendThread::OnPortMonitoringProc
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CPortMonitoringSendThread::OnPortMonitoringProc(void)
{
	unsigned short uiPort = 0;
	CString strTemp = _T("");
	unsigned int uiTemp = 0;
	unsigned int uiCommand = 0;
	int iPos = 0;
	// 加入端口分发功能
	iPos = 24;
	memcpy(&uiPort, &m_ucudp_buf[iPos], FramePacketSize2B);
	if (uiPort == HeadFramePort)
	{
		iPos = 28;
		memcpy(&uiCommand, &m_ucudp_buf[iPos], FrameCmdSize1B);
		if (uiCommand == CmdSn)
		{
			iPos += FrameCmdSize1B;
			// 显示SN
			for (int j=0; j<FramePacketSize4B; j++)
			{
				strTemp.Format(_T("%02x "), m_ucudp_buf[iPos]);
				iPos++;
				m_csHeadFrameShow += strTemp;
			}
			m_csHeadFrameShow += _T("\t");
			// 显示首包时刻
			iPos = 34;
			for (int k=0; k<FramePacketSize4B; k++)
			{
				strTemp.Format(_T("%02x "), m_ucudp_buf[iPos]);
				iPos++;
				m_csHeadFrameShow += strTemp;
			}
			m_csHeadFrameShow += _T("\r\n");
			m_uiHeadFrameNum++;
		}
	}
	else if (uiPort == IPSetPort)
	{
		iPos = 28;
		memcpy(&uiCommand, &m_ucudp_buf[iPos], FrameCmdSize1B);
		if (uiCommand == CmdSn)
		{
			iPos += FrameCmdSize1B;
			// 显示SN
			for (int j=0; j<FramePacketSize4B; j++)
			{
				strTemp.Format(_T("%02x "), m_ucudp_buf[iPos]);
				iPos++;
				m_csIPSetReturnShow += strTemp;
			}
			m_csIPSetReturnShow += _T("\t");
			// 显示IP
			iPos = 34;
			for (int k=0; k<FramePacketSize4B; k++)
			{
				strTemp.Format(_T("%02x "), m_ucudp_buf[iPos]);
				iPos++;
				m_csIPSetReturnShow += strTemp;
			}
			m_csIPSetReturnShow += _T("\r\n");
			m_uiIPSetReturnNum++;
		}
	}
	else if (uiPort == TailFramePort)
	{
		iPos = 33;
		memcpy(&uiCommand, &m_ucudp_buf[iPos], FrameCmdSize1B);
		if (uiCommand == CmdSn)
		{
			iPos += FrameCmdSize1B;
			// 显示SN
			for (int j=0; j<FramePacketSize4B; j++)
			{
				strTemp.Format(_T("%02x "), m_ucudp_buf[iPos]);
				iPos++;
				m_csTailFrameShow += strTemp;
			}
			m_csTailFrameShow += _T("\t");

			// 显示IP
			iPos = 29;
			for (int k=0; k<FramePacketSize4B; k++)
			{
				strTemp.Format(_T("%02x "),m_ucudp_buf[iPos]);
				iPos++;
				m_csTailFrameShow += strTemp;
			}
			m_csTailFrameShow += _T("\r\n");
			m_uiTailFrameNum++;
		}
	}
	else if (uiPort == TailTimeFramePort)
	{
		iPos = 28;
		memcpy(&uiCommand, &m_ucudp_buf[iPos], FrameCmdSize1B);
		if (uiCommand == CmdSn)
		{
			iPos += FrameCmdSize1B;
			// 显示SN
			for (int j=0; j<FramePacketSize4B; j++)
			{
				strTemp.Format(_T("%02x "),m_ucudp_buf[iPos]);
				iPos++;
				m_csTailTimeReturnShow += strTemp;
			}
			m_csTailTimeReturnShow += _T(" ");
			// 显示网络时间
			iPos = 34;
			for (int k=0; k<FramePacketSize4B; k++)
			{
				strTemp.Format(_T("%02x "),m_ucudp_buf[iPos]);
				iPos++;
				m_csTailTimeReturnShow += strTemp;
			}
			m_csTailTimeReturnShow += _T(" ");
			// 显示本地时间
			iPos = 39;
			for (int p=0; p<FramePacketSize4B; p++)
			{
				strTemp.Format(_T("%02x "),m_ucudp_buf[iPos]);
				iPos++;
				m_csTailTimeReturnShow += strTemp;
			}
			m_csTailTimeReturnShow += _T("\r\n");
			m_uiTailTimeReturnNum++;
		}
	}
	else if (uiPort == TimeSetPort)
	{
		iPos = 28;
		memcpy(&uiCommand, &m_ucudp_buf[iPos], FrameCmdSize1B);
		if (uiCommand == CmdLocalFixedTimeHigh)
		{
			// 显示IP
			iPos = 16;
			for (int j=0; j<FramePacketSize4B; j++)
			{
				strTemp.Format(_T("%02x "),m_ucudp_buf[iPos]);
				iPos++;
				m_csDelayTimeReturnShow += strTemp;
			}
			m_csDelayTimeReturnShow += _T(" ");

			// 显示延时修正高位
			iPos = 29;
			for (int k=0; k<FramePacketSize4B; k++)
			{
				strTemp.Format(_T("%02x "),m_ucudp_buf[iPos]);
				iPos++;
				m_csDelayTimeReturnShow += strTemp;
			}
			m_csDelayTimeReturnShow += _T(" ");

			// 显示延时修正低位
			iPos = 34;
			for (int p=0; p<FramePacketSize4B; p++)
			{
				strTemp.Format(_T("%02x "),m_ucudp_buf[iPos]);
				iPos++;
				m_csDelayTimeReturnShow += strTemp;
			}
			m_csDelayTimeReturnShow += _T("\r\n");
			m_uiDelayTimeReturnNum++;
		}
		else
		{
			CString strtemp2 = _T("");
			strTemp.Format(_T("解析时统设置应答帧出错！\r\n"));
			for (int i=0; i<RcvFrameSize; i++)
			{
				strtemp2.Format(_T("%02x "), m_ucudp_buf[i]);
				strTemp += strtemp2;
			}
			m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnPortMonitoringProc"), strTemp, ErrorStatus);
		}
	}
	else if (uiPort == ADSetReturnPort)
	{
		// 每次采集只有一个ADC设置应答帧
		m_uiADCSetReturnNum++;
	}
	else if (uiPort == QueryErrorCodePort)
	{
		unsigned int uiIP = 0;
		unsigned int uiInstrumentNb = 0;
		CString str = _T("");
		// 显示设备序号
		iPos = 16;
		memcpy(&uiIP, &m_ucudp_buf[iPos], FramePacketSize4B);
		for (unsigned int k=0; k<InstrumentMaxCount; k++)
		{
			if (uiIP == (IPSetAddrStart + k * IPSetAddrInterval))
			{
				uiInstrumentNb = k;
				m_uiErrorCodeReturnCount[uiInstrumentNb]++;
				break;
			}
		}
		strTemp.Format(_T("%d"), uiInstrumentNb);
		str += strTemp;
		iPos = 33;
		memcpy(&uiCommand, &m_ucudp_buf[iPos], FrameCmdSize1B);

		iPos += FrameCmdSize1B;
		str += _T("\t");
		for (int j=0; j<FramePacketSize4B; j++)
		{
			strTemp.Format(_T("%02x "), m_ucudp_buf[iPos]);
			iPos++;
			if (uiCommand == CmdLAUXErrorCode1)
			{
				if (iPos == 36)
				{
					iPos = 39;
				}
			}
			str += strTemp;
		}
		str += _T("\t");
		iPos = 34;
		for (int j=0; j<FramePacketSize4B; j++)
		{
			if (m_uiErrorCodeReturnCount[uiInstrumentNb] == 1)
			{
				strTemp.Format(_T("%02x "), m_ucudp_buf[iPos]);
			}
			else
			{
				if (m_ucudp_buf[iPos] < m_ucErrorCodeReturn[uiInstrumentNb][j])
				{
					uiTemp = 256 + m_ucudp_buf[iPos] - m_ucErrorCodeReturn[uiInstrumentNb][j];
				}
				else
				{
					uiTemp = m_ucudp_buf[iPos] - m_ucErrorCodeReturn[uiInstrumentNb][j] ;
				}
				if (j == 0)
				{
					m_uiDataErrorCount[uiInstrumentNb] += uiTemp;
				}
				else if (j == 1)
				{
					m_uiCmdErrorCount[uiInstrumentNb] += uiTemp;
				}
				strTemp.Format(_T("%02x "), uiTemp);
			}
			m_ucErrorCodeReturn[uiInstrumentNb][j] = m_ucudp_buf[iPos];
			iPos++;
			if (uiCommand == CmdLAUXErrorCode1)
			{
				if (iPos == 36)
				{
					iPos = 39;
				}
			}
			str += strTemp;
		}
		if(!((uiCommand == CmdFDUErrorCode) || (uiCommand == CmdLAUXErrorCode1)))
		{
			CString strtemp2 = _T("");
			strTemp.Format(_T("解析误码查询应答帧出错！\r\n"));
			for (int i=0; i<RcvFrameSize; i++)
			{
				strtemp2.Format(_T("%02x "), m_ucudp_buf[i]);
				strTemp += strtemp2;
			}
			m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnPortMonitoringProc"), strTemp, ErrorStatus);
		}
		// 暂时不在日志文件中写入每次误码查询结果
// 		strTemp = _T("误码查询结果为：") + str;
// 		m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnPortMonitoringProc"), strTemp, SuccessStatus);
		// 把误码查询结果由界面显示改为写入文件
//		m_csErrorCodeReturnShow += str + _T("\r\n");
		m_uiErrorCodeReturnNum++;
	}
	else if (uiPort == CollectSysTimePort)
	{
		m_uiCollectSysTimeReturnNum++;
	}
	else if (uiPort == ADRecPort)
	{
		m_uiADCRecNum++;
	}
	else
	{
		CString strtemp2 = _T("");
		strTemp.Format(_T("解析端口监视接收帧端口出错！\r\n"));
		for (int i=0; i<RcvFrameSize; i++)
		{
			strtemp2.Format(_T("%02x "), m_ucudp_buf[i]);
			strTemp += strtemp2;
		}
		m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnPortMonitoringProc"), strTemp, ErrorStatus);
	}
	m_uiRecFrameNum++;
	if (m_bPortDistribution == TRUE)
	{
		m_SendToAddr.sin_family = AF_INET;											// 填充套接字地址结构
		m_SendToAddr.sin_port = htons(uiPort);
		m_SendToAddr.sin_addr.S_un.S_addr = inet_addr(ConvertCStringToConstCharPointer(m_csIP));

		// 开启端口分发功能
		int icount = sendto(m_SendSocket, reinterpret_cast<const char*>(&m_ucudp_buf), RcvFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		if (icount == SOCKET_ERROR)
		{
			CString str = _T("");
			int iError = WSAGetLastError();
			str.Format(_T("端口监视发送线程sendto出错, 错误代码为%d"), iError);
			m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnPortMonitoringProc"), str, ErrorStatus);
		}
	}
	else
	{
		m_SendToAddr.sin_family = AF_INET;											// 填充套接字地址结构
		m_SendToAddr.sin_port = htons(static_cast<unsigned short>(m_iSendPort));
		m_SendToAddr.sin_addr.S_un.S_addr = inet_addr(ConvertCStringToConstCharPointer(m_csIP));

		int icount = sendto(m_SendSocket, reinterpret_cast<const char*>(&m_ucudp_buf), RcvFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
		if (icount == SOCKET_ERROR)
		{
			CString str = _T("");
			int iError = WSAGetLastError();
			str.Format(_T("端口监视发送线程sendto出错, 错误代码为%d"), iError);
			m_pLogFile->OnWriteLogFile(_T("CPortMonitoringSendThread::OnPortMonitoringProc"), str, ErrorStatus);
		}
	}
	m_pSaveFile->OnSaveSendData(m_ucudp_buf,RcvFrameSize);
}
// 关闭UDP套接字
void CPortMonitoringSendThread::OnCloseUDP(void)
{
	shutdown(m_SendSocket, SD_BOTH);
	closesocket(m_SendSocket);
	m_SendSocket = INVALID_SOCKET;
}
