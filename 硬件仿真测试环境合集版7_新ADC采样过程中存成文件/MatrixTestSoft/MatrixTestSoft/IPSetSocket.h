#pragma once
#include "InstrumentList.h"
// CIPSetSocket 命令目标

class CIPSetSocket : public CSocket
{
public:
	CIPSetSocket();
	virtual ~CIPSetSocket();
	virtual void OnReceive(int nErrorCode);
public:
	// 仪器串号
	DWORD m_uiSN;
	// 仪器的IP地址
	DWORD m_uiIPAddress;
	// 仪器列表指针
	CInstrumentList* m_oInstrumentList;
	// IP地址设置应答缓冲
	byte m_pIPSetReturnFrameData[256];
	// 单个IP地址设置应答帧处理
	void ProcIPSetReturnFrameOne(void);
	// 解析IP地址设置应答帧
	BOOL ParseIPSetReturnFrame(void);
	// 设备连接上的图标指针
	HICON m_iconConnected;
	// 界面指针
	CWnd* m_pwnd;
};


