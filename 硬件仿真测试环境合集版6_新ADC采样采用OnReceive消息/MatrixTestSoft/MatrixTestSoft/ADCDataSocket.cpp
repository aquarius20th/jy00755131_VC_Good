// ADCDataSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "ADCDataSocket.h"

// CADCDataSocket

CADCDataSocket::CADCDataSocket()
: m_pSelectObject(NULL)
, m_pSelectObjectNoise(NULL)
//, m_pwnd(NULL)
, m_bStartSample(FALSE)
, m_csIPSource(_T(""))
, m_uiSendPort(0)
, m_uiTestADCLost(0)
{
}

CADCDataSocket::~CADCDataSocket()
{
}

// CADCDataSocket 成员函数

void CADCDataSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret = 0;
	int icount = 0;
	unsigned short uiPort = 0;
	ret = Receive(ADCData,256);

	if(ret == 256) 
	{
		if (m_bStartSample == TRUE)
		{
			byte* pSampleData = &(ADCData[16]);
			ReceiveSampleData(pSampleData);
		}
	}
	icount = m_listADC.GetCount();
	if (icount>0)
	{
		m_structADC ADCStructTemp;
		POSITION pos = m_listADC.GetHeadPosition();
		for (int i = 0; i < icount; i++)
		{
			ADCStructTemp = m_listADC.GetNext(pos);
			MakeADCDataRetransmissionFrame(ADCStructTemp.usDataPointNb, ADCStructTemp.uiIPAim);
		}    
	}

	CSocket::OnReceive(nErrorCode);
}
// 接收采样数据
void CADCDataSocket::ReceiveSampleData(byte* pSampleData)
{
	// uiTestNb表明接收到帧所对应的仪器号
	unsigned int	uiTestNb = 0;
	unsigned int	uiCannelNb = 0;
	unsigned short	usDataPointNow = 0; // 接收到帧的数据的序号
	unsigned int	uiMinNb = 0;		// 重发帧中的最小位置（个数）
	int	iSampleData = 0;
	int pos = 0;
	double dSampleDataToV = 0.0;
	unsigned int uitmp = 0;
	CString str = "";
	unsigned short usLostDataFrameCount = 0;	// 丢失帧计数
	unsigned short usADCDataNb = ReceiveDataSize;	// ADC一帧的数据个数
	BOOL bADCRetransimission = FALSE;			// 判断是否为重发帧的标志位
	unsigned int uiADCRetransimissionNb = 0;	// 接收到的重发帧所处位置

	// 设m_pFrameData[16]到m_pFrameData[19]为源地址做为通道号
	memcpy(&uiTestNb, &pSampleData[pos], 4);
	pos += 4;
	// 设通道1,2,3,4分别对应四个仪器IP地址
	if (uiTestNb == 81)
	{
		uiCannelNb = 1;
	}
	else if (uiTestNb == 91)
	{
		uiCannelNb = 2;
	}
	else if (uiTestNb == 101)
	{
		uiCannelNb = 4;
	}
	else if (uiTestNb == 111)
	{
		uiCannelNb = 8;
	}

	for (UINT32 i=0; i<GraphViewNum; i++)
	{
		if (uiCannelNb == (UINT32)(1 << i))
		{
			
			// 接收数据缓存
			double m_dpReceiveData [ReceiveDataSize]; 
			pos += 8;
			// m_pFrameData[28]到m_pFrameData[29]做为采样个数的序号，暂时不用
			memcpy(&usDataPointNow, &pSampleData[pos], 2);
			pos += 2;
			// 用于测试ADC丢帧后补帧
// 			if (usDataPointNow == 24)
// 			{
// 				if (uiCannelNb == 4)
// 				{	
// 					m_uiTestADCLost++;
// 					if (m_uiTestADCLost == 1)
// 					{
// 						break;
// 					}
// 				}
// 			
// 			}

			// ADC应接收数据个数加一
			m_uiADCDataFrameCount[i]++;

			if (m_uiADCDataFrameCount[i] == 1)
			{
				m_usDataPointPrevious[i] = usDataPointNow;
			}
			else
			{
				m_structADC ADCFindFromStruct;
				POSITION pos;
				ADCFindFromStruct.uiIPAim = uiTestNb;
				ADCFindFromStruct.usDataPointNb = usDataPointNow;
				pos = m_listADC.Find(ADCFindFromStruct, NULL);
				// 接收到的ADC数据不是重发数据
				if (pos == NULL)
				{
					uitmp = m_uiADCDataFrameCount[i] % 10000;
					if (usDataPointNow > m_usDataPointPrevious[i])
					{
						m_usDataPointMove[i][uitmp]  = usDataPointNow - m_usDataPointPrevious[i];
					}
					else
					{
						m_usDataPointMove[i][uitmp]  = 2048 + usDataPointNow - m_usDataPointPrevious[i];
					}
					usLostDataFrameCount = m_usDataPointMove[i][uitmp] /usADCDataNb - 1;
					// 有ADC采集数据丢帧则加入ADC数据重发队列
					if (usLostDataFrameCount > 0)
					{
						for (unsigned short m=0; m<usLostDataFrameCount; m++)
						{
							m_structADC ADCDataStruct;
							ADCDataStruct.uiIPAim = uiTestNb;
							ADCDataStruct.usDataPointNb = (m_usDataPointPrevious[i] + (m+1)*usADCDataNb)%2048;
							ADCDataStruct.uiDataCount = m_uiADCDataFrameCount[i] + m;
							m_listADC.AddTail(ADCDataStruct);
							// 将丢失帧的ADC数据赋值为0
							for (int k=0; k<ReceiveDataSize; k++)
							{
								m_Sec.Lock();
								m_pSampleData[i]->push_back(0.0);
								m_Sec.Unlock();
							}
						}
						m_uiADCDataFrameCount[i] += usLostDataFrameCount;
					}

					m_usDataPointPrevious[i] = usDataPointNow;

				}
				else
				{
					// 如果为重发数据则ADC应接收数据个数不变 
					m_uiADCDataFrameCount[i]--;
					bADCRetransimission = TRUE;
					ADCFindFromStruct = m_listADC.GetAt(pos);
					uiADCRetransimissionNb = ADCFindFromStruct.uiDataCount;
					m_listADC.RemoveAt(pos);
				}
			}

			// 之后为数据区
			for (int j=0; j<ReceiveDataSize; j++)
			{
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
					CString str;
					str.Format("ADC采样数值溢出，溢出值为%d", dSampleDataToV);
					AfxMessageBox(str);
				}
				m_dpReceiveData[j] = dSampleDataToV;

				if (m_pSelectObject[i] == 1)
				{
					if (m_pSelectObjectNoise[i] == 0)
					{
						// 如果为重发数据则将数据赋值到相应位置
						if (bADCRetransimission == TRUE)
						{
							m_Sec.Lock();
						//	m_pSampleData[i]->insert(m_pSampleData[i]->begin()+(uiADCRetransimissionNb-1)*usADCDataNb+j, dSampleDataToV);
							m_pSampleData[i]->at((uiADCRetransimissionNb-1)*usADCDataNb+j) = dSampleDataToV;
							m_Sec.Unlock();
						}
						else
						{
							m_Sec.Lock();
							m_pSampleData[i]->push_back(dSampleDataToV);
							m_Sec.Unlock();
						}
					}
				}
			}
			// 无数据重发帧或已经全部接收到数据重发帧
			if(m_listADC.GetCount() == 0)
			{
				// 如果该帧为数据重发帧，处理该帧后再处理保存帧
				if (bADCRetransimission == TRUE)
				{
					m_Sec.Lock();
					m_pOScopeCtrl[i]->AppendPoint(m_dpReceiveData, ReceiveDataSize);
					m_Sec.Unlock();
					m_uiADCRecCompleteCount[i]++;
					// 接收到重发帧之前有保存帧
					if (m_dSampleTemp[i].size() > 0)
					{
						for (unsigned int m=0; m<(m_dSampleTemp[i].size()/usADCDataNb);m++)
						{
							for (int k=0; k<ReceiveDataSize; k++)
							{
								m_dpReceiveData[k] = m_dSampleTemp[i][m*usADCDataNb+k];
							}
							m_Sec.Lock();
							m_pOScopeCtrl[i]->AppendPoint(m_dpReceiveData, ReceiveDataSize);
							m_Sec.Unlock();
							m_uiADCRecCompleteCount[i]++;
						}
						m_dSampleTemp[i].clear();
					}
				}
				else
				{
					m_Sec.Lock();
					m_pOScopeCtrl[i]->AppendPoint(m_dpReceiveData, ReceiveDataSize);
					m_Sec.Unlock();
					m_uiADCRecCompleteCount[i]++;
				}
			}
			else
			{
				// 如果该帧为数据重发帧
				if (bADCRetransimission == TRUE)
				{
					m_Sec.Lock();
					m_pOScopeCtrl[i]->AppendPoint(m_dpReceiveData, ReceiveDataSize);
					m_Sec.Unlock();
					m_uiADCRecCompleteCount[i]++;
					uiMinNb = OnADCRetransimissionMinNb();
					// 如果等待重发帧的队列中最小的采样帧位置和接收到的重发帧之间有保存帧
					if (uiMinNb > (m_uiADCRecCompleteCount[i]+1))
					{
						for (unsigned int m=0; m<(uiMinNb - m_uiADCRecCompleteCount[i] - 1); m++)
						{
							for (int k=0; k<ReceiveDataSize; k++)
							{
								m_dpReceiveData[k] = m_dSampleTemp[i][m*usADCDataNb+k];
							}
							m_Sec.Lock();
							m_pOScopeCtrl[i]->AppendPoint(m_dpReceiveData, ReceiveDataSize);
							m_Sec.Unlock();
							m_uiADCRecCompleteCount[i]++;
						}
						// 将绘制完成的保存帧移除
						vector<double> dTemp;
						for (unsigned int n=(uiMinNb - m_uiADCRecCompleteCount[i] - 1)*usADCDataNb; n<m_dSampleTemp[i].size(); n++)
						{
							dTemp.push_back(m_dSampleTemp[i][n]);
						}
						m_dSampleTemp[i].clear();
						for (unsigned int p=0; p<dTemp.size();p++)
						{
							m_dSampleTemp[i].push_back(dTemp[p]);
						}
						dTemp.clear();
					}
				}
				else
				{
					// 如果有重发帧，而本帧不是重发帧则先将该帧保存
					for (int k=0; k<ReceiveDataSize; k++)
					{
						m_dSampleTemp[i].push_back(m_dpReceiveData[k]);
					}
				}
			}
			if (m_uiADCDataFrameCount[i] == 7255010)
			{
				m_uiADCDataFrameCount[i] = 0;
				m_pSampleData[i]->clear();
			}
		}
	}
}
// 生成ADC数据重发帧
void CADCDataSocket::MakeADCDataRetransmissionFrame(unsigned short usDataPointNb, unsigned int uiIPAim)
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
	TRACE1("尾包时刻查询帧-仪器IP地址：%d\r\n", uiIPAim);
	// 目标端口号
	memcpy(&m_ucADCRetransmission[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_ucADCRetransmission[26], &usCommand, 2);

	int iPos = 32;

	memmove(m_ucADCRetransmission + iPos, &usDataPointNb, 2);
	iPos = iPos + 2;
	m_ucADCRetransmission[iPos] = 0x00;

	int icount = SendTo(m_ucADCRetransmission, 128, m_uiSendPort, IPBroadcastAddr);
}
// ADC数据需要重新发送帧对应的最小个数
unsigned int CADCDataSocket::OnADCRetransimissionMinNb(void)
{
	POSITION pos = m_listADC.GetHeadPosition();
	int icount = m_listADC.GetCount();
	unsigned int uimin = 0;
	m_structADC ADCStructTemp;

	ADCStructTemp = m_listADC.GetNext(pos);
	uimin = ADCStructTemp.uiDataCount;

	for (int i = 1; i < icount; i++)
	{
		ADCStructTemp = m_listADC.GetNext(pos);
		if (uimin > ADCStructTemp.uiDataCount)
		{
			uimin = ADCStructTemp.uiDataCount;
		}
	} 
	return uimin;
}