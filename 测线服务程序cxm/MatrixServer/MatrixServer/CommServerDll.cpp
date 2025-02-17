#include "StdAfx.h"
#include "CommServerDll.h"


CCommServerDll::CCommServerDll(void)
	: m_uiLineNum(0)
	, m_uiColumnNum(0)
	, m_fInstrumentNoiseLimit(0)
	, m_fInstrumentDistortionLimit(0)
	, m_fInstrumentCrosstalkLimit(0)
	, m_fInstrumentGainPhaseLimit(0)
	, m_fInstrumentCMRRLimit(0)
	, m_fSensorResistanceLimitMin(0)
	, m_fSensorResistanceLimitMax(0)
	, m_fSensorLeakageLimit(0)
	, m_fSensorNoiseLimit(0)
	, m_fSensorTiltLimit(0)
	, m_fSensorTiltModelLimit(0)
	, m_fSeisMonitorLimit(0)
{
	m_pMatrixServer = NULL;
}


CCommServerDll::~CCommServerDll(void)
{
}

// 处理上电
void CCommServerDll::OnProcSetFieldOn(CCommRecThread* pRecThread)
{
	unsigned int uiFieldOnNeedTime = 0;;
	uiFieldOnNeedTime = m_pMatrixServer->Dll_Work();
	memcpy(pRecThread->m_pCommSndFrame->m_cProcBuf, &uiFieldOnNeedTime, 4);
	pRecThread->m_pCommSndFrame->MakeSetFrame(CmdFieldOnWaitTime, 
		pRecThread->m_pCommSndFrame->m_cProcBuf, 4);
}
// 处理断电
void CCommServerDll::OnProcSetFieldOff(CCommRecThread* pRecThread)
{
	m_pMatrixServer->Dll_Stop();
	pRecThread->m_oInstrumentWholeTableMap.clear();
}

// 查询所选区域仪器噪声测试数据和测试结果
unsigned int CCommServerDll::QueryInstrNoiseTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fInstrumentNoiseLimit) || (fData < -m_fInstrumentNoiseLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域仪器失真测试数据和测试结果
unsigned int CCommServerDll::QueryInstrDistortionTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fInstrumentDistortionLimit) || (fData < -m_fInstrumentDistortionLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域仪器串扰测试数据和测试结果
unsigned int CCommServerDll::QueryInstrCrosstalkTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fInstrumentCrosstalkLimit) || (fData < -m_fInstrumentCrosstalkLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域仪器共模抑制比测试数据和测试结果
unsigned int CCommServerDll::QueryInstrCMRRTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fInstrumentCMRRLimit) || (fData < -m_fInstrumentCMRRLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域仪器增益相位测试数据和测试结果
unsigned int CCommServerDll::QueryInstrGainPhaseTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fInstrumentGainPhaseLimit) || (fData < -m_fInstrumentGainPhaseLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域检波器阻抗测试数据和测试结果
unsigned int CCommServerDll::QuerySensorResistanceTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fSensorResistanceLimitMax) || (fData < m_fSensorResistanceLimitMin))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域检波器漏电测试数据和测试结果
unsigned int CCommServerDll::QuerySensorLeakageTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fSensorLeakageLimit) || (fData < -m_fSensorLeakageLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域检波器噪声测试数据和测试结果
unsigned int CCommServerDll::QuerySensorNoiseTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fSensorNoiseLimit) || (fData < -m_fSensorNoiseLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域检波器倾斜度测试数据和测试结果
unsigned int CCommServerDll::QuerySensorTiltTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fSensorTiltLimit) || (fData < -m_fSensorTiltLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域检波器倾斜度模式测试数据和测试结果
unsigned int CCommServerDll::QuerySensorTiltModelTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fSensorTiltModelLimit) || (fData < -m_fSensorTiltModelLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 查询所选区域地震监测测试数据和测试结果
unsigned int CCommServerDll::QuerySeisMonitorTestBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	float fData = 0.0;
	char cResult = 0;
	if (pInstrument != NULL)
	{
		// 数据处理
		fData = CalTestDataMeanSquare(pInstrument);
		fData = (float)(2.5 * 1e6 * fData / 0x800000);

		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &fData, 4);
		uiPos += 4;
		// 处理结果
		if ((fData > m_fSeisMonitorLimit) || (fData < -m_fSeisMonitorLimit))
		{
			cResult = TestResultFailure;
		}
		else
		{
			cResult = TestResultQualified;
		}
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
		cResult = TestResultNull;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &cResult, 1);
		uiPos += 1;
	}
	return uiPos;
}


// 计算测试数据的算术均方根
float CCommServerDll::CalTestDataMeanSquare(m_oInstrumentStruct* pInstrument)
{
	return (float)m_pMatrixServer->Dll_CalMeanSquare(pInstrument);
}
// 处理仪器设备表更新
void CCommServerDll::OnProcInstrumentTableUpdate(CCommRecThread* pRecThread)
{
	map<m_oLocationStruct, unsigned int>::iterator iterLocation;
	/** 客户端设备位置索引表*/
	map<m_oLocationStruct, unsigned int> InstrumentWholeTableMap;
	int iLineIndex = 0;
	int iPointIndex = 0;
	unsigned int uiSN = 0;
	unsigned int* uipSNClient = NULL;
	int iPos = 0;
	int iSize = 0;
	pRecThread->m_oInstrumentUpdataTableMap.clear();
	// 将SN索引表与客户端的仪器位置索引表相对照
	m_pMatrixServer->Dll_QueryInstrumentLocation(pRecThread->m_pCommSndFrame->m_cProcBuf, iSize);
	for (int i=0; i<iSize;)
	{
		memcpy(&iLineIndex, &pRecThread->m_pCommSndFrame->m_cProcBuf[i], 4);
		i += 4;
		memcpy(&iPointIndex, &pRecThread->m_pCommSndFrame->m_cProcBuf[i], 4);
		i += 4;
		memcpy(&uiSN, &pRecThread->m_pCommSndFrame->m_cProcBuf[i], 4);
		i += 4;
		// 客户端仪器索引表中找不到该设备
		if (FALSE == pRecThread->IfLocationExistInMap(iLineIndex, iPointIndex, &pRecThread->m_oInstrumentWholeTableMap))
		{
			// 将仪器加入整表
			pRecThread->AddLocationToMap(iLineIndex, iPointIndex, uiSN, &pRecThread->m_oInstrumentWholeTableMap);
			// 将仪器加入更新表
			pRecThread->AddLocationToMap(iLineIndex, iPointIndex, uiSN, &pRecThread->m_oInstrumentUpdataTableMap);
		}
		else
		{
			uipSNClient = pRecThread->GetSnPtrFromLocationMap(iLineIndex, iPointIndex, &pRecThread->m_oInstrumentWholeTableMap);
			if (uiSN != *uipSNClient)
			{
				// 整表中SN与当前位置SN不同
				*uipSNClient = uiSN;
				// 加入更新表
				pRecThread->AddLocationToMap(iLineIndex, iPointIndex, uiSN, &pRecThread->m_oInstrumentUpdataTableMap);
			}
		}
	}
	EnterCriticalSection(&m_pMatrixServer->m_pEnv->m_pLineList->m_oSecLineList);
	// 将客户端的仪器位置索引表与SN索引表相对照
	for (iterLocation = pRecThread->m_oInstrumentWholeTableMap.begin(); iterLocation != pRecThread->m_oInstrumentWholeTableMap.end();)
	{
		// 仪器位置索引表找不到该设备
		if (FALSE == m_pMatrixServer->Dll_IfLocationExistInMap(iterLocation->first.m_iLineIndex, 
			iterLocation->first.m_iPointIndex, &m_pMatrixServer->m_pEnv->m_pLineList->m_pInstrumentList->m_oInstrumentLocationMap))
		{
			// 在仪器位置索引中找不到整表中该位置仪器
			// 更新表中该位置SN更新为0
			pRecThread->AddLocationToMap(iterLocation->first.m_iLineIndex, iterLocation->first.m_iPointIndex, 0, 
				&pRecThread->m_oInstrumentUpdataTableMap);
			// 从整表中删除该位置仪器
			pRecThread->m_oInstrumentWholeTableMap.erase(iterLocation++);
		}
		else
		{
			uiSN = m_pMatrixServer->Dll_GetInstrumentFromLocationMap(iterLocation->first.m_iLineIndex, 
				iterLocation->first.m_iPointIndex, &m_pMatrixServer->m_pEnv->m_pLineList->m_pInstrumentList->m_oInstrumentLocationMap)->m_uiSN;
			// 当前位置SN与整表中SN不同
			if (uiSN != iterLocation->second)
			{
				// 更新整表中该位置SN
				iterLocation->second = uiSN;
				// 将该位置加入索引
				pRecThread->AddLocationToMap(iterLocation->first.m_iLineIndex, iterLocation->first.m_iPointIndex, uiSN, 
					&pRecThread->m_oInstrumentUpdataTableMap);
			}
			iterLocation++;
		}
	}
	LeaveCriticalSection(&m_pMatrixServer->m_pEnv->m_pLineList->m_oSecLineList);
//	pRecThread->m_oInstrumentWholeTableMap.swap(InstrumentWholeTableMap);

	// 对照后客户端仪器索引表与服务端仪器SN索引表一致，将更新表发送到客户端
	for (iterLocation = pRecThread->m_oInstrumentUpdataTableMap.begin();
		iterLocation != pRecThread->m_oInstrumentUpdataTableMap.end(); iterLocation++)
	{
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[iPos], &iterLocation->first.m_iLineIndex, 4);
		iPos += 4;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[iPos], &iterLocation->first.m_iPointIndex, 4);
		iPos += 4;
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[iPos], &iterLocation->second, 4);
		iPos += 4;
	}
	pRecThread->m_oInstrumentUpdataTableMap.clear();
	if (iPos > 0)
	{
		pRecThread->m_pCommSndFrame->MakeSetFrame(CmdQueryUpdateTable, 
			pRecThread->m_pCommSndFrame->m_cProcBuf, iPos);
	}
}

// 按区域查询仪器信息
unsigned int CCommServerDll::QueryBySN(CCommRecThread* pRecThread, unsigned int uiSN, 
	unsigned int uiSendPos, unsigned int(CCommServerDll::*ptrFun)(m_oInstrumentStruct* pInstrument, 
	unsigned int uiStartPos, CCommRecThread* pRecThread))
{
	unsigned int uiPos = uiSendPos;
	m_oInstrumentStruct* pInstrument = NULL;

	EnterCriticalSection(&m_pMatrixServer->m_pEnv->m_pLineList->m_oSecLineList);
	pInstrument = m_pMatrixServer->Dll_GetInstrumentFromMap(uiSN, &m_pMatrixServer->m_pEnv->m_pLineList->m_pInstrumentList->m_oSNInstrumentMap);
	memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &uiSN, 4);
	uiPos += 4;
	uiPos = (this->*ptrFun)(pInstrument, uiPos, pRecThread);
	LeaveCriticalSection(&m_pMatrixServer->m_pEnv->m_pLineList->m_oSecLineList);
	return uiPos;
}

// 处理仪器信息查询
void CCommServerDll::OnProcQueryBySN(CCommRecThread* pRecThread, unsigned short usCmd, char* pChar, unsigned int uiSize, 
	unsigned int(CCommServerDll::*ptrFun)(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread))
{
	unsigned int uiPos = 0;
	unsigned int uiSendPos = 0;
	unsigned int uiSN = 0;
	while(uiPos < uiSize)
	{
		memcpy(&uiSN, &pChar[uiPos], 4);
		uiPos += 4;
		uiSendPos = QueryBySN(pRecThread, uiSN, uiSendPos, ptrFun);
	}
	pRecThread->m_pCommSndFrame->MakeSetFrame(usCmd, 
		pRecThread->m_pCommSndFrame->m_cProcBuf, uiSendPos);
}

// 处理全部信息查询
void CCommServerDll::OnProcQueryInfoAll(CCommRecThread* pRecThread, unsigned short usCmd,
	unsigned int(CCommServerDll::*ptrFun)(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread))
{
	int iPos = 0;
	map<m_oLocationStruct, unsigned int>::iterator iter;
	int iSize = pRecThread->m_oInstrumentWholeTableMap.size();
	if (iSize == 0)
	{
		return;
	}
	char* pChar = new char[iSize * 4];
	for (iter = pRecThread->m_oInstrumentWholeTableMap.begin();
		iter != pRecThread->m_oInstrumentWholeTableMap.end(); iter++)
	{
		memcpy(&pChar[iPos], &iter->second, 4);
		iPos += 4;
	}
	OnProcQueryBySN(pRecThread, usCmd, pChar, iPos, ptrFun);
	delete[] pChar;
	pChar = NULL;
}

// 按区域查询仪器信息
unsigned int CCommServerDll::QueryInstrumentInfoBySN(m_oInstrumentStruct* pInstrument, unsigned int uiStartPos, CCommRecThread* pRecThread)
{
	unsigned int uiPos = uiStartPos;
	if (pInstrument != NULL)
	{
		memcpy(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], &pInstrument->m_uiIP, 4);
		uiPos += 4;
	}
	else
	{
		memset(&pRecThread->m_pCommSndFrame->m_cProcBuf[uiPos], 0, 4);
		uiPos += 4;
	}
	return uiPos;
}
