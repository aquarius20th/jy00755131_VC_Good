#include "stdafx.h"
#include "MatrixServerDll.h"

// 创建仪器队列结构体
m_oInstrumentListStruct* OnCreateInstrumentList(void)
{
	m_oInstrumentListStruct* pInstrumentList = NULL;
	pInstrumentList = new m_oInstrumentListStruct;
	pInstrumentList->m_pArrayInstrument = NULL;
	pInstrumentList->m_bSetByHand = false;
	return pInstrumentList;
}
// 重置仪器队列结构体
void OnResetInstrumentList(m_oInstrumentListStruct* pInstrumentList)
{
	ASSERT(pInstrumentList != NULL);
	
	// 清空SN仪器索引表
	pInstrumentList->m_oSNInstrumentMap.clear();
	// 清空IP地址仪器索引表
	pInstrumentList->m_oIPInstrumentMap.clear();
	// 清空空闲仪器队列
	pInstrumentList->m_olsInstrumentFree.clear();
	// 清空IP地址设置仪器索引表
	pInstrumentList->m_oIPSetInstrumentMap.clear();
	// 清空未完成ADC参数设置的仪器队列
	pInstrumentList->m_oADCSetInstrumentMap.clear();
	// 清空仪器位置索引表
	pInstrumentList->m_oInstrumentLocationMap.clear();
	// 清空丢帧索引
	pInstrumentList->m_oADCLostFrameMap.clear();
	// 清空参与施工采集的仪器索引
	pInstrumentList->m_oOptInstrumentMap.clear();

	// 空闲仪器数量
	pInstrumentList->m_uiCountFree = pInstrumentList->m_uiCountAll;
	for(unsigned int i = 0; i < pInstrumentList->m_uiCountAll; i++)
	{
		// 重置仪器
		OnInstrumentReset(&pInstrumentList->m_pArrayInstrument[i], pInstrumentList->m_bSetByHand);
		
		// 仪器加在空闲仪器队列尾部
		pInstrumentList->m_olsInstrumentFree.push_back(&pInstrumentList->m_pArrayInstrument[i]);
	}
	
}
// ADC参数设置改为手动设置
void OnSetADCSetByHand(m_oInstrumentListStruct* pInstrumentList)
{
	ASSERT(pInstrumentList != NULL);
	pInstrumentList->m_bSetByHand = true;
}
// 初始化仪器队列结构体
void OnInitInstrumentList(m_oInstrumentListStruct* pInstrumentList, m_oConstVarStruct* pConstVar)
{
	ASSERT(pInstrumentList != NULL);
	ASSERT(pConstVar != NULL);
	// 清空SN仪器索引表
	pInstrumentList->m_oSNInstrumentMap.clear();
	// 清空IP地址仪器索引表
	pInstrumentList->m_oIPInstrumentMap.clear();
	// 清空空闲仪器队列
	pInstrumentList->m_olsInstrumentFree.clear();
	// 清空IP地址设置仪器索引表
	pInstrumentList->m_oIPSetInstrumentMap.clear();
	// 清空未完成ADC参数设置的仪器索引
	pInstrumentList->m_oADCSetInstrumentMap.clear();
	// 清空仪器位置索引表
	pInstrumentList->m_oInstrumentLocationMap.clear();
	// 清空丢帧索引
	pInstrumentList->m_oADCLostFrameMap.clear();
	// 清空参与施工采集的仪器索引
	pInstrumentList->m_oOptInstrumentMap.clear();

	// 仪器队列中仪器个数
	pInstrumentList->m_uiCountAll = pConstVar->m_iInstrumentNum;
	// 生成仪器数组
	if (pInstrumentList->m_pArrayInstrument != NULL)
	{
		delete[] pInstrumentList->m_pArrayInstrument;
		pInstrumentList->m_pArrayInstrument = NULL;
	}
	pInstrumentList->m_pArrayInstrument = new m_oInstrumentStruct[pInstrumentList->m_uiCountAll];
	// 空闲仪器数量
	pInstrumentList->m_uiCountFree = pInstrumentList->m_uiCountAll;
	for(unsigned int i = 0; i < pInstrumentList->m_uiCountAll; i++)
	{
		// 仪器在仪器数组中的位置
		pInstrumentList->m_pArrayInstrument[i].m_uiIndex = i;
		pInstrumentList->m_pArrayInstrument[i].m_uiIP = pConstVar->m_iIPSetAddrStart 
			+ i * pConstVar->m_iIPSetAddrInterval;
		pInstrumentList->m_pArrayInstrument[i].m_olsADCDataSave.resize(pConstVar->m_uiSaveTestDataNum);
		// 重置仪器
		OnInstrumentReset(&pInstrumentList->m_pArrayInstrument[i], pInstrumentList->m_bSetByHand);
		// 仪器加在空闲仪器队列尾部
		pInstrumentList->m_olsInstrumentFree.push_back(&pInstrumentList->m_pArrayInstrument[i]);
	}
	
}
// 关闭仪器队列结构体
void OnCloseInstrumentList(m_oInstrumentListStruct* pInstrumentList)
{
	ASSERT(pInstrumentList != NULL);
	// 清空SN仪器索引表
	pInstrumentList->m_oSNInstrumentMap.clear();
	// 清空IP地址仪器索引表
	pInstrumentList->m_oIPInstrumentMap.clear();
	// 清空空闲仪器队列
	pInstrumentList->m_olsInstrumentFree.clear();
	// 清空IP地址设置仪器索引表
	pInstrumentList->m_oIPSetInstrumentMap.clear();
	// 清空未完成ADC参数设置的仪器队列
	pInstrumentList->m_oADCSetInstrumentMap.clear();
	// 清空仪器位置索引表
	pInstrumentList->m_oInstrumentLocationMap.clear();
	// 清空丢帧索引
	pInstrumentList->m_oADCLostFrameMap.clear();
	// 清空参与施工采集的仪器索引
	pInstrumentList->m_oOptInstrumentMap.clear();
	// 删除仪器数组
	if (pInstrumentList->m_pArrayInstrument != NULL)
	{
		for(unsigned int i = 0; i < pInstrumentList->m_uiCountAll; i++)
		{
			// 仪器在仪器数组中的位置
			pInstrumentList->m_pArrayInstrument[i].m_olsADCDataSave.clear();
		}
		delete[] pInstrumentList->m_pArrayInstrument;
		pInstrumentList->m_pArrayInstrument = NULL;
	}
	
}
// 释放仪器队列结构体
void OnFreeInstrumentList(m_oInstrumentListStruct* pInstrumentList)
{
	ASSERT(pInstrumentList != NULL);
	delete pInstrumentList;
	pInstrumentList = NULL;
}
// 得到一个空闲仪器
m_oInstrumentStruct* GetFreeInstrument(m_oInstrumentListStruct* pInstrumentList)
{
	ASSERT(pInstrumentList != NULL);
	m_oInstrumentStruct* pInstrument = NULL;
	list <m_oInstrumentStruct*>::iterator iter;
	
	if(pInstrumentList->m_uiCountFree > 0)	//有空闲仪器
	{
		// 从空闲队列中得到一个仪器
		iter = pInstrumentList->m_olsInstrumentFree.begin();
		pInstrument = *iter;
		pInstrumentList->m_olsInstrumentFree.pop_front();	
		pInstrument->m_bInUsed = true;	// 设置仪器为使用中
		pInstrumentList->m_uiCountFree--;	// 空闲仪器总数减1
	}
	
	return pInstrument;
}
// 增加一个空闲仪器
void AddFreeInstrument(m_oInstrumentStruct* pInstrument, m_oInstrumentListStruct* pInstrumentList)
{
	ASSERT(pInstrument != NULL);
	ASSERT(pInstrumentList != NULL);
	//初始化仪器
	OnInstrumentReset(pInstrument, pInstrumentList->m_bSetByHand);
	//加入空闲队列
	pInstrumentList->m_olsInstrumentFree.push_back(pInstrument);
	pInstrumentList->m_uiCountFree++;	// 空闲仪器总数加1
	
}
// 更新上次测网系统变化时刻
void UpdateLineChangeTime(m_oLineListStruct* pLineList)
{
	ASSERT(pLineList != NULL);
	EnterCriticalSection(&pLineList->m_oSecLineList);
	// 上次测网系统变化时刻
	pLineList->m_uiLineChangeTime = GetTickCount();
	LeaveCriticalSection(&pLineList->m_oSecLineList);
}