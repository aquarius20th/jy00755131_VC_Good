// ADCDataRecThread.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "ADCDataRecThread.h"


// CADCDataRecThread

IMPLEMENT_DYNCREATE(CADCDataRecThread, CWinThread)

CADCDataRecThread::CADCDataRecThread()
: m_bclose(false)
, m_pSelectObject(NULL)
, m_pSelectObjectNoise(NULL)
//, m_pwnd(NULL)
, m_bStartSample(FALSE)
, m_csIPSource(_T(""))
, m_uiSendPort(0)
, m_uiTestADCLost(0)
, m_csSaveFilePath(_T(""))
, m_uiADCSaveFileNum(0)
, m_pFileSave(NULL)
, m_bOpenADCSaveFile(FALSE)
, m_uiADCDataToSaveNum(0)
, m_uiADCFileLength(0)
, m_uiADCGraphIP(0)
, m_pOScopeCtrl(NULL)
, m_uiADCRetransmissionNb(0)
{
}

CADCDataRecThread::~CADCDataRecThread()
{
}

BOOL CADCDataRecThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CADCDataRecThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CADCDataRecThread, CWinThread)
END_MESSAGE_MAP()


// CADCDataRecThread 消息处理程序

int CADCDataRecThread::Run()
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	DWORD dwFrameCount = 0;
	int icount = 0;
	// 循环，直到关闭标志为真
	while(true)
	{
		if(m_bclose == true)
		{
			break;
		}
		ProcessMessages();
		// 得到网络接收缓冲区数据字节数
		m_ADCDataSocket.IOCtl(FIONREAD, &dwFrameCount);
		dwFrameCount = dwFrameCount/256;
		if(0 != dwFrameCount)	
		{
			for(unsigned int i = 0; i < dwFrameCount; i++)	// 依次检查每个仪器IP设置应答帧
			{
				ProcessMessages();
				icount = m_ADCDataSocket.Receive(ADCData,256);
				if (icount == 256)
				{
					byte* pSampleData = &(ADCData[16]);

					ReceiveSampleData(pSampleData);
				}
				else
				{
					break;
				}
			}
			OnADCDataRetransmission();
		}

		if (m_bclose == true)
		{
			break;
		}
		Sleep(50);
	}
	::SetEvent(m_hADCDataThreadClose);	// 设置事件对象为有信号状态

	return CWinThread::Run();
}

// 初始化
void CADCDataRecThread::OnInit(void)
{
	m_hADCDataThreadClose = ::CreateEvent(false, false, NULL, NULL);	// 创建事件对象
	::ResetEvent(m_hADCDataThreadClose);	// 设置事件对象为无信号状态

	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_uiADCDataFrameCount[i] = 0;
		m_usDataPointPrevious[i] = 0;
		m_uiADCDataFrameRecCount[i] = 0;
		m_dADCDataShow[i] = 0;
	}
}

// 关闭并结束线程
void CADCDataRecThread::OnClose(void)
{
	m_listADC.RemoveAll();
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_dSampleTemp[i].clear();
		m_dADCSave[i].clear();
	}
	m_ADCDataSocket.Close();
	m_bclose = true;
}

// 打开
void CADCDataRecThread::OnOpen(void)
{	
	m_listADC.RemoveAll();
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_dSampleTemp[i].clear();
	}
	m_ADCDataSocket.Close();
	BOOL flag = FALSE; 
	flag =  m_ADCDataSocket.Create(ADRecPort,SOCK_DGRAM);
	if (flag == FALSE)
	{
		AfxMessageBox(_T("ADC数据接收端口创建失败！"));
	}
	//设置广播模式
	int	iOptval, iOptlen;
	iOptlen = sizeof(int);
	iOptval = 1;
	m_ADCDataSocket.SetSockOpt(SO_BROADCAST, &iOptval, iOptlen, SOL_SOCKET);
}

// 停止
void CADCDataRecThread::OnStop(void)
{
	m_listADC.RemoveAll();
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_dSampleTemp[i].clear();
	}
	m_ADCDataSocket.Close();
}

// 接收采样数据
void CADCDataRecThread::ReceiveSampleData(byte* pSampleData)
{
	// uiTestNb表明接收到帧所对应的仪器号
	unsigned int	uiIPAim = 0;
	unsigned short	usDataPointNow = 0; // 接收到帧的数据的序号
	int	iSampleData = 0;
	int pos = 0;
	double dSampleDataToV = 0.0;
	unsigned int uitmp = 0;
	BOOL bADCRetransimission = FALSE;			// 判断是否为重发帧的标志位

	// 设m_pFrameData[16]到m_pFrameData[19]为源地址做为通道号
	memcpy(&uiIPAim, &pSampleData[pos], 4);
	pos += 4;

	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		if (m_pSelectObject[i] == 1)
		{
			uitmp = 81 + 10*i;
			if (uiIPAim	== uitmp)
			{
				// 接收数据缓存
				double dReceiveData [ReceiveDataSize]; 
				usDataPointNow = 0;
				pos += 8;
				// m_pFrameData[28]到m_pFrameData[29]做为采样个数的序号，暂时不用
				memcpy(&usDataPointNow, &pSampleData[pos], 2);
				pos += 2;
				// 用于测试ADC丢帧后补帧
				if (usDataPointNow == 24)
				{
					if (uiIPAim == 81)
					{	
						m_uiTestADCLost++;
						if (m_uiTestADCLost == 1)
						{
							break;
						}
					}
				}
				// ADC实际接收数据个数加一
				m_uiADCDataFrameRecCount[i]++;
				// ADC应接收数据个数加一
				m_uiADCDataFrameCount[i]++;

				if (m_uiADCDataFrameCount[i] == 1)
				{
					m_usDataPointPrevious[i] = usDataPointNow;
				}
				else
				{
					bADCRetransimission = OnCheckFrameIsRetransmission(i, uiIPAim, usDataPointNow);
				}

				// 之后为数据区
				for (int j=0; j<ReceiveDataSize; j++)
				{
					ProcessMessages();
					memcpy(&iSampleData, &pSampleData[pos], 3);
					pos += 3;

					// 24位数转换为电压值
					dSampleDataToV = iSampleData;
					if (dSampleDataToV < 0x7FFFFF)
					{
						dSampleDataToV = dSampleDataToV/( 0x7FFFFE )*2.5;
					}
					else if (dSampleDataToV > 0x800000)
					{
						dSampleDataToV = -(0xFFFFFF - dSampleDataToV)/( 0x7FFFFE )*2.5;
					}
					else
					{
// 						CString str;
// 						str.Format("ADC采样数值溢出，溢出值为%d", dSampleDataToV);
// 						AfxMessageBox(str);
					}
					dReceiveData[j] = dSampleDataToV;
				}
				m_dADCDataShow[i] = dSampleDataToV;
				// 无数据重发帧或已经全部接收到数据重发帧
				if(m_listADC.GetCount() == 0)
				{
					// 如果该帧为数据重发帧，处理该帧后再处理保存帧
					if (bADCRetransimission == TRUE)
					{
						OnRecOkIsRetransimissionFrame(i, uiIPAim, dReceiveData);
					}
					else
					{
						OnRecOkIsNormalFrame(i, uiIPAim, dReceiveData);
					}
				}
				else
				{
					// 如果该帧为数据重发帧
					if (bADCRetransimission == TRUE)
					{
						OnRecNotOkIsRetransimissionFrame(i, uiIPAim, dReceiveData);
					}
					else
					{
						// 如果有重发帧，而本帧不是重发帧则先将该帧保存
						OnRecNotOkIsNormalFrame(i, dReceiveData);
					}
				}
				// 将采样数据保存成文件
				OnSaveADCToFile();
				break;
			}
		}
	}
}
// 生成ADC数据重发帧
void CADCDataRecThread::MakeADCDataRetransmissionFrame(unsigned short usDataPointNb, unsigned int uiIPAim)
{
	unsigned int uiIPSource =	0;
	unsigned int usPortAim	=	0;
	unsigned int usCommand	=	0;
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
	usCommand	=	3;
	// 源IP地址
	memcpy(&m_ucADCRetransmission[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_ucADCRetransmission[20], &uiIPAim, 4);
	TRACE2("ADC数据重发帧-仪器IP地址：%d\t 指针偏移量为%d\r\n", uiIPAim, usDataPointNb);
	// 目标端口号
	memcpy(&m_ucADCRetransmission[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_ucADCRetransmission[26], &usCommand, 2);

	int iPos = 32;

	memmove(m_ucADCRetransmission + iPos, &usDataPointNb, 2);
	iPos = iPos + 2;
	m_ucADCRetransmission[iPos] = 0x00;

	int icount = m_ADCDataSocket.SendTo(m_ucADCRetransmission, 128, m_uiSendPort, IPBroadcastAddr);
}
// ADC数据需要重新发送帧对应的最小个数
unsigned int CADCDataRecThread::OnADCRetransimissionMinNb(void)
{
	POSITION pos = m_listADC.GetHeadPosition();
	int icount = m_listADC.GetCount();
	unsigned int uimin = 0;
	m_structADC ADCStructTemp;

	ADCStructTemp = m_listADC.GetNext(pos);
	uimin = ADCStructTemp.uiDataCount;

	for (int i = 1; i < icount; i++)
	{
		ProcessMessages();
		ADCStructTemp = m_listADC.GetNext(pos);
		if (uimin > ADCStructTemp.uiDataCount)
		{
			uimin = ADCStructTemp.uiDataCount;
		}
	} 
	return uimin;
}
// 创建并打开ADC保存数据文件
void CADCDataRecThread::OnOpenADCSaveFile(void)
{
	m_uiADCSaveFileNum++;

	CString strFileName = "";
	CString strOutput = "";
	CString strTemp = "";
	strFileName.Format("%s\\%d.text", m_csSaveFilePath,m_uiADCSaveFileNum);
	// 将ADC采样数据保存成ANSI格式的文件
	errno_t err;
	if((err = fopen_s(&m_pFileSave,strFileName,"w+"))!=NULL)
	{
		AfxMessageBox(_T("ADC数据存储文件创建失败！"));	
		return;
	}

	CString str = "";
	SYSTEMTIME  sysTime;
	unsigned int uiADCDataFrameCount = 0;
	GetLocalTime(&sysTime);
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		if (m_pSelectObject[i] == 1)
		{
			if (m_pSelectObjectNoise[i] == 0)
			{
				uiADCDataFrameCount = m_uiADCDataFrameCount[i];
				break;
			}
		}
	}
	str.Format(_T("%04d年%02d月%02d日%02d:%02d:%02d:%04d 由第%d个数据包开始记录ADC采样数据：\r\n\r\n"), sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds, uiADCDataFrameCount);
	strOutput += str;

	// 输出仪器标签
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		strTemp.Format("%s\t\t",m_cSelectObjectName[i]);
		strOutput += strTemp;
	}
	strOutput += "\r\n";

	fprintf(m_pFileSave,"%s", strOutput); 
	m_bOpenADCSaveFile = TRUE;
}
// 关闭ADC保存数据文件
void CADCDataRecThread::OnCloseADCSaveFile(void)
{
	if (m_pFileSave == NULL)
	{
		return;
	}
	fclose(m_pFileSave); 
	m_bOpenADCSaveFile = FALSE;
}
// 将ADC数据保存到文件中
void CADCDataRecThread::OnSaveADCToFile(void)
{
	CString strOutput = "";
	CString strTemp = "";
	unsigned int uiADCDataToSaveNum = 0;
	vector<double> dTemp;

	if(m_bOpenADCSaveFile == FALSE)
	{
		OnOpenADCSaveFile();
	}
	uiADCDataToSaveNum = OnADCRecDataMinNum();

	if (uiADCDataToSaveNum == 0)
	{
		return;
	}
	if ((m_uiADCDataToSaveNum + uiADCDataToSaveNum) > m_uiADCFileLength)
	{
		uiADCDataToSaveNum = m_uiADCFileLength - m_uiADCDataToSaveNum;
	}


	// 输出各仪器采样数据
	for (unsigned int i=0; i<uiADCDataToSaveNum; i++)
	{
		ProcessMessages();
		for (int j=0; j<GraphViewNum; j++)
		{
			ProcessMessages();
			if (m_pSelectObject[j] == 1)
			{
				if (m_pSelectObjectNoise[j] == 0)
				{
					strTemp.Format("%2.*lf\t",DecimalPlaces, m_dADCSave[j][i]);
					strOutput += strTemp;
				}
				else
				{
					strOutput += "\t\t";
				}
			}
			else
			{
				strOutput += "\t\t";
			}
		}
		strOutput += "\r\n";
	}
	fprintf(m_pFileSave,"%s", strOutput); 

	// 清除保存过的数据
	for (int j=0; j<GraphViewNum; j++)
	{
		ProcessMessages();
		if (m_pSelectObject[j] == 1)
		{
			if (m_pSelectObjectNoise[j] == 0)
			{
				for (unsigned int i=uiADCDataToSaveNum; i<m_dADCSave[j].size(); i++)
				{
					ProcessMessages();
					dTemp.push_back(m_dADCSave[j][i]);
				}
				m_dADCSave[j].clear();
				for (unsigned int k=0; k<dTemp.size();k++)
				{
					ProcessMessages();
					m_dADCSave[j].push_back(dTemp[k]);
				}
				dTemp.clear();
			}
		}
	}
	m_uiADCDataToSaveNum += uiADCDataToSaveNum;
	if (m_uiADCDataToSaveNum == m_uiADCFileLength)
	{
		m_uiADCDataToSaveNum = 0;
		OnCloseADCSaveFile();
	}
}
unsigned int CADCDataRecThread::OnADCRecDataMinNum(void)
{
	unsigned int uiMinSize = 10000;
	bool bSelect = false;
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		if (m_pSelectObject[i] == 1)
		{
			if (m_pSelectObjectNoise[i] == 0)
			{
				if (uiMinSize > m_dADCSave[i].size())
				{
					uiMinSize = m_dADCSave[i].size();
				}
				bSelect = true;
			}
		}
	}
	if (bSelect == false)
	{
		uiMinSize = 0;
	}
	return uiMinSize;
}

// 关闭采样后保存剩余数据
void CADCDataRecThread::OnSaveRemainADCData(unsigned int uiADCDataNeedToSave)
{
	unsigned int uiDataLength = 0;
	CString strOutput = "";
	CString strTemp ="";
	if (m_pFileSave == NULL)
	{
		return;
	}
	for (unsigned int i=0; i<uiADCDataNeedToSave; i++)
	{
		ProcessMessages();
		for (int j=0; j<GraphViewNum; j++)
		{
			ProcessMessages();
			if (m_pSelectObject[j] == 1)
			{
				if (m_pSelectObjectNoise[j] == 0)
				{
					uiDataLength = m_dADCSave[j].size();
					if (uiDataLength == 0 )
					{
						strOutput += "\t\t";
						continue;
					}
					if (uiDataLength > i)
					{	
						strTemp.Format("%2.*lf\t",DecimalPlaces, m_dADCSave[j][i]);
						strOutput += strTemp;
					}
					else
					{
						strOutput += "\t\t";
					}
				}
				else
				{
					strOutput += "\t\t";
				}
			}
			else
			{
				strOutput += "\t\t";
			}
		}
		strOutput += "\r\n";
	}
	fprintf(m_pFileSave,"%s", strOutput); 
	// 清空接收缓冲区

	for (unsigned int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		m_dADCSave[i].clear();
	}
	OnCloseADCSaveFile();
}


// ADC数据重发
void CADCDataRecThread::OnADCDataRetransmission(void)
{
	int icount = 0;
	unsigned int uiIPAim = 0;
	unsigned int uiInstrument = 0;
	m_Sec.Lock();
	icount = m_listADC.GetCount();
	m_Sec.Unlock();
	if (icount>0)
	{
		m_structADC ADCStructTemp;
		m_Sec.Lock();
		POSITION pos = m_listADC.GetHeadPosition();
		m_Sec.Unlock();
		for (int i = 0; i < icount; i++)
		{
			ProcessMessages();
			m_Sec.Lock();
			ADCStructTemp = m_listADC.GetNext(pos);
			m_Sec.Unlock();
			if (ADCStructTemp.uiRetransmissionNum == 0)
			{
				ADCStructTemp.uiRetransmissionNum++;
				MakeADCDataRetransmissionFrame(ADCStructTemp.usDataPointNb, ADCStructTemp.uiIPAim);
			}
			else if (ADCStructTemp.uiRetransmissionNum == ADCFrameRetransmissionNum)
			{
				double dReceiveData[ReceiveDataSize];
				ADCStructTemp.uiRetransmissionNum = 0;
				m_uiADCRetransmissionNb = ADCStructTemp.uiDataCount;
				uiIPAim = ADCStructTemp.uiIPAim;
				m_Sec.Lock();
				m_listADC.RemoveAt(pos);
				m_Sec.Unlock();
				for (int j=0; j<GraphViewNum; j++)
				{
					ProcessMessages();
					if (uiIPAim == 81 + 10*j)
					{
						uiInstrument = j;
						break;
					}
				}

				for (int k=0; k<ReceiveDataSize; k++)
				{
					ProcessMessages();
					dReceiveData[k] = 0.0;
				}
				// 已经全部接收到数据重发帧
				if(icount == 1)
				{
					OnRecOkIsRetransimissionFrame(uiInstrument, uiIPAim, dReceiveData);
				}
				else
				{
					OnRecNotOkIsRetransimissionFrame(uiInstrument, uiIPAim, dReceiveData);
				}
				// 将采样数据保存成文件
				OnSaveADCToFile();

			}
		}    
	}
}

// 防止程序在循环中运行无法响应消息
void CADCDataRecThread::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
// 检查接收帧是否为重发帧
BOOL CADCDataRecThread::OnCheckFrameIsRetransmission(unsigned int uiInstrumentNb, unsigned int uiIPAim, unsigned short usDataPointNow)
{
	CString str = "";
	unsigned short usLostDataFrameCount = 0;	// 丢失帧计数
	unsigned short usADCDataNb = ReceiveDataSize;	// ADC一帧的数据个数
	unsigned short usDataPointMove = 0;	// 记录指针偏移量
	m_structADC ADCFindFromStruct;
	POSITION pos;
	ADCFindFromStruct.uiIPAim = uiIPAim;
	ADCFindFromStruct.usDataPointNb = usDataPointNow;
	pos = m_listADC.Find(ADCFindFromStruct, NULL);
	// 接收到的ADC数据不是重发数据
	if (pos == NULL)
	{
		if (usDataPointNow > m_usDataPointPrevious[uiInstrumentNb])
		{
			usDataPointMove  = usDataPointNow - m_usDataPointPrevious[uiInstrumentNb];
		}
		else
		{
			usDataPointMove  = 2048 + usDataPointNow - m_usDataPointPrevious[uiInstrumentNb];
		}

		usLostDataFrameCount = usDataPointMove/usADCDataNb - 1;

		// 有ADC采集数据丢帧则加入ADC数据重发队列
		if (usLostDataFrameCount > 0)
		{
			for (unsigned short m=0; m<usLostDataFrameCount; m++)
			{
				ProcessMessages();
				m_structADC ADCDataStruct;
				ADCDataStruct.uiIPAim = uiIPAim;
				ADCDataStruct.usDataPointNb = (m_usDataPointPrevious[uiInstrumentNb] + (m+1)*usADCDataNb)%2048;
				ADCDataStruct.uiDataCount = m_uiADCDataFrameCount[uiInstrumentNb] + m;
				ADCDataStruct.uiRetransmissionNum = 0;
				m_listADC.AddTail(ADCDataStruct);
				for (int i=0; i<ReceiveDataSize; i++)
				{
					ProcessMessages();
					m_dSampleTemp[uiInstrumentNb].push_back(0.0);
				}
			}
			m_uiADCDataFrameCount[uiInstrumentNb] += usLostDataFrameCount;
		}

		m_usDataPointPrevious[uiInstrumentNb] = usDataPointNow;
		return FALSE;

	}
	else
	{
		// 如果为重发数据则ADC应接收数据个数不变 
		m_uiADCDataFrameCount[uiInstrumentNb]--;
		ADCFindFromStruct = m_listADC.GetAt(pos);
		m_uiADCRetransmissionNb = ADCFindFromStruct.uiDataCount;
		m_listADC.RemoveAt(pos);
		return TRUE;
	}
}
// 该帧为最后一个数据重发帧
void CADCDataRecThread::OnRecOkIsRetransimissionFrame(unsigned int uiInstrumentNb, unsigned int uiIPAim, double* pReceiveData)
{
	unsigned short usADCDataNb = ReceiveDataSize;	// ADC一帧的数据个数
	if(uiIPAim == m_uiADCGraphIP)
	{
		m_Sec.Lock();
		m_pOScopeCtrl->AppendPoint(pReceiveData, ReceiveDataSize);
		m_Sec.Unlock();
	}

	for (int t=0; t<ReceiveDataSize; t++)
	{
		ProcessMessages();
		m_dADCSave[uiInstrumentNb].push_back(pReceiveData[t]);
	}
	// 接收到重发帧之前有保存帧
	if (m_dSampleTemp[uiInstrumentNb].size() > ReceiveDataSize)
	{
		// 将绘制完成的重发帧从缓冲区中移除
		vector<double> dTemp;
		for (unsigned int n=usADCDataNb; n<m_dSampleTemp[uiInstrumentNb].size(); n++)
		{
			ProcessMessages();
			dTemp.push_back(m_dSampleTemp[uiInstrumentNb][n]);
		}
		m_dSampleTemp[uiInstrumentNb].clear();
		for (unsigned int p=0; p<dTemp.size();p++)
		{
			ProcessMessages();
			m_dSampleTemp[uiInstrumentNb].push_back(dTemp[p]);
		}
		dTemp.clear();

		for (unsigned int m=0; m<(m_dSampleTemp[uiInstrumentNb].size()/usADCDataNb);m++)
		{
			ProcessMessages();
			for (int k=0; k<ReceiveDataSize; k++)
			{
				ProcessMessages();
				pReceiveData[k] = m_dSampleTemp[uiInstrumentNb][m*usADCDataNb+k];
			}
			if(uiIPAim == m_uiADCGraphIP)
			{
				m_Sec.Lock();
				m_pOScopeCtrl->AppendPoint(pReceiveData, ReceiveDataSize);
				m_Sec.Unlock();
			}
			for (int t=0; t<ReceiveDataSize; t++)
			{
				ProcessMessages();
				m_dADCSave[uiInstrumentNb].push_back(pReceiveData[t]);
			}
		}
		m_dSampleTemp[uiInstrumentNb].clear();
	}
}

// 该帧为普通帧且之前没有重发帧
void CADCDataRecThread::OnRecOkIsNormalFrame(unsigned int uiInstrumentNb, unsigned int uiIPAim, double* pReceiveData)
{
	if(uiIPAim == m_uiADCGraphIP)
	{
		m_Sec.Lock();
		m_pOScopeCtrl->AppendPoint(pReceiveData, ReceiveDataSize);
		m_Sec.Unlock();
	}
	for (int t=0; t<ReceiveDataSize; t++)
	{
		ProcessMessages();
		m_dADCSave[uiInstrumentNb].push_back(pReceiveData[t]);
	}
}

// 该帧为重发帧但不是最后一个重发帧
void CADCDataRecThread::OnRecNotOkIsRetransimissionFrame(unsigned int uiInstrumentNb, unsigned int uiIPAim, double* pReceiveData)
{
	unsigned short usADCDataNb = ReceiveDataSize;	// ADC一帧的数据个数
	unsigned int	uiMinNb = 0;		// 重发帧中的最小位置（个数）
	uiMinNb = OnADCRetransimissionMinNb();
	// 该重发帧之前各帧均已经收到
	if (uiMinNb > m_uiADCRetransmissionNb)
	{
		if(uiIPAim == m_uiADCGraphIP)
		{
			m_Sec.Lock();
			m_pOScopeCtrl->AppendPoint(pReceiveData, ReceiveDataSize);
			m_Sec.Unlock();
		}
		for (int t=0; t<ReceiveDataSize; t++)
		{
			ProcessMessages();
			m_dADCSave[uiInstrumentNb].push_back(pReceiveData[t]);
		}

		// 将绘制完成的重发帧从缓冲区中移除
		vector<double> dTemp;
		dTemp.clear();
		for (unsigned int n=usADCDataNb; n<m_dSampleTemp[uiInstrumentNb].size(); n++)
		{
			ProcessMessages();
			dTemp.push_back(m_dSampleTemp[uiInstrumentNb][n]);
		}
		m_dSampleTemp[uiInstrumentNb].clear();
		for (unsigned int p=0; p<dTemp.size();p++)
		{
			ProcessMessages();
			m_dSampleTemp[uiInstrumentNb].push_back(dTemp[p]);
		}
		dTemp.clear();

		// 如果等待重发帧的队列中最小的采样帧位置和接收到的重发帧之间有保存帧
		if (uiMinNb > (m_uiADCRetransmissionNb + 1))
		{
			for (unsigned int m=0; m<(uiMinNb - m_uiADCRetransmissionNb - 1); m++)
			{
				ProcessMessages();
				for (int k=0; k<ReceiveDataSize; k++)
				{
					ProcessMessages();
					pReceiveData[k] = m_dSampleTemp[uiInstrumentNb][m*usADCDataNb+k];
				}
				if(uiIPAim == m_uiADCGraphIP)
				{
					m_Sec.Lock();
					m_pOScopeCtrl->AppendPoint(pReceiveData, ReceiveDataSize);
					m_Sec.Unlock();
				}
				for (int t=0; t<ReceiveDataSize; t++)
				{
					ProcessMessages();
					m_dADCSave[uiInstrumentNb].push_back(pReceiveData[t]);
				}
			}
			// 将绘制完成的保存帧移除
			vector<double> dTemp2;
			dTemp2.clear();
			for (unsigned int n=(uiMinNb - m_uiADCRetransmissionNb - 1)*usADCDataNb; n<m_dSampleTemp[uiInstrumentNb].size(); n++)
			{
				ProcessMessages();
				dTemp2.push_back(m_dSampleTemp[uiInstrumentNb][n]);
			}
			m_dSampleTemp[uiInstrumentNb].clear();
			for (unsigned int p=0; p<dTemp2.size();p++)
			{
				ProcessMessages();
				m_dSampleTemp[uiInstrumentNb].push_back(dTemp2[p]);
			}
			dTemp2.clear();
		}
	}
	// 该帧之前还有重发帧未收到
	else
	{
		// 将重发帧数据保存到缓冲区
		for (int i=0; i<ReceiveDataSize; i++)
		{
			ProcessMessages();
			m_dSampleTemp[uiInstrumentNb][(m_uiADCRetransmissionNb - uiMinNb)*usADCDataNb + i] = pReceiveData[i];
		}
	}

}
// 该帧为普通帧且之前有重发帧
void CADCDataRecThread::OnRecNotOkIsNormalFrame(unsigned int uiInstrumentNb, double* pReceiveData)
{
	for (int k=0; k<ReceiveDataSize; k++)
	{
		ProcessMessages();
		m_dSampleTemp[uiInstrumentNb].push_back(pReceiveData[k]);
	}
}