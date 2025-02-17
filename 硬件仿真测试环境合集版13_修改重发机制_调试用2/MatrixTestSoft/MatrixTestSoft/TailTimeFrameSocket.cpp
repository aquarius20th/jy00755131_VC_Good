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

void CTailTimeFrameSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà

	int ret=0;
	unsigned short uiPort = 0;
	ret = Receive(m_pTailTimeRecData,256);

	if(ret == 256) 
	{
		// 单个尾包时刻查询处理
		ProcTailTimeFrameOne();
	}

	CSocket::OnReceive(nErrorCode);
}
// 单个尾包时刻查询处理
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
BOOL CTailTimeFrameSocket::PraseTailTimeFrame(void)
{
	byte	usCommand = 0;
	int iPos = 28;
	// 仪器SN号
	memcpy(&usCommand, &m_pTailTimeRecData[iPos], 1);
	iPos++;
	if (usCommand != 0x01)
	{
		return FALSE;
	}
	memcpy(&m_uiSN, &m_pTailTimeRecData[iPos], 4);
	iPos = iPos + 4;

	// 新仪器指针为空
	CInstrument* pInstrument = NULL;
	// 在索引表中则找到该仪器,得到该仪器指针
	if (TRUE == m_pInstrumentList->GetInstrumentFromMap(m_uiSN, pInstrument))
	{	
		// 没有过期尾包，只有丢失帧
		// 如果是过期的尾包时刻查询帧
		// 		if (pInstrument->m_bTailTimeExpired == true)
		// 		{
		// 			pInstrument->m_bTailTimeExpired = false;
		// 			return FALSE;
		//		}
		// 仪器网络时刻
		memcpy(&usCommand, &m_pTailTimeRecData[iPos], 1);
		iPos++;
		if (usCommand != 0x19)
		{
			return FALSE;
		}
		memcpy(&m_uiNetTime, &m_pTailTimeRecData[iPos], 4);
		iPos = iPos + 4;

		// 仪器本地系统时间
		memcpy(&usCommand, &m_pTailTimeRecData[iPos], 1);
		iPos++;
		if (usCommand != 0x04)
		{
			return FALSE;
		}
		memcpy(&m_uiSystemTime, &m_pTailTimeRecData[iPos], 4);
		iPos = iPos + 4;

		// 仪器类型	1-交叉站
		if(1 == pInstrument->m_uiInstrumentType)
		{
			// 命令字0x1B  交叉站大线尾包接收时刻
			memcpy(&usCommand, &m_pTailTimeRecData[iPos], 1);
			iPos++;
			if (usCommand != 0x1B)
			{
				return FALSE;
			}
			memcpy(&m_usTailRecTime, &m_pTailTimeRecData[iPos], 2);
			iPos = iPos + 2;
			memcpy(&m_usTailSendTime, &m_pTailTimeRecData[iPos], 2);
			iPos = iPos + 2;
		}
		else
		{
			// 命令字0x16接收、发送时刻低位
			memcpy(&usCommand, &m_pTailTimeRecData[iPos], 1);
			iPos++;
			if (usCommand != 0x16)
			{
				return FALSE;
			}
			memcpy(&m_usTailRecTime, &m_pTailTimeRecData[iPos], 2);
			iPos = iPos + 2;
			memcpy(&m_usTailSendTime, &m_pTailTimeRecData[iPos], 2);
			iPos = iPos + 2;
		}
		pInstrument->m_uiNetTime = m_uiNetTime;
		pInstrument->m_uiSystemTime = m_uiSystemTime;
		// 低14位为帧内时间
		pInstrument->m_usTailRecTime = m_usTailRecTime & 0x3fff;
		pInstrument->m_usTailSendTime = m_usTailSendTime & 0x3fff;
		pInstrument->m_bTailTimeReturnOK = true;
		m_uiRecTailTimeFrameCount++;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

// 时间同步计算
void CTailTimeFrameSocket::TimeDelayCalculation(void)
{
	CInstrument* pInstrument = NULL;	// 仪器	
	POSITION pos = NULL;	// 位置	
	unsigned int uiKey;	// 索引键
	unsigned int uiInstrumentCount = 0;
	unsigned short usSendTailTime = 0;
	unsigned short usRecTailTime = 0;
	unsigned int uiNetTime = 0;
	// 得到索引表起始仪器位置
	pos = m_pInstrumentList->m_oInstrumentMap.GetStartPosition();
	// 当前位置有仪器
	while(NULL != pos)
	{
		pInstrument = NULL;
		// 得到仪器
		m_pInstrumentList->m_oInstrumentMap.GetNextAssoc(pos, uiKey, pInstrument);
		if(NULL != pInstrument)	// 得到仪器
		{
			if (pInstrument->m_bTailTimeReturnOK == true)
			{
				m_oInstrumentLocationMap.SetAt(pInstrument->m_uiLocation, pInstrument);
			}
			pInstrument->m_bSendTailTimeFrame = false;
			pInstrument->m_bTailTimeReturnOK = false;
		}
	}
	uiInstrumentCount = m_oInstrumentLocationMap.GetCount();
	// 只有交叉站则不需做时延设置
	if (uiInstrumentCount == 1)
	{
		m_oInstrumentLocationMap.RemoveAll();
		return;
	}
	// 记录交叉站接收大线方向尾包的接收时刻
	m_oInstrumentLocationMap.Lookup(0, pInstrument);
	usRecTailTime = pInstrument->m_usTailRecTime;
	uiNetTime = pInstrument->m_uiNetTime;
	m_uiTimeLow = 0;
	m_uiTimeHigh = 0;
// 	for (unsigned int i=1; i<uiInstrumentCount;i++)
// 	{
// 		if (m_oInstrumentLocationMap.Lookup(i, pInstrument) == FALSE)
// 		{
// 			break;
// 		}
// 		// 高位做修正也只能修正一次，毕竟是查询帧，误差较大，如果连续修正高位反而不准
// 		if (m_uiDelayTimeCount == 0)
// 		{
// 			m_iTimeHigh = m_iTimeHigh + uiNetTime - pInstrument->m_uiNetTime;
// 		}
// 		else
// 		{
// 			m_iTimeHigh = 0;
// 		}
// 		m_iTimeHigh = 0;
// 		//		m_iTimeLow = m_iTimeLow - (usRecTailTime - pInstrument->m_usTailSendTime)/2 - 10;
// 		int temp = usRecTailTime - pInstrument->m_usTailSendTime;
// 		if (temp<0)
// 		{
// 			temp += 0x4000;
// 		}
// 		// 张经理算法
// 		m_iTimeLow = m_iTimeLow -(temp- 4112);
// 		// 赵总算法
// 		//		m_iTimeLow = m_iTimeLow - temp/2 - 10;
// 		usRecTailTime = pInstrument->m_usTailRecTime;
// 		// 生成时统报文设置帧
// 		MakeTimeDelayFrameData(pInstrument);
// 		// 发送时统报文设置帧
// 		SendTimeDelayFrameData();
// 	}
	int temp1 = 0;
	int temp2 = 0;
	for (unsigned int i=0; i<uiInstrumentCount;i++)
	{
		if (m_oInstrumentLocationMap.Lookup(i, pInstrument) == FALSE)
		{
			break;
		}
		
		if(i > 0)
		{
			temp1 = usRecTailTime - pInstrument->m_usTailSendTime;
			usRecTailTime = pInstrument->m_usTailRecTime;
		}
		temp1 = temp1&0x0ff;
		temp2 = temp2 + temp1/2 +10;

		if(m_uiDelayTimeCount < 2)
		{
			m_uiTimeHigh = pInstrument->m_uiNetTime - pInstrument->m_uiSystemTime;
		}
		else m_uiTimeHigh = 0x00;
		m_uiDelayTimeCount++;

		m_uiTimeLow = temp2&0x3fff;
		
		// 生成时统报文设置帧
		MakeTimeDelayFrameData(pInstrument, m_uiTimeHigh, m_uiTimeLow);
		// 发送时统报文设置帧
		SendTimeDelayFrameData();
	}
	m_oInstrumentLocationMap.RemoveAll(); 
}

// 生成时统报文设置帧
void CTailTimeFrameSocket::MakeTimeDelayFrameData(CInstrument* pInstrument, unsigned int uiTimeHigh, unsigned int uiTimeLow)
{
	unsigned int uiIPSource =	0;
	unsigned int uiIPAim	=	0;
	unsigned int usPortAim	=	0;
	unsigned int usCommand	=	0;
	m_pTimeDelayData[0] = 0x11;
	m_pTimeDelayData[1] = 0x22;
	m_pTimeDelayData[2] = 0x33;
	m_pTimeDelayData[3] = 0x44;
	m_pTimeDelayData[4] = 0x00;
	m_pTimeDelayData[5] = 0x00;
	m_pTimeDelayData[6] = 0x00;
	m_pTimeDelayData[7] = 0x00;
	m_pTimeDelayData[8] = 0x00;
	m_pTimeDelayData[9] = 0x00;
	m_pTimeDelayData[10] = 0x00;
	m_pTimeDelayData[11] = 0x00;
	m_pTimeDelayData[12] = 0x00;
	m_pTimeDelayData[13] = 0x00;
	m_pTimeDelayData[14] = 0x00;
	m_pTimeDelayData[15] = 0x00;

	uiIPSource	=	inet_addr(m_csIPSource);
	uiIPAim		=	pInstrument->m_uiIPAddress;
	usPortAim	=	TimeSetPort;
	usCommand	=	1;
	// 源IP地址
	memcpy(&m_pTimeDelayData[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_pTimeDelayData[20], &uiIPAim, 4);
	TRACE1("时统设置帧-仪器IP地址：%d\r\n", uiIPAim);
	// 目标端口号
	memcpy(&m_pTimeDelayData[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pTimeDelayData[26], &usCommand, 2);

	int iPos = 32;
	// 命令字0x05修正时延高位
	m_pTimeDelayData[iPos] = 0x05;
	iPos++;
	memcpy(&m_pTimeDelayData[iPos], &uiTimeHigh, 4);
	iPos = iPos + 4;

	// 命令字0x06修正时延低位
	m_pTimeDelayData[iPos] = 0x06;
	iPos++;
	memcpy(&m_pTimeDelayData[iPos], &uiTimeLow, 4);
	iPos = iPos + 4;

	// 设置命令字结束
	m_pTimeDelayData[iPos] = 0x00;
}

// 发送时统报文设置帧
void CTailTimeFrameSocket::SendTimeDelayFrameData(void)
{
	int iCount = SendTo(m_pTimeDelayData, 128, m_uiSendPort, IPBroadcastAddr);
}