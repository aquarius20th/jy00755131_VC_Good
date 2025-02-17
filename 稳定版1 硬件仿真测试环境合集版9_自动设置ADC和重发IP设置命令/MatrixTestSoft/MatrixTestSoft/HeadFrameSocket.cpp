// HeadFrameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "HeadFrameSocket.h"
#include "Parameter.h"

// CHeadFrameSocket

CHeadFrameSocket::CHeadFrameSocket()
: m_uiSN(0)
, m_uiHeadFrameTime(0)
, m_uiRoutAddress(0)
, m_csIPSource(_T(""))
, m_uiSendPort(0)
, m_pwnd(NULL)
{
}

CHeadFrameSocket::~CHeadFrameSocket()
{
}

// CHeadFrameSocket 成员函数

void CHeadFrameSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	unsigned short uiPort = 0;
	ret = Receive(m_pHeadFrameData,256);

	if(ret == 256) 
	{
		// 单个首包处理
		ProcHeadFrameOne();
	}

	CSocket::OnReceive(nErrorCode);
}
// 处理单个首包
void CHeadFrameSocket::ProcHeadFrameOne()
{
	if (ParseHeadFrame() == FALSE)
	{
		return;
	}
	// 新仪器指针为空
	CInstrument* pInstrument = NULL;
	// 如果在索引表中没找到该仪器
	if (TRUE != m_oInstrumentList->IfIndexExistInMap(m_uiSN))
	{
		// 得到新仪器
		pInstrument = m_oInstrumentList->GetFreeInstrument();

		// 设置新仪器的SN
		pInstrument->m_uiSN = m_uiSN;
		// 设置新仪器的路由IP地址
		pInstrument->m_uiRoutAddress = m_uiRoutAddress;

		if (m_uiRoutAddress == 0)
		{
			// 设置新仪器的仪器类型
			pInstrument->m_uiInstrumentType = 1;
		}
		else
		{
			pInstrument->m_uiInstrumentType = 3;
		}
		// 设置新仪器的首包时刻
		pInstrument->m_uiHeadFrameTime = m_uiHeadFrameTime;
		// 首包计数器加一
		pInstrument->m_iHeadFrameCount++;

		// 新仪器加入SN索引表
		m_oInstrumentList->AddInstrumentToMap(pInstrument->m_uiSN, pInstrument);
	}
	else
	{
		// 在索引表中则找到该仪器,得到该仪器指针
		if (TRUE == m_oInstrumentList->GetInstrumentFromMap(m_uiSN, pInstrument))
		{
			// 首包计数器加一
			pInstrument->m_iHeadFrameCount++;
		}
	}

	// 设置仪器IP地址，如果是交叉站还需设置路由地址
	if (pInstrument->m_iHeadFrameCount == HeadFrameCount)
	{
		pInstrument->m_iHeadFrameCount = 0;
		MakeIPSetFrame(pInstrument);
		SendIPSetFrame();
		if (pInstrument->m_uiInstrumentType == 3)
		{
			// IP地址设置应答监测，如未收到应答则再发设置帧
			KillTimer(m_pwnd->m_hWnd, 5);
			SetTimer(m_pwnd->m_hWnd, 5, 10000, NULL);
		}
	}
}

// 解析首包
BOOL CHeadFrameSocket::ParseHeadFrame(void)
{
	byte	usCommand = 0;

	// 仪器SN号
	memcpy(&usCommand, &m_pHeadFrameData[28], 1);
	if (usCommand != 0x01)
	{
		return FALSE;
	}
	memcpy(&m_uiSN, &m_pHeadFrameData[29], 4);
	// 仪器首包时刻
	memcpy(&usCommand, &m_pHeadFrameData[33], 1);
	if (usCommand != 0x02)
	{
		return FALSE;
	}
	memcpy(&m_uiHeadFrameTime, &m_pHeadFrameData[34], 4);
	// 路由IP地址
	memcpy(&usCommand, &m_pHeadFrameData[38], 1);
	if (usCommand == 0x00)
	{
		m_uiRoutAddress = 0;
	}
	else if (usCommand == 0x3F)
	{
		memcpy(&m_uiRoutAddress, &m_pHeadFrameData[39], 4);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

// 生成IP地址设置帧
void CHeadFrameSocket::MakeIPSetFrame(CInstrument* pInstrument)
{
	unsigned int	uiIPSource;
	unsigned int	uiIPAim;
	unsigned short	usPortAim;
	unsigned short	usCommand;
	unsigned int	uiRoutIPTop;
	unsigned int	uiRoutIPDown;
	unsigned int	uiRoutIPLeft;
	unsigned int	uiRoutIPRight;
	unsigned int	uiADCBroadcastPort;
	m_pIPSetFrameData[0] = 0x11;
	m_pIPSetFrameData[1] = 0x22;
	m_pIPSetFrameData[2] = 0x33;
	m_pIPSetFrameData[3] = 0x44;
	m_pIPSetFrameData[4] = 0x00;
	m_pIPSetFrameData[5] = 0x00;
	m_pIPSetFrameData[6] = 0x00;
	m_pIPSetFrameData[7] = 0x00;
	m_pIPSetFrameData[8] = 0x00;
	m_pIPSetFrameData[9] = 0x00;
	m_pIPSetFrameData[10] = 0x00;
	m_pIPSetFrameData[11] = 0x00;
	m_pIPSetFrameData[12] = 0x00;
	m_pIPSetFrameData[13] = 0x00;
	m_pIPSetFrameData[14] = 0x00;
	m_pIPSetFrameData[15] = 0x00;

	// 源IP地址

	uiIPSource	=	inet_addr(m_csIPSource);
	// 目标IP地址
	uiIPAim		=	0xFFFFFFFF;
	// 目标端口号
	usPortAim	=	IPSetPort;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	usCommand	=	1;

	uiRoutIPTop		=	5;
	uiRoutIPDown	=	6;
	uiRoutIPLeft	=	7;
	uiRoutIPRight	=	8;
	// 源IP地址
	memcpy(&m_pIPSetFrameData[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_pIPSetFrameData[20], &uiIPAim, 4);
	// 目标端口号
	memcpy(&m_pIPSetFrameData[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pIPSetFrameData[26], &usCommand, 2);

	int iPos = 32;
	// 仪器串号设置命令字
	m_pIPSetFrameData[iPos] = 0x01;
	iPos++;
	// 仪器SN号
	memcpy(&m_pIPSetFrameData[iPos], &m_uiSN, 4);
	iPos = iPos + 4;

	// 仪器本地站点IP地址设置命令字
	m_pIPSetFrameData[iPos] = 0x03;
	iPos++;
	// 仪器本地IP
	memcpy(&m_pIPSetFrameData[iPos], &pInstrument->m_uiIPAddress, 4);
	iPos = iPos + 4;
	if(1 != pInstrument->m_uiInstrumentType)	// 仪器类型 1-交叉站；2-电源站；3-采集站
	{
		// 设置广播端口
		m_pIPSetFrameData[iPos] = 0x0A;
		iPos++;
		// 广播命令，端口6666
		uiADCBroadcastPort = ADCSetBroadcastPort;     
		memcpy(&m_pIPSetFrameData[iPos], &uiADCBroadcastPort, 4);
		iPos = iPos + 4;
		// 设置命令字结束
		m_pIPSetFrameData[iPos] = 0x00;
	}
	else	// 需要设置路由IP地址
	{
		// 仪器本地站点IP地址设置命令字
		m_pIPSetFrameData[iPos] = 0x1F;
		iPos++;
		// 路由IP地址，路由方向 1-上
		memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPTop, 4);
		iPos = iPos + 4;
		// 仪器本地站点IP地址设置命令字
		m_pIPSetFrameData[iPos] = 0x1F;
		iPos++;
		// 路由IP地址，路由方向 2-下
		memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPDown, 4);
		iPos = iPos + 4;
		// 仪器本地站点IP地址设置命令字
		m_pIPSetFrameData[iPos] = 0x1F;
		iPos++;
		// 路由IP地址，路由方向 3-左
		memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPLeft, 4);
		iPos = iPos + 4;
		// 仪器本地站点IP地址设置命令字
		m_pIPSetFrameData[iPos] = 0x1F;
		iPos++;
		// 路由IP地址，路由方向 4-右
		memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPRight, 4);
		iPos = iPos + 4;
		// 设置命令字结束
		m_pIPSetFrameData[iPos] = 0x00;
	}
}

// 发送IP地址设置帧
void CHeadFrameSocket::SendIPSetFrame(void)
{
	// 发送帧
	// 需要创建广播端口
	int iCount = SendTo(m_pIPSetFrameData, 128, m_uiSendPort, IPBroadcastAddr);
}
