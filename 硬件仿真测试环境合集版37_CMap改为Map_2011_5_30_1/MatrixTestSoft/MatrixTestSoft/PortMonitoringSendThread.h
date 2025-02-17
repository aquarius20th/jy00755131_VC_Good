#pragma once
#include "Parameter.h"
#include "SaveFile.h"
#include "LogFile.h"

// CPortMonitoringSendThread

class CPortMonitoringSendThread : public CWinThread
{
	DECLARE_DYNCREATE(CPortMonitoringSendThread)

public:
	CPortMonitoringSendThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CPortMonitoringSendThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	// 数据接收缓冲
	unsigned char m_ucudp_buf[RcvFrameSize];
	// UDP接收帧指针偏移量
	unsigned int m_uiUdpCount;
	// UDP接收缓冲区
	unsigned char m_ucUdpBuf[PortMonitoringSendBufSize];
public:
	// 首包计数
	unsigned int m_uiHeadFrameNum;
	// IP地址设置应答计数
	unsigned int m_uiIPSetReturnNum;
	// 尾包计数
	unsigned int m_uiTailFrameNum;
	// 尾包时刻查询应答计数
	unsigned int m_uiTailTimeReturnNum;
	// 时延设置应答计数
	unsigned int m_uiDelayTimeReturnNum;
	// ADC设置应答计数
	unsigned int m_uiADCSetReturnNum;
	// 硬件设备错误查询应答帧个数
	unsigned int m_uiErrorCodeReturnNum;
	// 查询得到的本地时间帧数
	unsigned int m_uiCollectSysTimeReturnNum;
	// 接收得到的ADC数据帧数
	unsigned int m_uiADCRecNum;
	// 硬件设备错误查询应答帧计数
	unsigned int m_uiErrorCodeReturnCount[InstrumentMaxCount];
	// 记录硬件设备错误查询首个应答帧
	unsigned char m_ucErrorCodeReturn[InstrumentMaxCount][QueryErrorCodeNum];
	// 显示首包帧，包含SN和首包时刻
	CString m_csHeadFrameShow;
	// 显示IP地址设置应答帧，包含SN和设置的IP地址
	CString m_csIPSetReturnShow;
	// 显示尾包帧，包含SN
	CString m_csTailFrameShow;
	// 显示尾包时刻查询应答帧，包含IP地址和查询结果
	CString m_csTailTimeReturnShow;
	// 显示时延设置应答帧，包含IP地址和设置内容
	CString m_csDelayTimeReturnShow;
	// 显示硬件错误码和码差
	CString m_csErrorCodeReturnShow;
	// 统计数据误码计数
	unsigned int m_uiDataErrorCount[InstrumentMaxCount];
	// 统计命令误码计数
	unsigned int m_uiCmdErrorCount[InstrumentMaxCount];
	// 发送方Socket套接字
//	CSocket m_SendSocket;
	sockaddr_in m_RecvAddr, m_SendToAddr;
	SOCKET m_SendSocket;
	// 接收端口
	int m_iRecPort;
	// 发送端口
	int m_iSendPort;
	// 目标IP地址
	CString m_csIP;
	// 保存文件类的指针
	CSaveFile* m_pSaveFile;
	// 端口分发功能
	BOOL m_bPortDistribution;
	// 线程关闭标志
	bool m_bclose;
	// 线程结束事件
	HANDLE m_hPortMonitoringSendThreadClose;
	// 接收帧数
	unsigned int m_uiRecFrameNum;
	// 日志类指针
	CLogFile* m_pLogFile;
protected:
	// 避免端口阻塞
	void OnAvoidIOBlock(SOCKET socket);
public:
	virtual int Run();
	// 初始化
	void OnInit(void);
	// 打开
	void OnOpen(void);
	// 停止
	void OnStop(void);
	// 关闭
	void OnClose(void);
	// 防止程序在循环中运行无法响应消息
	void ProcessMessages(void);
	// 重置界面
	void OnReset(void);
	// 端口监视处理函数
	void OnPortMonitoringProc(void);
	// 数据处理
	void OnProcess(int iCount);
	// 关闭UDP套接字
	void OnCloseUDP(void);
};


