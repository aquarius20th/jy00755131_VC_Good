// TailTimeFrameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TailTimeFrameSocket.h"
#include "Parameter.h"

// CTailTimeFrameSocket

CTailTimeFrame::CTailTimeFrame()
: m_uiIPSource(0)
, m_pInstrumentList(NULL)
, m_pLogFile(NULL)
, m_pWnd(NULL)
, m_TailTimeSocket(INVALID_SOCKET)
{
}

CTailTimeFrame::~CTailTimeFrame()
{
	m_oInstrumentLocationMap.clear(); 
}

// CTailTimeFrameSocket 成员函数

//************************************
// Method:    OnReceive
// FullName:  CTailTimeFrameSocket::OnReceive
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int nErrorCode
//************************************
void CTailTimeFrame::OnReceive(void)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	sockaddr_in SenderAddr;
	int n = sizeof(SenderAddr);
	ret = recvfrom(m_TailTimeSocket, reinterpret_cast<char*>(&m_pTailTimeRecData), sizeof(m_pTailTimeRecData), 0, reinterpret_cast<sockaddr*>(&SenderAddr), &n);
	if(ret == RcvFrameSize) 
	{
		// 单个尾包时刻查询处理
		ProcTailTimeFrameOne();
	}
	else if (ret == SOCKET_ERROR)
	{
		int iError = 0;
		CString str = _T("");
		iError = WSAGetLastError();
		if (iError != WSAEWOULDBLOCK)
		{
			str.Format(_T("尾包时刻查询接收帧错误，错误号为%d！"), iError);
			m_pLogFile->OnWriteLogFile(_T("CTailTimeFrame::OnReceive"), str, ErrorStatus);
		}
	}
	else
	{
		CString str = _T("");
		str.Format(_T("尾包时刻查询接收帧帧长错误，帧长为%d！"), ret);
		m_pLogFile->OnWriteLogFile(_T("CTailTimeFrame::OnReceive"), str, ErrorStatus);
	}
}
// 单个尾包时刻查询处理
//************************************
// Method:    ProcTailTimeFrameOne
// FullName:  CTailTimeFrameSocket::ProcTailTimeFrameOne
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailTimeFrame::ProcTailTimeFrameOne(void)
{
	if (PraseTailTimeFrame() == FALSE)
	{
		CString str = _T("");
		CString strtemp = _T("");
		str.Format(_T("解析尾包时刻查询应答帧出错！\r\n"));
		for (int i=0; i<RcvFrameSize; i++)
		{
			strtemp.Format(_T("%02x "), m_pTailTimeRecData[i]);
			str += strtemp;
		}
		m_pLogFile->OnWriteLogFile(_T("CTailTimeFrame::PraseTailTimeFrame"), str, ErrorStatus);
		return;
	}
	if (true == m_pInstrumentList->OnCheckTailTimeReturn())
	{
		// 停止尾包时刻查询
		KillTimer(m_pWnd->m_hWnd, TabSampleTimeCalTimerNb);
		// 完成时统
		TimeDelayCalculation();
		// 开启ADC参数设置
		SetTimer(m_pWnd->m_hWnd, TabSampleADCSetTimerNb, TabSampleADCSetTimerSet, NULL);
	}
}

// 解析尾包时刻查询帧
//************************************
// Method:    PraseTailTimeFrame
// FullName:  CTailTimeFrameSocket::PraseTailTimeFrame
// Access:    private 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************
BOOL CTailTimeFrame::PraseTailTimeFrame(void)
{
	byte	usCommand = 0;
	int iPos = 28;
	unsigned short usCRC16 = 0;
	unsigned int uiSN = 0;
	unsigned int uiNetTime = 0;
	unsigned int uiSystemTime = 0;
	unsigned short usTailRecTime = 0;
	unsigned short usTailSendTime = 0;
	unsigned short usTailRecTimeLeft = 0;
	unsigned short usTailRecTimeRight = 0;
	unsigned short usTailRecTimeTop = 0;
	unsigned short usTailRecTimeDown = 0;
	CString str = _T("");
	memcpy(&usCRC16, &m_pTailTimeRecData[RcvFrameSize - CRCSize], CRCSize);
	if (usCRC16 != get_crc_16(&m_pTailTimeRecData[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
	{
		//	return FALSE;
	}
	// 仪器SN号
	memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
	iPos += FrameCmdSize1B;
	// 如果为广播命令
	if (usCommand == CmdBroadCastPortSet)
	{
		iPos = iPos + FramePacketSize4B;
		iPos += FrameCmdSize1B;
	}
	memcpy(&uiSN, &m_pTailTimeRecData[iPos], FramePacketSize4B);
	iPos = iPos + FramePacketSize4B;

	// 新仪器指针为空
	CInstrument* pInstrument = NULL;
	// 在索引表中则找到该仪器,得到该仪器指针
	if (TRUE == m_pInstrumentList->GetInstrumentFromSNMap(uiSN, pInstrument))
	{	
		// 仪器网络时刻
		memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
		iPos += FrameCmdSize1B;
		if (usCommand != CmdNetTime)
		{
			return FALSE;
		}
		memcpy(&uiNetTime, &m_pTailTimeRecData[iPos], FramePacketSize4B);
		iPos = iPos + FramePacketSize4B;

		// 仪器本地系统时间
		memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
		iPos += FrameCmdSize1B;
		if (usCommand != CmdLocalSysTime)
		{
			return FALSE;
		}
		memcpy(&uiSystemTime, &m_pTailTimeRecData[iPos], FramePacketSize4B);
		iPos = iPos + FramePacketSize4B;

		pInstrument->m_uiNetTime = uiNetTime;
		pInstrument->m_uiSystemTime = uiSystemTime;
		// 仪器类型	1-交叉站
		if((InstrumentTypeLAUX == pInstrument->m_uiInstrumentType) || (InstrumentTypeLCI == pInstrument->m_uiInstrumentType))
		{
			// 命令字0x1B  交叉站大线尾包接收时刻
			memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (usCommand != CmdLineTailRecTimeLAUX)
			{
				return FALSE;
			}
			memcpy(&usTailRecTimeLeft, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;
			memcpy(&usTailRecTimeRight, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;

			// 低14位为帧内时间
			pInstrument->m_usTailRecTimeLeft = usTailRecTimeLeft & 0x3fff;
			pInstrument->m_usTailRecTimeRight = usTailRecTimeRight & 0x3fff;

			// 命令字0x1C  交叉站交叉线尾包接收时刻
			memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (usCommand != CmdLAUTailRecTimeLAUX)
			{
				return FALSE;
			}
			memcpy(&usTailRecTimeTop, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;
			memcpy(&usTailRecTimeDown, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;

			// 低14位为帧内时间
			pInstrument->m_usTailRecTimeTop = usTailRecTimeTop & 0x3fff;
			pInstrument->m_usTailRecTimeDown = usTailRecTimeDown & 0x3fff;

			// 命令字0x16交叉站尾包发送时刻
			memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (usCommand != CmdTailRecSndTimeLow)
			{
				return FALSE;
			}
			iPos = iPos + FramePacketSize2B;
			memcpy(&usTailSendTime, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;

			// 低14位为帧内时间
			pInstrument->m_usTailSendTime = usTailSendTime & 0x3fff;
		}
		else
		{
			// 命令字0x16接收、发送时刻低位
			memcpy(&usCommand, &m_pTailTimeRecData[iPos], 1);
			iPos += FrameCmdSize1B;
			if (usCommand != CmdTailRecSndTimeLow)
			{
				return FALSE;
			}
			memcpy(&usTailRecTime, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;
			memcpy(&usTailSendTime, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;

			// 低14位为帧内时间
			pInstrument->m_usTailRecTime = usTailRecTime & 0x3fff;
			pInstrument->m_usTailSendTime = usTailSendTime & 0x3fff;
		}

		pInstrument->m_bTailTimeReturnOK = true;
		str.Format(_T("接收到的尾包时刻查询帧IP地址：%d\t发送时刻%d接收时刻%d\t本地时刻%d"), 
			pInstrument->m_uiIPAddress, usTailSendTime, usTailRecTime, uiSystemTime);
		m_pLogFile->OnWriteLogFile(_T("CTailTimeFrameSocket::PraseTailTimeFrame"), str, SuccessStatus);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

// 时间同步计算
//************************************
// Method:    TimeDelayCalculation
// FullName:  CTailTimeFrameSocket::TimeDelayCalculation
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailTimeFrame::TimeDelayCalculation(void)
{
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	hash_map<unsigned int, unsigned int>::iterator  iter2;
	// 交叉线方向时统
	iter = m_pInstrumentList->m_oInstrumentSNMap.find(m_pInstrumentList->m_uiLCISn);
	if (iter->second->m_pInstrumentTop != NULL)
	{
		CInstrument* pInstrument = NULL;
		unsigned int uiLocation = 0;
		pInstrument = iter->second;
		m_oInstrumentLocationMap.clear();

		while (pInstrument != NULL)
		{
			m_oInstrumentLocationMap.insert(hash_map<unsigned int, CInstrument*>::value_type (uiLocation, pInstrument));
			uiLocation++;
			pInstrument = pInstrument->m_pInstrumentTop;
		}
	}
	if (iter->second->m_pInstrumentDown != NULL)
	{
		CInstrument* pInstrument = NULL;
		unsigned int uiLocation = 0;
		pInstrument = iter->second;
		m_oInstrumentLocationMap.clear();

		while (pInstrument != NULL)
		{
			m_oInstrumentLocationMap.insert(hash_map<unsigned int, CInstrument*>::value_type (uiLocation, pInstrument));
			uiLocation++;
			pInstrument = pInstrument->m_pInstrumentDown;
		}
	}
	// 大线方向时统
	for (iter2=m_pInstrumentList->m_oRoutAddrMap.begin(); iter2!=m_pInstrumentList->m_oRoutAddrMap.end(); iter2++)
	{
		m_oInstrumentLocationMap.clear();
		for(iter=m_pInstrumentList->m_oInstrumentSNMap.begin(); iter!=m_pInstrumentList->m_oInstrumentSNMap.end(); iter++)
		{
			if (NULL != iter->second)
			{
				if ((iter->second->m_uiRoutAddress == iter2->first)
					|| (iter->second->m_uiRoutAddressLeft == iter2->first)
					|| (iter->second->m_uiRoutAddressRight == iter2->first))
				{
					m_oInstrumentLocationMap.insert(hash_map<unsigned int, CInstrument*>::value_type (iter->second->m_uiLineRoutLocation, iter->second));
				}
			}
		}
		TRACE(_T("1\r\n"));
		// 完成该路由方向上仪器时统
		TimeDelayCalculationLineRout();
	}
}

// 生成时统报文设置帧
//************************************
// Method:    MakeTimeDelayFrameData
// FullName:  CTailTimeFrameSocket::MakeTimeDelayFrameData
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CInstrument * pInstrument
// Parameter: unsigned int uiTimeHigh
// Parameter: unsigned int uiTimeLow
//************************************
void CTailTimeFrame::MakeTimeDelayFrameData(CInstrument* pInstrument, unsigned int uiTimeHigh, unsigned int uiTimeLow)
{
	unsigned int uiIPSource =	0;
	unsigned int uiIPAim	=	0;
	unsigned int usPortAim	=	0;
	unsigned int usCommand	=	0;
	memset(m_pTimeDelayData, SndFrameBufInit, SndFrameSize);
	m_pTimeDelayData[0] = FrameHeadCheck0;
	m_pTimeDelayData[1] = FrameHeadCheck1;
	m_pTimeDelayData[2] = FrameHeadCheck2;
	m_pTimeDelayData[3] = FrameHeadCheck3;
	memset(&m_pTimeDelayData[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	uiIPSource	= m_uiIPSource;
	uiIPAim		=	pInstrument->m_uiIPAddress;
	usPortAim	=	TimeSetPort;
	usCommand	=	SendSetCmd;
	int iPos = 16;
	// 源IP地址
	memcpy(&m_pTimeDelayData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标IP地址
	memcpy(&m_pTimeDelayData[iPos], &uiIPAim, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标端口号
	memcpy(&m_pTimeDelayData[iPos], &usPortAim, FramePacketSize2B);
	iPos += FramePacketSize2B;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pTimeDelayData[iPos], &usCommand, FramePacketSize2B);

	iPos = 32;
	// 命令字0x05修正时延高位
	m_pTimeDelayData[iPos] = CmdLocalFixedTimeHigh;
	iPos += FrameCmdSize1B;
	memcpy(&m_pTimeDelayData[iPos], &uiTimeHigh, FramePacketSize4B);
	iPos = iPos + FramePacketSize4B;

	// 命令字0x06修正时延低位
	m_pTimeDelayData[iPos] = CmdLocalFixedTimeLow;
	iPos += FrameCmdSize1B;
	memcpy(&m_pTimeDelayData[iPos], &uiTimeLow, FramePacketSize4B);
	iPos = iPos + FramePacketSize4B;

	// 设置命令字结束
	m_pTimeDelayData[iPos] = SndFrameBufInit;

	unsigned short usCRC16 = 0;
	usCRC16 = get_crc_16(&m_pTimeDelayData[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_pTimeDelayData[SndFrameSize - CRCSize], &usCRC16, CRCSize);
}

// 发送时统报文设置帧
//************************************
// Method:    SendTimeDelayFrameData
// FullName:  CTailTimeFrameSocket::SendTimeDelayFrameData
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailTimeFrame::SendTimeDelayFrameData(void)
{
	// 发送帧
	sendto(m_TailTimeSocket, reinterpret_cast<const char*>(&m_pTimeDelayData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
}
// 防止程序在循环中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CTailTimeFrameSocket::ProcessMessages
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTailTimeFrame::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

// 设置设备的本地时间
void CTailTimeFrame::OnSetTimeDelay(unsigned int uiTimeHigh, unsigned int uiTimeLow)
{
	CString str = _T("");
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	for(iter=m_pInstrumentList->m_oInstrumentSNMap.begin(); iter!=m_pInstrumentList->m_oInstrumentSNMap.end(); iter++)
	{
//		ProcessMessages();
		if (NULL != iter->second)
		{
			if (iter->second->m_bIPSetOK == true)
			{
				// 生成时统报文设置帧
				MakeTimeDelayFrameData(iter->second, uiTimeHigh, uiTimeLow);
				// 发送时统报文设置帧
				SendTimeDelayFrameData();
				str.Format(_T("向IP地址为%d，SN为0x%04x的仪器发送时统设置帧，高位为%d，低位为%d"), 
					iter->second->m_uiIPAddress, iter->second->m_uiSN, uiTimeHigh, uiTimeLow);
				m_pLogFile->OnWriteLogFile(_T("CTailTimeFrameSocket::OnSetTimeDelay"), str, SuccessStatus);
			}
		}
	}
}

// 关闭UDP套接字
void CTailTimeFrame::OnCloseUDP(void)
{
	shutdown(m_TailTimeSocket, SD_BOTH);
	closesocket(m_TailTimeSocket);
	m_TailTimeSocket = INVALID_SOCKET;
}

// 处理该大线路由方向的时统
void CTailTimeFrame::TimeDelayCalculationLineRout(void)
{
	// 仪器设备计数
	unsigned int uiInstrumentCount = 0;
	// 尾包接收时刻
	unsigned short usRecTailTime = 0;
	// 仪器大线方向连接方向
	unsigned int uiLineDirection = 0;
	// 时间修正高位
	unsigned int uiTimeHigh = 0;
	// 时间修正低位
	unsigned int uiTimeLow = 0;
	// 交叉站时间修正高位
	unsigned int uiTimeHighLX = 0;
	// 交叉站时间修正低位
	unsigned int uiTimeLowLX = 0;
	CString str = _T("");
	int itmp1 = 0;
	int itmp2 = 0;
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	uiInstrumentCount = m_oInstrumentLocationMap.size();
	// 只有交叉站则不需做时延设置
	if (uiInstrumentCount == 1)
	{
		return;
	}
	iter = m_oInstrumentLocationMap.find(1);
	uiLineDirection = iter->second->m_uiLineDirection;
	// 记录交叉站接收大线方向尾包的接收时刻
	iter = m_oInstrumentLocationMap.find(0);
	uiTimeHighLX = iter->second->m_uiTimeHigh;
	uiTimeLowLX = iter->second->m_uiTimeLow;
	if (uiLineDirection == DirectionLeft)
	{
		usRecTailTime = iter->second->m_usTailRecTimeLeft;
	}
	else if (uiLineDirection == DirectionRight)
	{
		usRecTailTime = iter->second->m_usTailRecTimeRight;
	}
	for (unsigned int i=1; i<uiInstrumentCount;i++)
	{
		iter = m_oInstrumentLocationMap.find(i);
		if (iter == m_oInstrumentLocationMap.end())
		{
			break;
		}
		itmp1 = usRecTailTime - iter->second->m_usTailSendTime;
		// cxm 2010.10.11低位修改校正
		// temp2 = temp2 + temp1/2 + 10;
		itmp2 += itmp1/2 - 12;
		uiTimeLow = itmp2&0xffff;
		// cxm 2010.10.11修正低位即可
		uiTimeHigh = (iter->second->m_uiNetTime - iter->second->m_uiSystemTime) & 0xffffffff;
		uiTimeLow += uiTimeLowLX;
		uiTimeLow &= 0xffff;
		uiTimeHigh += uiTimeHighLX;
		uiTimeHigh &= 0xffffffff;
		// 生成时统报文设置帧
		MakeTimeDelayFrameData(iter->second, uiTimeHigh, uiTimeLow);
		// 发送时统报文设置帧
		SendTimeDelayFrameData();
		str.Format(_T("向IP地址为%d，SN为0x%04x的仪器发送时统设置帧，高位为%d，低位为%d"), 
			iter->second->m_uiIPAddress, iter->second->m_uiSN, uiTimeHigh, uiTimeLow);
		m_pLogFile->OnWriteLogFile(_T("CTailTimeFrameSocket::TimeDelayCalculation"), str, SuccessStatus);
		iter->second->m_uiTimeHigh = uiTimeHigh;
		iter->second->m_uiTimeLow = uiTimeLow;
		usRecTailTime = iter->second->m_usTailRecTime;
	}
}
