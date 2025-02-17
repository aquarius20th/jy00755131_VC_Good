#pragma once
#include "Parameter.h"
#include "SaveFile.h"
#include "LogFile.h"

// CPortMonitoringRecThread

class CPortMonitoringRecThread : public CWinThread
{
	DECLARE_DYNCREATE(CPortMonitoringRecThread)

public:
	CPortMonitoringRecThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CPortMonitoringRecThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	// 数据接收缓冲
	unsigned char m_ucudp_buf[SndFrameSize];
	// UDP接收帧指针偏移量
	unsigned int m_uiUdpCount;
	// UDP接收缓冲区
	unsigned char m_ucUdpBuf[PortMonitoringRcvBufSize];
public:
	// 心跳计数
	unsigned int m_uiHeartBeatNum;
	// IP地址设置计数
	unsigned int m_uiIPSetNum;
	// 尾包时刻查询计数
	unsigned int m_uiTailTimeNum;
	// 时延设置计数
	unsigned int m_uiDelayTimeSetNum;
	// ADC设置计数
	unsigned int m_uiADCSetNum;
	// 接收方Socket套接字
//	CSocket m_RecSocket;
	sockaddr_in m_RecvAddr, m_SendToAddr;
	SOCKET m_RecSocket;
	// 接收端口
	int m_iRecPort;
	// 保存文件类的指针
	CSaveFile* m_pSaveFile;
	// 线程关闭标志
	bool m_bclose;
	// 线程结束事件
	HANDLE m_hPortMonitoringRecThreadClose;
	// 发送帧数
	unsigned int m_uiSendFrameNum;
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
	// 处理数据
	void OnProcess(int iCount);
	// 关闭UDP套接字
	void OnCloseUDP(void);
};


