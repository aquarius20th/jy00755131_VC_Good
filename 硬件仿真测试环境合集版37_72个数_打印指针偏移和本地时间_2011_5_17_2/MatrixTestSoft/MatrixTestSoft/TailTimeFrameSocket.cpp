// TailTimeFrameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TailTimeFrameSocket.h"
#include "Parameter.h"

// CTailTimeFrameSocket

CTailTimeFrameSocket::CTailTimeFrameSocket()
: m_uiSN(0)
, m_uiNetTime(0)
, m_uiSystemTime(0)
, m_usTailRecTime(0)
, m_usTailSendTime(0)
, m_uiTimeHigh(0)
, m_uiTimeLow(0)
, m_uiDelayTimeCount(0)
, m_csIPSource(_T(""))
, m_uiSendPort(0)
{
}

CTailTimeFrameSocket::~CTailTimeFrameSocket()
{
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
void CTailTimeFrameSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà

	int ret=0;
	unsigned short uiPort = 0;
	ret = Receive(m_pTailTimeRecData,RcvFrameSize);

	if(ret == RcvFrameSize) 
	{
		// 单个尾包时刻查询处理
		ProcTailTimeFrameOne();
	}

	CSocket::OnReceive(nErrorCode);
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
void CTailTimeFrameSocket::ProcTailTimeFrameOne(void)
{
	if (PraseTailTimeFrame() == FALSE)
	{
		return;
	}
	if (m_uiRecTailTimeFrameCount == m_uiSendTailTimeFrameCount)
	{
		m_uiSendTailTimeFrameCount = 0;
		m_uiRecTailTimeFrameCount = 0;
		// 完成时统
		TimeDelayCalculation();
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
BOOL CTailTimeFrameSocket::PraseTailTimeFrame(void)
{
	byte	usCommand = 0;
	int iPos = 28;
	unsigned short usCRC16 = 0;
	memcpy(&usCRC16, &m_pTailTimeRecData[RcvFrameSize - CRCSize], CRCSize);
	if (usCRC16 != get_crc_16(&m_pTailTimeRecData[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
	{
		//	return FALSE;
	}
	// 仪器SN号
	memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
	iPos += FrameCmdSize1B;
	if (usCommand != CmdSn)
	{
		return FALSE;
	}
	memcpy(&m_uiSN, &m_pTailTimeRecData[iPos], FramePacketSize4B);
	iPos = iPos + FramePacketSize4B;

	// 新仪器指针为空
	CInstrument* pInstrument = NULL;
	// 在索引表中则找到该仪器,得到该仪器指针
	if (TRUE == m_pInstrumentList->GetInstrumentFromMap(m_uiSN, pInstrument))
	{	
		// 仪器网络时刻
		memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
		iPos += FrameCmdSize1B;
		if (usCommand != CmdNetTime)
		{
			return FALSE;
		}
		memcpy(&m_uiNetTime, &m_pTailTimeRecData[iPos], FramePacketSize4B);
		iPos = iPos + FramePacketSize4B;

		// 仪器本地系统时间
		memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
		iPos += FrameCmdSize1B;
		if (usCommand != CmdLocalSysTime)
		{
			return FALSE;
		}
		memcpy(&m_uiSystemTime, &m_pTailTimeRecData[iPos], FramePacketSize4B);
		iPos = iPos + FramePacketSize4B;

		// 仪器类型	1-交叉站
		if(InstrumentTypeLAUX == pInstrument->m_uiInstrumentType)
		{
			// 命令字0x1B  交叉站大线尾包接收时刻
			memcpy(&usCommand, &m_pTailTimeRecData[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (usCommand != CmdTailRecTimeLAUX)
			{
				return FALSE;
			}
			memcpy(&m_usTailRecTime, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;
			memcpy(&m_usTailSendTime, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;
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
			memcpy(&m_usTailRecTime, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;
			memcpy(&m_usTailSendTime, &m_pTailTimeRecData[iPos], FramePacketSize2B);
			iPos = iPos + FramePacketSize2B;
		}
		pInstrument->m_uiNetTime = m_uiNetTime;
		pInstrument->m_uiSystemTime = m_uiSystemTime;
		// 低14位为帧内时间
		pInstrument->m_usTailRecTime = m_usTailRecTime & 0x3fff;
		pInstrument->m_usTailSendTime = m_usTailSendTime & 0x3fff;
		// 防止应答帧重复
		if (pInstrument->m_bTailTimeReturnOK == false)
		{
			m_uiRecTailTimeFrameCount++;
		}
		pInstrument->m_bTailTimeReturnOK = true;
		TRACE(_T("接收到的尾包时刻查询帧IP地址：%d\r\n"), pInstrument->m_uiIPAddress);
		TRACE(_T("接收到的尾包时刻查询帧发送时刻%d接收时刻%d\r\n"), m_usTailSendTime,m_usTailRecTime);
		TRACE(_T("接收到的尾包时刻查询帧本地时刻%d\r\n"), m_uiSystemTime);
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
void CTailTimeFrameSocket::TimeDelayCalculation(void)
{
	POSITION pos = NULL;	// 位置	
	unsigned int uiKey = 0;	// 索引键
	unsigned int uiInstrumentCount = 0;
	unsigned short usSendTailTime = 0;
	unsigned short usRecTailTime = 0;
	unsigned int uiNetTime = 0;

	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	for(iter=m_pInstrumentList->m_oInstrumentMap.begin(); iter!=m_pInstrumentList->m_oInstrumentMap.end(); iter++)
	{
		ProcessMessages();
		if (NULL != iter->second)
		{
			if (iter->second->m_bTailTimeReturnOK == true)
			{
				m_oInstrumentLocationMap.insert(hash_map<unsigned int, CInstrument*>::value_type (iter->second->m_uiLocation, iter->second));
			}
			iter->second->m_bSendTailTimeFrame = false;
			iter->second->m_bTailTimeReturnOK = false;
		}
	}
	uiInstrumentCount = m_oInstrumentLocationMap.size();
	// 只有交叉站则不需做时延设置
	if (uiInstrumentCount == 1)
	{
		m_oInstrumentLocationMap.clear();
		return;
	}
	// 记录交叉站接收大线方向尾包的接收时刻
	iter = m_oInstrumentLocationMap.find(0);
/*	m_oInstrumentLocationMap.Lookup(0, pInstrument);*/
	usRecTailTime = iter->second->m_usTailRecTime;
	uiNetTime = iter->second->m_uiNetTime;
	m_uiTimeLow = 0;
	m_uiTimeHigh = 0;

	int temp1 = 0;
	int temp2 = 0;
	for (unsigned int i=0; i<uiInstrumentCount;i++)
	{
		ProcessMessages();
		iter = m_oInstrumentLocationMap.find(i);
		if (iter == m_oInstrumentLocationMap.end())
		{
			break;
		}
		
		if(i > 0)
		{
			temp1 = usRecTailTime - iter->second->m_usTailSendTime;
			usRecTailTime = iter->second->m_usTailRecTime;
		}
		temp1 = temp1&0x0ff;
		temp2 = temp2 + temp1/2 +10;

		if(m_uiDelayTimeCount < 2)
		{
			m_uiTimeHigh = iter->second->m_uiNetTime - iter->second->m_uiSystemTime;
		}
		else
		{
			m_uiTimeHigh = 0x00;
		}
		m_uiDelayTimeCount++;
		m_uiTimeLow = temp2&0x3fff;
		
		// 生成时统报文设置帧
		MakeTimeDelayFrameData(iter->second, m_uiTimeHigh, m_uiTimeLow);
		// 发送时统报文设置帧
		SendTimeDelayFrameData();
	}
	m_oInstrumentLocationMap.clear(); 
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
void CTailTimeFrameSocket::MakeTimeDelayFrameData(CInstrument* pInstrument, unsigned int uiTimeHigh, unsigned int uiTimeLow)
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

	uiIPSource	=	inet_addr(m_csIPSource);
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
	TRACE1("时统设置帧-仪器IP地址：%d\r\n", uiIPAim);
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
void CTailTimeFrameSocket::SendTimeDelayFrameData(void)
{
	int iCount = SendTo(m_pTimeDelayData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
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
void CTailTimeFrameSocket::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

// 设置设备的本地时间
void CTailTimeFrameSocket::OnSetTimeDelay(unsigned int uiTimeHigh, unsigned int uiTimeLow)
{
	POSITION pos = NULL;				// 位置	
	unsigned int uiKey = 0;					// 索引键
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	for(iter=m_pInstrumentList->m_oInstrumentMap.begin(); iter!=m_pInstrumentList->m_oInstrumentMap.end(); iter++)
	{
		ProcessMessages();
		if (NULL != iter->second)
		{
			if (iter->second->m_bIPSetOK == true)
			{
				// 生成时统报文设置帧
				MakeTimeDelayFrameData(iter->second, uiTimeHigh, uiTimeLow);
				// 发送时统报文设置帧
				SendTimeDelayFrameData();
			}
		}
	}
}
