// TailFrameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TailFrameSocket.h"
#include "Parameter.h"
// CTailFrameSocket
CTailFrame::CTailFrame()
: m_uiSN(0)
, m_uiIPSource(0)
, m_bTailRecLAUX(FALSE)
, m_uiTailRecLAUXCount(0)
, m_bTailRecFDU(FALSE)
, m_uiTailRecFDUCount(0)
, m_pInstrumentList(NULL)
, m_pLogFile(NULL)
, m_TailFrameSocket(INVALID_SOCKET)
, m_uiRoutAddress(0)
, m_uiSysTime(0)
{
}

CTailFrame::~CTailFrame()
{
}


// CTailFrameSocket 成员函数

//************************************
// Method:    OnReceive
// FullName:  CTailFrameSocket::OnReceive
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int nErrorCode
//************************************
void CTailFrame::OnReceive(void)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	sockaddr_in SenderAddr;
	int n = sizeof(SenderAddr);
	ret = recvfrom(m_TailFrameSocket, reinterpret_cast<char*>(&m_pTailFrameData), sizeof(m_pTailFrameData), 0, reinterpret_cast<sockaddr*>(&SenderAddr), &n);
	if(ret == RcvFrameSize) 
	{
		// 单个尾包处理
		ProcTailFrameOne();
	}
	else if (ret == SOCKET_ERROR)
	{
		int iError = 0;
		CString str = _T("");
		iError = WSAGetLastError();
		if (iError != WSAEWOULDBLOCK)
		{
			str.Format(_T("尾包接收帧错误，错误号为%d！"), iError);
			m_pLogFile->OnWriteLogFile(_T("CTailFrame::OnReceive"), str, ErrorStatus);
		}
	}
	else
	{
		CString str = _T("");
		str.Format(_T("尾包接收帧帧长错误，帧长为%d！"), ret);
		m_pLogFile->OnWriteLogFile(_T("CTailFrame::OnReceive"), str, ErrorStatus);
	}
}
// 处理单个尾包
//************************************
// Method:    ProcTailFrameOne
// FullName:  CTailFrameSocket::ProcTailFrameOne
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailFrame::ProcTailFrameOne(void)
{
	if (ParseTailFrame() == FALSE)
	{
		CString str = _T("");
		CString strtemp = _T("");
		str.Format(_T("解析尾包帧出错！\r\n"));
		for (int i=0; i<RcvFrameSize; i++)
		{
			strtemp.Format(_T("%02x "), m_pTailFrameData[i]);
			str += strtemp;
		}
		m_pLogFile->OnWriteLogFile(_T("CTailFrame::ParseTailFrame"), str, ErrorStatus);
		return;
	}
	CInstrument* pInstrument = NULL;
	// 在索引表中则找到该仪器,得到该仪器指针
	if (TRUE == m_pInstrumentList->GetInstrumentFromSNMap(m_uiSN, pInstrument))
	{
		// 清除与该仪器路由方向相同的之前仪器的尾包计数
		m_pInstrumentList->OnClearSameRoutTailCount(pInstrument);
		// 尾包计数器加一
		pInstrument->m_iTailFrameCount++;
		// 判断尾包计数器达到设定值则删除相同路由之后的仪器
		if ((pInstrument->m_iTailFrameCount > TailFrameCount)
			&& (pInstrument->m_bIPSetOK == true))
		{
			// @@@@@@@@@@@@ 暂不考虑收到LCI的尾包情况
			// @@@@@@@@@@@@ 暂不考虑迂回道的尾包情况
			m_pInstrumentList->TailFrameDeleteInstrument(pInstrument);
			pInstrument->m_iTailFrameCount = 0;
// 			// 如果在路由索引表中找不到该仪器则加入索引表
// 			if (FALSE == m_pInstrumentList->IfIndexExistInRoutMap(m_uiRoutAddress))
// 			{
// 				m_pInstrumentList->AddInstrumentToRoutMap(m_uiRoutAddress, pInstrument);
// 			}
// 			else
// 			{ 
// 				CInstrument* pInstrument2 = NULL;
// 				hash_map<unsigned int, CInstrument*>::iterator iter;
// 				if (TRUE == m_pInstrumentList->GetInstrumentFromRoutMap(m_uiRoutAddress, pInstrument2))
// 				{
// 					// 该路由方向的尾包仪器发生变化
// 					if (pInstrument2->m_uiSN != m_uiSN)
// 					{
// 						// 删除路由索引表旧的发尾包的仪器
// 						iter = m_pInstrumentList->m_oInstrumentRoutMap.find(m_uiRoutAddress);
// 						m_pInstrumentList->m_oInstrumentRoutMap.erase(iter);
// 						// 将新发尾包的仪器加入路由索引表
// 						m_pInstrumentList->AddInstrumentToRoutMap(m_uiRoutAddress, pInstrument);
// 					}
// 				}
// 			}
		}
	}
	else
	{
		return;
	}
	if ((pInstrument->m_uiInstrumentType == InstrumentTypeLAUX) || (pInstrument->m_uiInstrumentType == InstrumentTypeLCI))
	{
		m_bTailRecLAUX = TRUE;
		m_uiTailRecLAUXCount++;
	}
	else if ((pInstrument->m_uiInstrumentType == InstrumentTypeFDU) || (pInstrument->m_uiInstrumentType == InstrumentTypeLAUL))
	{
		m_bTailRecFDU = TRUE;
		m_uiTailRecFDUCount++;
	}
}

// 解析尾包
//************************************
// Method:    ParseTailFrame
// FullName:  CTailFrameSocket::ParseTailFrame
// Access:    private 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************
BOOL CTailFrame::ParseTailFrame(void)
{
	byte	usCommand = 0;
	unsigned short usCRC16 = 0;
	CString str = _T("");
	memcpy(&usCRC16, &m_pTailFrameData[RcvFrameSize - CRCSize], CRCSize);
	if (usCRC16 != get_crc_16(&m_pTailFrameData[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
	{
		//	return FALSE;
	}
	int iPos = 33;
	// 仪器SN号
	memcpy(&usCommand, &m_pTailFrameData[iPos], FrameCmdSize1B);
	if (usCommand != CmdSn)
	{
		return FALSE;
	}
	iPos += FrameCmdSize1B;
	memcpy(&m_uiSN, &m_pTailFrameData[iPos], FramePacketSize4B);
	iPos += FramePacketSize4B;

	iPos += FrameCmdSize1B;
	memcpy(&m_uiSysTime, &m_pTailFrameData[iPos], FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 路由
	memcpy(&usCommand, &m_pTailFrameData[iPos], FrameCmdSize1B);
	iPos += FrameCmdSize1B;
	if (usCommand == CmdReturnRout)
	{
		memcpy(&m_uiRoutAddress, &m_pTailFrameData[iPos], FramePacketSize4B);
		iPos += FramePacketSize4B;
	}
	else
	{
		m_uiRoutAddress = 0;
	}

	str.Format(_T("接收到SN为0x%x的仪器的尾包！"), m_uiSN);
	m_pLogFile->OnWriteLogFile(_T("CTailFrameSocket::ParseTailFrame"), str, SuccessStatus);
	return TRUE;
}

// 向仪器发送尾包时刻查询帧
//************************************
// Method:    SendTailTimeFrame
// FullName:  CTailFrameSocket::SendTailTimeFrame
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailFrame::SendTailTimeFrame(void)
{
	CString str = _T("");
	// 发送大线方向的尾包时刻查询帧
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	hash_map<unsigned int, unsigned int>::iterator  iter2;
	for(iter=m_pInstrumentList->m_oInstrumentSNMap.begin(); iter!=m_pInstrumentList->m_oInstrumentSNMap.end(); iter++)
	{
//		ProcessMessages();
		if (NULL != iter->second)
		{
			if (iter->second->m_bIPSetOK == true)
			{
				// 交叉站和LCI则查询大线方向的尾包时刻
				if ((iter->second->m_uiInstrumentType == InstrumentTypeLCI)
					|| (iter->second->m_uiInstrumentType == InstrumentTypeLAUX))
				{
					// 生成仪器尾包时刻查询帧
					MakeTailTimeFrameData(iter->second->m_uiIPAddress, 0);
					// 发送仪器尾包时刻查询帧
					SendTailTimeFrameToSocket();
					str.Format(_T("向IP地址为%d的仪器发送尾包时刻查询帧！"), iter->second->m_uiIPAddress);
					m_pLogFile->OnWriteLogFile(_T("CTailFrameSocket::SendTailTimeFrame"), str, SuccessStatus);
				}
			}
		}
	}
	for(iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
	{
		// 广播发送并将路由地址加入索引表
		// 生成仪器尾包时刻查询帧
		MakeTailTimeFrameData(IPBroadcastAddr, iter2->second);
		// 发送仪器尾包时刻查询帧
		SendTailTimeFrameToSocket();
		str.Format(_T("向路由地址为%d的仪器广播发送尾包时刻查询帧！"), iter2->first);
		m_pLogFile->OnWriteLogFile(_T("CTailFrameSocket::SendTailTimeFrame"), str, SuccessStatus);
	}
}

// 生成尾包时刻查询帧
//************************************
// Method:    MakeTailTimeFrameData
// FullName:  CTailFrameSocket::MakeTailTimeFrameData
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: CInstrument * pInstrument
//************************************
void CTailFrame::MakeTailTimeFrameData(unsigned int uiIPAim, unsigned int uiBroadcastPort)
{
	unsigned int uiIPSource =	0;
	unsigned short usPortAim	=	0;
	unsigned short usCommand	=	0;

	memset(m_pTailTimeSendData, SndFrameBufInit, SndFrameSize);
	m_pTailTimeSendData[0] = FrameHeadCheck0;
	m_pTailTimeSendData[1] = FrameHeadCheck1;
	m_pTailTimeSendData[2] = FrameHeadCheck2;
	m_pTailTimeSendData[3] = FrameHeadCheck3;
	memset(&m_pTailTimeSendData[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	uiIPSource	=	m_uiIPSource;
	usPortAim	=	TailTimeFramePort;
	usCommand	=	SendQueryCmd;
	int iPos = FrameHeadSize;
	// 源IP地址
	memcpy(&m_pTailTimeSendData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标IP地址
	memcpy(&m_pTailTimeSendData[iPos], &uiIPAim, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标端口号
	memcpy(&m_pTailTimeSendData[iPos], &usPortAim, FramePacketSize2B);
	iPos += FramePacketSize2B;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pTailTimeSendData[iPos], &usCommand, FramePacketSize2B);
	iPos += FramePacketSize2B;

	iPos += FramePacketSize4B;
	// 不在主交叉线上则发送广播命令
	if (uiIPAim == IPBroadcastAddr)
	{
		//广播命令
		m_pTailTimeSendData[iPos] = CmdBroadCastPortSet;
		iPos += FrameCmdSize1B;
		//广播命令端口
		memcpy(&m_pTailTimeSendData[iPos], &uiBroadcastPort, FramePacketSize4B);
		iPos += FramePacketSize4B;
	}
	// 命令字0x01读取设备SN
	m_pTailTimeSendData[iPos] = CmdSn;
	iPos += FrameCmdSize1B;
	memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
	iPos += FramePacketSize4B;

	// 命令字0x19读取网络时刻
	m_pTailTimeSendData[iPos] = CmdNetTime;
	iPos += FrameCmdSize1B;
	memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
	iPos += FramePacketSize4B;

	// 命令字0x04读取本地系统时间
	m_pTailTimeSendData[iPos] = CmdLocalSysTime;
	iPos += FrameCmdSize1B;
	memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
	iPos += FramePacketSize4B;

	// 仪器类型	1-交叉站
	if(uiIPAim != IPBroadcastAddr)
	{
		// 命令字0x1B  交叉站大线尾包接收时刻
		m_pTailTimeSendData[iPos] = CmdLineTailRecTimeLAUX;
		iPos += FrameCmdSize1B;
		memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
		iPos += FramePacketSize4B;

		// 命令字0x1c交叉站交叉线尾包接收时刻
		m_pTailTimeSendData[iPos] = CmdLAUTailRecTimeLAUX;
		iPos += FrameCmdSize1B;
		memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
		iPos += FramePacketSize4B;

		// 命令字0x16交叉站尾包发送时刻
		m_pTailTimeSendData[iPos] = CmdTailRecSndTimeLow;
		iPos += FrameCmdSize1B;
		memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
		iPos += FramePacketSize4B;
	}
	else
	{
		// 命令字0x16接收、发送时刻低位
		m_pTailTimeSendData[iPos] = CmdTailRecSndTimeLow;
		iPos += FrameCmdSize1B;
		memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
		iPos += FramePacketSize4B;
	}

	// 设置命令字结束
	m_pTailTimeSendData[iPos] = SndFrameBufInit;

	unsigned short usCRC16 = 0;
	usCRC16 = get_crc_16(&m_pTailTimeSendData[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_pTailTimeSendData[SndFrameSize - CRCSize], &usCRC16, CRCSize);
}

// 发送尾包时刻查询帧
//************************************
// Method:    SendTailTimeFrameToSocket
// FullName:  CTailFrameSocket::SendTailTimeFrameToSocket
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailFrame::SendTailTimeFrameToSocket(void)
{
	// 发送帧
	sendto(m_TailFrameSocket, reinterpret_cast<const char*>(&m_pTailTimeSendData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
}
// 防止程序在循环中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CTailFrameSocket::ProcessMessages
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailFrame::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

// 关闭UDP套接字
void CTailFrame::OnCloseUDP(void)
{
	shutdown(m_TailFrameSocket, SD_BOTH);
	closesocket(m_TailFrameSocket);
	m_TailFrameSocket = INVALID_SOCKET;
	m_olsTailTimeJobRoutQueue.clear();
}

// 生成尾包时刻工作队列
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void CTailFrame::OnMakeTailTimeJobRoutQueue(void)
{
	CInstrument* pInstrumentLCI = NULL;
	CInstrument* pInstrument = NULL;
	m_olsTailTimeJobRoutQueue.clear();
	// 找到主交叉站指针
	if (TRUE == m_pInstrumentList->GetInstrumentFromIPMap(IPSetAddrStart, pInstrumentLCI))
	{
		// 如果主交叉站主交叉线方向上方有仪器则加入路由工作序列
		if (pInstrumentLCI->m_pInstrumentTop != NULL)
		{
			m_olsTailTimeJobRoutQueue.push_back(pInstrumentLCI->m_uiRoutAddressTop);
			pInstrument = pInstrumentLCI->m_pInstrumentTop;
			while (pInstrument != NULL)
			{
				OnMakeOneLineJobQueue(pInstrument);
				pInstrument = pInstrument->m_pInstrumentTop;
			}
		}
		// 如果主交叉站主交叉线方向下方有仪器则加入路由工作序列
		if (pInstrumentLCI->m_pInstrumentDown != NULL)
		{
			m_olsTailTimeJobRoutQueue.push_back(pInstrumentLCI->m_uiRoutAddressDown);
			pInstrument = pInstrumentLCI->m_pInstrumentDown;
			while (pInstrument != NULL)
			{
				OnMakeOneLineJobQueue(pInstrument);
				pInstrument = pInstrument->m_pInstrumentDown;
			}
		}
		OnMakeOneLineJobQueue(pInstrumentLCI);
	}
}

// 生成一条测线的路由工作队列
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void CTailFrame::OnMakeOneLineJobQueue(CInstrument* pInstrument)
{
	CInstrument* pInstrumentRight = NULL;
	pInstrumentRight = pInstrument;
	while(pInstrumentRight != NULL)
	{
		if ((pInstrumentRight->m_uiInstrumentType == InstrumentTypeLCI)
			|| (pInstrumentRight->m_uiInstrumentType == InstrumentTypeLAUX))
		{
			m_olsTailTimeJobRoutQueue.push_back(pInstrumentRight->m_uiRoutAddressRight);
		}
	}
}
