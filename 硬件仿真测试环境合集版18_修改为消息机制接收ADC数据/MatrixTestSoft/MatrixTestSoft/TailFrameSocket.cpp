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
, m_bTailRec(TRUE)
{
}

CTailFrameSocket::~CTailFrameSocket()
{
}


// CTailFrameSocket 成员函数

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

		// 发送尾包时刻查询帧
		// 由自动对时改为手动对时
// 		SendTailTimeFrame();
	}
	pInstrument = NULL;
	delete pInstrument;
}

// 解析尾包
BOOL CTailFrameSocket::ParseTailFrame(void)
{
	byte	usCommand = 0;
	unsigned short usCRC16 = 0;
	memcpy(&usCRC16, &m_pTailFrameData[RcvFrameSize - CRCSize], 2);
	if (usCRC16 != get_crc_16(&m_pTailFrameData[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
	{
		//	return FALSE;
	}

	// 仪器SN号
	memcpy(&usCommand, &m_pTailFrameData[33], 1);
	if (usCommand != 0x01)
	{
		return FALSE;
	}
	memcpy(&m_uiSN, &m_pTailFrameData[34], 4);

	return TRUE;
}

// 向仪器发送尾包时刻查询帧
void CTailFrameSocket::SendTailTimeFrame(void)
{
	CInstrument* pInstrument = NULL;	// 仪器	
	POSITION pos = NULL;	// 位置	
	unsigned int uiKey;	// 索引键

	// 得到索引表起始仪器位置
	pos = m_pInstrumentList->m_oInstrumentMap.GetStartPosition();
	// 当前位置有仪器
	while(NULL != pos)
	{
		ProcessMessages();
		pInstrument = NULL;
		// 得到仪器
		m_pInstrumentList->m_oInstrumentMap.GetNextAssoc(pos, uiKey, pInstrument);
		if(NULL != pInstrument)	// 得到仪器
		{
			if (pInstrument->m_bIPSetOK == true)
			{
				// 生成仪器尾包时刻查询帧
				MakeTailTimeFrameData(pInstrument);
				// 发送仪器尾包时刻查询帧
				SendTailTimeFrameToSocket();
				m_uiSendTailTimeFrameCount++;
				pInstrument->m_bSendTailTimeFrame = true;
			}
		}
	}
	pInstrument = NULL;
	delete pInstrument;
}

// 生成尾包时刻查询帧
void CTailFrameSocket::MakeTailTimeFrameData(CInstrument* pInstrument)
{
	unsigned int uiIPSource =	0;
	unsigned int uiIPAim	=	0;
	unsigned int usPortAim	=	0;
	unsigned int usCommand	=	0;
	for(int i=0; i<SndFrameSize; i++)
	{
		m_pTailTimeSendData[i] = 0x00;
	}
	m_pTailTimeSendData[0] = 0x11;
	m_pTailTimeSendData[1] = 0x22;
	m_pTailTimeSendData[2] = 0x33;
	m_pTailTimeSendData[3] = 0x44;
	m_pTailTimeSendData[4] = 0x00;
	m_pTailTimeSendData[5] = 0x00;
	m_pTailTimeSendData[6] = 0x00;
	m_pTailTimeSendData[7] = 0x00;
	m_pTailTimeSendData[8] = 0x00;
	m_pTailTimeSendData[9] = 0x00;
	m_pTailTimeSendData[10] = 0x00;
	m_pTailTimeSendData[11] = 0x00;
	m_pTailTimeSendData[12] = 0x00;
	m_pTailTimeSendData[13] = 0x00;
	m_pTailTimeSendData[14] = 0x00;
	m_pTailTimeSendData[15] = 0x00;

	uiIPSource	=	inet_addr(m_csIPSource);
	uiIPAim		=	pInstrument->m_uiIPAddress;
	usPortAim	=	TailTimeFramePort;
	usCommand	=	SendQueryCmd;
	// 源IP地址
	memcpy(&m_pTailTimeSendData[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_pTailTimeSendData[20], &uiIPAim, 4);
	TRACE1("尾包时刻查询帧-仪器IP地址：%d\r\n", uiIPAim);
	// 目标端口号
	memcpy(&m_pTailTimeSendData[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pTailTimeSendData[26], &usCommand, 2);

	int iPos = 32;
	// 命令字0x01读取设备SN
	m_pTailTimeSendData[iPos] = 0x01;
	iPos++;
	memset(&m_pTailTimeSendData[iPos], 0, 4);
	iPos = iPos + 4;

	// 命令字0x19读取网络时刻
	m_pTailTimeSendData[iPos] = 0x19;
	iPos++;
	memset(&m_pTailTimeSendData[iPos], 0, 4);
	iPos = iPos + 4;

	// 命令字0x04读取本地系统时间
	m_pTailTimeSendData[iPos] = 0x04;
	iPos++;
	memset(&m_pTailTimeSendData[iPos], 0, 4);
	iPos = iPos + 4;

	// 仪器类型	1-交叉站
	if(1 == pInstrument->m_uiInstrumentType)
	{
		// 命令字0x1B  交叉站大线尾包接收时刻
		m_pTailTimeSendData[iPos] = 0x1B;
		iPos++;
		memset(&m_pTailTimeSendData[iPos], 0, 4);
		iPos = iPos + 4;
	}
	else
	{
		// 命令字0x16接收、发送时刻低位
		m_pTailTimeSendData[iPos] = 0x16;
		iPos++;
		memset(&m_pTailTimeSendData[iPos], 0, 4);
		iPos = iPos + 4;
	}

	// 设置命令字结束
	m_pTailTimeSendData[iPos] = 0x00;

	unsigned short usCRC16 = 0;
	usCRC16 = get_crc_16(&m_pTailTimeSendData[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_pTailTimeSendData[SndFrameSize - CRCSize], &usCRC16, 2);

}

// 发送尾包时刻查询帧
void CTailFrameSocket::SendTailTimeFrameToSocket(void)
{
	// 发送帧
	int iCount = SendTo(m_pTailTimeSendData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
}
// 防止程序在循环中运行无法响应消息
void CTailFrameSocket::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
