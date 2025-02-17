#include "StdAfx.h"
#include "ADCDataSaveToFile.h"
#include <iostream>
#include <iomanip>

CADCDataSaveToFile::CADCDataSaveToFile(void)
: m_bOpenADCSaveFile(FALSE)
, m_uiADCSaveFileNum(0)
/*, m_pFileSave(NULL)*/
, m_uiADCDataToSaveNum(0)
, m_uiADCFileLength(0)
, m_csSaveFilePath(_T(""))
, m_arFileSave(NULL)
, m_uiSampleInstrumentNum(0)
, m_pInstrumentList(NULL)
{
}

CADCDataSaveToFile::~CADCDataSaveToFile(void)
{
}

// 将ADC数据保存到文件中
void CADCDataSaveToFile::OnSaveADCToFile(int(* iADCDataBuf)[ADCDataTempDataSize], unsigned int* upADCDataNum,
										 unsigned int uiADCDataToSaveNum, bool bFinish)
{
	CString strOutput = _T("");
	CString strTemp = _T("");
	unsigned int uiDataLength = 0;
// 	char   buffer[_CVTBUFSIZE]; 
// 	int dec,sign;
	wchar_t buffer[_CVTBUFSIZE];
	if(m_bOpenADCSaveFile == FALSE)
	{
		OnOpenADCSaveFile();
	}
	// @@@@@@@@停止采集时跨文件的情况
	// 输出各仪器采样数据
	for (unsigned int i=0; i<uiADCDataToSaveNum; i++)
	{
		//		ProcessMessages();
		for (unsigned int j=0; j<m_pInstrumentList->m_oInstrumentIPMap.size(); j++)
		{
			//			ProcessMessages();
			uiDataLength = upADCDataNum[j];
			if (uiDataLength == 0 )
			{
//				strOutput += _T("  \t \t");
				continue;
			}
			if (uiDataLength > i)
			{
				// 方法1：也可以实现double转换为string，只是转换后的数据采用科学计数法，CPU占用率达到22%
// 				_gcvt_s(buffer, _CVTBUFSIZE, dpADCDataBuf[j][i], 9);
// 				strOutput += buffer;
				ZeroMemory(buffer, _CVTBUFSIZE);
				// 方法2：采用_stprintf_s函数的方法，CPU占用率达到10%
				_stprintf_s(buffer, _CVTBUFSIZE, _T("%*d"), DecimalPlaces, iADCDataBuf[j][i]);
				strOutput += buffer;
				strOutput +=  _T(" \t");

				// 方法3：采用_fcvt_s函数的方法，CPU占用率达到25%
// 				_fcvt_s(buffer, _CVTBUFSIZE, dpADCDataBuf[j][i], DecimalPlaces, &dec, &sign);
// 				char* length = Convert(buffer,dec,sign);
// 				strOutput += length;
// 				strOutput +=  _T("\t");

				// 方法4：采用Format函数的方法，CPU占用率达到20%
//				strTemp.Format(_T("%2.*lf\t"), DecimalPlaces, dpADCDataBuf[j][i]);
//				strOutput += strTemp;
			}
			else
			{
				// 针对有采集站数据不全的情况采用补零处理
				ZeroMemory(buffer, _CVTBUFSIZE);
				// 方法2：采用_stprintf_s函数的方法，CPU占用率达到10%
				_stprintf_s(buffer, _CVTBUFSIZE, _T("%*d"), DecimalPlaces, 0);
				strOutput += buffer;
				strOutput +=  _T(" \t");
			}
		}
		strOutput += _T("\r\n");
	}
	//	fprintf(m_pFileSave, _T("%s"), strOutput); 
// 	CArchive ar(&m_FileSave, CArchive::store);
// 	ar.WriteString(strOutput);
// 	ar.Close();
	//因为需要保存的内容包含中文，所以需要如下的转换过程
// 	WriteCHToCFile(m_arFileSave, strOutput);
	if (m_FileSave.m_hFile != CFile::hFileNull)
	{
		m_arFileSave->WriteString(strOutput);
	}

	if (bFinish == false)
	{
		m_uiADCDataToSaveNum += uiADCDataToSaveNum;
		if (m_uiADCDataToSaveNum == m_uiADCFileLength)
		{
			m_uiADCDataToSaveNum = 0;
			OnCloseADCSaveFile();
		}
	}
	else
	{
		OnCloseADCSaveFile();
		m_uiADCDataToSaveNum = 0;
		m_uiADCSaveFileNum = 0;
	}
}
// 创建并打开ADC保存数据文件
void CADCDataSaveToFile::OnOpenADCSaveFile(void)
{
	CString strFileName = _T("");
	CString strOutput = _T("");
	CString strTemp = _T("");
	//	errno_t err;
	CString str = _T("");
	unsigned int uiADCStartNum = 0;
	unsigned int uiADCDataCovNb = ADCDataConvert;
	SYSTEMTIME  sysTime;
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	// 选中的仪器对象名称
//	wchar_t cSelectObjectName[InstrumentNum][RcvFrameSize];
//	char cSelectObjectName[InstrumentNum][RcvFrameSize];
	strFileName += m_csSaveFilePath;
	m_uiADCSaveFileNum++;
	strTemp.Format(_T("\\%d.text"), m_uiADCSaveFileNum);
	strFileName += strTemp;
	// 将ADC采样数据保存成ANSI格式的文件
	// 	if((err = fopen_s(&m_pFileSave,strFileName,"w+"))!=NULL)
	// 	{
	// 		AfxMessageBox(_T("ADC数据存储文件创建失败！"));	
	// 		return;
	// 	}
	//保存成UNICODE格式的文件
	if(m_FileSave.Open(strFileName, CFile::modeCreate|CFile::modeWrite) == FALSE)
	{
		AfxMessageBox(_T("ADC数据存储文件创建失败！"));	
		return;
	}
	m_FileSave.Write("\xff\xfe", 2);    // 设置Unicode编码文件头
	m_arFileSave = new CArchive(&m_FileSave, CArchive::store);
	GetLocalTime(&sysTime);
	str.Format(_T("%04d年%02d月%02d日%02d:%02d:%02d:%03d开始记录ADC采样数据：\r\n"), sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	strOutput += str;
	uiADCStartNum = (m_uiADCSaveFileNum - 1) * m_uiADCFileLength;
	str.Format(_T("采集站设备总数%d，从第%d个数据开始存储ADC数据，数据转换方式采用方式%d！\r\n"), m_uiSampleInstrumentNum, uiADCStartNum, uiADCDataCovNb);
	strOutput += str;

	// 输出仪器标签
	for(iter=m_pInstrumentList->m_oInstrumentIPMap.begin(); iter!=m_pInstrumentList->m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if ((iter->second->m_uiInstrumentType == InstrumentTypeFDU)
				&&(iter->second->m_uiInstrumentType == BST_CHECKED)
				&&(iter->second->m_bIPSetOK == true))
			{
				strTemp.Format(_T("仪器%d \t"), iter->second->m_uiFDUIndex + 1);
				int iLength = strTemp.GetLength();
				if (iLength < DecimalPlaces + ADCDataPlaces)
				{
					for (int j=0; j<(DecimalPlaces + ADCDataPlaces - iLength); j++)
					{
						strTemp = _T(" ") + strTemp;
					}
				}
				strOutput += strTemp;
			}
		}
	}

	strOutput += _T("\r\n");
	//	fprintf(m_pFileSave, _T("%s"), strOutput); 
// 	CArchive ar(&m_FileSave, CArchive::store);
// 	ar.WriteString(strOutput);
// 	ar.Close();

	//因为需要保存的内容包含中文，所以需要如下的转换过程
//	WriteCHToCFile(m_arFileSave, strOutput);
	if (m_FileSave.m_hFile != CFile::hFileNull)
	{
		m_arFileSave->WriteString(strOutput);
	}
	m_bOpenADCSaveFile = TRUE;
}
// 关闭ADC保存数据文件
void CADCDataSaveToFile::OnCloseADCSaveFile(void)
{
	if(m_FileSave.m_hFile != CFile::hFileNull)    // virtual member function
	{
		m_arFileSave->Close();
		m_FileSave.Close();
		delete m_arFileSave;
		m_bOpenADCSaveFile = FALSE;
	}
}
// 防止程序在循环中运行无法响应消息
void CADCDataSaveToFile::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}