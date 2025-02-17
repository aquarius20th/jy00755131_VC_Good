// ADCDataRecThread.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "ADCDataRecThread.h"
#include <algorithm>

using std::vector;
// CADCDataRecThread

IMPLEMENT_DYNCREATE(CADCDataRecThread, CWinThread)

CADCDataRecThread::CADCDataRecThread()
: m_bclose(false)
, m_bStartSample(FALSE)
, m_uiIPSource(0)
, m_uiTestADCLost(0)
, m_uiADCRetransmissionNb(0)
, m_uiUdpCount(0)
, m_pADCDataSaveToFile(NULL)
, m_pADCFrameInfo(NULL)
, m_pLogFile(NULL)
, m_ADCDataSocket(INVALID_SOCKET)
, m_ADCGraphShowSocket(INVALID_SOCKET)
, m_uispsSelect(0)
, m_pInstrumentList(NULL)
, m_uiSamplingRate(1)
, m_bStartGraphShow(FALSE)
, m_bCheckADCDataFrameCountEqule(FALSE)
{
	for (int i=0; i<InstrumentMaxCount; i++)
	{
		m_pOScopeCtrl[i] = NULL;
	}
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
	sockaddr_in SenderAddr;
	int n = sizeof(SenderAddr);
	int iError = 0;
	CString str = _T("");
	// 循环，直到关闭标志为真
	while(true)
	{
		if(m_bclose == true)
		{
			break;
		}
//		ProcessMessages();
		// 得到网络接收缓冲区数据字节数
		while(SOCKET_ERROR != ioctlsocket(m_ADCDataSocket, FIONREAD, &dwFrameCount))
		{
			if(dwFrameCount > 0) 
			{
				icount = recvfrom(m_ADCDataSocket, reinterpret_cast<char *>(&m_ucUdpBuf), sizeof(m_ucUdpBuf), 0, reinterpret_cast<sockaddr*>(&SenderAddr), &n);
// 				str.Format(_T("从ADC数据接收缓冲区读取数据大小为%d！"), icount);
// 				m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::Run"), str, SuccessStatus);
				if (icount != SOCKET_ERROR)
				{
					OnProcess(icount);
				}
				else if(icount == ADCDataBufSize)
				{
					str.Format(_T("ADC数据接收帧超过缓冲区大小，缓冲区大小为%d！"), ADCDataBufSize);
					m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::Run"), str, ErrorStatus);
				}
				else
				{
					iError = WSAGetLastError();
					str.Format(_T("ADC数据接收帧错误，错误号为%d！"), iError);
					m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::Run"), str, ErrorStatus);
				}
			}
			else
			{
				break;
			}
		}
//		OnADCDataRetransmission();
// 		str.Format(_T("sleep！"));
// 		m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::Run"), str, SuccessStatus);
		if (m_bclose == true)
		{
			break;
		}
		Sleep(ADCDataRecThreadSleepTime);
	}
	::SetEvent(m_hADCDataThreadClose);	// 设置事件对象为有信号状态

	return CWinThread::Run();
}
// 数据处理
//************************************
// Method:    OnProcess
// FullName:  CADCDataRecThread::OnProcess
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int iCount
//************************************
void CADCDataRecThread::OnProcess(int iCount)
{
	unsigned short usCRC16 = 0;
	for (int i=0; i<iCount; i++)
	{
//		ProcessMessages();
		switch(m_uiUdpCount)
		{
		case 0:
			if (m_ucUdpBuf[i] == FrameHeadCheck0)
			{
				m_ucudp_buf[0] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}

		case 1:
			if (m_ucUdpBuf[i] == FrameHeadCheck1)
			{
				m_ucudp_buf[1] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}
		case 2:
			if (m_ucUdpBuf[i] == FrameHeadCheck2)
			{
				m_ucudp_buf[2] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}
		case 3:
			if (m_ucUdpBuf[i] == FrameHeadCheck3)
			{
				m_ucudp_buf[3] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				break;
			}
			else
			{
				//	m_uiUdpCount = 0;
				m_uiUdpCount++;
				break;
			}
		default:
			if (m_uiUdpCount >=  RcvFrameSize)
			{
				m_uiUdpCount = 0;
				break;
			}
			m_ucudp_buf[m_uiUdpCount] = m_ucUdpBuf[i];
			m_uiUdpCount++;
			if (m_uiUdpCount == RcvFrameSize)
			{
				m_uiUdpCount = 0;
				if (m_bStartSample == TRUE)
				{
					memcpy(&usCRC16, &m_ucudp_buf[RcvFrameSize - CRCSize], CRCSize);
					if (usCRC16 != get_crc_16(&m_ucudp_buf[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
					{
						//	continue;
					}
					ReceiveSampleData(&m_ucudp_buf[0]);
					/*OnADCDataRetransmission();*/
				}
			}
			break;
		}
	}
}
// 初始化
void CADCDataRecThread::OnInit(void)
{
	m_hADCDataThreadClose = ::CreateEvent(false, false, NULL, NULL);	// 创建事件对象
	if ((m_hADCDataThreadClose == NULL)||(GetLastError() == ERROR_ALREADY_EXISTS))
	{
		AfxMessageBox(_T("创建端口监视发送线程关闭事件出错！"));
	}
	else
	{
		::ResetEvent(m_hADCDataThreadClose);	// 设置事件对象为无信号状态
	}

	for (int i=0; i<InstrumentMaxCount; i++)
	{
		memset(&m_iSampleTemp[i], SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
		m_uiSampleTempNum[i] = 0;
		memset(&m_iADCSave[i], SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
		m_uiADCSaveNum[i] = 0;
	}
	memset(&m_iTemp, SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
}

// 关闭并结束线程
void CADCDataRecThread::OnClose(void)
{
	OnCloseUDP();
	for (int i=0; i<InstrumentMaxCount; i++)
	{
		m_oADCLostMap[i].clear();
	}
	m_bclose = true;
}
// 接收采样数据
void CADCDataRecThread::ReceiveSampleData(byte* pSampleData)
{
	// uiTestNb表明接收到帧所对应的仪器号
	unsigned int	uiIPAim = 0;
	unsigned short	usDataPointNow = 0; // 接收到帧的数据的序号
	int	iSampleData = 0;
	double dSampleDataToV = 0;
	double dReceiveData [ReceiveDataSize];			// 接收数据缓存
	int iPos = FrameHeadSize;
	unsigned int uiSysTime = 0;				// ADC数据帧发送时仪器的本地时间
	DWORD dwADCRetransimission = 0;				// 判断是否为重发帧的标志位
	int iReceiveData [ReceiveDataSize];			// 接收数据缓存
	CInstrument* pInstrument = NULL;
	// 设m_pFrameData[16]到m_pFrameData[19]为源地址做为通道号
	memcpy(&uiIPAim, &pSampleData[iPos], FramePacketSize4B);
	iPos += FramePacketSize4B;
	memset(&dReceiveData, 0, sizeof(double) * ReceiveDataSize);
	memset(&iReceiveData, 0, sizeof(int) * ReceiveDataSize);
	if (m_pInstrumentList->GetInstrumentFromIPMap(uiIPAim, pInstrument))
	{
		unsigned int uiLocation = pInstrument->m_uiFDUIndex;
		usDataPointNow = 0;
		iPos += FramePacketSize4B;
		iPos += FramePacketSize2B;
		iPos += FramePacketSize2B;
		// m_pFrameData[28]到m_pFrameData[29]做为AD数据起始指针
		memcpy(&usDataPointNow, &pSampleData[iPos], FramePacketSize2B);
		iPos += FramePacketSize2B;

		// 如果是72个数一帧，记录本地时间
		memcpy(&uiSysTime, &pSampleData[iPos], FramePacketSize4B);
		iPos += ADCDataSize3B;
		iPos += ADCDataSize3B;

		m_pADCFrameInfo->OnSaveFile(uiLocation, usDataPointNow, uiSysTime);

		// ADC实际接收数据个数加一
		m_uiADCDataFrameRecCount[uiLocation]++;
		// ADC应接收数据个数加一
		m_uiADCDataFrameCount[uiLocation]++;

		// 之后为数据区
		for (int j=0; j<ReceiveDataSize; j++)
		{
			iSampleData = 0;
			memcpy(&iSampleData, &pSampleData[iPos], ADCDataSize3B);
			iPos += ADCDataSize3B;

			// 24位数转换为电压值
// 			dSampleDataToV = iSampleData;
// 			if (dSampleDataToV < 0x7FFFFF)
// 			{
// 				dSampleDataToV = dSampleDataToV/( 0x7FFFFE ) * DrawGraphYAxisUpper;
// 			}
// 			else if (iSampleDataToV > 0x800000)
// 			{
// 				dSampleDataToV = (0xFFFFFF - dSampleDataToV)/( 0x7FFFFE ) * (DrawGraphYAxisLower);
// 			}
	
			if (iSampleData > 0x800000)
			{
				iSampleData = - (0xFFFFFF - iSampleData);
			}

			iReceiveData[j] = iSampleData;
			dSampleDataToV = iSampleData;
			dSampleDataToV = dSampleDataToV/( 0x7FFFFE ) * DrawGraphYAxisUpper;
			dReceiveData[j] = dSampleDataToV;
		}
		// 不显示ADC数据改为显示各站实收帧数
//		m_dADCDataShow[i] = dSampleDataToV;
		m_uiADCDataShow[uiLocation] = m_uiADCDataFrameRecCount[uiLocation];

		if (m_uiADCDataFrameCount[uiLocation] == 1)
		{
			CString str = _T("");
			m_usDataPointPrevious[uiLocation] = usDataPointNow;
			str.Format(_T("设备序号%d，接收到ADC数据帧指针偏移量起始为%d！"), uiLocation, usDataPointNow);
			m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::ReceiveSampleData"), str, SuccessStatus);
			if (m_bStartGraphShow == TRUE)
			{
				if (m_bCheckADCDataFrameCountEqule == FALSE)
				{
					int iReturn = 0;
					iReturn = OnADCRecDataCheckOpt(OptCheckEqule, m_uiADCDataFrameCount);
					if (iReturn == CheckEquleReturn)
					{
						m_bCheckADCDataFrameCountEqule = TRUE;
					}
				}
				if (m_bCheckADCDataFrameCountEqule == TRUE)
				{
					OnMakeADCGraphShowFrame(SendADCCmd);
				}
			}
		}
		else
		{
			dwADCRetransimission = OnCheckFrameIsRetransmission(uiLocation, uiIPAim, usDataPointNow);
			if (dwADCRetransimission == 0)
			{
				return;
			}
		}
		if(uiIPAim == m_uiADCGraphIP[uiLocation])
		{
			m_pOScopeCtrl[uiLocation]->AppendPoint(dReceiveData, ReceiveDataSize);
		}
		// 无数据重发帧或已经全部接收到数据重发帧
		if(m_oADCLostMap[uiLocation].size() == 0)
		{
			// 如果该帧为数据重发帧，处理该帧后再处理保存帧
			if (dwADCRetransimission == 2)
			{
				OnRecOkIsRetransimissionFrame(uiLocation, iReceiveData);
			}
			else if(dwADCRetransimission == 1)
			{
				OnRecOkIsNormalFrame(uiLocation, iReceiveData);
			}
		}
		else
		{
			// 如果该帧为数据重发帧
			if (dwADCRetransimission == 2)
			{
				OnRecNotOkIsRetransimissionFrame(uiLocation, iReceiveData);
			}
			else if(dwADCRetransimission == 1)
			{
				// 如果有重发帧，而本帧不是重发帧则先将该帧保存
				OnRecNotOkIsNormalFrame(uiLocation, iReceiveData);
			}
		}
		// 将采样数据保存成文件
		OnSaveADCToFile(false);
	}
	else
	{
		CString str = _T("");
		CString strtemp = _T("");
		str.Format(_T("ADC接收帧IP地址错误，错误的IP地址为：%d\r\n"), uiIPAim);
		for (int i=0; i<RcvFrameSize; i++)
		{
			strtemp.Format(_T("%02x "), pSampleData[i]);
			str += strtemp;
		}
		m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::ReceiveSampleData"), str, ErrorStatus);
	}
}
// 生成ADC数据重发帧
void CADCDataRecThread::MakeADCDataRetransmissionFrame(unsigned short usDataPointNb, unsigned int uiIPAim)
{
	unsigned int uiIPSource =	0;
	unsigned int usPortAim	=	0;
	unsigned int usCommand	=	0;
	CString str = _T("");
	int iPos = 0;
	memset(m_ucADCRetransmission, SndFrameBufInit, SndFrameSize);
	m_ucADCRetransmission[0] = FrameHeadCheck0;
	m_ucADCRetransmission[1] = FrameHeadCheck1;
	m_ucADCRetransmission[2] = FrameHeadCheck2;
	m_ucADCRetransmission[3] = FrameHeadCheck3;
	memset(&m_ucADCRetransmission[FrameHeadCheckSize], SndFrameBufInit, (FrameHeadSize - FrameHeadCheckSize));

	uiIPSource	=	m_uiIPSource;
	usPortAim	=	ADRecPort;
	usCommand	=	SendADCCmd;
	// 源IP地址
	iPos = 16;
	memcpy(&m_ucADCRetransmission[iPos], &uiIPSource, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标IP地址
	memcpy(&m_ucADCRetransmission[iPos], &uiIPAim, FramePacketSize4B);
	iPos += FramePacketSize4B;
	// 目标端口号
	memcpy(&m_ucADCRetransmission[iPos], &usPortAim, FramePacketSize2B);
	iPos += FramePacketSize2B;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_ucADCRetransmission[iPos], &usCommand, FramePacketSize2B);
	iPos += FramePacketSize2B;

	iPos = 32;
	memcpy(&m_ucADCRetransmission[iPos], &usDataPointNb, FramePacketSize2B);
	iPos += FramePacketSize2B;
	m_ucADCRetransmission[iPos] = SndFrameBufInit;

	unsigned short usCRC16 = 0;
	usCRC16 = get_crc_16(&m_ucADCRetransmission[FrameHeadSize], SndFrameSize - FrameHeadSize - CRCCheckSize);
	memcpy(&m_ucADCRetransmission[SndFrameSize - CRCSize], &usCRC16, CRCSize);

	sendto(m_ADCDataSocket, reinterpret_cast<const char*>(&m_ucADCRetransmission), SndFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendToAddr), sizeof(m_SendToAddr));
}
// ADC数据需要重新发送帧对应的最小个数
unsigned int CADCDataRecThread::OnADCRetransimissionMinNb(unsigned int uiInstrumentNb)
{
	unsigned int uimin = 0;								// 最小值
	unsigned int uiCount = 0;							// 计数

	hash_map<unsigned int, m_structADC>::iterator  iter;
	for(iter = m_oADCLostMap[uiInstrumentNb].begin(); iter != m_oADCLostMap[uiInstrumentNb].end(); iter++)
	{
		uiCount++;
		if (uiCount == 1)
		{
			uimin = iter->second.uiDataCount;
		}
		else
		{
			if (uimin > iter->second.uiDataCount)
			{
				uimin = iter->second.uiDataCount;
			}
		}
	}
	return uimin;
}
// 将ADC数据保存到文件中
void CADCDataRecThread::OnSaveADCToFile(bool bfinish)
{
	unsigned int uiADCDataToSaveNum = 0;
	unsigned int uiMinSize = 0;
	unsigned int uiMaxSize = 0;
	unsigned int uiLostFrameCount = 0;
	unsigned int uiLostFrameNum = 0;
	CString str = _T("");
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;

	uiMinSize = OnADCRecDataCheckOpt(OptMinValue, m_uiADCSaveNum);
	uiMaxSize = OnADCRecDataCheckOpt(OptMaxValue, m_uiADCSaveNum);
	if (bfinish == false)
	{
		uiADCDataToSaveNum = uiMinSize;
	}
	else
	{
		uiADCDataToSaveNum = uiMaxSize;
	}
	// 如果设备掉线则不影响其它设备ADC数据采集
	if ((uiMinSize == 0) && (uiMaxSize > ADCDataTempDataSize/2))
	{
		uiADCDataToSaveNum  = uiMaxSize;
	}

	if (uiADCDataToSaveNum == 0)
	{
		// 停止ADC数据采集
		if (bfinish == true)
		{
			m_pADCDataSaveToFile->OnCloseADCSaveFile();
			m_pADCDataSaveToFile->m_uiADCDataToSaveNum = 0;
			m_pADCDataSaveToFile->m_uiADCSaveFileNum = 0;

			uiLostFrameCount = 0;

			for (unsigned int i=0; i<m_pInstrumentList->m_oInstrumentIPMap.size(); i++)
			{
				uiLostFrameNum = m_uiADCDataFrameCount[i] - m_uiADCDataFrameRecCount[i];
				str.Format(_T("设备序号%d，应收帧数%d，实际接收帧数%d，丢失帧数%d"), i, 
					m_uiADCDataFrameCount[i], m_uiADCDataFrameRecCount[i], uiLostFrameNum);
				m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnSaveADCToFile"), str, SuccessStatus);
				uiLostFrameCount += uiLostFrameNum;
			}
	
			str.Format(_T("ADC数据采样丢失帧总数为%d"), uiLostFrameCount);
			m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnSaveADCToFile"), str, SuccessStatus);
		}
		return;
	}
	if (bfinish == false)
	{	
		if ((m_pADCDataSaveToFile->m_uiADCDataToSaveNum + uiADCDataToSaveNum) > m_pADCDataSaveToFile->m_uiADCFileLength)
		{
			uiADCDataToSaveNum = m_pADCDataSaveToFile->m_uiADCFileLength - m_pADCDataSaveToFile->m_uiADCDataToSaveNum;
		}
	}
 	m_pADCDataSaveToFile->OnSaveADCToFile(m_iADCSave, m_uiADCSaveNum, uiADCDataToSaveNum, bfinish);

	// 清除保存过的数据
	for(iter=m_pInstrumentList->m_oInstrumentIPMap.begin(); iter!=m_pInstrumentList->m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if (iter->second->m_uiInstrumentType != InstrumentTypeFDU)
			{
				continue;
			}
			unsigned int uiLocation = iter->second->m_uiFDUIndex;
			if (bfinish == false)
			{
				if ((iter->second->m_iSelectObject == BST_CHECKED)&&(iter->second->m_iSelectObjectNoise == BST_UNCHECKED)
				&&(iter->second->m_bIPSetOK == true)&&(iter->second->m_uiInstrumentType == InstrumentTypeFDU))
				{
					if (m_uiADCSaveNum[uiLocation] >= uiADCDataToSaveNum)
					{
						if (m_uiADCSaveNum[uiLocation] - uiADCDataToSaveNum > ADCDataTempDataSize)
						{
							str.Format(_T("设备序号%d的ADC数据缓冲区中数据个数%ud，要保存的数据个数为%ud，"), 
								uiLocation, m_uiADCSaveNum[uiLocation], uiADCDataToSaveNum);
							str += _T("ADC数据处理过程缓冲区大小不够1！");
							AfxMessageBox(str);
						}
						else
						{
							m_uiADCSaveNum[uiLocation] = m_uiADCSaveNum[uiLocation] - uiADCDataToSaveNum;
							memcpy(&m_iTemp, &m_iADCSave[uiLocation][uiADCDataToSaveNum], m_uiADCSaveNum[uiLocation]*(sizeof(int)));
							memcpy(&m_iADCSave[uiLocation], &m_iTemp, m_uiADCSaveNum[uiLocation]*(sizeof(int)));
						}
					}
				}
			} 
			else
			{
				memset(&m_iADCSave[uiLocation], SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
				m_uiADCSaveNum[uiLocation] = 0;
			}
		}
	}
	if (bfinish == true)
	{
		uiLostFrameCount = 0;
		for (unsigned int i=0; i<m_pInstrumentList->m_oInstrumentIPMap.size(); i++)
		{
			uiLostFrameNum = m_uiADCDataFrameCount[i] - m_uiADCDataFrameRecCount[i];
			str.Format(_T("设备序号%d，应收帧数%d，实际接收帧数%d，丢失帧数%d"), i, 
				m_uiADCDataFrameCount[i], m_uiADCDataFrameRecCount[i], uiLostFrameNum);
			m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnSaveADCToFile"), str, SuccessStatus);
			uiLostFrameCount += uiLostFrameNum;
		}
		str.Format(_T("ADC数据采样丢失帧总数为%d"), uiLostFrameCount);
		m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnSaveADCToFile"), str, SuccessStatus);
	}
}
// 被选择仪器的数值比较
int CADCDataRecThread::OnADCRecDataCheckOpt(unsigned short usOperation, unsigned int* pData)
{
	int iReturn = 0;
	vector<unsigned int> vSort;
	vector<unsigned int> ::size_type uiSize;
	vSort.reserve(InstrumentMaxCount);
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	for(iter=m_pInstrumentList->m_oInstrumentIPMap.begin(); iter!=m_pInstrumentList->m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if ((iter->second->m_bIPSetOK == true) && (iter->second->m_uiInstrumentType == InstrumentTypeFDU))
			{
				unsigned int uiLocation = iter->second->m_uiFDUIndex;
				if ((iter->second->m_iSelectObject == BST_CHECKED)&&(iter->second->m_iSelectObjectNoise == BST_UNCHECKED))
				{
					vSort.push_back(pData[uiLocation]);
				}
			}
		}
	}
	uiSize = vSort.size();
	if (uiSize > 0)
	{
		sort(vSort.begin(), vSort.end());
		if (usOperation == OptMinValue)
		{
			iReturn = vSort[0];
		}
		else if (usOperation == OptMaxValue)
		{
			iReturn = vSort[uiSize - 1];
		}
		else if (usOperation == OptCheckEqule)
		{
			if (vSort[0] == vSort[uiSize - 1])
			{
				iReturn = CheckEquleReturn;
			}
			else
			{
				iReturn = CheckNotEquleReturn;
			}
		}
	}
	vSort.clear();
	return iReturn;
}
// ADC数据重发
void CADCDataRecThread::OnADCDataRetransmission(void)
{
	int iReceiveData[ReceiveDataSize];
	CString str = _T("");
	unsigned int uiTemp = 0;
	memset(&iReceiveData, SndFrameBufInit, ReceiveDataSize * (sizeof(int)));
	hash_map<unsigned int, m_structADC>::iterator  iter;
	for (unsigned int i=0; i<m_pInstrumentList->m_oInstrumentIPMap.size(); i++)
	{
		//		ProcessMessages();
		if (m_oADCLostMap[i].size() == 0)
		{
			continue;
		}
		for(iter = m_oADCLostMap[i].begin(); iter != m_oADCLostMap[i].end();)
		{
			if (m_oADCLostMap[i].size() == 0)
			{
				break;
			}
			iter->second.uiRetransmissionNum++;
			// 根据采样率确定超过硬件保存ADC采样数据的时间的一半则对丢失帧补零处理
			uiTemp = (HardwareSaveADCNum * 1000) / (m_uispsSelect * 250 * ADCDataRecThreadSleepTime * 2);
			if (iter->second.uiRetransmissionNum == uiTemp)
			{
				m_uiADCRetransmissionNb = iter->second.uiDataCount;
				str.Format(_T("设备序号%d，ADC数据帧用0补齐丢帧，补齐帧的偏移量为%d！"), i, iter->first);
				m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnADCDataRetransmission"), str, WarningStatus);

				m_oADCLostMap[i].erase(iter++);
				// 已经全部接收到数据重发帧
				if(0 == m_oADCLostMap[i].size())
				{
					OnRecOkIsRetransimissionFrame(i, iReceiveData);
				}
				else
				{
					OnRecNotOkIsRetransimissionFrame(i, iReceiveData);
				}
				// 将采样数据保存成文件
				OnSaveADCToFile(false);
			}
			else
			{
				iter++;
			}
		}
	}
}
// ADC数据重发
void CADCDataRecThread::OnADCDataRetransmission(unsigned int uiInstrumentNb)
{
	int iReceiveData[ReceiveDataSize];
	CString str = _T("");
	unsigned int uiTemp = 0;
	memset(&iReceiveData, SndFrameBufInit, ReceiveDataSize * (sizeof(int)));
	hash_map<unsigned int, m_structADC>::iterator  iter;

	if (m_oADCLostMap[uiInstrumentNb].size() == 0)
	{
		return;
	}
	for(iter = m_oADCLostMap[uiInstrumentNb].begin(); iter != m_oADCLostMap[uiInstrumentNb].end();)
	{
		if (m_oADCLostMap[uiInstrumentNb].size() == 0)
		{
			break;
		}
		iter->second.uiRetransmissionNum++;
		// 根据采样率确定超过硬件保存ADC采样数据个数则对丢失帧补零处理
//		uiTemp = (HardwareSaveADCNum * 1000) / (m_uispsSelect * 250 * ADCDataRecThreadSleepTime * 2);
		uiTemp = HardwareSaveADCNum / ReceiveDataSize;
		if (iter->second.uiRetransmissionNum == uiTemp)
		{
			m_uiADCRetransmissionNb = iter->second.uiDataCount;
			str.Format(_T("设备序号%d，ADC数据帧用0补齐丢帧，补齐帧的偏移量为%d！"), uiInstrumentNb, iter->first);
			m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnADCDataRetransmission"), str, WarningStatus);

			m_oADCLostMap[uiInstrumentNb].erase(iter++);
			// 已经全部接收到数据重发帧
			if(0 == m_oADCLostMap[uiInstrumentNb].size())
			{
				OnRecOkIsRetransimissionFrame(uiInstrumentNb, iReceiveData);
			}
			else
			{
				OnRecNotOkIsRetransimissionFrame(uiInstrumentNb, iReceiveData);
			}
			// 将采样数据保存成文件
			OnSaveADCToFile(false);
		}
		else
		{
			if(iter->second.uiRetransmissionNum == 1)
			{
				MakeADCDataRetransmissionFrame(static_cast<unsigned short>(iter->first), (IPSetAddrStart + (uiInstrumentNb + 1) * IPSetAddrInterval));
				str.Format(_T("设备序号%d，发送ADC数据帧重发指令，重发帧的偏移量为%d！"), uiInstrumentNb, iter->first);
				m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnADCDataRetransmission"), str, SuccessStatus);
			}
			iter++;
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
DWORD CADCDataRecThread::OnCheckFrameIsRetransmission(unsigned int uiInstrumentNb, unsigned int uiIP, unsigned short usDataPointNow)
{
	CString str = _T("");
	unsigned short usLostDataFrameCount = 0;	// 丢失帧计数
	unsigned short usADCDataNb = ReceiveDataSize;	// ADC一帧的数据个数
	unsigned short usDataPointMove = 0;	// 记录指针偏移量
	unsigned short usDataPointNb = 0;
	double dLostRecData[ReceiveDataSize];
	memset(&dLostRecData, SndFrameBufInit, ReceiveDataSize*(sizeof(double)));
	// 接收到的ADC数据不是重发数据
	hash_map<unsigned int, m_structADC>::iterator iter;
	iter = m_oADCLostMap[uiInstrumentNb].find(usDataPointNow);
	if (iter == m_oADCLostMap[uiInstrumentNb].end())
	{
		// 索引中没找到
		if (usDataPointNow > m_usDataPointPrevious[uiInstrumentNb])
		{
			usDataPointMove  = usDataPointNow - m_usDataPointPrevious[uiInstrumentNb];
		}
		else
		{
			usDataPointMove  = ADCFramePointLength + usDataPointNow - m_usDataPointPrevious[uiInstrumentNb];
		}
		// 指针偏移量的差不是指针偏移量的整数倍则认为是过期数据
		if ((usDataPointNow % 2 ) != 0)
		{
			str.Format(_T("设备序号%d，接收到错误的指针偏移量为%d！"), uiInstrumentNb, usDataPointNow);
			m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnCheckFrameIsRetransmission"), str, ErrorStatus);
		}
		if ((usDataPointMove % usADCDataNb) != 0)
		{
			m_uiADCDataFrameCount[uiInstrumentNb]--;
			m_uiADCDataFrameRecCount[uiInstrumentNb]--;
			str.Format(_T("设备序号%d，接收到过期重发帧的指针偏移量为%d！"), uiInstrumentNb, usDataPointNow);
			m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnCheckFrameIsRetransmission"), str, WarningStatus);
			return 0;
		}
		else
		{
			usLostDataFrameCount = usDataPointMove/usADCDataNb - 1;
		}
		
		// 有ADC采集数据丢帧则加入ADC数据重发队列
		if (usLostDataFrameCount > 0)
		{
			for (unsigned short m=0; m<usLostDataFrameCount; m++)
			{
//				ProcessMessages();
				m_structADC ADCDataStruct;
				usDataPointNb = (m_usDataPointPrevious[uiInstrumentNb] + (m + 1) * usADCDataNb) % ADCFramePointLength;
				ADCDataStruct.uiDataCount = m_uiADCDataFrameCount[uiInstrumentNb];
				m_uiADCDataFrameCount[uiInstrumentNb] ++;
				ADCDataStruct.uiRetransmissionNum = 0;
				m_oADCLostMap[uiInstrumentNb].insert(hash_map<unsigned int, m_structADC>::value_type (usDataPointNb, ADCDataStruct)); 
				str.Format(_T("设备序号%d，丢失帧指针偏移量为%d！"), uiInstrumentNb, usDataPointNb);
				m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnCheckFrameIsRetransmission"), str, SuccessStatus);

// 				MakeADCDataRetransmissionFrame(usDataPointNb, (IPSetAddrStart + (uiInstrumentNb + 1) * IPSetAddrInterval));
// 				str.Format(_T("设备序号%d，发送ADC数据帧重发指令，重发帧的偏移量为%d！"), uiInstrumentNb, usDataPointNb);	
// 				m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnCheckFrameIsRetransmission"), str, WarningStatus);

				if ((m_uiSampleTempNum[uiInstrumentNb] + ReceiveDataSize) > ADCDataTempDataSize)
				{
					str.Format(_T("设备序号%d，丢失帧数%d，保存不连续的数据个数为%d，"),
						uiInstrumentNb, usLostDataFrameCount, m_uiSampleTempNum[uiInstrumentNb]);
					str += _T("ADC数据处理过程缓冲区大小不够2！");
					AfxMessageBox(str);
				}
				else
				{
					memset(&m_iSampleTemp[uiInstrumentNb][m_uiSampleTempNum[uiInstrumentNb]], SndFrameBufInit, ReceiveDataSize*(sizeof(int)));
					m_uiSampleTempNum[uiInstrumentNb] += ReceiveDataSize;
				}
				if(uiIP == m_uiADCGraphIP[uiInstrumentNb])
				{
					m_pOScopeCtrl[uiInstrumentNb]->AppendPoint(dLostRecData, ReceiveDataSize);
				}
				OnADCDataRetransmission(uiInstrumentNb);
				if (m_bStartGraphShow == TRUE)
				{
					if (m_bCheckADCDataFrameCountEqule == FALSE)
					{
						int iReturn = 0;
						iReturn = OnADCRecDataCheckOpt(OptCheckEqule, m_uiADCDataFrameCount);
						if (iReturn == CheckEquleReturn)
						{
							m_bCheckADCDataFrameCountEqule = TRUE;
						}
					}
					if (m_bCheckADCDataFrameCountEqule == TRUE)
					{
						OnMakeADCGraphShowFrame(SendADCCmd);
					}
				}
			}
		}
		OnADCDataRetransmission(uiInstrumentNb);
		m_usDataPointPrevious[uiInstrumentNb] = usDataPointNow;
		if (m_bStartGraphShow == TRUE)
		{
			if (m_bCheckADCDataFrameCountEqule == FALSE)
			{
				int iReturn = 0;
				iReturn = OnADCRecDataCheckOpt(OptCheckEqule, m_uiADCDataFrameCount);
				if (iReturn == CheckEquleReturn)
				{
					m_bCheckADCDataFrameCountEqule = TRUE;
				}
			}
			if (m_bCheckADCDataFrameCountEqule == TRUE)
			{
				OnMakeADCGraphShowFrame(SendADCCmd);
			}
		}
		return 1;
	}
	else
	{
		// 如果为重发数据则ADC应接收数据个数不变 
		m_uiADCDataFrameCount[uiInstrumentNb]--;
		m_uiADCRetransmissionNb = iter->second.uiDataCount;
		str.Format(_T("设备序号%d，接收到ADC的数据重发帧，重发帧的偏移量为%d！"), uiInstrumentNb, iter->first);
		m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnCheckFrameIsRetransmission"), str, SuccessStatus);
		m_oADCLostMap[uiInstrumentNb].erase(iter);
		return 2;
	}
}
// 该帧为最后一个数据重发帧
void CADCDataRecThread::OnRecOkIsRetransimissionFrame(unsigned int uiInstrumentNb, int* piReceiveData)
{
	if ((m_uiADCSaveNum[uiInstrumentNb] + ReceiveDataSize) > ADCDataTempDataSize)
	{
		CString str = _T("");
		str.Format(_T("设备序号%d，保存成文件的数据缓冲区中数据个数%d，"), uiInstrumentNb, m_uiADCSaveNum[uiInstrumentNb]);
		str += _T("ADC保存成文件的缓冲区大小不够3！");
		AfxMessageBox(str);
	}
	else
	{
		memcpy(&m_iADCSave[uiInstrumentNb][m_uiADCSaveNum[uiInstrumentNb]], &piReceiveData[0], ReceiveDataSize*(sizeof(int)));
		m_uiADCSaveNum[uiInstrumentNb] += ReceiveDataSize;
	}
	// 接收到重发帧之前有保存帧
	if (m_uiSampleTempNum[uiInstrumentNb] > ReceiveDataSize)
	{
		// 将除重发帧之外的保存帧复制到文件存储缓冲区
		m_uiSampleTempNum[uiInstrumentNb] = m_uiSampleTempNum[uiInstrumentNb] - ReceiveDataSize;
		if (m_uiADCSaveNum[uiInstrumentNb] + m_uiSampleTempNum[uiInstrumentNb] > ADCDataTempDataSize)
		{
			CString str = _T("");
			str.Format(_T("设备序号%d，保存成文件的数据缓冲区中数据个数%d，预先保留的不连续的数据个数%d，"),
				uiInstrumentNb, m_uiADCSaveNum[uiInstrumentNb], m_uiSampleTempNum[uiInstrumentNb]);
			str += _T("ADC保存成文件的缓冲区大小不够5！");
			AfxMessageBox(str);
		}
		else
		{
			memcpy(&m_iADCSave[uiInstrumentNb][m_uiADCSaveNum[uiInstrumentNb]], &m_iSampleTemp[uiInstrumentNb][ReceiveDataSize], m_uiSampleTempNum[uiInstrumentNb]*(sizeof(int)));
			m_uiADCSaveNum[uiInstrumentNb] += m_uiSampleTempNum[uiInstrumentNb];
		}
	}
	memset(&m_iSampleTemp[uiInstrumentNb], SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
	m_uiSampleTempNum[uiInstrumentNb] = 0;
}

// 该帧为普通帧且之前没有重发帧
void CADCDataRecThread::OnRecOkIsNormalFrame(unsigned int uiInstrumentNb, int* piReceiveData)
{
	if ((m_uiADCSaveNum[uiInstrumentNb] + ReceiveDataSize) > ADCDataTempDataSize)
	{
		CString str = _T("");
		str.Format(_T("设备序号%d，保存成文件的数据缓冲区中数据个数%d，"), 
			uiInstrumentNb, m_uiADCSaveNum[uiInstrumentNb]);
		str += _T("ADC保存成文件的缓冲区大小不够6！");
		AfxMessageBox(str);
	}
	else
	{
		memcpy(&m_iADCSave[uiInstrumentNb][m_uiADCSaveNum[uiInstrumentNb]], &piReceiveData[0], ReceiveDataSize * (sizeof(int)));
		m_uiADCSaveNum[uiInstrumentNb] += ReceiveDataSize;
	}
}

// 该帧为重发帧但不是最后一个重发帧
void CADCDataRecThread::OnRecNotOkIsRetransimissionFrame(unsigned int uiInstrumentNb, int* piReceiveData)
{
	unsigned int	uiMinNb = 0;		// 重发帧中的最小位置（个数）
	uiMinNb = OnADCRetransimissionMinNb(uiInstrumentNb);
	// 该重发帧之前各帧均已经收到
	if (uiMinNb > m_uiADCRetransmissionNb)
	{
		if ((m_uiADCSaveNum[uiInstrumentNb] + ReceiveDataSize) > ADCDataTempDataSize)
		{
			CString str = _T("");
			str.Format(_T("设备序号%d，保存成文件的数据缓冲区中数据个数%d，"), 
				uiInstrumentNb, m_uiADCSaveNum[uiInstrumentNb]);
			str += _T("ADC保存成文件的缓冲区大小不够7！");
			AfxMessageBox(str);
		}
		else
		{
			memcpy(&m_iADCSave[uiInstrumentNb][m_uiADCSaveNum[uiInstrumentNb]], &piReceiveData[0], ReceiveDataSize*(sizeof(int)));
			m_uiADCSaveNum[uiInstrumentNb] += ReceiveDataSize;
		}

		// 将重发帧从数据缓冲区中移除
		m_uiSampleTempNum[uiInstrumentNb] = m_uiSampleTempNum[uiInstrumentNb] - ReceiveDataSize;
		memcpy(&m_iTemp, &m_iSampleTemp[uiInstrumentNb][ReceiveDataSize], m_uiSampleTempNum[uiInstrumentNb]*(sizeof(int)));
		memcpy(&m_iSampleTemp[uiInstrumentNb], &m_iTemp, m_uiSampleTempNum[uiInstrumentNb]*(sizeof(int)));

		// 如果等待重发帧的索引中最小的采样帧位置和接收到的重发帧之间有保存帧
		if (uiMinNb > (m_uiADCRetransmissionNb + 1))
		{
			if ((m_uiADCSaveNum[uiInstrumentNb] + (uiMinNb - m_uiADCRetransmissionNb - 1) * ReceiveDataSize) > ADCDataTempDataSize)
			{
				CString str = _T("");
				str.Format(_T("设备序号%d，保存成文件的数据缓冲区中数据个数%d，"), 
					uiInstrumentNb, m_uiADCSaveNum[uiInstrumentNb]);
				str += _T("ADC保存成文件的缓冲区大小不够9！");
				AfxMessageBox(str);
			} 
			else
			{
				memcpy(&m_iADCSave[uiInstrumentNb][m_uiADCSaveNum[uiInstrumentNb]], 
					&m_iSampleTemp[uiInstrumentNb], 
					(uiMinNb - m_uiADCRetransmissionNb - 1) * ReceiveDataSize*(sizeof(int)));
				m_uiADCSaveNum[uiInstrumentNb] += (uiMinNb - m_uiADCRetransmissionNb - 1) * ReceiveDataSize;
			}
			// 将绘制完成的保存帧移除
			m_uiSampleTempNum[uiInstrumentNb] = m_uiSampleTempNum[uiInstrumentNb] - (uiMinNb - m_uiADCRetransmissionNb - 1) * ReceiveDataSize;
			memcpy(&m_iTemp, &m_iSampleTemp[uiInstrumentNb][(uiMinNb - m_uiADCRetransmissionNb - 1) * ReceiveDataSize], m_uiSampleTempNum[uiInstrumentNb]*(sizeof(int)));
			memcpy(&m_iSampleTemp[uiInstrumentNb], &m_iTemp, m_uiSampleTempNum[uiInstrumentNb]*(sizeof(int)));
		}
	}
	// 该帧之前还有重发帧未收到
	else
	{
		memcpy(&m_iSampleTemp[uiInstrumentNb][(m_uiADCRetransmissionNb - uiMinNb) * ReceiveDataSize], &piReceiveData[0], ReceiveDataSize*(sizeof(int)));
	}
}
// 该帧为普通帧且之前有重发帧
void CADCDataRecThread::OnRecNotOkIsNormalFrame(unsigned int uiInstrumentNb, int* piReceiveData)
{
	if ((m_uiSampleTempNum[uiInstrumentNb] + ReceiveDataSize) > ADCDataTempDataSize)
	{
		CString str = _T("");
		str.Format(_T("设备序号%d应收帧数%d，实收帧数%d，预先保留不连续的数据个数%d，"), 
			uiInstrumentNb, m_uiADCDataFrameCount[uiInstrumentNb], m_uiADCDataFrameRecCount[uiInstrumentNb], 
			m_uiSampleTempNum[uiInstrumentNb]);
		str += _T("ADC数据处理过程缓冲区大小不够11！");
		AfxMessageBox(str);
	} 
	else
	{
		memcpy(&m_iSampleTemp[uiInstrumentNb][m_uiSampleTempNum[uiInstrumentNb]], &piReceiveData[0], 
			ReceiveDataSize*(sizeof(int)));
		m_uiSampleTempNum[uiInstrumentNb] += ReceiveDataSize;
	}
}
// 重置变量的值
void CADCDataRecThread::OnReset(void)
{
	for (int i=0; i<InstrumentMaxCount; i++)
	{
//		ProcessMessages();
		memset(&m_iSampleTemp[i], SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
		m_uiSampleTempNum[i] = 0;
		memset(&m_iADCSave[i], SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
		m_uiADCSaveNum[i] = 0;
		m_uiADCDataFrameCount[i] = 0;
		m_uiADCDataFrameRecCount[i] = 0;
		m_uiADCDataShow[i] = 0;
		m_oADCLostMap[i].clear();
		m_uiADCGraphIP[i] = 0;
		m_usDataPointPrevious[i] = 0;
		m_pOScopeCtrl[i] = NULL;
	}
	memset(&m_iTemp, SndFrameBufInit, ADCDataTempDataSize*(sizeof(int)));
	m_bStartSample = FALSE;
}
// 关闭UDP套接字
void CADCDataRecThread::OnCloseUDP(void)
{
	shutdown(m_ADCDataSocket, SD_BOTH);
	shutdown(m_ADCGraphShowSocket, SD_BOTH);
	closesocket(m_ADCDataSocket);
	closesocket(m_ADCGraphShowSocket);
	m_ADCDataSocket = INVALID_SOCKET;
	m_ADCGraphShowSocket = INVALID_SOCKET;
}

// 生成ADC数据图形化显示帧
void CADCDataRecThread::OnMakeADCGraphShowFrame(unsigned short usCommand)
{
	int iPos = 0;
	int iPosHead = 0;
	int iPosSend = 0;
	if (usCommand == SendSetCmd)
	{
		m_ucADCGraphShowSetFrame[0] = FrameHeadCheck0;
		m_ucADCGraphShowSetFrame[1] = FrameHeadCheck1;
		m_ucADCGraphShowSetFrame[2] = FrameHeadCheck2;
		m_ucADCGraphShowSetFrame[3] = FrameHeadCheck3;
		// 源IP地址
		iPos = FrameHeadSize;
		iPos += FramePacketSize4B;
		// 目标IP地址
		iPos += FramePacketSize4B;
		// 目标端口号
		iPos += FramePacketSize2B;
		// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
		memcpy(&m_ucADCGraphShowSetFrame[iPos], &usCommand, FramePacketSize2B);
		iPos += FramePacketSize2B;

		// 统计参与采集的设备个数
		memcpy(&m_ucADCGraphShowSetFrame[iPos], &m_pADCDataSaveToFile->m_uiSampleInstrumentNum, FramePacketSize2B);
		iPos += FramePacketSize2B;
		m_ucADCGraphShowSetFrame[iPos] = SndFrameBufInit;

		unsigned short usCRC16 = 0;
		usCRC16 = get_crc_16(&m_ucADCGraphShowSetFrame[FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize);
		memcpy(&m_ucADCGraphShowSetFrame[RcvFrameSize - CRCSize], &usCRC16, CRCSize);
		sendto(m_ADCGraphShowSocket, reinterpret_cast<const char*>(&m_ucADCGraphShowSetFrame), RcvFrameSize, 0, reinterpret_cast<sockaddr*>(&m_SendADCGraphToAddr), sizeof(m_SendADCGraphToAddr));
	} 
	else if (usCommand == SendADCCmd)
	{
		CInstrument* pInstrument = NULL;
		unsigned int uiIPAim = 0;
		iPos = FrameHeadSize;
		// 设m_pFrameData[16]到m_pFrameData[19]为源地址做为通道号
		memcpy(&uiIPAim, &m_ucudp_buf[iPos], FramePacketSize4B);
		if (m_pInstrumentList->GetInstrumentFromIPMap(uiIPAim, pInstrument))
		{
			unsigned int uiLocation = pInstrument->m_uiFDUIndex;
			unsigned int uiLocationGraph = uiLocation - pInstrument->m_uiUnCheckedNum;
			unsigned int uiADCDataFrameCount = m_uiADCDataFrameCount[uiLocation] - 1;
			unsigned int uiLabelIndex = pInstrument->m_uiFDUIndex + 1;
			memcpy(&m_ucADCGraphShowDataFrame[uiLocation][iPos], &uiLocationGraph, FramePacketSize4B);
			iPos += FramePacketSize4B;
			memcpy(&m_ucADCGraphShowDataFrame[uiLocation][iPos], &uiLabelIndex, FramePacketSize4B);
			iPos += FramePacketSize4B;
			iPos += FramePacketSize2B;
			// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
			memcpy(&m_ucADCGraphShowDataFrame[uiLocation][iPos], &usCommand, FramePacketSize2B);
			iPos += FramePacketSize2B;
			iPos += FramePacketSize2B;
			memcpy(&m_ucADCGraphShowDataFrame[uiLocation][iPos], &uiADCDataFrameCount, FramePacketSize4B);
			iPos += FramePacketSize4B;
			iPos += FramePacketSize2B;
			iPosHead = iPos;
			iPos += m_uiADCGraphShowSourcePoint[uiLocation];
			while(1)
			{
				if (iPos < (RcvFrameSize - CRCCheckSize))
				{
					iPosSend = iPosHead + m_uiADCGraphShowSamplingNum[uiLocation] * ADCDataSize3B;
					memcpy(&m_ucADCGraphShowDataFrame[uiLocation][iPosSend], &m_ucudp_buf[iPos], ADCDataSize3B);
					iPos += ADCDataSize3B * m_uiSamplingRate;
					m_uiADCGraphShowSamplingNum[uiLocation]++;
					if (m_uiADCGraphShowSamplingNum[uiLocation] == ReceiveDataSize)
					{
						m_uiADCGraphShowSamplingNum[uiLocation] = 0;
						sendto(m_ADCGraphShowSocket, reinterpret_cast<const char*>(&m_ucADCGraphShowDataFrame[uiLocation]), RcvFrameSize, 0, 
							reinterpret_cast<sockaddr*>(&m_SendADCGraphToAddr), sizeof(m_SendADCGraphToAddr));
					}
				}
				else
				{
					m_uiADCGraphShowSourcePoint[uiLocation] = iPos - (RcvFrameSize - CRCCheckSize);
					break;
				}
			}
		}
	}
}
// 接收绘图程序UDP数据包
void CADCDataRecThread::OnReceive(void)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	sockaddr_in SenderAddr;
	int n = sizeof(SenderAddr);
	ret = recvfrom(m_ADCGraphShowSocket, reinterpret_cast<char *>(&m_pADCGraphSetFrameData), sizeof(m_pADCGraphSetFrameData), 0, reinterpret_cast<sockaddr*>(&SenderAddr), &n);
	if(ret == RcvFrameSize) 
	{
		// ADC图形化显示设置帧处理
		ProcADCGraphSetFrame();
	}
	else if (ret == SOCKET_ERROR)
	{
		int iError = 0;
		CString str = _T("");
		iError = WSAGetLastError();
		if (iError != WSAEWOULDBLOCK)
		{
			str.Format(_T("ADC数据图形化显示接收设置帧错误，错误号为%d！"), iError);
			m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnReceive"), str, ErrorStatus);			
		}
	}
	else
	{
		CString str = _T("");
		str.Format(_T("ADC数据图形化显示接收设置帧帧长错误，帧长为%d！"), ret);
		m_pLogFile->OnWriteLogFile(_T("CADCDataRecThread::OnReceive"), str, ErrorStatus);
	}
}
// ADC图形化显示设置帧处理函数
void CADCDataRecThread::ProcADCGraphSetFrame(void)
{
	int iPos = 0;
	byte ucCommand = 0;
	unsigned short usCommand = 0;
	iPos = 26;
	memcpy(&usCommand, &m_pADCGraphSetFrameData[iPos], FramePacketSize2B);
	iPos += FramePacketSize2B;
	if (usCommand == StartGraphShow)
	{
		while(1)
		{
			memcpy(&ucCommand, &m_pADCGraphSetFrameData[iPos], FrameCmdSize1B);
			iPos += FrameCmdSize1B;
			if (ucCommand == CmdADCDataSamplingRate)
			{
				memcpy(&m_uiSamplingRate, &m_pADCGraphSetFrameData[iPos], FramePacketSize4B);
				iPos += FramePacketSize4B;
			}
			else if (ucCommand == SndFrameBufInit)
			{
				break;
			}
		}
		if (m_bStartSample == TRUE)
		{
			memset(m_ucADCGraphShowSetFrame, SndFrameBufInit, RcvFrameSize);
			for (unsigned int i=0; i<m_pInstrumentList->m_oInstrumentIPMap.size(); i++)
			{
				memset(m_ucADCGraphShowDataFrame[i], SndFrameBufInit, RcvFrameSize);
				m_uiADCGraphShowSamplingNum[i] = 0;
				m_uiADCGraphShowSourcePoint[i] = 0;
			}
			OnMakeADCGraphShowFrame(SendSetCmd);
			m_bStartGraphShow = TRUE;
		}
	}
	else if (usCommand == StopGraphShow)
	{
		m_bStartGraphShow = FALSE;
		m_bCheckADCDataFrameCountEqule = FALSE;
	}
}
