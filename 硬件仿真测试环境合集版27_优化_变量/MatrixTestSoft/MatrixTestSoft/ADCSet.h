#pragma once
#include "TabADCSettings.h"
#include "Parameter.h"
class CADCSet
{
public:
	CADCSet(void);
	~CADCSet(void);
public:
	// ADC设置帧
	unsigned char m_ucFrameData[SndFrameSize];
	// 源IP地址
	CString m_csIPSource;
public:
	// ADC设置帧头
	int ADCSetFrameHead(unsigned int uiIPAim, bool bBroadCast, unsigned short	usCommand, unsigned short usPortAim);
	// 发送ADC设置的18指令
	int ADCCommand_18(int iPos, byte * cADCSet, unsigned int uiLength);
	// 设置正弦输出
	void OnSetSine(int iPos);
	// 设置停止采集
	void OnStopSample(int iPos);
	// 打开电源
	void OnOpenPowerTBLow(int iPos);
	// 打开TB电源
	void OnOpenPowerTBHigh(int iPos);
	// 打开开关
	void OnOpenSwitchTBLow(int iPos);
	// 打开TB开关
	void OnOpenSwitchTBHigh(int iPos);
	// 读ADC寄存器
	void OnADCRegisterRead(int iPos);
	// 写ADC寄存器
	void OnADCRegisterWrite(int iPos, bool bSwitch);
	// ADC设置参数返回
	void OnADCSetReturn(int iPos);
	// ADC连续读取数据
	void OnADCReadContinuous(int iPos);
	// 设置TB时间
	void OnSetTB(int iPos, unsigned int tnow, bool bSwitch);
	// 打开零漂矫正电源及开关
	void OnOpenPowerZeroDrift(int iPos);
	// 查询ADC寄存器
	void OnADCRegisterQuery(int iPos);
	// ADC数据采集同步
	void OnADCSampleSynchronization(int iPos);
	// ADC零漂偏移矫正
	void OnADCZeroDriftCorrection(int iPos);
	// ADC零漂设置（按IP地址）
	void OnADCZeroDriftSetFromIP(int iPos, unsigned char* ucZeroDrift);
	// ADC参数设置选择界面指针
	CTabADCSettings* m_pTabADCSettings;
	// 计算CRC校验值
	void OnCRCCal(void);
	// 打开TB开关
	void OnSetTBSwitchOpen(int iPos);
	// 查询采集站错误代码
	void OnQueryErrorCodeFdu(int iPos);
	// 查询交叉站故障计数
	void OnQueryErrorCodeLAUX(int iPos);
};
