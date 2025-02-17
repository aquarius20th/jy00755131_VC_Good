// IPSetSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "IPSetSocket.h"
#include "Parameter.h"


// CIPSetSocket

CIPSetSocket::CIPSetSocket()
: m_uiSN(0)
, m_uiIPAddress(0)
, m_pwnd(NULL)
, m_uiSendPort(0)
, m_pSelectObject(NULL)
{
}

CIPSetSocket::~CIPSetSocket()
{
}


// CIPSetSocket 成员函数

void CIPSetSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	unsigned short uiPort = 0;
	ret = Receive(m_pIPSetReturnFrameData,RcvFrameSize);

	if(ret == RcvFrameSize) 
	{
		// 单个IP地址设置应答帧处理
		ProcIPSetReturnFrameOne();
	}

	CSocket::OnReceive(nErrorCode);
}
// 单个IP地址设置应答帧处理
void CIPSetSocket::ProcIPSetReturnFrameOne(void)
{
	if (ParseIPSetReturnFrame() == FALSE)
	{
		return;
	}
	// 在索引表中找到该仪器
	if (TRUE == m_pInstrumentList->IfIndexExistInMap(m_uiSN))
	{
		// 在索引表中找到该仪器,得到该仪器指针
		CInstrument* pInstrument = NULL;
		if (TRUE == m_pInstrumentList->GetInstrumentFromMap(m_uiSN, pInstrument))
		{
			if (m_uiIPAddress == pInstrument->m_uiIPAddress)
			{
				pInstrument->m_bIPSetOK = true;
				OnShowConnectedIcon(pInstrument->m_uiIPAddress);
			}
		}
		pInstrument = NULL;
		delete pInstrument;
	}

}

// 解析IP地址设置应答帧
BOOL CIPSetSocket::ParseIPSetReturnFrame(void)
{
	byte	usCommand = 0;
	unsigned short usCRC16 = 0;
	memcpy(&usCRC16, &m_pIPSetReturnFrameData[RcvFrameSize - CRCSize], 2);
	if (usCRC16 != get_crc_16(&m_pIPSetReturnFrameData[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
	{
//		return FALSE;
	}


	// 仪器SN号
	memcpy(&usCommand, &m_pIPSetReturnFrameData[28], 1);
	if (usCommand != 0x01)
	{
		return FALSE;
	}
	memcpy(&m_uiSN, &m_pIPSetReturnFrameData[29], 4);
	// 仪器被设置的IP地址
	memcpy(&usCommand, &m_pIPSetReturnFrameData[33], 1);
	if (usCommand != 0x03)
	{
		return FALSE;
	}
	memcpy(&m_uiIPAddress, &m_pIPSetReturnFrameData[34], 4);
	return TRUE;
}

// 显示设备连接图标
void CIPSetSocket::OnShowConnectedIcon(unsigned int uiIPAddress)
{
	CButton* iconbutton = NULL;
	CStatic* iconstatic = NULL;
	CButton* pButton = NULL;

	for (int i=0 ;i<= InstrumentNum; i++)
	{
		PorcessMessages();
		if (uiIPAddress == (71 + i*10))
		{
			if (i == 0)
			{
				iconstatic =(CStatic*)m_pwnd->GetDlgItem(IDC_STATIC_LAUX);
				iconstatic->SetIcon(m_iconLAUXConnected);
			}
			else
			{
				iconbutton = (CButton*)m_pwnd->GetDlgItem(m_iButtonIDFDU[i-1]);
				iconbutton->SetIcon(m_iconFDUConnected);
				pButton = (CButton*)m_pwnd->GetDlgItem(m_iCheckIDInstrumentFDU[i-1]);
				pButton->SetCheck(1);
			}
			break;
		}
	}
	iconstatic = NULL;
	iconbutton = NULL;
	pButton = NULL;
	delete iconstatic;
	delete iconbutton;
	delete pButton;
}

// 防止程序在循环过程中无法响应消息
void CIPSetSocket::PorcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
