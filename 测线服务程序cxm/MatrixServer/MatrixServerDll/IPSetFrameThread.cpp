#include "stdafx.h"
#include "MatrixServerDll.h"

// 创建IP地址设置线程
m_oIPSetFrameThreadStruct* OnCreateIPSetFrameThread(void)
{
	m_oIPSetFrameThreadStruct* pIPSetFrameThread = NULL;
	pIPSetFrameThread = new m_oIPSetFrameThreadStruct;
	pIPSetFrameThread->m_pThread = new m_oThreadStruct;
	pIPSetFrameThread->m_pLineList = NULL;
	pIPSetFrameThread->m_pIPSetFrame = NULL;
	InitializeCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	return pIPSetFrameThread;
}
// 处理单个IP地址设置应答帧
void  ProcIPSetReturnFrameOne(m_oIPSetFrameThreadStruct* pIPSetFrameThread)
{
	ASSERT(pIPSetFrameThread != NULL);
	unsigned int uiIPInstrument = 0;
	m_oInstrumentStruct* pInstrument = NULL;
	m_oRoutStruct* pRout = NULL;
	unsigned short usCommand = 0;
	char cLAUXRoutOpenSet = 0;
	CString str = _T("");
	string strFrameData = "";
	string strConv = "";
	// 得到仪器IP
	EnterCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
	uiIPInstrument = pIPSetFrameThread->m_pIPSetFrame->m_pCommandStructReturn->m_uiSrcIP;
	usCommand = pIPSetFrameThread->m_pIPSetFrame->m_pCommandStructReturn->m_usCommand;
	cLAUXRoutOpenSet = pIPSetFrameThread->m_pIPSetFrame->m_pCommandStructReturn->m_cLAUXRoutOpenSet;
	LeaveCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
	EnterCriticalSection(&pIPSetFrameThread->m_pLineList->m_oSecLineList);
	// 仪器在索引表中
	if (TRUE == IfIndexExistInMap(uiIPInstrument, &pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPSetInstrumentMap))
	{
		pInstrument = GetInstrumentFromMap(uiIPInstrument, &pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPSetInstrumentMap);
		// 更新仪器的存活时间
		UpdateInstrActiveTime(pInstrument);
		// 从IP地址设置索引表中删除仪器
		DeleteInstrumentFromMap(uiIPInstrument, &pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPSetInstrumentMap);
		// 将仪器加入IP地址索引表
		pInstrument->m_bIPSetOK = true;
		// 路由方向仪器个数加一
		pRout = GetRout(pInstrument->m_uiRoutIP, &pIPSetFrameThread->m_pLineList->m_pRoutList->m_oRoutMap);
		if (FALSE == IfIndexExistInMap(uiIPInstrument, &pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPInstrumentMap))
		{
			pRout->m_uiInstrumentNum++;
			AddInstrumentToMap(uiIPInstrument, pInstrument, &pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPInstrumentMap);
		}
		// 加入仪器位置索引表
		AddLocationToMap(pInstrument->m_iLineIndex, pInstrument->m_iPointIndex, pInstrument, 
			&pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oInstrumentLocationMap);
		if (usCommand == pIPSetFrameThread->m_pThread->m_pConstVar->m_usSendSetCmd)
		{
			str.Format(_T("接收到SN = 0x%x，IP地址 = 0x%x仪器的IP地址设置应答, 仪器软件版本 = 0x%x"), 
				pInstrument->m_uiSN, uiIPInstrument, pInstrument->m_uiVersion);
		}
		else if (usCommand == pIPSetFrameThread->m_pThread->m_pConstVar->m_usSendQueryCmd)
		{
			str.Format(_T("接收到SN = 0x%x，IP地址 = 0x%x仪器的IP地址查询应答, 仪器软件版本 = 0x%x"), 
				pInstrument->m_uiSN, uiIPInstrument, pInstrument->m_uiVersion);
		}
		strConv = (CStringA)str;
		AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetReturnFrameOne", 
			strConv);
	}
	else
	{
		LeaveCriticalSection(&pIPSetFrameThread->m_pLineList->m_oSecLineList);
		EnterCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
		GetFrameInfo(pIPSetFrameThread->m_pIPSetFrame->m_cpRcvFrameData,
			pIPSetFrameThread->m_pThread->m_pConstVar->m_iRcvFrameSize, &strFrameData);
		LeaveCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
		AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetReturnFrameOne", 
			strFrameData, ErrorType, IDS_ERR_IPSETMAP_NOTEXIT);
		EnterCriticalSection(&pIPSetFrameThread->m_pLineList->m_oSecLineList);
	}
	// 如果仪器位于已分配IP地址索引表
	if (TRUE == IfIndexExistInMap(uiIPInstrument, &pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPInstrumentMap))
	{
		pInstrument = GetInstrumentFromMap(uiIPInstrument, &pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPInstrumentMap);
		// 如果仪器类型为LCI或交叉站
		if ((pInstrument->m_iInstrumentType == InstrumentTypeLCI)
			|| (pInstrument->m_iInstrumentType == InstrumentTypeLAUX))
		{
			// 关闭交叉线A电源
			if (cLAUXRoutOpenSet & (0x01 << 7))
			{
				AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetReturnFrameOne", 
					"关闭交叉线A电源");
				pRout = GetRout(pInstrument->m_uiRoutIPDown, &pIPSetFrameThread->m_pLineList->m_pRoutList->m_oRoutMap);
				// 在路由方向上删除该仪器之后的全部仪器
				DeleteAllInstrumentAlongRout(pInstrument, pRout, pIPSetFrameThread->m_pLineList, 
					pIPSetFrameThread->m_pThread->m_pLogOutPut);
			}
			// 关闭交叉线B电源
			if (cLAUXRoutOpenSet & (0x01 << 6))
			{
				AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetReturnFrameOne", 
					"关闭交叉线B电源");
				pRout = GetRout(pInstrument->m_uiRoutIPTop, &pIPSetFrameThread->m_pLineList->m_pRoutList->m_oRoutMap);
				// 在路由方向上删除该仪器之后的全部仪器
				DeleteAllInstrumentAlongRout(pInstrument, pRout, pIPSetFrameThread->m_pLineList, 
					pIPSetFrameThread->m_pThread->m_pLogOutPut);
			}
			// 关闭大线A电源
			if (cLAUXRoutOpenSet & (0x01 << 5))
			{
				AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetReturnFrameOne", 
					"关闭大线A电源");
				pRout = GetRout(pInstrument->m_uiRoutIPRight, &pIPSetFrameThread->m_pLineList->m_pRoutList->m_oRoutMap);
				// 在路由方向上删除该仪器之后的全部仪器
				DeleteAllInstrumentAlongRout(pInstrument, pRout, pIPSetFrameThread->m_pLineList, 
					pIPSetFrameThread->m_pThread->m_pLogOutPut);
			}
			// 关闭大线B电源
			if (cLAUXRoutOpenSet & (0x01 << 4))
			{
				AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetReturnFrameOne", 
					"关闭大线B电源");
				pRout = GetRout(pInstrument->m_uiRoutIPLeft, &pIPSetFrameThread->m_pLineList->m_pRoutList->m_oRoutMap);
				// 在路由方向上删除该仪器之后的全部仪器
				DeleteAllInstrumentAlongRout(pInstrument, pRout, pIPSetFrameThread->m_pLineList, 
					pIPSetFrameThread->m_pThread->m_pLogOutPut);
			}
		}
	}
	LeaveCriticalSection(&pIPSetFrameThread->m_pLineList->m_oSecLineList);
}
// 处理IP地址设置应答帧
void ProcIPSetReturnFrame(m_oIPSetFrameThreadStruct* pIPSetFrameThread)
{
	ASSERT(pIPSetFrameThread != NULL);
	// 帧数量设置为0
	int iFrameCount = 0;
	EnterCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
	// 得到首包接收网络端口帧数量
	iFrameCount = GetFrameCount(pIPSetFrameThread->m_pIPSetFrame->m_oIPSetFrameSocket,
		pIPSetFrameThread->m_pThread->m_pConstVar->m_iRcvFrameSize, 
		pIPSetFrameThread->m_pThread->m_pLogOutPut);
	LeaveCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
	// 判断帧数量是否大于0
	if(iFrameCount > 0)
	{
		// 循环处理每1帧
		for(int i = 0; i < iFrameCount; i++)
		{
			EnterCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
			// 得到帧数据
			if (false == GetFrameData(pIPSetFrameThread->m_pIPSetFrame->m_oIPSetFrameSocket,
				pIPSetFrameThread->m_pIPSetFrame->m_cpRcvFrameData, 
				pIPSetFrameThread->m_pThread->m_pConstVar->m_iRcvFrameSize, 
				pIPSetFrameThread->m_pThread->m_pLogOutPut))
			{
				LeaveCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
				continue;
			}
			LeaveCriticalSection(&pIPSetFrameThread->m_pIPSetFrame->m_oSecIPSetFrame);
			if (false == ParseInstrIPSetReturnFrame(pIPSetFrameThread->m_pIPSetFrame, 
				pIPSetFrameThread->m_pThread->m_pConstVar, pIPSetFrameThread->m_pThread->m_pLogOutPut))
			{
				AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, 
					"ParseInstrumentIPSetReturnFrame", "", ErrorType, IDS_ERR_PARSE_IPSETRETURNFRAME);
				continue;
			}
			// 处理单个IP地址设置应答帧
			ProcIPSetReturnFrameOne(pIPSetFrameThread);			
		}
	}
}
// 按照IP地址设置索引发送IP地址设置帧
void ProcIPSetFrame(m_oIPSetFrameThreadStruct* pIPSetFrameThread)
{
	ASSERT(pIPSetFrameThread != NULL);
	CString str = _T("");
	string strConv = "";
	// hash_map迭代器
	hash_map<unsigned int, m_oInstrumentStruct*>::iterator  iter;
	EnterCriticalSection(&pIPSetFrameThread->m_pLineList->m_oSecLineList);
	// IP地址设置索引不为空
	if (false == pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPSetInstrumentMap.empty())
	{
		// 发送索引表内设备的IP地址设置帧
		for(iter = pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPSetInstrumentMap.begin(); 
			iter != pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPSetInstrumentMap.end();)
		{
			// IP地址设置次数为0
			if (iter->second->m_iIPSetCount == 0)
			{
				// 生成IP地址设置帧
				MakeInstrIPSetFrame(pIPSetFrameThread->m_pIPSetFrame, 
					pIPSetFrameThread->m_pThread->m_pConstVar, iter->second,
					pIPSetFrameThread->m_pThread->m_pLogOutPut);
				// 第几次设置IP地址
				iter->second->m_iIPSetCount++;
				iter++;
			}
			// IP地址设置次数不超过指定次数则重新设置IP地址和查询IP地址
			else if (iter->second->m_iIPSetCount <= pIPSetFrameThread->m_pThread->m_pConstVar->m_iIPAddrResetTimes)
			{
				// 生成IP地址查询帧
				MakeInstrIPQueryFrame(pIPSetFrameThread->m_pIPSetFrame, 
					pIPSetFrameThread->m_pThread->m_pConstVar, iter->second->m_uiIP,
					pIPSetFrameThread->m_pThread->m_pLogOutPut);
				// 生成IP地址设置帧
				MakeInstrIPSetFrame(pIPSetFrameThread->m_pIPSetFrame, 
					pIPSetFrameThread->m_pThread->m_pConstVar, iter->second,
					pIPSetFrameThread->m_pThread->m_pLogOutPut);
				// 第几次设置IP地址
				iter->second->m_iIPSetCount++;
				iter++;
				AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetFrame",
					"重发IP地址设置帧和IP地址查询帧", WarningType);
			}
			// IP地址设置次数超过指定次数则从索引表中删除该仪器指针
			else
			{
				str.Format(_T("仪器SN = 0x%x，IP地址 = 0x%x 的仪器发送IP地址设置帧超过指定次数"), 
					iter->second->m_uiSN, iter->second->m_uiIP);
				// 加入错误日志
				strConv = (CStringA)str;
				AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "ProcIPSetFrame",
					strConv, WarningType);
				iter->second->m_iIPSetCount = 0;
				pIPSetFrameThread->m_pLineList->m_pInstrumentList->m_oIPSetInstrumentMap.erase(iter++);
			}
		}
	}
	LeaveCriticalSection(&pIPSetFrameThread->m_pLineList->m_oSecLineList);
}
// 线程等待函数
void WaitIPSetFrameThread(m_oIPSetFrameThreadStruct* pIPSetFrameThread)
{
	ASSERT(pIPSetFrameThread != NULL);
	// 初始化等待次数为0
	int iWaitCount = 0;
	bool bClose = false;
	// 循环
	while(true)
	{	// 休眠50毫秒
		Sleep(pIPSetFrameThread->m_pThread->m_pConstVar->m_iOneSleepTime);
		// 等待次数加1
		iWaitCount++;
		EnterCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		bClose = pIPSetFrameThread->m_pThread->m_bClose;
		LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		// 判断是否可以处理的条件
		if(bClose == true)
		{
			// 返回
			return;
		}
		// 判断等待次数是否大于等于最多等待次数
		if(pIPSetFrameThread->m_pThread->m_pConstVar->m_iIPSetFrameSleepTimes == iWaitCount)
		{
			// 返回
			return;
		}		
	}
}
// 线程函数
DWORD WINAPI RunIPSetFrameThread(m_oIPSetFrameThreadStruct* pIPSetFrameThread)
{
	ASSERT(pIPSetFrameThread != NULL);
	bool bClose = false;
	bool bWork = false;
	while(true)
	{
		EnterCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		bClose = pIPSetFrameThread->m_pThread->m_bClose;
		bWork = pIPSetFrameThread->m_pThread->m_bWork;
		LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		if (bClose == true)
		{
			break;
		}
		if (bWork == true)
		{
			// 处理IP地址设置应答帧
			ProcIPSetReturnFrame(pIPSetFrameThread);
			// 按照IP地址设置索引发送IP地址设置帧
			ProcIPSetFrame(pIPSetFrameThread);
		}
		EnterCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		bClose = pIPSetFrameThread->m_pThread->m_bClose;
		LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		if (bClose == true)
		{
			break;
		}
		WaitIPSetFrameThread(pIPSetFrameThread);
	}
	EnterCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	SetEvent(pIPSetFrameThread->m_pThread->m_hThreadClose); // 设置事件对象为有信号状态,释放等待线程后将事件置为无信号
	LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	return 1;
}
// 初始化IP地址设置线程
bool OnInitIPSetFrameThread(m_oIPSetFrameThreadStruct* pIPSetFrameThread, 
	m_oLogOutPutStruct* pLogOutPut, m_oConstVarStruct* pConstVar)
{
	ASSERT(pIPSetFrameThread != NULL);
	EnterCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	if (false == OnInitThread(pIPSetFrameThread->m_pThread, pLogOutPut, pConstVar))
	{
		LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		return false;
	}
	ResetEvent(pIPSetFrameThread->m_pThread->m_hThreadClose);	// 设置事件对象为无信号状态
	// 创建线程
	pIPSetFrameThread->m_pThread->m_hThread = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 
		0,
		(LPTHREAD_START_ROUTINE)RunIPSetFrameThread,
		pIPSetFrameThread, 
		0, 
		&pIPSetFrameThread->m_pThread->m_dwThreadID);
	if (pIPSetFrameThread->m_pThread->m_hThread == NULL)
	{
		LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "OnInitIPSetFrameThread", 
			"pIPSetFrameThread->m_pThread->m_hThread", ErrorType, IDS_ERR_CREATE_THREAD);
		return false;
	}
	LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "OnInitIPSetFrameThread", 
		"IP地址设置线程创建成功");
	return true;
}
// 初始化IP地址设置线程
bool OnInit_IPSetFrameThread(m_oEnvironmentStruct* pEnv)
{
	ASSERT(pEnv != NULL);
	pEnv->m_pIPSetFrameThread->m_pIPSetFrame = pEnv->m_pIPSetFrame;
	pEnv->m_pIPSetFrameThread->m_pLineList = pEnv->m_pLineList;
	return OnInitIPSetFrameThread(pEnv->m_pIPSetFrameThread, pEnv->m_pLogOutPutOpt, pEnv->m_pConstVar);
}
// 关闭IP地址设置线程
bool OnCloseIPSetFrameThread(m_oIPSetFrameThreadStruct* pIPSetFrameThread)
{
	ASSERT(pIPSetFrameThread != NULL);
	EnterCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	if (false == OnCloseThread(pIPSetFrameThread->m_pThread))
	{
		LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
		AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "OnCloseIPSetFrameThread", 
			"IP地址设置线程强制关闭", WarningType);
		return false;
	}
	LeaveCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	AddMsgToLogOutPutList(pIPSetFrameThread->m_pThread->m_pLogOutPut, "OnCloseIPSetFrameThread", 
		"IP地址设置线程成功关闭");
	return true;
}
// 释放IP地址设置线程
void OnFreeIPSetFrameThread(m_oIPSetFrameThreadStruct* pIPSetFrameThread)
{
	ASSERT(pIPSetFrameThread != NULL);
	if (pIPSetFrameThread->m_pThread != NULL)
	{
		delete pIPSetFrameThread->m_pThread;
		pIPSetFrameThread->m_pThread = NULL;
	}
	DeleteCriticalSection(&pIPSetFrameThread->m_oSecIPSetFrameThread);
	delete pIPSetFrameThread;
	pIPSetFrameThread = NULL;
}