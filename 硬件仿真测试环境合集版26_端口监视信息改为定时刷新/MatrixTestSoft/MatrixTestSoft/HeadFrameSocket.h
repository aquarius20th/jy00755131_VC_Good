#pragma once
#include "InstrumentList.h"

// CHeadFrameSocket 命令目标

class CHeadFrameSocket : public CSocket
{
public:
	CHeadFrameSocket();
	virtual ~CHeadFrameSocket();
	virtual void OnReceive(int nErrorCode);
public:
	// 处理单个首包
	void ProcHeadFrameOne();
	// 解析首包
	BOOL ParseHeadFrame(void);
	// 仪器串号
	DWORD m_uiSN;
	// 首包时刻
	DWORD m_uiHeadFrameTime;
	// 路由地址
	DWORD m_uiRoutAddress;

	// 仪器列表指针
	CInstrumentList* m_pInstrumentList;
	// 生成IP地址设置帧
	void MakeIPSetFrame(CInstrument* pInstrument, BOOL bSetIP);
	// 首包接收数据缓冲
	byte m_pHeadFrameData[RcvFrameSize];
	// 设置IP地址缓冲
	byte m_pIPSetFrameData[SndFrameSize];
	// 发送IP地址设置帧
	void SendIPSetFrame(void);
	// 源IP地址
	CString m_csIPSource;
	// 发送端口
	unsigned int m_uiSendPort;
	// 界面指针
	CWnd* m_pwnd;
};


