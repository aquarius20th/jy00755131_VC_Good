// ADCRecSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "ADCRecSocket.h"


// CADCRecSocket

CADCRecSocket::CADCRecSocket()
: udp_count(0)
, m_uiUdpCount(0)
, m_bStartSample(FALSE)
, m_pSelectObject(NULL)
, m_pADCDataProcessThread(NULL)
, m_uiADCProcCountForTest(0)
{
}

CADCRecSocket::~CADCRecSocket()
{
}


// CADCRecSocket 成员函数

void CADCRecSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int icount = 0;
	unsigned short usCRC16 = 0;
	icount = Receive(m_ucUdpBuf, ADCDataBufSize);
	if(icount == ERROR)
	{
		TRACE("ERROR!");
	}
	else if(icount != 0xffffffff) 
	{
		for (int i=0; i<icount; i++)
		{
			ProcessMessages();
			switch(m_uiUdpCount)
			{
			case 0:
				if (m_ucUdpBuf[i] == 0x11)
				{
					udp_buf[udp_count][0] = m_ucUdpBuf[i];
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
				if (m_ucUdpBuf[i] == 0x22)
				{
					udp_buf[udp_count][1] = m_ucUdpBuf[i];
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
				if (m_ucUdpBuf[i] == 0x33)
				{
					udp_buf[udp_count][2] = m_ucUdpBuf[i];
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
				if (m_ucUdpBuf[i] == 0x44)
				{
					udp_buf[udp_count][3] = m_ucUdpBuf[i];
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
				udp_buf[udp_count][m_uiUdpCount] = m_ucUdpBuf[i];
				m_uiUdpCount++;
				if (m_uiUdpCount == RcvFrameSize)
				{
					m_uiUdpCount = 0;
					if (m_bStartSample == TRUE)
					{
						memcpy(&usCRC16, &udp_buf[udp_count][RcvFrameSize - CRCSize], 2);
						if (usCRC16 != get_crc_16(&udp_buf[udp_count][FrameHeadSize], RcvFrameSize - FrameHeadSize - CRCCheckSize))
						{
							//	continue;
						}
						// 将数据复制到ADC数据处理线程缓冲区
						OnProcess(udp_buf[udp_count]);
					}
					udp_count +=1;
					udp_count = udp_count % ADCRecBufNum;
				}
				break;
			}
		}
	}
	CSocket::OnReceive(nErrorCode);
}
// 防止程序在循环中运行无法响应消息
void CADCRecSocket::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
// 对ADC数据进行数据处理
void CADCRecSocket::OnProcess(unsigned char* ucUdpBuf)
{
	int pos = 0;
	unsigned int	uiIPAim = 0;
	unsigned short	usDataPointNow = 0; // 接收到帧的数据的序号
	int	iSampleData = 0;
	double dSampleDataToV = 0.0;
	pos = 16;
	// [16]到[19]为源IP地址
	memcpy(&uiIPAim, &ucUdpBuf[pos], 4);
	pos += 4;

	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		if (m_pSelectObject[i] == 1)
		{
			if (uiIPAim	== (81 + 10*i))
			{
				// 接收数据缓存
				double dReceiveData [ReceiveDataSize]; 
				usDataPointNow = 0;
				pos += 8;
				// m_pFrameData[28]到m_pFrameData[29]为指针偏移量
				memcpy(&usDataPointNow, &ucUdpBuf[pos], 2);
				pos += 2;

				// 之后为数据区
				for (int j=0; j<ReceiveDataSize; j++)
				{
					ProcessMessages();
					memcpy(&iSampleData, &ucUdpBuf[pos], 3);
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
						// CString str;
						// str.Format("ADC采样数值溢出，溢出值为%d", dSampleDataToV);
						// AfxMessageBox(str);
					}
					dReceiveData[j] = dSampleDataToV;
				}
				m_Sec.Lock();
				m_dADCDataShow[i] = dSampleDataToV;
				m_Sec.Unlock();
				OnProcessADCData(i, usDataPointNow, dReceiveData);
				break;
			}
		}
	}
}
// 将数据交给ADC数据处理线程
void CADCRecSocket::OnProcessADCData(unsigned int uiInstrumentNb, unsigned short usDataPoint, double* pReceiveData)
{
	double dProcData[ReceiveDataSize + ADCFrameInfoSize];
	dProcData[0] = usDataPoint;
	for (int i=0; i<ReceiveDataSize; i++)
	{
		dProcData[ADCFrameInfoSize + i] = pReceiveData[i];
	}
//	memcpy(&dProcData[ADCFrameInfoSize], pReceiveData, ReceiveDataSize);
	m_Sec.Lock();
	unsigned int uiADCDataFrameRecCount = 0;
	uiADCDataFrameRecCount = m_pADCDataProcessThread->m_uiADCDataFrameRecCount[uiInstrumentNb];
	m_Sec.Unlock();
	uiADCDataFrameRecCount = uiADCDataFrameRecCount * (ReceiveDataSize + ADCFrameInfoSize);
	if (uiADCDataFrameRecCount > (ADCDataProcBufSize - ReceiveDataSize - ADCFrameInfoSize ))
	{
		//????用来避免缓冲区溢出，临时方案
//		uiADCDataFrameRecCount = ADCDataProcBufSize - ReceiveDataSize - ADCFrameInfoSize;
		AfxMessageBox(_T("ADC接收数据超过缓冲区大小！"));
	}
 	else
 	{
		m_uiADCProcCountForTest ++;
		TRACE2("设备序号为%d, 成功处理次数%d\r\n", uiInstrumentNb, m_uiADCProcCountForTest);
		m_Sec.Lock();
//		memcpy(&(m_pADCDataProcessThread->m_dADCDataRecBuf[uiInstrumentNb][uiADCDataFrameRecCount]), dProcData, ReceiveDataSize + ADCFrameInfoSize);
		for (int i=0; i<(ReceiveDataSize + ADCFrameInfoSize); i++)
		{
			m_pADCDataProcessThread->m_dADCDataRecBuf[uiInstrumentNb][uiADCDataFrameRecCount + i] = dProcData[i];
		}
		m_pADCDataProcessThread->m_uiADCDataFrameRecCount[uiInstrumentNb]++;
		::SetEvent(m_pADCDataProcessThread->m_hADCDataProcessThreadWork);
		m_Sec.Unlock();
	}
}
