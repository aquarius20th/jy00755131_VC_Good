#pragma once
#include "InstrumentList.h"
#include <hash_map>
#include "LogFile.h"

using stdext::hash_map;
// CTailTimeFrameSocket
class CTailTimeFrame
{
public:
	CTailTimeFrame();
	virtual ~CTailTimeFrame();
private:
	// 尾包时刻查询结果数据接收缓冲
	byte m_pTailTimeRecData[RcvFrameSize];
	// 时延设置帧
	byte m_pTimeDelayData[SndFrameSize];
private:
	// 单个尾包时刻查询处理
	void ProcTailTimeFrameOne(void);
	// 解析尾包时刻查询帧
	BOOL PraseTailTimeFrame(void);
	// 时间同步计算
	void TimeDelayCalculation(void);
	// 防止程序在循环中运行无法响应消息
	void ProcessMessages(void);
public:
	// 仪器列表指针
	CInstrumentList* m_pInstrumentList;
	// 源IP地址
	unsigned int m_uiIPSource;
	// 仪器位置索引表
	hash_map<unsigned int, CInstrument*> m_oInstrumentLocationMap;
	// 日志类指针
	CLogFile* m_pLogFile;
	// 界面指针
	CWnd* m_pWnd;
	// Socket套接字
	sockaddr_in m_RecvAddr, m_SendToAddr;
	SOCKET m_TailTimeSocket;
public:
	// 生成时统报文设置帧
	void MakeTimeDelayFrameData(CInstrument* pInstrument, unsigned int uiTimeHigh, unsigned int uiTimeLow);
	// 发送时统报文设置帧
	void SendTimeDelayFrameData(void);
	// 设置设备的本地时间
	void OnSetTimeDelay(unsigned int uiTimeHigh, unsigned int uiTimeLow);
	// 消息处理函数
	void OnReceive(void);
	// 关闭UDP套接字
	void OnCloseUDP(void);
	// 处理该大线路由方向的时统
	void TimeDelayCalculationLineRout(void);
};


