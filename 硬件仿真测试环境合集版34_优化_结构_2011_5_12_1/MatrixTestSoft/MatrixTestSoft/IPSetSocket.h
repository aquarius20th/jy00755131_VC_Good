#pragma once
#include "InstrumentList.h"
#include "ADCSet.h"
// CIPSetSocket 命令目标

class CIPSetSocket : public CSocket
{
public:
	CIPSetSocket();
	virtual ~CIPSetSocket();
	virtual void OnReceive(int nErrorCode);
protected:
	// 仪器串号
	DWORD m_uiSN;
	// 仪器的IP地址
	DWORD m_uiIPAddress;
	// IP地址设置应答缓冲
	byte m_pIPSetReturnFrameData[RcvFrameSize];
public:
	// 仪器列表指针
	CInstrumentList* m_pInstrumentList;
	// 采集站设备连接上的图标指针
	HICON m_iconFDUConnected;
	// 交叉站设备连接上的图标指针
	HICON m_iconLAUXConnected;
	// 界面指针
	CWnd* m_pwnd;
	// 目标端口
	unsigned int m_uiSendPort;
	// 选择仪器对象
	int* m_pSelectObject;
	// 采集站设备Button控件ID
	int m_iButtonIDFDU[InstrumentNum];
	// 采集站设备仪器选择Check控件ID
	int m_iCheckIDInstrumentFDU[InstrumentNum];
protected:
	// 单个IP地址设置应答帧处理
	void ProcIPSetReturnFrameOne(void);
	// 解析IP地址设置应答帧
	BOOL ParseIPSetReturnFrame(void);
	// 显示设备连接图标
	void OnShowConnectedIcon(unsigned int uiIPAddress);
	// 防止程序在循环过程中无法响应消息
	void PorcessMessages(void);
};


