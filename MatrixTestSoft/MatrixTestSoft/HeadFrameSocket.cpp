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
, m_pInstrumentList(NULL)
, m_pwnd(NULL)
{
}

CHeadFrameSocket::~CHeadFrameSocket()
{
	if (m_pInstrumentList != NULL)
	{
		m_pInstrumentList = NULL;
		delete m_pInstrumentList;
	}
	if (m_pwnd != NULL)
	{
		m_pwnd = NULL;
		delete m_pwnd;
	}
}

// CHeadFrameSocket 成员函数

void CHeadFrameSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	unsigned short uiPort = 0;
	ret = Receive(m_pHeadFrameData,RcvFrameSize);

	if(ret == RcvFrameSize) 
	{
		// 单个首包处理
		ProcHeadFrameOne();
	}

	CSocket::OnReceive(nErrorCode);
}
// 处理单个首包
//************************************
// Method:    ProcHeadFrameOne
// FullName:  CHeadFrameSocket::ProcHeadFrameOne
// Access:    protected 
// Returns:   void
// Qualifier:
//************************************
void CHeadFrameSocket::ProcHeadFrameOne()
{
	if (ParseHeadFrame() == FALSE)
	{
		return;
	}
	// 新仪器指针为空
	CInstrument* pInstrument = NULL;
	// 如果在索引表中没找到该仪器
	if (TRUE != m_pInstrumentList->IfIndexExistInMap(m_uiSN))
	{
		// 得到新仪器
		pInstrument = m_pInstrumentList->GetFreeInstrument();
		if (pInstrument != NULL)
		{
			// 设置新仪器的SN
			pInstrument->m_uiSN = m_uiSN;
			// 设置新仪器的路由IP地址
			pInstrument->m_uiRoutAddress = m_uiRoutAddress;

			// 设置新仪器的仪器类型
			if (m_uiRoutAddress == 0)
			{
				pInstrument->m_uiInstrumentType = InstrumentTypeLAUX;
			}
			else
			{
				pInstrument->m_uiInstrumentType = InstrumentTypeFDU;
			}
			//		pInstrument->m_uiInstrumentType = m_uiSN % 4;
			//		TRACE2(_T("仪器SN：%d	仪器类型%d\r\n"), m_uiSN, pInstrument->m_uiInstrumentType);

			// 设置新仪器的首包时刻，首包时刻不变
			pInstrument->m_uiHeadFrameTime = m_uiHeadFrameTime;

			// 新仪器加入SN索引表
			m_pInstrumentList->AddInstrumentToMap(pInstrument->m_uiSN, pInstrument);
		}
		else
		{
			return;
		}
	}
	else
	{
		// 在索引表中则找到该仪器,得到该仪器指针
		if (TRUE == m_pInstrumentList->GetInstrumentFromMap(m_uiSN, pInstrument))
		{
			// 设置新仪器的首包时刻，首包时刻不变
			pInstrument->m_uiHeadFrameTime = m_uiHeadFrameTime;
			m_pInstrumentList->SetInstrumentLocation(pInstrument);
		}
	}

	// 设置仪器IP地址，如果是交叉站还需设置路由地址
	if (pInstrument->m_iHeadFrameCount == HeadFrameCount)
	{
		pInstrument->m_iHeadFrameCount = 0;
		pInstrument->m_uiIPAddress = IPSetAddrStart + (pInstrument->m_uiLocation)*IPSetAddrInterval;
		TRACE(_T("仪器SN%04x,"), pInstrument->m_uiSN);
		TRACE(_T("仪器IP地址%d\r\n"), pInstrument->m_uiIPAddress);
		if(InstrumentTypeLAUX == pInstrument->m_uiInstrumentType)	// 仪器类型 1-交叉站；2-电源站；3-采集站
		{
			MakeIPSetFrame(pInstrument, TRUE);
			SendIPSetFrame();
		}
		// IP地址设置应答监测，如未收到应答则再发设置帧
// 		KillTimer(m_pwnd->m_hWnd, TabSampleIPSetMonitorTimerNb);
// 		SetTimer(m_pwnd->m_hWnd, TabSampleIPSetMonitorTimerNb, TabSampleIPSetMonitorTimerSet, NULL);
	}
}

// 解析首包
BOOL CHeadFrameSocket::ParseHeadFrame(void)
{
	unsigned char	ucCommand = 0;
	unsigned short usCRC16 = 0;
	int iPos = 0;

	memcpy(&usCRC16, &m_pHeadFrameData[RcvFrameSize - CRCSize], CRCSize);
	if (usCRC16 != get_crc_16(&m_pHeadFrameData[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
	{
//		return FALSE;
	}

	// 仪器SN号
	iPos = 28;
	memcpy(&ucCommand, &m_pHeadFrameData[iPos], FrameCmdSize1B);
	iPos += FrameCmdSize1B;
	if (ucCommand != CmdSn)
	{
		return FALSE;
	}
	memcpy(&m_uiSN, &m_pHeadFrameData[iPos], FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 仪器首包时刻
	memcpy(&ucCommand, &m_pHeadFrameData[iPos], FrameCmdSize1B);
	iPos += FrameCmdSize1B;
	if (ucCommand != CmdHeadFrameTime)
	{
		return FALSE;
	}
	memcpy(&m_uiHeadFrameTime, &m_pHeadFrameData[iPos], FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 路由IP地址
	// 新版硬件程序
	iPos += FrameCmdSize1B;
	iPos += FramePacketSize4B;
//	memcpy(&usCommand, &m_pHeadFrameData[iPos], FrameCmdSize1B);
	// 旧版硬件程序
	memcpy(&ucCommand, &m_pHeadFrameData[iPos], FrameCmdSize1B);
	iPos += FrameCmdSize1B;
	if (ucCommand == 0x00)
	{
		m_uiRoutAddress = 0;
	}
	else if (ucCommand == CmdReturnRout)
	{
		memcpy(&m_uiRoutAddress, &m_pHeadFrameData[iPos], FramePacketSize4B);
		iPos += FramePacketSize4B;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

// 生成IP地址设置帧
void CHeadFrameSocket::MakeIPSetFrame(CInstrument* pInstrument, BOOL bSetIP)
{
	unsigned int	uiIPSource = 0;
	unsigned int	uiIPAim = 0;
	unsigned short	usPortAim = 0;
	unsigned short	usCommand = 0;
	unsigned int	uiRoutIPTop = 0;
	unsigned int	uiRoutIPDown = 0;
	unsigned int	uiRoutIPLeft = 0;
	unsigned int	uiRoutIPRight = 0;
	unsigned int	uiADCBroadcastPort = 0;
	unsigned short usCRC16 = 0;
	int iPos = 0;
	memset(m_pIPSetFrameData, SndFrameBufInit, SndFrameSize);
	m_pIPSetFrameData[0] = FrameHeadCheck0;
	m_pIPSetFrameData[1] = FrameHeadCheck1;
	m_pIPSetFrameData[2] = FrameHeadCheck2;
	m_pIPSetFrameData[3] = FrameHeadCheck3;
	memset(&m_pIPSetFrameData[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	// CString转换为const char*
	char pach[100];
	CStringW strw;
	wstring wstr;
	strw = m_csIPSource;
	wstr = strw;
	string mstring = WideCharToMultiChar(wstr );
	strcpy_s( pach, sizeof(pach), mstring.c_str() );

	// 源IP地址
	uiIPSource	=	inet_addr(pach);
	// 目标端口号
	usPortAim	=	IPSetPort;

	if (bSetIP == TRUE)
	{
		// 目标IP地址
		uiIPAim		=	BroadCastPort;
		// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
		usCommand	=	SendSetCmd;
	}
	else
	{
		// 目标IP地址
		uiIPAim		=	pInstrument->m_uiIPAddress;
		// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
		usCommand	=	SendQueryCmd;
	}

	uiRoutIPTop		=	RoutIPTop;
	uiRoutIPDown	=	RoutIPDown;
	uiRoutIPLeft		=	RoutIPLeft;
	uiRoutIPRight		=	RoutIPRight;
	// 源IP地址
	iPos = 16;
	memcpy(&m_pIPSetFrameData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标IP地址
	memcpy(&m_pIPSetFrameData[iPos], &uiIPAim, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标端口号
	memcpy(&m_pIPSetFrameData[iPos], &usPortAim, FramePacketSize2B);
	iPos += FramePacketSize2B;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pIPSetFrameData[iPos], &usCommand, FramePacketSize2B);
	iPos += FramePacketSize2B;

	iPos = 32;
	// 仪器串号设置命令字
	m_pIPSetFrameData[iPos] = CmdSn;
	iPos += FrameCmdSize1B;
	// 仪器SN号
	memcpy(&m_pIPSetFrameData[iPos], &pInstrument->m_uiSN, FramePacketSize4B);
	iPos += FramePacketSize4B;

	// 仪器本地站点IP地址设置命令字
	m_pIPSetFrameData[iPos] = CmdLocalIPAddr;
	iPos += FrameCmdSize1B;
	// 仪器本地IP
	memcpy(&m_pIPSetFrameData[iPos], &pInstrument->m_uiIPAddress, FramePacketSize4B);
	iPos += FramePacketSize4B;
	if (bSetIP == TRUE)
	{
		if(InstrumentTypeLAUX != pInstrument->m_uiInstrumentType)	// 仪器类型 1-交叉站；2-电源站；3-采集站
		{
			// 设置广播端口
			m_pIPSetFrameData[iPos] = CmdSetBroadCastPort;
			iPos += FrameCmdSize1B;
			// 广播命令，端口66666666
			uiADCBroadcastPort = ADCSetBroadcastPort;     
			memcpy(&m_pIPSetFrameData[iPos], &uiADCBroadcastPort, FramePacketSize4B);
			iPos += FramePacketSize4B;
			TRACE(_T("设置广播端口%d\r\n"), pInstrument->m_uiIPAddress);
		}
		else	// 需要设置路由IP地址
		{
			// 仪器本地站点IP地址设置命令字
			m_pIPSetFrameData[iPos] = CmdLAUXSetRout;
			iPos += FrameCmdSize1B;
			// 路由IP地址，路由方向 1-上
			memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPTop, FramePacketSize4B);
			iPos += FramePacketSize4B;
			// 仪器本地站点IP地址设置命令字
			m_pIPSetFrameData[iPos] = CmdLAUXSetRout;
			iPos += FrameCmdSize1B;
			// 路由IP地址，路由方向 2-下
			memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPDown, FramePacketSize4B);
			iPos += FramePacketSize4B;
			// 仪器本地站点IP地址设置命令字
			m_pIPSetFrameData[iPos] = CmdLAUXSetRout;
			iPos += FrameCmdSize1B;
			// 路由IP地址，路由方向 3-左
			memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPLeft, FramePacketSize4B);
			iPos += FramePacketSize4B;
			// 仪器本地站点IP地址设置命令字
			m_pIPSetFrameData[iPos] = CmdLAUXSetRout;
			iPos += FrameCmdSize1B;
			// 路由IP地址，路由方向 4-右
			memcpy(&m_pIPSetFrameData[iPos], &uiRoutIPRight, FramePacketSize4B);
			iPos += FramePacketSize4B;
		}
	}
	// 设置命令字结束
	m_pIPSetFrameData[iPos] = SndFrameBufInit;

	usCRC16 = get_crc_16(&m_pIPSetFrameData[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_pIPSetFrameData[SndFrameSize - CRCSize], &usCRC16, CRCSize);
}

// 发送IP地址设置帧
void CHeadFrameSocket::SendIPSetFrame(void)
{
	// 发送帧
	// 需要创建广播端口
	int iCount = SendTo(m_pIPSetFrameData, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
}

// 监测IP地址设置应答帧
//************************************
// Method:    OnMonitorIPSetReturn
// FullName:  CHeadFrameSocket::OnMonitorIPSetReturn
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool CHeadFrameSocket::OnMonitorIPSetReturn(void)
{
	POSITION pos = NULL;				// 位置
	unsigned int uiKey = 0;					// 索引键	
	unsigned int icount = m_pInstrumentList->m_oInstrumentMap.size();
	if (icount == 0)
	{
		return false;
	}
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	for(iter=m_pInstrumentList->m_oInstrumentMap.begin(); iter!=m_pInstrumentList->m_oInstrumentMap.end(); iter++)
	{
		ProcessMessages();
		if (NULL != iter->second)
		{
			if (iter->second->m_bIPSetOK == false)
			{
				if (iter->second->m_uiIPAddress != 0)
				{
					if (iter->second->m_uiIPResetTimes < IPAddrResetTimes)
					{
						// 按IP地址查询应答
						MakeIPSetFrame(iter->second, FALSE);
						SendIPSetFrame();
						// 广播设置IP地址
						MakeIPSetFrame(iter->second, TRUE);
						SendIPSetFrame();
						// 设备IP地址重设次数加一
						iter->second->m_uiIPResetTimes++;
						//						SetTimer(5, 10000, NULL);
						return false;
					}
					else
					{
						CString strtmp = _T("");
						strtmp.Format(_T("设备SN为%04x已连续设置%d次IP地址失败！"), iter->second->m_uiSN, iter->second->m_uiIPResetTimes);
						AfxMessageBox(strtmp);
						return false;
					}
				}
			}
		}
	}
	return true;
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
void CHeadFrameSocket::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}