// TailFrameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TailFrameSocket.h"
#include "Parameter.h"

// CTailFrameSocket

CTailFrameSocket::CTailFrameSocket()
: m_uiSN(0)
, m_csIPSource(_T(""))
, m_uiSendPort(0)
, m_bTailRec(FALSE)
, m_uiTailRecCount(0)
, m_bTailCountStart(false)
, m_pInstrumentList(NULL)
{
}

CTailFrameSocket::~CTailFrameSocket()
{
	if (m_pInstrumentList != NULL)
	{
		m_pInstrumentList = NULL;
		delete m_pInstrumentList;
	}
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
void CTailFrameSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	unsigned short uiPort = 0;
	ret = Receive(m_pTailFrameData,RcvFrameSize);

	if(ret == RcvFrameSize) 
	{
		// 单个尾包处理
		ProcTailFrameOne();
	}

	CSocket::OnReceive(nErrorCode);
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
void CTailFrameSocket::ProcTailFrameOne(void)
{
	if (ParseTailFrame() == FALSE)
	{
		return;
	}
	m_bTailRec = TRUE;
	CInstrument* pInstrument = NULL;
	// 在索引表中则找到该仪器,得到该仪器指针
	if (TRUE == m_pInstrumentList->GetInstrumentFromMap(m_uiSN, pInstrument))
	{
		// 如果是交叉站尾包则不处理，如果是采集站尾包
		if (pInstrument->m_uiInstrumentType == 3)
		{
			// 尾包计数器加一
			pInstrument->m_iTailFrameCount++;
			// 如果尾包时刻查询在下一次尾包来临之际还未收全，则放弃上次查询结果并对未收到尾包时刻查询的仪器设为过期标志
			if (m_uiSendTailTimeFrameCount != m_uiRecTailTimeFrameCount)
			{
				m_uiRecTailTimeFrameCount = 0;
				m_uiSendTailTimeFrameCount = 0;
				m_pInstrumentList->ClearExperiedTailTimeResult();
			}
		}
	}
	else
	{
		return;
	}
	// 判断尾包计数器达到设定值
	if (pInstrument->m_iTailFrameCount == TailFrameCount)
	{
		pInstrument->m_iTailFrameCount = 0;
		// 判断采集站位置后面是否还有仪器，有则删除
		m_pInstrumentList->TailFrameDeleteInstrument(pInstrument);
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
BOOL CTailFrameSocket::ParseTailFrame(void)
{
	byte	usCommand = 0;
	unsigned short usCRC16 = 0;
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
void CTailFrameSocket::SendTailTimeFrame(void)
{
// 	POSITION pos = NULL;	// 位置	
// 	unsigned int uiKey = 0;	// 索引键
// 
// 	// hash_map迭代器
// 	hash_map<unsigned int, CInstrument*>::iterator  iter;
// 	for(iter=m_pInstrumentList->m_oInstrumentMap.begin(); iter!=m_pInstrumentList->m_oInstrumentMap.end(); iter++)
// 	{
// 		ProcessMessages();
// 		if (NULL != iter->second)
// 		{
// 			if (iter->second->m_bIPSetOK == true)
// 			{
// 				// 生成仪器尾包时刻查询帧
// 				MakeTailTimeFrameData(iter->second);
// 				// 发送仪器尾包时刻查询帧
// 				SendTailTimeFrameToSocket();
// 				m_uiSendTailTimeFrameCount++;
// 				iter->second->m_bSendTailTimeFrame = true;
// 			}
// 		}
// 	}
	// 生成仪器尾包时刻查询帧
	MakeTailTimeFrameData();
	// 发送仪器尾包时刻查询帧
	SendTailTimeFrameToSocket();
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
void CTailFrameSocket::MakeTailTimeFrameData(void)
{
	unsigned int uiIPSource =	0;
	unsigned int uiIPAim	=	0;
	unsigned short usPortAim	=	0;
	unsigned short usCommand	=	0;
	unsigned char	ucCommand = 0;
	unsigned int	uiADCBroadcastPort = 0;
	memset(m_pTailTimeSendData, SndFrameBufInit, SndFrameSize);
	m_pTailTimeSendData[0] = FrameHeadCheck0;
	m_pTailTimeSendData[1] = FrameHeadCheck1;
	m_pTailTimeSendData[2] = FrameHeadCheck2;
	m_pTailTimeSendData[3] = FrameHeadCheck3;
	memset(&m_pTailTimeSendData[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	// CString转换为const char*
	char pach[100];
	CStringW strw;
	wstring wstr;
	strw = m_csIPSource;
	wstr = strw;
	string mstring = WideCharToMultiChar(wstr );
	strcpy_s( pach, sizeof(pach), mstring.c_str() );

	uiIPSource	=	inet_addr(pach);
	uiIPAim		=	BroadCastPort;
	usPortAim	=	TailTimeFramePort;
	usCommand	=	SendQueryCmd;
	int iPos = FrameHeadSize;
	// 源IP地址
	memcpy(&m_pTailTimeSendData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标IP地址
	memcpy(&m_pTailTimeSendData[iPos], &uiIPAim, FramePacketSize4B);
	TRACE1("发送的尾包时刻查询帧-仪器IP地址：%d\r\n", uiIPAim);
	iPos += FramePacketSize4B;
	// 目标端口号
	memcpy(&m_pTailTimeSendData[iPos], &usPortAim, FramePacketSize2B);
	iPos += FramePacketSize2B;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pTailTimeSendData[iPos], &usCommand, FramePacketSize2B);
	iPos += FramePacketSize2B;

	iPos += FramePacketSize4B;
	
	//广播命令
	ucCommand = CmdBroadCastPortSet;
	memcpy(&m_pTailTimeSendData[iPos], &ucCommand, FrameCmdSize1B);
	iPos += FrameCmdSize1B;
	//广播命令端口
	uiADCBroadcastPort = ADCSetBroadcastPort;
	memcpy(&m_pTailTimeSendData[iPos], &uiADCBroadcastPort, FramePacketSize4B);
	iPos += FramePacketSize4B;

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
	
	// 命令字0x16接收、发送时刻低位
	m_pTailTimeSendData[iPos] = CmdTailRecSndTimeLow;
	iPos += FrameCmdSize1B;
	memset(&m_pTailTimeSendData[iPos], SndFrameBufInit, FramePacketSize4B);
	iPos += FramePacketSize4B;

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
void CTailFrameSocket::SendTailTimeFrameToSocket(void)
{
	// 发送帧
	int iCount = SendTo(m_pTailTimeSendData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
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
void CTailFrameSocket::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

// 监测尾包
//************************************
// Method:    OnTailMonitor
// FullName:  CTailFrameSocket::OnTailMonitor
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CTailFrameSocket::OnTailMonitor(void)
{
	if (m_bTailRec == TRUE)
	{
		m_bTailRec = FALSE;
		m_uiTailRecCount = 0;
		m_bTailCountStart = true;
	}
	if(m_bTailCountStart == true)
	{
		m_uiTailRecCount++;
		if (m_uiTailRecCount == TailFrameCount)
		{
			// 连续5秒未收到尾包则删除所有仪器
			m_pInstrumentList->DeleteAllInstrument();
			m_bTailCountStart = false;
			m_uiTailRecCount = 0;
			return true;
		}
	}
	return false;
}
