// HeartBeatThread.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "HeartBeatThread.h"
#include "Parameter.h"

// CHeartBeatThread

IMPLEMENT_DYNCREATE(CHeartBeatThread, CWinThread)

CHeartBeatThread::CHeartBeatThread()
: m_bclose(false)
, m_uiIPSource(0)
, m_HeartBeatSocket(INVALID_SOCKET)
{
}

CHeartBeatThread::~CHeartBeatThread()
{
}

BOOL CHeartBeatThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CHeartBeatThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CHeartBeatThread, CWinThread)
END_MESSAGE_MAP()

// 初始化
//************************************
// Method:    OnInit
// FullName:  CHeartBeatThread::OnInit
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CHeartBeatThread::OnInit(void)
{
	m_hHeartBeatThreadClose = ::CreateEvent(false, false, NULL, NULL);	// 创建事件对象
	if ((m_hHeartBeatThreadClose == NULL)||(GetLastError() == ERROR_ALREADY_EXISTS))
	{
		AfxMessageBox(_T("创建心跳线程关闭事件出错！"));
	}
	else
	{
		::ResetEvent(m_hHeartBeatThreadClose);	// 设置事件对象为无信号状态
	}
	MakeHeartBeatFrame();
}
// 线程函数在Run中执行
int CHeartBeatThread::Run()
{
	// 循环，直到关闭标志为真
	while(true)
	{
		if(m_bclose == true)
		{
			break;
		}
		// 发送心跳帧
		SendHeartBeatFrame();
		if (m_bclose == true)
		{
			break;
		}
 		 Sleep(HeartBeatThreadSleepTime);
	}
	::SetEvent(m_hHeartBeatThreadClose);	// 设置事件对象为有信号状态,释放等待线程后将事件置为无信号
	// 返回
	return CWinThread::Run();
}

// 生成心跳帧
//************************************
// Method:    MakeHeartBeatFrame
// FullName:  CHeartBeatThread::MakeHeartBeatFrame
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CHeartBeatThread::MakeHeartBeatFrame(void)
{
	unsigned int uiIPSource = 0;
	unsigned int uiIPAim = 0;
	unsigned short usPortAim	=	0;
	unsigned short usCommand	=	0;
	int iPos = 0;
	memset(m_pFrameData, SndFrameBufInit, SndFrameSize);
	m_pFrameData[0] = FrameHeadCheck0;
	m_pFrameData[1] = FrameHeadCheck1;
	m_pFrameData[2] = FrameHeadCheck2;
	m_pFrameData[3] = FrameHeadCheck3;
	memset(&m_pFrameData[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	// 源IP地址
	uiIPSource = m_uiIPSource;
	// 目标IP地址
	uiIPAim = IPBroadcastAddr;
	// 目标端口号
	usPortAim = HeartBeatPort;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	usCommand = SendQueryCmd;
	// 源IP地址
	iPos = 16;
	memcpy(&m_pFrameData[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标IP地址
	memcpy(&m_pFrameData[iPos], &uiIPAim, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标端口号
	memcpy(&m_pFrameData[iPos], &usPortAim, FramePacketSize2B);
	iPos += FramePacketSize2B;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pFrameData[iPos], &usCommand, FramePacketSize2B);
	iPos += FramePacketSize2B;

	iPos = 32;
	// 设置命令字
	m_pFrameData[iPos] = SndFrameBufInit;

	unsigned short usCRC16 = 0;
	usCRC16 = get_crc_16(&m_pFrameData[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_pFrameData[SndFrameSize - CRCSize], &usCRC16, CRCSize);
}

// 发送心跳帧
//************************************
// Method:    SendHeartBeatFrame
// FullName:  CHeartBeatThread::SendHeartBeatFrame
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CHeartBeatThread::SendHeartBeatFrame(void)
{
	// 发送广播命令帧
	sendto(m_HeartBeatSocket, reinterpret_cast<const char*>(&m_pFrameData), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
}

// 关闭并结束线程
//************************************
// Method:    OnClose
// FullName:  CHeartBeatThread::OnClose
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CHeartBeatThread::OnClose(void)
{
	OnCloseUDP();
	m_bclose = true;
}

// 防止程序在循环中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CHeartBeatThread::ProcessMessages
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CHeartBeatThread::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

// 关闭UDP套接字
void CHeartBeatThread::OnCloseUDP(void)
{
	shutdown(m_HeartBeatSocket, SD_BOTH);
	closesocket(m_HeartBeatSocket);
	m_HeartBeatSocket = INVALID_SOCKET;
}
