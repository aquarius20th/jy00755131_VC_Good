// IPSetSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "IPSetSocket.h"


// CIPSetSocket

CIPSetSocket::CIPSetSocket()
: m_uiSN(0)
, m_uiIPAddress(0)
, m_pwnd(NULL)
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
	ret = Receive(m_pIPSetReturnFrameData,256);

	if(ret == 256) 
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
	if (TRUE == m_oInstrumentList->IfIndexExistInMap(m_uiSN))
	{
		// 在索引表中找到该仪器,得到该仪器指针
		CInstrument* pInstrument = NULL;
		if (TRUE == m_oInstrumentList->GetInstrumentFromMap(m_uiSN, pInstrument))
		{
			if (m_uiIPAddress == pInstrument->m_uiIPAddress)
			{
				pInstrument->m_bIPSetOK = true;
				CStatic* icon;
				if(m_uiIPAddress == 81)
				{
					icon =(CStatic*)m_pwnd->GetDlgItem(IDC_STATIC_INSTRUMENT1);
					icon->SetIcon(m_iconConnected);
				}
				else if(m_uiIPAddress == 91)
				{
					icon =(CStatic*)m_pwnd->GetDlgItem(IDC_STATIC_INSTRUMENT2);
					icon->SetIcon(m_iconConnected);
				}
				else if(m_uiIPAddress == 101)
				{
					icon =(CStatic*)m_pwnd->GetDlgItem(IDC_STATIC_INSTRUMENT3);
					icon->SetIcon(m_iconConnected);
				}
				else if(m_uiIPAddress == 111)
				{
					icon =(CStatic*)m_pwnd->GetDlgItem(IDC_STATIC_INSTRUMENT4);
					icon->SetIcon(m_iconConnected);
				}
			}
		}
	}

}

// 解析IP地址设置应答帧
BOOL CIPSetSocket::ParseIPSetReturnFrame(void)
{
	byte	usCommand = 0;

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
