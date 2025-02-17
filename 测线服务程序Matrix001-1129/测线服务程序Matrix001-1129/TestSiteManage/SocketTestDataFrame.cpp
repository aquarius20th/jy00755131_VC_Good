// SocketTestDataFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "TestSiteManage.h"
#include "SocketTestDataFrame.h"

// CSocketTestDataRev

CSocketTestDataFrame::CSocketTestDataFrame()
{
	// 帧字节数
	m_iFrameSize = FrameLength;
}

CSocketTestDataFrame::~CSocketTestDataFrame()
{
}

/**
* 初始化
* @param CString strIPForInstrument 信宿IP地址
* @param UINT uiPortForTestDataFrame	信宿端口号
* @return BOOL TRUE：成功；FALSE：失败
*/
BOOL CSocketTestDataFrame::OnInit(CString strIPForInstrument, UINT uiPortForTestDataFrame)
{
	BOOL bReturn = false;

	// 和现场仪器通讯的本机IP地址
	m_strIPForInstrument = strIPForInstrument;
	// 用于接收的端口号
	m_uiPortForTestDataFrame = uiPortForTestDataFrame;
	// 创建socket
	if(!this->Create(m_uiPortForTestDataFrame,SOCK_DGRAM,m_strIPForInstrument))
	{		
		return false;
	}
	Bind(m_uiPortForTestDataFrame,m_strIPForInstrument);
	// 设置接收缓冲区大小
	//int RevBuf = 2048*1024;
	int RevBuf = 256*256*64;
	if (this->SetSockOpt( SO_RCVBUF, (char*)&RevBuf, sizeof(RevBuf)) == SOCKET_ERROR) 
	{
		this->Close();		
		return false;
	}
	// 设置发送缓冲区大小
	if (this->SetSockOpt(SO_SNDBUF, (char*)&RevBuf, sizeof(RevBuf)) == SOCKET_ERROR) 
	{
		this->Close();		
		return false;
	}
	// 设置允许接受广播信息
	RevBuf = 1;
	if (this->SetSockOpt(SO_BROADCAST, (char*)&RevBuf, sizeof(RevBuf)) == SOCKET_ERROR) 
	{
		this->Close();		
		return false;
	}
	
	DWORD   dwBytesReturned   =   0; 
	BOOL     bNewBehavior   =   FALSE; 
	DWORD   status; 
	status   =   WSAIoctl(this->m_hSocket,SIO_UDP_CONNRESET,&bNewBehavior, sizeof   (bNewBehavior), 
                                                    NULL,   0,   &dwBytesReturned, NULL,   NULL); 
	
	CString strDesc;
	strDesc.Format("CSocketTestSetFrame_id=%d\r\n",GetCurrentThreadId());
	char * tmp=strDesc.GetBuffer(strDesc.GetLength() + 1);
	OutputDebugString(tmp );
	okBegin=0;	
	m_irevFrameCount=0;
	m_iFrameCount=0;
	return bReturn;
}

/**
* 响应数据到达消息，接收网络数据
* @param int nErrorCode
* @return void
*/

void CSocketTestDataFrame::OnReceive(int nErrorCode)
{
	byte m_pFD[256];
	// TODO: 在此添加专用代码和/或调用基类
	int j=m_irevFrameCount%UDPbufCount;
	int iCount = Receive(OnReceive_buf+j*FrameLength, FrameLength);
	if(iCount<0){
		TRACE1("OnReceive Error %d\r\n",iCount);
		CSocket::OnReceive(nErrorCode);
		return;
	}
	if(iCount==FrameLength){
		int idx=0;
		int iIP=0;
		memcpy(&m_pFD,OnReceive_buf+j*FrameLength,FrameLength);
		memcpy(&iIP, &m_pFD[16], 4);
		memcpy(&idx, &m_pFD[28], 2);
		if((m_irevFrameCount)%(MaxFDUCount*55)==0){		//每秒显示一次
			;
			//		if(m_irevFrameCount<MaxFDUCount || (m_irevFrameCount)%(MaxFDUCount)==0)
			TRACE3("OnReceive revFrameCount=%d,iIP=%d idx=%d\r\n",m_irevFrameCount,iIP,idx);
		}
		if((idx==0 || idx==FrameADCount72) && okBegin==0)
			okBegin=1;
		if(okBegin==0){
			CSocket::OnReceive(nErrorCode);
			return;
		}
		if(iCount>2560){
			TRACE1("OnReceive Error %d\r\n",iCount);
			CSocket::OnReceive(nErrorCode);
			return;
		}
	}
	for(int i=0;i<iCount;i+=FrameLength){
		m_irevFrameCount ++;
		if(iCount != FrameLength){
			;
			TRACE2("OnReceive Error IP=%d DataIndex=%d\r\n", m_oFrameTestData.m_uiIPSource,m_oFrameTestData.m_usDataIndex);
		}
	}
	CSocket::OnReceive(nErrorCode);
}

/*
void CSocketTestDataFrame::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	int j=m_irevFrameCount%UDPbufCount;
	int iCount = Receive(OnReceive_buf+j*FrameLength, FrameLength*10);
	if(iCount<0){
		TRACE("OnReceive Error \r\n");
		//CSocket::OnReceive(nErrorCode);
		return;
	}
	for(int i=0;i<iCount;i+=FrameLength){
		if(iCount == FrameLength)
		{
			// 帧字节数
			m_irevFrameCount ++;
			memcpy(&m_oFrameTestData.m_pFrameData,&OnReceive_buf+i,FrameLength);
			if(m_oFrameTestData.ParseFrame()==false){
				//error
				return;
			}
			//if(!PostThreadMessage(m_oProcTestThreadID,MY_MSG,(WPARAM)m_oFrameTestData.m_pFrameData,0)){	//post thread msg
			PostMessage(HWND_BROADCAST,m_dwMessage,&OnReceive_buf+i,FrameLength);
				printf("post message failed,errno:%d\n",::GetLastError());
			}
		}
		if(iCount%256!=0)
			TRACE2("OnReceive Error IP=%d DataIndex=%d\r\n", m_oFrameTestData.m_uiIPSource,m_oFrameTestData.m_usDataIndex);
	}

	CSocket::OnReceive(nErrorCode);
}
*/

/**
* 得到网络接收缓冲区收到的帧数量
* @param void
* @return DWORD 帧数量
*/
DWORD CSocketTestDataFrame::GetFrameCount1()
{
/*	if(m_irevFrameCount>0x10000 || m_iFrameCount>0x10000)
		TRACE1("GetFrameCount1 Error %d\r\n",m_irevFrameCount);
*/	
	if((m_irevFrameCount-m_iFrameCount)>UDPbufCount){
		TRACE2("GetFrameCount1 Error3 %d %d\r\n",m_irevFrameCount,m_iFrameCount);
		return UDPbufCount;
	}
	if((m_irevFrameCount-m_iFrameCount)>0)
		return m_irevFrameCount-m_iFrameCount;
	else
		return 0;
}

/**
* 得到网络接收缓冲区收到的帧数量
* @param void
* @return DWORD 帧数量
*/
DWORD CSocketTestDataFrame::GetFrameCount()
{
	DWORD dwFrameCount = 0;

	// 得到网络接收缓冲区数据字节数
	IOCtl(FIONREAD, &dwFrameCount);
	// 得到帧数量
	dwFrameCount = dwFrameCount / m_iFrameSize;

	return dwFrameCount;
}

/**
* 设置接收缓冲区可以接收的帧数量
* @param int iFrameCount IP帧数量
* @return BOOL TRUE：成功；FALSE：失败
*/
BOOL CSocketTestDataFrame::SetBufferSize(int iFrameCount)
{
	BOOL bReturn = false;
	int iOptionValue = iFrameCount * m_iFrameSize;
	int iOptionLen = sizeof(int);
	bReturn = SetSockOpt(SO_RCVBUF, (void*)&iOptionValue, iOptionLen, SOL_SOCKET);

	return bReturn;
}

/**
* 得到帧数据
* @param void
* @return BOOL TRUE：成功；FALSE：失败
*/

BOOL CSocketTestDataFrame::GetFrameData1()
{
	BOOL bReturn = false;
	// 得到帧数据
	if(m_irevFrameCount>0){
		int j=m_iFrameCount%UDPbufCount;
		if((m_irevFrameCount-m_iFrameCount)>UDPbufCount){
			TRACE("OnReceive Error3 m_irevFrameCount=%d m_iFrameCount=%d\r\n",m_irevFrameCount,m_iFrameCount);
			return bReturn;
		}
		memcpy(&m_oFrameTestData.m_pFrameData,OnReceive_buf+j*FrameLength,FrameLength);
		m_iFrameCount++;
		bReturn = m_oFrameTestData.ParseFrame();
//		if(bReturn!=TRUE)
//			TRACE("OnReceive Error2 \r\n");
	}
	return bReturn;
}

/**
* 得到帧数据
* @param void
* @return BOOL TRUE：成功；FALSE：失败
*/
/*
BOOL CSocketTestDataFrame::GetFrameData()
{
	BOOL bReturn = false;

	// 得到帧数据
	int iCount = Receive(m_oFrameTestData.m_pFrameData, m_iFrameSize);

	if(iCount == m_iFrameSize)	//帧大小正确
	{
		// 解析帧数据
		bReturn = m_oFrameTestData.ParseFrame();
	}
	return bReturn;
}
*/

/**
* 得到帧数据，不解析
* @param void
* @return BOOL TRUE：成功；FALSE：失败
*/
BOOL CSocketTestDataFrame::GetFrameDataNotParse1()
{
	BOOL bReturn = false;
	// 得到帧数据
	if(m_irevFrameCount>0){
		//memcpy(m_oFrameTestData.m_pFrameData,OnReceive_buf+m_iFrameCount*FrameLength,FrameLength);
		m_iFrameCount++;
		bReturn=true;
	}
	return bReturn;
}

/**
* 得到帧数据，不解析
* @param void
* @return BOOL TRUE：成功；FALSE：失败
*/
BOOL CSocketTestDataFrame::GetFrameDataNotParse()
{
	BOOL bReturn = false;

	// 得到帧数据
	int iCount = Receive(m_oFrameTestData.m_pFrameData, m_iFrameSize);
	if(iCount == m_iFrameSize)	//帧大小正确
	{
		bReturn = true;
	}
	return bReturn;
}