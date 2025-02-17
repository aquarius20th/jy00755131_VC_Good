// ADCDataProcessThread.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "ADCDataProcessThread.h"


// CADCDataProcessThread

IMPLEMENT_DYNCREATE(CADCDataProcessThread, CWinThread)

CADCDataProcessThread::CADCDataProcessThread()
: m_bclose(false)
, m_uiADCRetransmissionNb(0)
, m_pOScopeCtrl(NULL)
, m_csIPSource(_T(""))
, m_uiSendPort(0)
, m_pADCDataSaveToFileThread(NULL)
, m_pSelectObjectNoise(NULL)
, m_uiADCGraphIP(0)
, m_bStopSample(FALSE)
{
}

CADCDataProcessThread::~CADCDataProcessThread()
{
}

BOOL CADCDataProcessThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CADCDataProcessThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CADCDataProcessThread, CWinThread)
END_MESSAGE_MAP()


// 初始化

void CADCDataProcessThread::OnInit(void)
{
	m_hADCDataProcessThreadClose = ::CreateEvent(false, false, NULL, NULL);	// 创建事件对象
	::ResetEvent(m_hADCDataProcessThreadClose);	// 设置事件对象为无信号状态
	m_hADCDataProcessThreadWork = ::CreateEvent(false, false, NULL, NULL);	// 创建线程工作事件对象
	::ResetEvent(m_hADCDataProcessThreadWork); // 设置事件对象为无信号状态
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_uiADCDataFrameProcCount[i] = 0;
		m_uiADCDataFrameRecCount[i] = 0;
		m_uiADCDataFrameProcNum[i] = 0;
		m_uiCopySaveBufNum[i] = 0;
		memset(m_dADCDataNeedToSaveBuf[i], 0, ADCDataNeedToSaveBufSize);
		m_oADCLostMap[i].RemoveAll();
	}
}

// 打开
void CADCDataProcessThread::OnOpen(void)
{
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_uiADCDataFrameProcCount[i] = 0;
		m_uiADCDataFrameRecCount[i] = 0;
		m_uiADCDataFrameProcNum[i] = 0;
		m_uiCopySaveBufNum[i] = 0;
		memset(m_dADCDataNeedToSaveBuf[i], 0, ADCDataNeedToSaveBufSize);
		m_oADCLostMap[i].RemoveAll();
	}
	m_ADCRetransmissionSocket.Close();
	if (FALSE == m_ADCRetransmissionSocket.Create(ADCFrameRetransmissionPort,SOCK_DGRAM))
	{
		AfxMessageBox(_T("ADC数据重发端口创建失败！"));
	}
	//设置广播模式
	int	iOptval, iOptlen;
	iOptlen = sizeof(int);
	iOptval = 1;
	m_ADCRetransmissionSocket.SetSockOpt(SO_BROADCAST, (void*)&iOptval, iOptlen, SOL_SOCKET);

	int iOptionValue = ADCRetransmissionBufSize;
	int iOptionLen = sizeof(int);
	m_ADCRetransmissionSocket.SetSockOpt(SO_SNDBUF, (void*)&iOptionValue, iOptionLen, SOL_SOCKET);
}

// 停止
void CADCDataProcessThread::OnStop(void)
{
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_oADCLostMap[i].RemoveAll();
	}
	m_ADCRetransmissionSocket.Close();
}

// 关闭
void CADCDataProcessThread::OnClose(void)
{
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_oADCLostMap[i].RemoveAll();
	}
	m_ADCRetransmissionSocket.Close();
	m_bclose = true;
}

int CADCDataProcessThread::Run()
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	// 循环，直到关闭标志为真
	while(true)
	{
		if(m_bclose == true)
		{
			break;
		}
		OnProcess();
		if (m_bclose == true)
		{
			break;
		}
//		Sleep(1);
		WaitForSingleObject(m_hADCDataProcessThreadWork, 5); 
		::ResetEvent(m_hADCDataProcessThreadWork); // 设置事件对象为无信号状态
	}
	::SetEvent(m_hADCDataProcessThreadClose);	// 设置事件对象为有信号状态

	return CWinThread::Run();
}

// 防止程序在循环中运行无法响应消息
void CADCDataProcessThread::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

void CADCDataProcessThread::OnProcess(void)
{
	for (int i=0; i<GraphViewNum; i++)
	{
		if (m_uiADCDataFrameRecCount[i] > 0)
		{
			ProcessMessages();
			// 将接收缓冲区中数据拷贝出来，接收缓冲区置0
			m_uiADCDataFrameProcCount[i] = m_uiADCDataFrameRecCount[i];
			for (int j=0; j<ADCDataProcBufSize; j++)
			{
				m_dADCDataProcBuf[i][j] = m_dADCDataRecBuf[i][j];
			}
//			memcpy(&m_dADCDataProcBuf[i], &m_dADCDataRecBuf[i], ADCDataProcBufSize);
			memset(&m_dADCDataRecBuf[i], 0, ADCDataProcBufSize);
			m_uiADCDataFrameRecCount[i] = 0;
			TRACE2("处理线程设备序号%d，接收帧数%d\r\n", i, m_uiADCDataFrameProcCount[i]);
		}
	}
	for (int i=0; i<GraphViewNum; i++)
	{
		if (m_uiADCDataFrameProcCount[i] > 0)
		{
			for (unsigned int j=0; j<m_uiADCDataFrameProcCount[i]; j++)
			{
				ProcessMessages();
				OnProcessOneFrame(i, j);
			}
			memset(&m_dADCDataProcBuf[i], 0, ADCDataProcBufSize);
			m_uiADCDataFrameProcCount[i] = 0;
		}
	}
	// 采样停止时将剩余数据保存到文件缓冲区中
//	OnSaveRemainDataToFileBuf();
}
// 处理一帧ADC数据
void CADCDataProcessThread::OnProcessOneFrame(unsigned int uiInstrumentNb, unsigned int uiADCDataFrameProcNb)
{
	BOOL bADCRetransimission = FALSE;			// 判断是否为重发帧的标志位
	unsigned int uiADCGraphIP = 0;				// 设备IP地址
	m_uiADCDataFrameProcNum[uiInstrumentNb]++;
	// 清除索引中重发次数达到上限的及达到保存帧数界限的
	OnClearOutdatedFromMap(uiInstrumentNb);

	if (m_uiADCDataFrameProcNum[uiInstrumentNb] == 1)
	{
		m_usDataPointPrevious[uiInstrumentNb] = (unsigned short)m_dADCDataProcBuf[uiInstrumentNb][0];
		uiADCGraphIP = 81 + uiInstrumentNb*10;
		if(uiADCGraphIP == m_uiADCGraphIP)
		{
			// 绘图
			m_pOScopeCtrl->AppendPoint(&m_dADCDataProcBuf[uiInstrumentNb][uiADCDataFrameProcNb*(ReceiveDataSize + ADCFrameInfoSize) + ADCFrameInfoSize], ReceiveDataSize);
		}
	}
	else
	{
		// 判断是否是重发数据
		bADCRetransimission = OnCheckFrameIsRetransmission(uiInstrumentNb, uiADCDataFrameProcNb);
	}
	// 判断采样是否为噪声
	if (m_pSelectObjectNoise[uiInstrumentNb] == 0)
	{
		// 拷贝到文件缓冲区
		OnSaveADCFrameToFileBuf(uiInstrumentNb, bADCRetransimission, uiADCDataFrameProcNb);
	}
	
	// 索引表中的项发送ADC数据查询帧
	OnSendADCFrameFromMap();

}
// 检查接收帧是否为重发帧
BOOL CADCDataProcessThread::OnCheckFrameIsRetransmission(unsigned int uiInstrumentNb, unsigned int uiADCDataFrameProcNb)
{
	CString str = "";
	unsigned short usLostDataFrameCount = 0;	// 丢失帧计数
	unsigned short usDataPointMove = 0;	// 记录指针偏移量
	m_structADC ADCFindFromStruct;
	unsigned int uiDataCount = 0;		// 记录帧位置
	unsigned short usDataPointNow = 0;	// 记录指针偏移量
	unsigned int uiADCGraphIP;			// 记录设备IP地址
	usDataPointNow = (unsigned short)m_dADCDataProcBuf[uiInstrumentNb][uiADCDataFrameProcNb*(ReceiveDataSize + ADCFrameInfoSize)];
	uiADCGraphIP = 81 + uiInstrumentNb*10;
	// 接收到的ADC数据不是重发数据
	if (m_oADCLostMap[uiInstrumentNb].Lookup(usDataPointNow, ADCFindFromStruct) == NULL)
	{
		if (usDataPointNow > m_usDataPointPrevious[uiInstrumentNb])
		{
			usDataPointMove  = usDataPointNow - m_usDataPointPrevious[uiInstrumentNb];
		}
		else
		{
			usDataPointMove  = 2048 + usDataPointNow - m_usDataPointPrevious[uiInstrumentNb];
		}

		usLostDataFrameCount = usDataPointMove/ReceiveDataSize - 1;

		// 有ADC采集数据丢帧则加入ADC数据重发队列
		if (usLostDataFrameCount > 0)
		{
			for (unsigned short m=0; m<usLostDataFrameCount; m++)
			{
				ProcessMessages();
				m_structADC ADCDataStruct;
				unsigned short usDataPointNb = (m_usDataPointPrevious[uiInstrumentNb] + (m + 1) * ReceiveDataSize) % 2048;
				ADCDataStruct.uiDataCount = m_uiADCDataFrameProcNum[uiInstrumentNb];
				ADCDataStruct.uiRetransmissionNum = 0;
				m_oADCLostMap[uiInstrumentNb].SetAt(usDataPointNb, ADCDataStruct);
				m_uiADCDataFrameProcNum[uiInstrumentNb]++;
				//绘图
				if(uiADCGraphIP == m_uiADCGraphIP)
				{
					m_pOScopeCtrl->AppendPoint(&m_dADCDataProcBuf[uiInstrumentNb][uiADCDataFrameProcNb*(ReceiveDataSize + ADCFrameInfoSize) + ADCFrameInfoSize], ReceiveDataSize);
				}
			}
		}
		m_usDataPointPrevious[uiInstrumentNb] = usDataPointNow;
		// 绘图
		if(uiADCGraphIP == m_uiADCGraphIP)
		{
			m_pOScopeCtrl->AppendPoint(&m_dADCDataProcBuf[uiInstrumentNb][uiADCDataFrameProcNb*(ReceiveDataSize + ADCFrameInfoSize) + ADCFrameInfoSize], ReceiveDataSize);
		}
		return FALSE;

	}
	else
	{
		// 如果为重发数据则ADC应接收数据个数不变 
		m_uiADCDataFrameProcNum[uiInstrumentNb]--;
		// 记录重发帧的位置
		m_uiADCRetransmissionNb = ADCFindFromStruct.uiDataCount;
		m_oADCLostMap[uiInstrumentNb].RemoveKey(usDataPointNow);
		return TRUE;
	}
}
// 保存ADC数据帧到写文件临时缓冲区
void CADCDataProcessThread::OnSaveADCFrameToFileBuf(unsigned int uiInstrumentNb, BOOL bADCRetransimission, unsigned int uiADCDataFrameProcNb)
{
	unsigned int uiPos = 0;	// 存储目标的位置
	unsigned int uiPos2 = 0;	// 待处理的ADC数据起始位置
	// 如果是重发帧
	if (bADCRetransimission == TRUE)
	{
		uiPos = ((m_uiADCRetransmissionNb - 1)%(ADCFrameNumNeedToSave * ADCDataNeedToSaveBufNum)) * ReceiveDataSize;
	}
	else
	{
		uiPos = ((m_uiADCDataFrameProcNum[uiInstrumentNb] - 1)%(ADCFrameNumNeedToSave * ADCDataNeedToSaveBufNum)) * ReceiveDataSize;
	}
	uiPos2 = uiADCDataFrameProcNb*(ReceiveDataSize + ADCFrameInfoSize) + ADCFrameInfoSize;
	for (int i=0; i<ReceiveDataSize; i++)
	{
		m_dADCDataNeedToSaveBuf[uiInstrumentNb][uiPos + i] = m_dADCDataProcBuf[uiInstrumentNb][uiPos2 + i];
	}
//	memcpy(&m_dADCDataNeedToSaveBuf[uiInstrumentNb][uiPos], &m_dADCDataProcBuf[uiInstrumentNb][uiPos2], ReceiveDataSize);
	//先不把数据发送到写文件缓冲区
	OnCopyToFileBuf(uiInstrumentNb);
}

// 从索引表中清除过期数据
void CADCDataProcessThread::OnClearOutdatedFromMap(unsigned int uiInstrumentNb)
{
	POSITION pos = m_oADCLostMap[uiInstrumentNb].GetStartPosition();	// 得到索引表起始位置
	unsigned int uiKey;									// 索引键
	unsigned int uiCount = 0;							// 计数

	while(NULL != pos)
	{
		ProcessMessages();
		m_structADC ADCStructTemp;	
		m_oADCLostMap[uiInstrumentNb].GetNextAssoc(pos, uiKey, ADCStructTemp);	// 得到仪器对象
		if (ADCStructTemp.uiRetransmissionNum == ADCFrameRetransmissionNum)
		{
			m_oADCLostMap[uiInstrumentNb].RemoveKey(uiKey);
		}
		else if (m_uiADCDataFrameProcNum[uiInstrumentNb] % ADCFrameNumNeedToSave == 0)
		{
			if (m_uiADCDataFrameProcNum[uiInstrumentNb] > ADCFrameNumNeedToSave)
			{
				if (ADCStructTemp.uiDataCount < (m_uiADCDataFrameProcNum[uiInstrumentNb] - ADCFrameNumNeedToSave))
				{
					m_oADCLostMap[uiInstrumentNb].RemoveKey(uiKey);
				}
			}
		}
	}
}

// 从索引表中发送ADC数据查询帧
void CADCDataProcessThread::OnSendADCFrameFromMap(void)
{
	int icount = 0;
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		POSITION pos = m_oADCLostMap[i].GetStartPosition();	// 得到索引表起始位置
		unsigned int uiKey;									// 索引键	
		while(NULL != pos)
		{
			ProcessMessages();
			m_structADC ADCStructTemp;	
			m_oADCLostMap[i].GetNextAssoc(pos, uiKey, ADCStructTemp);	// 得到仪器对象
			
			ADCStructTemp.uiRetransmissionNum++;
			m_oADCLostMap[i].SetAt(uiKey, ADCStructTemp);
			//??????暂时去掉重发功能
		//	MakeADCDataRetransmissionFrame(uiKey, 81+i*10);
		}
	}
}
// 生成ADC数据重发帧
void CADCDataProcessThread::MakeADCDataRetransmissionFrame(unsigned short usDataPointNb, unsigned int uiIPAim)
{
	unsigned int uiIPSource =	0;
	unsigned int usPortAim	=	0;
	unsigned int usCommand	=	0;
	for (int i=0; i<SndFrameSize; i++)
	{
		ProcessMessages();
		m_ucADCRetransmission[i] = 0x00;
	}
	m_ucADCRetransmission[0] = 0x11;
	m_ucADCRetransmission[1] = 0x22;
	m_ucADCRetransmission[2] = 0x33;
	m_ucADCRetransmission[3] = 0x44;
	m_ucADCRetransmission[4] = 0x00;
	m_ucADCRetransmission[5] = 0x00;
	m_ucADCRetransmission[6] = 0x00;
	m_ucADCRetransmission[7] = 0x00;
	m_ucADCRetransmission[8] = 0x00;
	m_ucADCRetransmission[9] = 0x00;
	m_ucADCRetransmission[10] = 0x00;
	m_ucADCRetransmission[11] = 0x00;
	m_ucADCRetransmission[12] = 0x00;
	m_ucADCRetransmission[13] = 0x00;
	m_ucADCRetransmission[14] = 0x00;
	m_ucADCRetransmission[15] = 0x00;

	uiIPSource	=	inet_addr(m_csIPSource);
	usPortAim	=	ADRecPort;
	usCommand	=	SendADCRetransmissionCmd;
	// 源IP地址
	memcpy(&m_ucADCRetransmission[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_ucADCRetransmission[20], &uiIPAim, 4);
//	TRACE2("ADC数据重发帧-仪器IP地址：%d\t 指针偏移量为%d\r\n", uiIPAim, usDataPointNb);
	// 目标端口号
	memcpy(&m_ucADCRetransmission[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_ucADCRetransmission[26], &usCommand, 2);

	int iPos = 32;

	memmove(m_ucADCRetransmission + iPos, &usDataPointNb, 2);
	iPos = iPos + 2;
	m_ucADCRetransmission[iPos] = 0x00;

	unsigned short usCRC16 = 0;
	usCRC16 = get_crc_16(&m_ucADCRetransmission[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_ucADCRetransmission[SndFrameSize - CRCSize], &usCRC16, 2);
	int icount = m_ADCRetransmissionSocket.SendTo(m_ucADCRetransmission, SndFrameSize, m_uiSendPort, IPBroadcastAddr);
}
// 将临时缓冲区文件拷贝到文件存储缓冲区
void CADCDataProcessThread::OnCopyToFileBuf(unsigned int uiInstrumentNb)
{
	// 达到文件存储条件则复制数据到文件存储缓冲区
	if (m_uiADCDataFrameProcNum[uiInstrumentNb] % ADCFrameNumNeedToSave == 0)
	{
		unsigned int uiPos = 0;
		unsigned int uiSaveLength = 0;
		uiSaveLength = ReceiveDataSize * ADCFrameNumNeedToSave;
		uiPos = m_uiCopySaveBufNum[uiInstrumentNb] * uiSaveLength;

		unsigned int uiADCFrameSaveToFileRecNum = 0;
		m_Sec.Lock();
		uiADCFrameSaveToFileRecNum = m_pADCDataSaveToFileThread->m_uiADCFrameSaveToFileRecNum[uiInstrumentNb];
		m_Sec.Unlock();
		uiADCFrameSaveToFileRecNum = uiADCFrameSaveToFileRecNum * uiSaveLength;
		if (uiADCFrameSaveToFileRecNum > (ADCDataSaveToFileBufSize - uiSaveLength))
		{
			//????用来避免缓冲区溢出，临时方案
//			uiADCFrameSaveToFileRecNum = 0;
			AfxMessageBox(_T("ADC接收数据超过文件存储缓冲区大小！"));
		}
 		else
 		{
			m_Sec.Lock();
// 			memcpy(&(m_pADCDataSaveToFileThread->m_dADCSaveToFileRecBuf[uiInstrumentNb][uiADCFrameSaveToFileRecNum]), 
// 				&m_dADCDataNeedToSaveBuf[uiInstrumentNb][uiPos], uiSaveLength);
			for (unsigned int i=0; i<uiSaveLength; i++)
			{
				m_pADCDataSaveToFileThread->m_dADCSaveToFileRecBuf[uiInstrumentNb][uiADCFrameSaveToFileRecNum + i] = m_dADCDataNeedToSaveBuf[uiInstrumentNb][uiPos + i];
			}
			m_pADCDataSaveToFileThread->m_uiADCFrameSaveToFileRecNum[uiInstrumentNb]++;
			::SetEvent(m_pADCDataSaveToFileThread->m_hADCDataSaveToFileThreadWork);	// 设置事件对象为有信号状态
			m_Sec.Unlock();
		}
		memset(&m_dADCDataNeedToSaveBuf[uiInstrumentNb][uiPos], 0, uiSaveLength);
		m_uiCopySaveBufNum[uiInstrumentNb]++;
		if (m_uiCopySaveBufNum[uiInstrumentNb] == ADCDataNeedToSaveBufNum)
		{
			m_uiCopySaveBufNum[uiInstrumentNb] = 0;

		}
	}
}

// 采样停止时将剩余数据保存到文件缓冲区中
void CADCDataProcessThread::OnSaveRemainDataToFileBuf(void)
{
	unsigned int uiRemainNum = 0;
	unsigned int uiSaveLength = 0;
	unsigned int uiADCFrameSaveToFileRecNum = 0;
	unsigned int uiPos = 0;
	// 采样停止时将不足的帧发送到文件存储线程
	if (m_bStopSample == TRUE)
	{
		m_bStopSample = FALSE;
		m_Sec.Lock();
		// 复制缓冲区剩余内容到文件存储缓冲区
		for(int i=0; i<GraphViewNum; i++)
		{
			uiRemainNum = m_uiADCDataFrameProcNum[i] % ADCFrameNumNeedToSave;
			if (uiRemainNum != 0)
			{
				TRACE2("仪器序号%d, 剩余帧数%d", i, uiRemainNum);
				uiSaveLength = ReceiveDataSize * uiRemainNum;
				uiADCFrameSaveToFileRecNum = m_pADCDataSaveToFileThread->m_uiADCFrameSaveToFileProcNum[i];
				uiADCFrameSaveToFileRecNum = uiADCFrameSaveToFileRecNum * ReceiveDataSize * ADCFrameNumNeedToSave;
				uiPos = m_uiCopySaveBufNum[i] * ReceiveDataSize * ADCFrameNumNeedToSave;
				for (unsigned int j=0; j<uiSaveLength; j++)
				{
					m_pADCDataSaveToFileThread->m_dADCSaveToFileProcBuf[i][uiADCFrameSaveToFileRecNum + i] = m_dADCDataNeedToSaveBuf[i][uiPos + i];
				}
				m_pADCDataSaveToFileThread->m_uiADCFrameRemain[i] = uiRemainNum;
			}
		}
		m_pADCDataSaveToFileThread->m_bStopSample = TRUE;
		::SetEvent(m_pADCDataSaveToFileThread->m_hADCDataSaveToFileThreadWork);	// 设置事件对象为有信号状态
		m_Sec.Unlock();
	}
}
