#pragma once

// CSysTimeSocket 命令目标

class CSysTimeSocket : public CSocket
{
public:
	CSysTimeSocket();
	virtual ~CSysTimeSocket();
public:
	virtual void OnReceive(int nErrorCode);
protected:
	unsigned char udp_buf[1024];
	unsigned char ADCSampleCmd[128];
public:
	// 采集站本地时间
	unsigned int m_uiSysTime;
	// 目标IP地址
	unsigned int m_uiIPAim;
	// 生成数据采样设置帧
	void MakeFrameData(unsigned int uiIPAim, unsigned int tnow);
	// 发送数据采样设置帧
	void SendFrameData(void);
	// 设置ADC采样数据返回端口和IP地址
	void MakeSetPortFrameData(unsigned int uiIPAim);
};


