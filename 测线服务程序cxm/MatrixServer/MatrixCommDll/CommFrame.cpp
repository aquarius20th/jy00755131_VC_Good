#include "stdafx.h"
#include "MatrixCommDll.h"

CCommFrame::CCommFrame(void)
{
}

CCommFrame::~CCommFrame(void)
{
}

// 重置接收帧结构体
void CCommFrame::OnResetFrameStruct(m_oCommFrameStructPtr pFrameStruct)
{
	memset(pFrameStruct, 0, sizeof(m_oCommFrameStruct));
}
// 初始化
void CCommFrame::OnInit(void)
{
	InitializeCriticalSection(&m_oSecClientFrame);
	EnterCriticalSection(&m_oSecClientFrame);
	m_uiCountFree = FrameStructNumMax;
	m_uiPacketIndex = 0;
	m_uiCmdIndex = 0;
	m_olsCommFrameFree.clear();
	m_olsCommWorkFrame.clear();
	OnResetProcBuf();
	for(unsigned int i = 0; i < FrameStructNumMax; i++)
	{
		// 重置接收帧结构体
		OnResetFrameStruct(&m_oCommFrameArray[i]);
		// 仪器加在空闲接收帧结构体队列尾部
		m_olsCommFrameFree.push_back(&m_oCommFrameArray[i]);
	}
	LeaveCriticalSection(&m_oSecClientFrame);
}

// 得到一个空闲接收帧结构体
m_oCommFrameStructPtr CCommFrame::GetFreeFrameStruct(void)
{
	m_oCommFrameStructPtr pFrameStruct = NULL;
	list <m_oCommFrameStructPtr>::iterator iter;
	if(m_uiCountFree > 0)	//有空闲接收帧结构体
	{
		// 从空闲队列中得到一个接收帧结构体
		iter = m_olsCommFrameFree.begin();
		pFrameStruct = *iter;
		m_olsCommFrameFree.pop_front();	
		m_uiCountFree--;	// 空闲接收帧结构体总数减1
	}
	else
	{
		// 写入错误日志
	}
	return pFrameStruct;
}

// 增加一个空闲接收帧结构体
void CCommFrame::AddFreeFrameStruct(m_oCommFrameStructPtr pFrameStruct)
{
	//初始化接收帧结构体
	OnResetFrameStruct(pFrameStruct);
	//加入空闲队列
	m_olsCommFrameFree.push_back(pFrameStruct);
	m_uiCountFree++;	// 空闲接收帧结构体总数加1
}

// 关闭
void CCommFrame::OnClose(void)
{
	m_olsCommFrameFree.clear();
	m_olsCommWorkFrame.clear();
	DeleteCriticalSection(&m_oSecClientFrame);
}

// 重置数据处理缓冲区
void CCommFrame::OnResetProcBuf(void)
{
	memset(m_cProcBuf, 0, ProcBufferSize);
}