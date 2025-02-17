#pragma once
#include <vector>
#include "OScopeCtrl.h"
#include "Parameter.h"
#include <afxmt.h>
using namespace std;

// CADCDataRecThread

class CADCDataRecThread : public CWinThread
{
	DECLARE_DYNCREATE(CADCDataRecThread)

public:
	CADCDataRecThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CADCDataRecThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int Run();
public:
	// 线程结束事件
	HANDLE m_hADCDataThreadClose;
	// AD数据接收Socket
	CSocket m_ADCDataSocket;
	// 线程关闭标志
	bool m_bclose;
	// 选择仪器对象
	int* m_pSelectObject;
	// 选择仪器对象做噪声监测
	int* m_pSelectObjectNoise;
	// 图形模板指针
	COScopeCtrl* m_pOScopeCtrl[InstrumentNum];
	// 定义临界区变量
	CCriticalSection m_Sec;
	// 记录前一个接收到的ADC数据的指针偏移量                  
	unsigned short m_usDataPointPrevious[InstrumentNum];
	// 记录应接收ADC数据包个数
	unsigned int m_uiADCDataFrameCount[InstrumentNum];
	// 记录实际接收ADC数据包个数
	unsigned int m_uiADCDataFrameRecCount[InstrumentNum];
	// 判断ADC采样开始标志位，用于丢弃TB之前的无效数据
	BOOL m_bStartSample;
	// ADC数据重发帧
	unsigned char m_ucADCRetransmission[SndFrameSize];
	// 源IP地址
	CString m_csIPSource;
	// 发送端口
	unsigned int m_uiSendPort;
	// 需要重发的ADC数据信息结构
	typedef struct m_structADC
	{
		unsigned int uiDataCount;		// 重发帧所对应的帧位置（个数）
		unsigned short uiRetransmissionNum;	// 重发计数
	};
	// 需要重发的ADC索引表
	CMap<unsigned int, unsigned int, m_structADC, m_structADC&> m_oADCLostMap[InstrumentNum];
	// 有ADC数据重发故先保留不连续的数据
	vector <double> m_dSampleTemp[InstrumentNum];
	// ADC数据保存成文件的数据缓冲区
	vector <double> m_dADCSave[InstrumentNum];
	// ADC数据处理过程缓冲区
	vector <double> m_dTemp;
	// 测试丢帧情况
	unsigned int m_uiTestADCLost;
	// 保存数据到文件的存储路径
	CString m_csSaveFilePath;
	// 保存ADC数据文件个数
	unsigned int m_uiADCSaveFileNum;
	// 保存文件的文件指针
	FILE * m_pFileSave;
	// 文件打开标志位
	BOOL m_bOpenADCSaveFile;
	// 选中的仪器对象名称
	char* m_cSelectObjectName[InstrumentNum];
	// 被保存的ADC数据个数
	unsigned int m_uiADCDataToSaveNum;
	// ADC数据保存文件尺寸
	unsigned int m_uiADCFileLength;
	// ADC图形显示设备IP
	unsigned int m_uiADCGraphIP[InstrumentNum];
	// ADC数据界面显示缓冲区
	double m_dADCDataShow[InstrumentNum];
	// ADC接收到的数据重发帧的序号
	unsigned int m_uiADCRetransmissionNb;
	// 数据帧接收缓冲
	unsigned char m_ucudp_buf[RcvBufNum][RcvFrameSize];
	// 缓冲区序号
	unsigned short m_usudp_count;
	// UDP接收帧指针偏移量
	unsigned int m_uiUdpCount;
	// UDP接收缓冲区
	unsigned char m_ucUdpBuf[ADCDataBufSize];


public:
	// 初始化
	void OnInit(void);
	// 关闭并结束线程
	void OnClose(void);
	// 打开
	void OnOpen(void);
	// 停止
	void OnStop(void);
	// 接收采样数据
	void ReceiveSampleData(byte* pSampleData);
	// 生成ADC数据重发帧
	void MakeADCDataRetransmissionFrame(unsigned short usDataPointNb, unsigned int uiIPAim);
	// ADC数据需要重新发送帧对应的最小个数
	unsigned int OnADCRetransimissionMinNb(unsigned int uiInstrumentNb);
	// 创建并打开ADC保存数据文件
	void OnOpenADCSaveFile(void);
	// 关闭ADC保存数据文件
	void OnCloseADCSaveFile(void);
	// 将ADC数据保存到文件中
	void OnSaveADCToFile(void);
	// 被选择仪器的ADC数据个数的最小值
	unsigned int OnADCRecDataMinNum(void);
	// 关闭采样后保存剩余数据
	void OnSaveRemainADCData(unsigned int uiADCDataNeedToSave);
	// ADC数据重发
	void OnADCDataRetransmission(void);
	// 防止程序在循环中运行无法响应消息
	void ProcessMessages(void);
	// 检查接收帧是否为重发帧
	BOOL OnCheckFrameIsRetransmission(unsigned int uiInstrumentNb, unsigned int uiIPAim, unsigned short usDataPointNow);
	// 该帧为最后一个数据重发帧
	void OnRecOkIsRetransimissionFrame(unsigned int uiInstrumentNb, unsigned int uiIPAim, double* pReceiveData);
	// 该帧为普通帧且之前没有重发帧
	void OnRecOkIsNormalFrame(unsigned int uiInstrumentNb, unsigned int uiIPAim, double* pReceiveData);
	// 该帧为重发帧但不是最后一个重发帧
	void OnRecNotOkIsRetransimissionFrame(unsigned int uiInstrumentNb, unsigned int uiIPAim, double* pReceiveData);
	// 该帧为普通帧且之前有重发帧
	void OnRecNotOkIsNormalFrame(unsigned int uiInstrumentNb, double* pReceiveData);
};


