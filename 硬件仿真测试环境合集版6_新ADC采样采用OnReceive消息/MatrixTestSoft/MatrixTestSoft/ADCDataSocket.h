#pragma once
#include <vector>
#include "OScopeCtrl.h"
#include "Parameter.h"
#include <afxmt.h>
using namespace std;
// CADCDataSocket 命令目标

class CADCDataSocket : public CSocket
{
public:
	CADCDataSocket();
	virtual ~CADCDataSocket();
public:
	// ADC接收数据缓冲区
	byte ADCData[256];
	// 接收采样数据
	void ReceiveSampleData(byte* pSampleData);
	// 选择仪器对象
	int* m_pSelectObject;
	// 选择仪器对象做噪声监测
	int* m_pSelectObjectNoise;
	// 采集到的数据
	vector <double>* m_pSampleData[GraphViewNum];
	// 图形模板指针
	COScopeCtrl* m_pOScopeCtrl[GraphViewNum];
	// 定义临界区变量
	CCriticalSection m_Sec;
	virtual void OnReceive(int nErrorCode);
	// 记录指针偏移量
	unsigned short m_usDataPointMove[GraphViewNum][10000];
	// 记录前一个接收到的ADC数据的指针偏移量                  
	unsigned short m_usDataPointPrevious[GraphViewNum];
	// 记录应接收ADC数据包个数
	unsigned int m_uiADCDataFrameCount[GraphViewNum];
// 	// 界面指针
// 	CWnd* m_pwnd;
	// 判断ADC采样开始标志位，用于丢弃TB之前的无效数据
	BOOL m_bStartSample;
	// 生成ADC数据重发帧
	void MakeADCDataRetransmissionFrame(unsigned short usDataPointNb, unsigned int uiIPAim);
	// ADC数据重发帧
	unsigned char m_ucADCRetransmission[128];
	// 源IP地址
	CString m_csIPSource;
	// 发送端口
	unsigned int m_uiSendPort;
	// 需要重发的ADC数据信息结构
	typedef struct m_structADC
	{
		unsigned int uiIPAim;			// 重发帧IP地址
		unsigned short usDataPointNb;	// 重发帧的指针偏移量
		unsigned int uiDataCount;		// 重发帧所对应的帧位置（个数）
		BOOL operator == (const m_structADC&   tdl)const
		{ 

			if(uiIPAim != tdl.uiIPAim) 
				return   FALSE; 
			if(usDataPointNb != tdl.usDataPointNb) 
				return   FALSE; 
			return   TRUE; 
		} 
	};
	// 需要重发的ADC数据链表
	CList <m_structADC, m_structADC&> m_listADC;
	// 记录完成绘图的ADC数据个数
	unsigned int m_uiADCRecCompleteCount[GraphViewNum];
	// ADC数据需要重新发送帧对应的最小个数
	unsigned int OnADCRetransimissionMinNb(void);
	// 有ADC数据重发故先保留不连续的数据
	vector <double> m_dSampleTemp[GraphViewNum];
	// 测试丢帧情况
	unsigned int m_uiTestADCLost;
};
