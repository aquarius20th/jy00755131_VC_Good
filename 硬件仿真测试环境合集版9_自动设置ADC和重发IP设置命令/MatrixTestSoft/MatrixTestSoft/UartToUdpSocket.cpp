// UartToUdpSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "UartToUdpSocket.h"

// CUartToUdpSocket

CUartToUdpSocket::CUartToUdpSocket()
: m_usUdpBufCount(0)
, m_pctrlMSComm1(NULL)
{
}

CUartToUdpSocket::~CUartToUdpSocket()
{
}


// CUartToUdpSocket 成员函数

void CUartToUdpSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	ret = Receive(m_ucUdpBuf[m_usUdpBufCount],1024);

	if(ret==ERROR)
	{
		TRACE("ERROR!");
	}
	else if(ret != 0xffffffff) 
	{

		OnSendFromSerialPort(m_ucUdpBuf[m_usUdpBufCount],ret);
		m_usUdpBufCount +=1;
		m_usUdpBufCount = m_usUdpBufCount%8;
	}

	CSocket::OnReceive(nErrorCode);
}
// 从串口发送数据
void CUartToUdpSocket::OnSendFromSerialPort(unsigned char* TXDATA, unsigned int uiCount)
{
	// 如果端口未打开则不发送数据
// 	if(NULL == hCom)
// 	{
// 		return;
// 	}
// 
// 	OVERLAPPED wol;
// 	wol.Internal=0;
// 	wol.InternalHigh=0;
// 	wol.Offset=0;
// 	wol.OffsetHigh=0;
// 	wol.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
// 	
// 	WriteFile(hCom,TXDATA,uiCount,NULL,&wol);
	if(m_pctrlMSComm1->get_PortOpen() == FALSE)
	{
		return;
	}

	CByteArray array;

	array.RemoveAll();

	array.SetSize(uiCount);

	for(unsigned int i=0; i<uiCount; i++)
	{
		array.SetAt(i, TXDATA[i]);
	}

	m_pctrlMSComm1->put_Output(COleVariant(array)); // 发送数据
}
