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
{
}

CADCDataSocket::~CADCDataSocket()
{
}

// CADCDataSocket 成员函数

void CADCDataSocket::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	unsigned short uiPort = 0;
	ret = Receive(ADCData,256);

	if(ret == 256) 
	{
		byte* pSampleData = &(ADCData[16]);

		ReceiveSampleData(pSampleData);
	}

	CSocket::OnReceive(nErrorCode);
}
// 接收采样数据
void CADCDataSocket::ReceiveSampleData(byte* pSampleData)
{
	// uiTestNb表明接收到帧所对应的仪器号
	unsigned int	uiTestNb = 0;
	unsigned short	uisDataNb = 0; // 接收到帧的数据的序号
	int	iSampleData = 0;
	int pos = 0;
	double dSampleDataToV = 0.0;
	unsigned int uitmp = 0;
	CString str = "";
	// 设m_pFrameData[16]到m_pFrameData[19]为源地址做为通道号
	memcpy(&uiTestNb, &pSampleData[pos], 4);
	pos += 4;
	// 设通道1,2,3,4分别对应四个仪器IP地址
	if (uiTestNb == 111)
	{
		uiTestNb = 1;
	}
	else if (uiTestNb == 101)
	{
		uiTestNb = 2;
	}
	else if (uiTestNb == 91)
	{
		uiTestNb = 4;
	}
	else if (uiTestNb == 81)
	{
		uiTestNb = 8;
	}
	// append the new value to the plot
	for (UINT32 i=0; i<GraphViewNum; i++)
	{
		if (uiTestNb == (UINT32)(1 << i))
		{
			// 接收数据缓存
			double m_dpReceiveData [ReceiveDataSize]; 
			pos += 8;
			// m_pFrameData[28]到m_pFrameData[29]做为采样个数的序号，暂时不用
			memcpy(&uisDataNb, &pSampleData[pos], 2);
			uitmp = m_uiDataFrameNum[i]%10000;
			if (uisDataNb > m_uiDataPointLast[i])
			{
				m_uiDataPointMove[i][uitmp] = uisDataNb - m_uiDataPointLast[i];
			}
			else
			{
				m_uiDataPointMove[i][uitmp] = 2048- m_uiDataPointLast[i]+ uisDataNb;
			}
			m_uiDataPointLast[i] = uisDataNb;
			m_uiDataFrameNum[i]++;
			pos += 2;

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
						m_Sec.Lock();
						m_pSampleData[i]->push_back(dSampleDataToV);
						m_Sec.Unlock();
						m_dADCSaveData[i].push_back(dSampleDataToV);
					}
				}
			}
			if (m_uiDataFrameNum[i] % 406 == 0)
			{
				// 保存成文件

				m_dADCSaveData[i].clear();
			}
			else if (m_uiDataFrameNum[i] == 7255010)
			{
				m_pSampleData[i]->clear();
			}
			m_Sec.Lock();
			m_pOScopeCtrl[i]->AppendPoint(m_dpReceiveData, ReceiveDataSize);
			m_Sec.Unlock();
		}
	}
}