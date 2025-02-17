#include "stdafx.h"
#include "MatrixServerDll.h"

// 初始化测线客户程序设置信息
void OnInitLineClientXMLSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	InitializeCriticalSection(&pLineSetupData->m_oSecCommInfo);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread = NULL;
	// 重置测线客户端信息
	OnResetLineClientXMLSetupData(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 创建测线客户端通讯信息结构体
m_oLineSetupDataStruct* OnCreateLineAppSetupData(void)
{
	m_oLineSetupDataStruct* pLineSetupData = NULL;
	pLineSetupData = new m_oLineSetupDataStruct;
	// 初始化测线客户程序设置信息
	OnInitLineClientXMLSetupData(pLineSetupData);
	return pLineSetupData;
}
// 打开测线客户端程序配置文件
BOOL OpenLineClientXMLFile(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strOLEObject;
	COleException oError;
	COleVariant oVariant;
	CString str = _T("");
	strOLEObject = _T("msxml2.domdocument");
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 初始化COM库
	CoInitialize(NULL);
	if (FALSE == pLineSetupData->m_oXMLDOMDocument.CreateDispatch(strOLEObject, &oError))
	{
		AfxMessageBox(_T("XML结构创建失败！"));
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
		return FALSE;
	}
	str = pLineSetupData->m_strLineXMLFilePath.c_str();
	oVariant = str;
	if (false == IfFileExist(str))
	{
		AfxMessageBox(_T("文件不存在"));
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
		return FALSE;
	}
	if (FALSE == pLineSetupData->m_oXMLDOMDocument.load(oVariant))
	{
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
		return FALSE;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	return TRUE;
}
// 关闭测线客户端程序配置文件
void CloseLineClientXMLFile(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	pLineSetupData->m_oXMLDOMDocument.DetachDispatch();
	// 释放COM库
	CoUninitialize();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Survery
void OnResetSurveryList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oSurveryStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Survery
	for (iter = pLineSetupData->m_olsSurveryStruct.begin(); 
		iter != pLineSetupData->m_olsSurveryStruct.end(); iter++)
	{
		if (iter->m_pcReceiverSection != NULL)
		{
			delete[] iter->m_pcReceiverSection;
			iter->m_pcReceiverSection = NULL;
		}
	}
	pLineSetupData->m_olsSurveryStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Point Code
void OnResetPointCodeList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oPointCodeStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Survery
	for (iter = pLineSetupData->m_olsPointCodeStruct.begin(); 
		iter != pLineSetupData->m_olsPointCodeStruct.end(); iter++)
	{
		if (iter->m_pcLabel != NULL)
		{
			delete[] iter->m_pcLabel;
			iter->m_pcLabel = NULL;
		}
		if (iter->m_pcSensorType != NULL)
		{
			delete[] iter->m_pcSensorType;
			iter->m_pcSensorType = NULL;
		}
	}
	pLineSetupData->m_olsPointCodeStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Sensor
void OnResetSensorList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oSensorStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Survery
	for (iter = pLineSetupData->m_olsSensorStruct.begin(); 
		iter != pLineSetupData->m_olsSensorStruct.end(); iter++)
	{
		if (iter->m_pcLabel != NULL)
		{
			delete[] iter->m_pcLabel;
			iter->m_pcLabel = NULL;
		}
	}
	pLineSetupData->m_olsSensorStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Marker
void OnResetMarkerList(m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Marker
	pLineSetupData->m_olsMarkerStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Aux
void OnResetAuxList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oAuxStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Aux
	for (iter = pLineSetupData->m_olsAuxStruct.begin(); 
		iter != pLineSetupData->m_olsAuxStruct.end(); iter++)
	{
		if (iter->m_pcLabel != NULL)
		{
			delete[] iter->m_pcLabel;
			iter->m_pcLabel = NULL;
		}
		if (iter->m_pcComments != NULL)
		{
			delete[] iter->m_pcComments;
			iter->m_pcComments = NULL;
		}
	}
	pLineSetupData->m_olsAuxStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Detour
void OnResetDetourList(m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Detour
	pLineSetupData->m_olsDetourStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Mute
void OnResetMuteList(m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Mute
	pLineSetupData->m_olsMuteStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置BlastMachine
void OnResetBlastMachineList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oBlastMachineStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// BlastMachine
	for (iter = pLineSetupData->m_olsBlastMachineStruct.begin(); 
		iter != pLineSetupData->m_olsBlastMachineStruct.end(); iter++)
	{
		if (iter->m_pcLabel != NULL)
		{
			delete[] iter->m_pcLabel;
			iter->m_pcLabel = NULL;
		}
		if (iter->m_pcComments != NULL)
		{
			delete[] iter->m_pcComments;
			iter->m_pcComments = NULL;
		}
	}
	pLineSetupData->m_olsBlastMachineStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Absolute
void OnResetAbsoluteMap(m_oLineSetupDataStruct* pLineSetupData)
{
	map<unsigned int, list<m_oAbsoluteStruct>>::iterator iterMap;
	list<m_oAbsoluteStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Absolute
	for (iterMap = pLineSetupData->m_oAbsoluteStructMap.begin();
		iterMap != pLineSetupData->m_oAbsoluteStructMap.end(); iterMap++)
	{
		for (iter = iterMap->second.begin(); iter != iterMap->second.end(); iter++)
		{
			if (iter->m_pcLabel != NULL)
			{
				delete[] iter->m_pcLabel;
				iter->m_pcLabel = NULL;
			}
			if (iter->m_pcAbsoluteSpread != NULL)
			{
				delete[] iter->m_pcAbsoluteSpread;
				iter->m_pcAbsoluteSpread = NULL;
			}
		}
		iterMap->second.clear();
	}
	pLineSetupData->m_oAbsoluteStructMap.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Generic
void OnResetGenericList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oGenericStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsGenericStruct.begin(); 
		iter != pLineSetupData->m_olsGenericStruct.end(); iter++)
	{
		if (iter->m_pcLabel != NULL)
		{
			delete[] iter->m_pcLabel;
			iter->m_pcLabel = NULL;
		}
		if (iter->m_pcLine != NULL)
		{
			delete[] iter->m_pcLine;
			iter->m_pcLine = NULL;
		}
		if (iter->m_pcSpread != NULL)
		{
			delete[] iter->m_pcSpread;
			iter->m_pcSpread = NULL;
		}
	}
	// Generic
	pLineSetupData->m_olsGenericStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置FormLine
void OnResetFormLineList(m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Form Line
	pLineSetupData->m_olsFormLineStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Instrument_SensorTestBase
void OnResetInstrument_SensorTestBaseList(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	list<Instrument_SensorTestBase_Struct>::iterator iter;
	list<Instrument_SensorTestBase_Struct>* pList = NULL;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	if (bInstrument == true)
	{
		pList = &pLineSetupData->m_olsInstrumentTestBaseStruct;
	}
	else
	{
		pList = &pLineSetupData->m_olsSensorTestBaseStruct;
	}
	for (iter = pList->begin(); iter != pList->end(); iter++)
	{
		if (iter->m_pcDescr != NULL)
		{
			delete[] iter->m_pcDescr;
			iter->m_pcDescr = NULL;
		}
	}
	pList->clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Instrument_SensorTestLimit
void OnResetInstrument_SensorTestLimitList(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	list<Instrument_SensorTestLimit_Struct>::iterator iter;
	list<Instrument_SensorTestLimit_Struct>* pList = NULL;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	if (bInstrument == true)
	{
		pList = &pLineSetupData->m_olsInstrumentTestLimitStruct;
	}
	else
	{
		pList = &pLineSetupData->m_olsSensorTestLimitStruct;
	}
	for (iter = pList->begin(); iter != pList->end(); iter++)
	{
		if (iter->m_pcDescr != NULL)
		{
			delete[] iter->m_pcDescr;
			iter->m_pcDescr = NULL;
		}
		if (iter->m_pcUnit != NULL)
		{
			delete[] iter->m_pcUnit;
			iter->m_pcUnit = NULL;
		}
	}
	pList->clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Instrument Test
void OnResetInstrumentTestList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oInstrumentTestStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsInstrumentTestStruct.begin(); 
		iter != pLineSetupData->m_olsInstrumentTestStruct.end(); iter++)
	{
		if (iter->m_pcAbsoluteSpread != NULL)
		{
			delete[] iter->m_pcAbsoluteSpread;
			iter->m_pcAbsoluteSpread = NULL;
		}
		if (iter->m_pcAuxiliaryDescr != NULL)
		{
			delete[] iter->m_pcAuxiliaryDescr;
			iter->m_pcAuxiliaryDescr = NULL;
		}
	}
	// Instrument Test
	pLineSetupData->m_olsInstrumentTestStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Sensor Test
void OnResetSensorTestList(m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oSensorTestStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsSensorTestStruct.begin(); 
		iter != pLineSetupData->m_olsSensorTestStruct.end(); iter++)
	{
		if (iter->m_pcAbsoluteSpread != NULL)
		{
			delete[] iter->m_pcAbsoluteSpread;
			iter->m_pcAbsoluteSpread = NULL;
		}
	}
	// Sensor Test
	pLineSetupData->m_olsSensorTestStruct.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置Multiple Test
void OnResetMultipleTestMap(m_oLineSetupDataStruct* pLineSetupData)
{
	map<m_oMultipleTestKeyStruct, list<m_oMultipleTestTaskStruct>>::iterator iterMap;
	list<m_oMultipleTestTaskStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// Absolute
	for (iterMap = pLineSetupData->m_oMultpleTestStructMap.begin();
		iterMap != pLineSetupData->m_oMultpleTestStructMap.end(); iterMap++)
	{
		if (iterMap->first.m_pcAbsoluteSpread != NULL)
		{
			delete[] iterMap->first.m_pcAbsoluteSpread;
		}
		if (iterMap->first.m_pcAuxiliaryDescr != NULL)
		{
			delete[] iterMap->first.m_pcAuxiliaryDescr;
		}
		if (iterMap->first.m_pcTestName != NULL)
		{
			delete[] iterMap->first.m_pcTestName;
		}
		iterMap->second.clear();
	}
	pLineSetupData->m_oMultpleTestStructMap.clear();
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置SeisMonitor
void OnResetSeisMonitor(m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// SeisMonitor
	if (pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread != NULL)
	{
		delete[] pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread;
		pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread = NULL;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 重置测线客户端信息
void OnResetLineClientXMLSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	// 重置Survery
	OnResetSurveryList(pLineSetupData);
	// 重置Point Code
	OnResetPointCodeList(pLineSetupData);
	// 重置Sensor
	OnResetSensorList(pLineSetupData);
	// 重置Marker
	OnResetMarkerList(pLineSetupData);
	// 重置Aux
	OnResetAuxList(pLineSetupData);
	// 重置Detour
	OnResetDetourList(pLineSetupData);
	// 重置Mute
	OnResetMuteList(pLineSetupData);
	// 重置BlastMachine
	OnResetBlastMachineList(pLineSetupData);
	// 重置Absolute
	OnResetAbsoluteMap(pLineSetupData);
	// 重置Generic
	OnResetGenericList(pLineSetupData);
	// 重置FormLine
	OnResetFormLineList(pLineSetupData);
	// 重置Instrument Test base
	OnResetInstrument_SensorTestBaseList(true, pLineSetupData);
	// 重置Sensor Test base
	OnResetInstrument_SensorTestBaseList(false, pLineSetupData);
	// 重置Instrument Limit
	OnResetInstrument_SensorTestLimitList(true, pLineSetupData);
	// 重置Sensor Limit
	OnResetInstrument_SensorTestLimitList(false, pLineSetupData);
	// 重置Instrument Test
	OnResetInstrumentTestList(pLineSetupData);
	// 重置Sensor Test
	OnResetSensorTestList(pLineSetupData);
	// 重置Multiple Test
	OnResetMultipleTestMap(pLineSetupData);
	// 重置SeisMonitor
	OnResetSeisMonitor(pLineSetupData);
}

// 加载Survery设置数据
void LoadSurvery(m_oSurveryStruct* pSurveryStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "LineName";
		pSurveryStruct->m_uiLine = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "ReceiverSection";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pSurveryStruct->m_usReceiverSectionSize = (unsigned short)(strConv.size() + 1);
		pSurveryStruct->m_pcReceiverSection = new char[pSurveryStruct->m_usReceiverSectionSize];
//		pSurveryStruct->m_pcReceiverSection = (char*)strConv.data();
		memcpy(pSurveryStruct->m_pcReceiverSection, strConv.c_str(), pSurveryStruct->m_usReceiverSectionSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Survery设置队列数据
void LoadSurveryList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Survery设置区
		strKey = "SurverySetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Survery总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oSurveryStruct oSurveryStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadSurvery(&oSurveryStruct, &oElement);
			// 增加Survery
			pLineSetupData->m_olsSurveryStruct.push_back(oSurveryStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Survery设置数据
void LoadSurverySetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Survery
	OnResetSurveryList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Survery设置队列数据
		LoadSurveryList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Survery设置数据
void SaveSurvery(m_oSurveryStruct* pSurveryStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "LineName";
		oVariant = (long)pSurveryStruct->m_uiLine;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ReceiverSection";
		strConv = pSurveryStruct->m_pcReceiverSection;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Survery设置队列数据
void SaveSurveryList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oSurveryStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Survery设置区
		strKey = "SurverySetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Survery总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsSurveryStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsSurveryStruct.begin();
			iter != pLineSetupData->m_olsSurveryStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveSurvery(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Survery设置数据
void SaveSurverySetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Survery设置队列数据
		SaveSurveryList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Survery设置数据
void SetSurverySetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oSurveryStruct oSurveryStruct;
	unsigned int uiPos = 0;
	OnResetSurveryList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oSurveryStruct.m_uiLine, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSurveryStruct.m_usReceiverSectionSize, &pChar[uiPos], 2);
		uiPos += 2;
		oSurveryStruct.m_pcReceiverSection = new char[oSurveryStruct.m_usReceiverSectionSize];
		memcpy(oSurveryStruct.m_pcReceiverSection, &pChar[uiPos], oSurveryStruct.m_usReceiverSectionSize);
		uiPos += oSurveryStruct.m_usReceiverSectionSize;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsSurveryStruct.push_back(oSurveryStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveSurverySetupData(pLineSetupData);
	}
}
// 加载Point Code设置数据
void LoadPointCode(m_oPointCodeStruct* pPointCodeStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		pPointCodeStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Label";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pPointCodeStruct->m_usLabelSize = (unsigned short)(strConv.size() + 1);
		pPointCodeStruct->m_pcLabel = new char[pPointCodeStruct->m_usLabelSize];
		memcpy(pPointCodeStruct->m_pcLabel, strConv.c_str(), pPointCodeStruct->m_usLabelSize);
		strKey = "SensorType";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pPointCodeStruct->m_usSensorTypeSize = (unsigned short)(strConv.size() + 1);
		pPointCodeStruct->m_pcSensorType = new char[pPointCodeStruct->m_usSensorTypeSize];
		memcpy(pPointCodeStruct->m_pcSensorType, strConv.c_str(), pPointCodeStruct->m_usSensorTypeSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Point Code设置队列数据
void LoadPointCodeList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到PointCode设置区
		strKey = "PointCodeSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到PointCode总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oPointCodeStruct oPointCodeStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadPointCode(&oPointCodeStruct, &oElement);
			// 增加PointCode
			pLineSetupData->m_olsPointCodeStruct.push_back(oPointCodeStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Point Code设置数据
void LoadPointCodeSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Point Code
	OnResetPointCodeList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载PointCode设置队列数据
		LoadPointCodeList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Point Code设置数据
void SavePointCode(m_oPointCodeStruct* pPointCodeStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		oVariant = (long)pPointCodeStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Label";
		strConv = pPointCodeStruct->m_pcLabel;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SensorType";
		strConv = pPointCodeStruct->m_pcSensorType;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Point Code设置队列数据
void SavePointCodeList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oPointCodeStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到PointCode设置区
		strKey = "PointCodeSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置PointCode总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsPointCodeStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsPointCodeStruct.begin();
			iter != pLineSetupData->m_olsPointCodeStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SavePointCode(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Point Code设置数据
void SavePointCodeSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Point Code设置队列数据
		SavePointCodeList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Point Code设置数据
void SetPointCodeSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oPointCodeStruct oPointCodeStruct;
	unsigned int uiPos = 0;
	OnResetPointCodeList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oPointCodeStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oPointCodeStruct.m_usLabelSize, &pChar[uiPos], 2);
		uiPos += 2;
		oPointCodeStruct.m_pcLabel = new char[oPointCodeStruct.m_usLabelSize];
		memcpy(oPointCodeStruct.m_pcLabel, &pChar[uiPos], oPointCodeStruct.m_usLabelSize);
		uiPos += oPointCodeStruct.m_usLabelSize;
		memcpy(&oPointCodeStruct.m_usSensorTypeSize, &pChar[uiPos], 2);
		uiPos += 2;
		oPointCodeStruct.m_pcSensorType = new char[oPointCodeStruct.m_usSensorTypeSize];
		memcpy(oPointCodeStruct.m_pcSensorType, &pChar[uiPos], oPointCodeStruct.m_usSensorTypeSize);
		uiPos += oPointCodeStruct.m_usSensorTypeSize;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsPointCodeStruct.push_back(oPointCodeStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SavePointCodeSetupData(pLineSetupData);
	}
}
// 加载Sensor设置数据
void LoadSensor(m_oSensorStruct* pSensorStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		pSensorStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Label";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pSensorStruct->m_usLabelSize = (unsigned short)(strConv.size() + 1);
		pSensorStruct->m_pcLabel = new char[pSensorStruct->m_usLabelSize];
		memcpy(pSensorStruct->m_pcLabel, strConv.c_str(), pSensorStruct->m_usLabelSize);
		strKey = "ContinuityMin";
		pSensorStruct->m_fContinuityMin = CXMLDOMTool::GetElementAttributeFloat(pElement, strKey);
		strKey = "ContinuityMax";
		pSensorStruct->m_fContinuityMax = CXMLDOMTool::GetElementAttributeFloat(pElement, strKey);
		strKey = "Tilt";
		pSensorStruct->m_fTilt = CXMLDOMTool::GetElementAttributeFloat(pElement, strKey);
		strKey = "Noise";
		pSensorStruct->m_fNoise = CXMLDOMTool::GetElementAttributeFloat(pElement, strKey);
		strKey = "Leakage";
		pSensorStruct->m_fLeakage = CXMLDOMTool::GetElementAttributeFloat(pElement, strKey);
		strKey = "SEGDCode";
		pSensorStruct->m_uiSEGDCode = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Sensor设置队列数据
void LoadSensorList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Sensor设置区
		strKey = "SensorSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Sensor总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oSensorStruct oSensorStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadSensor(&oSensorStruct, &oElement);
			// 增加Sensor
			pLineSetupData->m_olsSensorStruct.push_back(oSensorStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Sensor设置数据
void LoadSensorSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Sensor
	OnResetSensorList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Sensor设置队列数据
		LoadSensorList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Sensor设置数据
void SaveSensor(m_oSensorStruct* pSensorStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		oVariant = (long)pSensorStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SEGDCode";
		oVariant = (long)pSensorStruct->m_uiSEGDCode;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Label";
		strConv = pSensorStruct->m_pcLabel;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ContinuityMin";
		oVariant = (long)pSensorStruct->m_fContinuityMin;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ContinuityMax";
		oVariant = (long)pSensorStruct->m_fContinuityMax;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Tilt";
		oVariant = (long)pSensorStruct->m_fTilt;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Noise";
		oVariant = (long)pSensorStruct->m_fNoise;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Leakage";
		oVariant = (long)pSensorStruct->m_fLeakage;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Sensor设置队列数据
void SaveSensorList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oSensorStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Sensor设置区
		strKey = "SensorSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Sensor总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsSensorStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsSensorStruct.begin();
			iter != pLineSetupData->m_olsSensorStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveSensor(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Sensor设置数据
void SaveSensorSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Sensor设置队列数据
		SaveSensorList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Sensor设置数据
void SetSensorSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oSensorStruct oSensorStruct;
	unsigned int uiPos = 0;
	OnResetSensorList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oSensorStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorStruct.m_usLabelSize, &pChar[uiPos], 2);
		uiPos += 2;
		oSensorStruct.m_pcLabel = new char[oSensorStruct.m_usLabelSize];
		memcpy(oSensorStruct.m_pcLabel, &pChar[uiPos], oSensorStruct.m_usLabelSize);
		uiPos += oSensorStruct.m_usLabelSize;
		memcpy(&oSensorStruct.m_fContinuityMin, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorStruct.m_fContinuityMax, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorStruct.m_fTilt, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorStruct.m_fNoise, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorStruct.m_fLeakage, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorStruct.m_uiSEGDCode, &pChar[uiPos], 4);
		uiPos += 4;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsSensorStruct.push_back(oSensorStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveSensorSetupData(pLineSetupData);
	}
}
// 加载Marker设置数据
void LoadMarker(m_oMarkerStruct* pMarkerStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "BoxType";
		pMarkerStruct->m_uiBoxType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SN";
		pMarkerStruct->m_uiSN = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "LineName";
		pMarkerStruct->m_uiLineName = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "PointNb";
		pMarkerStruct->m_uiPointNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "ChannelNb";
		pMarkerStruct->m_uiChannelNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "MarkerIncr";
		pMarkerStruct->m_uiMarkerIncr = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Reversed";
		pMarkerStruct->m_uiReversed = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Marker设置队列数据
void LoadMarkerList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Marker设置区
		strKey = "MarkerSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Marker总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oMarkerStruct oMarkerStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadMarker(&oMarkerStruct, &oElement);
			// 增加Marker
			pLineSetupData->m_olsMarkerStruct.push_back(oMarkerStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Marker设置数据
void LoadMarkerSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Marker
	OnResetMarkerList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Marker设置队列数据
		LoadMarkerList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Marker设置数据
void SaveMarker(m_oMarkerStruct* pMarkerStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "BoxType";
		oVariant = (long)pMarkerStruct->m_uiBoxType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SN";
		oVariant = (long)pMarkerStruct->m_uiSN;
		pElement->setAttribute(strKey, oVariant);
		strKey = "LineName";
		oVariant = (long)pMarkerStruct->m_uiLineName;
		pElement->setAttribute(strKey, oVariant);
		strKey = "PointNb";
		oVariant = (long)pMarkerStruct->m_uiPointNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ChannelNb";
		oVariant = (long)pMarkerStruct->m_uiChannelNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "MarkerIncr";
		oVariant = (long)pMarkerStruct->m_uiMarkerIncr;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Reversed";
		oVariant = (long)pMarkerStruct->m_uiReversed;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Marker设置队列数据
void SaveMarkerList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oMarkerStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Marker设置区
		strKey = "MarkerSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Marker总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsMarkerStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsMarkerStruct.begin();
			iter != pLineSetupData->m_olsMarkerStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveMarker(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Marker设置数据
void SaveMarkerSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Marker设置队列数据
		SaveMarkerList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Marker设置数据
void SetMarkerSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oMarkerStruct oMarkerStruct;
	unsigned int uiPos = 0;
	OnResetMarkerList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oMarkerStruct.m_uiBoxType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMarkerStruct.m_uiSN, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMarkerStruct.m_uiLineName, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMarkerStruct.m_uiPointNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMarkerStruct.m_uiChannelNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMarkerStruct.m_uiMarkerIncr, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMarkerStruct.m_uiReversed, &pChar[uiPos], 4);
		uiPos += 4;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsMarkerStruct.push_back(oMarkerStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveMarkerSetupData(pLineSetupData);
	}
}
// 加载Aux设置数据
void LoadAux(m_oAuxStruct* pAuxStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		pAuxStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Label";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pAuxStruct->m_usLabelSize = (unsigned short)(strConv.size() + 1);
		pAuxStruct->m_pcLabel = new char[pAuxStruct->m_usLabelSize];
		memcpy(pAuxStruct->m_pcLabel, strConv.c_str(), pAuxStruct->m_usLabelSize);
		strKey = "BoxType";
		pAuxStruct->m_uiBoxType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SN";
		pAuxStruct->m_uiSN = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "ChannelNb";
		pAuxStruct->m_uiChannelNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Gain";
		pAuxStruct->m_uiGain = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "DPGNb";
		pAuxStruct->m_uiDpgNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Comments";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pAuxStruct->m_usCommentsSize = (unsigned short)(strConv.size() + 1);
		pAuxStruct->m_pcComments = new char[pAuxStruct->m_usCommentsSize];
		memcpy(pAuxStruct->m_pcComments, strConv.c_str(), pAuxStruct->m_usCommentsSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Aux设置队列数据
void LoadAuxList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Aux设置区
		strKey = "AuxSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Aux总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oAuxStruct oAuxStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadAux(&oAuxStruct, &oElement);
			// 增加Aux
			pLineSetupData->m_olsAuxStruct.push_back(oAuxStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Aux设置数据
void LoadAuxSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Aux
	OnResetAuxList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Aux设置队列数据
		LoadAuxList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Aux设置数据
void SaveAux(m_oAuxStruct* pAuxStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		oVariant = (long)pAuxStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Label";
		strConv = pAuxStruct->m_pcLabel;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "BoxType";
		oVariant = (long)pAuxStruct->m_uiBoxType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SN";
		oVariant = (long)pAuxStruct->m_uiSN;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ChannelNb";
		oVariant = (long)pAuxStruct->m_uiChannelNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Gain";
		oVariant = (long)pAuxStruct->m_uiGain;
		pElement->setAttribute(strKey, oVariant);
		strKey = "DPGNb";
		oVariant = (long)pAuxStruct->m_uiDpgNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Comments";
		strConv = pAuxStruct->m_pcComments;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Aux设置队列数据
void SaveAuxList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oAuxStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Aux设置区
		strKey = "AuxSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Aux总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsAuxStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsAuxStruct.begin();
			iter != pLineSetupData->m_olsAuxStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveAux(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Aux设置数据
void SaveAuxSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Aux设置队列数据
		SaveAuxList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Aux设置数据
void SetAuxSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oAuxStruct oAuxStruct;
	unsigned int uiPos = 0;
	OnResetAuxList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oAuxStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oAuxStruct.m_usLabelSize, &pChar[uiPos], 2);
		uiPos += 2;
		oAuxStruct.m_pcLabel = new char[oAuxStruct.m_usLabelSize];
		memcpy(oAuxStruct.m_pcLabel, &pChar[uiPos], oAuxStruct.m_usLabelSize);
		uiPos += oAuxStruct.m_usLabelSize;
		memcpy(&oAuxStruct.m_uiBoxType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oAuxStruct.m_uiSN, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oAuxStruct.m_uiChannelNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oAuxStruct.m_uiGain, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oAuxStruct.m_uiDpgNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oAuxStruct.m_usCommentsSize, &pChar[uiPos], 2);
		uiPos += 2;
		oAuxStruct.m_pcComments = new char[oAuxStruct.m_usCommentsSize];
		memcpy(oAuxStruct.m_pcComments, &pChar[uiPos], oAuxStruct.m_usCommentsSize);
		uiPos += oAuxStruct.m_usCommentsSize;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsAuxStruct.push_back(oAuxStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveAuxSetupData(pLineSetupData);
	}
}
// 加载Detour设置数据
void LoadDetour(m_oDetourStruct* pDetourStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "BoxTypeLow";
		pDetourStruct->m_uiLowBoxType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SNLow";
		pDetourStruct->m_uiLowSN = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "ChannelNbLow";
		pDetourStruct->m_uiLowChanNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "BoxTypeHigh";
		pDetourStruct->m_uiHighBoxType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SNHigh";
		pDetourStruct->m_uiHighSN = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "ChannelNbHigh";
		pDetourStruct->m_uiHighChanNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "StopMarking";
		pDetourStruct->m_uiStopMarking = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Detour设置队列数据
void LoadDetourList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Detour设置区
		strKey = "DetourSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Aux总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oDetourStruct oDetourStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadDetour(&oDetourStruct, &oElement);
			// 增加Aux
			pLineSetupData->m_olsDetourStruct.push_back(oDetourStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Detour设置数据
void LoadDetourSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Detour
	OnResetDetourList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Detour设置队列数据
		LoadDetourList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Detour设置数据
void SaveDetour(m_oDetourStruct* pDetourStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "BoxTypeLow";
		oVariant = (long)pDetourStruct->m_uiLowBoxType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SNLow";
		oVariant = (long)pDetourStruct->m_uiLowSN;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ChannelNbLow";
		oVariant = (long)pDetourStruct->m_uiLowChanNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "BoxTypeHigh";
		oVariant = (long)pDetourStruct->m_uiHighBoxType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SNHigh";
		oVariant = (long)pDetourStruct->m_uiHighSN;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ChannelNbHigh";
		oVariant = (long)pDetourStruct->m_uiHighChanNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "StopMarking";
		oVariant = (long)pDetourStruct->m_uiStopMarking;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Detour设置队列数据
void SaveDetourList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oDetourStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Detour设置区
		strKey = "DetourSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Aux总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsDetourStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsDetourStruct.begin();
			iter != pLineSetupData->m_olsDetourStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveDetour(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Detour设置数据
void SaveDetourSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Detour设置队列数据
		SaveDetourList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Detour设置数据
void SetDetourSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oDetourStruct oDetourStruct;
	unsigned int uiPos = 0;
	OnResetDetourList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oDetourStruct.m_uiLowBoxType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oDetourStruct.m_uiLowSN, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oDetourStruct.m_uiLowChanNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oDetourStruct.m_uiHighBoxType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oDetourStruct.m_uiHighSN, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oDetourStruct.m_uiHighChanNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oDetourStruct.m_uiStopMarking, &pChar[uiPos], 4);
		uiPos += 4;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsDetourStruct.push_back(oDetourStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveDetourSetupData(pLineSetupData);
	}
}
// 加载Mute设置数据
void LoadMute(m_oMuteStruct* pMuteStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "LineName";
		pMuteStruct->m_uiLineName = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "PointNb";
		pMuteStruct->m_uiPointNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Mute设置队列数据
void LoadMuteList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Mute设置区
		strKey = "MuteSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Mute总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oMuteStruct oMuteStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadMute(&oMuteStruct, &oElement);
			// 增加Mute
			pLineSetupData->m_olsMuteStruct.push_back(oMuteStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Mute设置数据
void LoadMuteSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Mute
	OnResetMuteList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Mute设置队列数据
		LoadMuteList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Mute设置数据
void SaveMute(m_oMuteStruct* pMuteStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "LineName";
		oVariant = (long)pMuteStruct->m_uiLineName;
		pElement->setAttribute(strKey, oVariant);
		strKey = "PointNb";
		oVariant = (long)pMuteStruct->m_uiPointNb;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Mute设置队列数据
void SaveMuteList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oMuteStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Mute设置区
		strKey = "MuteSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Mute总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsMuteStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsMuteStruct.begin();
			iter != pLineSetupData->m_olsMuteStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveMute(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Mute设置数据
void SaveMuteSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Mute设置队列数据
		SaveMuteList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Mute设置数据
void SetMuteSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oMuteStruct oMuteStruct;
	unsigned int uiPos = 0;
	OnResetMuteList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oMuteStruct.m_uiLineName, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMuteStruct.m_uiPointNb, &pChar[uiPos], 4);
		uiPos += 4;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsMuteStruct.push_back(oMuteStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveMuteSetupData(pLineSetupData);
	}
}
// 加载BlastMachine设置数据
void LoadBlastMachine(m_oBlastMachineStruct* pBlastMachineStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		pBlastMachineStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Label";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pBlastMachineStruct->m_usLabelSize = (unsigned short)(strConv.size() + 1);
		pBlastMachineStruct->m_pcLabel = new char[pBlastMachineStruct->m_usLabelSize];
		memcpy(pBlastMachineStruct->m_pcLabel, strConv.c_str(), pBlastMachineStruct->m_usLabelSize);
		strKey = "BoxType";
		pBlastMachineStruct->m_uiBoxType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SN";
		pBlastMachineStruct->m_uiSN = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "ChannelNb";
		pBlastMachineStruct->m_uiChannelNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Gain";
		pBlastMachineStruct->m_uiGain = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "DPGNb";
		pBlastMachineStruct->m_uiDpgNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Comments";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pBlastMachineStruct->m_usCommentsSize = (unsigned short)(strConv.size() + 1);
		pBlastMachineStruct->m_pcComments = new char[pBlastMachineStruct->m_usCommentsSize];
		memcpy(pBlastMachineStruct->m_pcComments, strConv.c_str(), pBlastMachineStruct->m_usCommentsSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载BlastMachine设置队列数据
void LoadBlastMachineList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到BlastMachine设置区
		strKey = "BlastMachineSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到BlastMachine总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oBlastMachineStruct oBlastMachineStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadBlastMachine(&oBlastMachineStruct, &oElement);
			// 增加BlastMachine
			pLineSetupData->m_olsBlastMachineStruct.push_back(oBlastMachineStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载BlastMachine设置数据
void LoadBlastMachineSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置BlastMachine
	OnResetBlastMachineList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载BlastMachine设置队列数据
		LoadBlastMachineList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存BlastMachine设置数据
void SaveBlastMachine(m_oBlastMachineStruct* pBlastMachineStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		oVariant = (long)pBlastMachineStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Label";
		strConv = pBlastMachineStruct->m_pcLabel;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "BoxType";
		oVariant = (long)pBlastMachineStruct->m_uiBoxType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SN";
		oVariant = (long)pBlastMachineStruct->m_uiSN;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ChannelNb";
		oVariant = (long)pBlastMachineStruct->m_uiChannelNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Gain";
		oVariant = (long)pBlastMachineStruct->m_uiGain;
		pElement->setAttribute(strKey, oVariant);
		strKey = "DPGNb";
		oVariant = (long)pBlastMachineStruct->m_uiDpgNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Comments";
		strConv = pBlastMachineStruct->m_pcComments;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存BlastMachine设置队列数据
void SaveBlastMachineList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oBlastMachineStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到BlastMachine设置区
		strKey = "BlastMachineSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置BlastMachine总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsBlastMachineStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsBlastMachineStruct.begin();
			iter != pLineSetupData->m_olsBlastMachineStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveBlastMachine(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存BlastMachine设置数据
void SaveBlastMachineSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存BlastMachine设置队列数据
		SaveBlastMachineList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置BlastMachine设置数据
void SetBlastMachineSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oBlastMachineStruct oBlastMachineStruct;
	unsigned int uiPos = 0;
	OnResetBlastMachineList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oBlastMachineStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oBlastMachineStruct.m_usLabelSize, &pChar[uiPos], 2);
		uiPos += 2;
		oBlastMachineStruct.m_pcLabel = new char[oBlastMachineStruct.m_usLabelSize];
		memcpy(oBlastMachineStruct.m_pcLabel, &pChar[uiPos], oBlastMachineStruct.m_usLabelSize);
		uiPos += oBlastMachineStruct.m_usLabelSize;
		memcpy(&oBlastMachineStruct.m_uiBoxType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oBlastMachineStruct.m_uiSN, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oBlastMachineStruct.m_uiChannelNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oBlastMachineStruct.m_uiGain, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oBlastMachineStruct.m_uiDpgNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oBlastMachineStruct.m_usCommentsSize, &pChar[uiPos], 2);
		uiPos += 2;
		oBlastMachineStruct.m_pcComments = new char[oBlastMachineStruct.m_usCommentsSize];
		memcpy(oBlastMachineStruct.m_pcComments, &pChar[uiPos], oBlastMachineStruct.m_usCommentsSize);
		uiPos += oBlastMachineStruct.m_usCommentsSize;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsBlastMachineStruct.push_back(oBlastMachineStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveBlastMachineSetupData(pLineSetupData);
	}
}
// 加载Absolute设置数据
void LoadAbsolute(m_oAbsoluteStruct* pAbsoluteStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		pAbsoluteStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Label";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pAbsoluteStruct->m_usLabelSize = (unsigned short)(strConv.size() + 1);
		pAbsoluteStruct->m_pcLabel = new char[pAbsoluteStruct->m_usLabelSize];
		memcpy(pAbsoluteStruct->m_pcLabel, strConv.c_str(), pAbsoluteStruct->m_usLabelSize);
		strKey = "Spread";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pAbsoluteStruct->m_usAbsoluteSpreadSize = (unsigned short)(strConv.size() + 1);
		pAbsoluteStruct->m_pcAbsoluteSpread = new char[pAbsoluteStruct->m_usAbsoluteSpreadSize];
		memcpy(pAbsoluteStruct->m_pcAbsoluteSpread, strConv.c_str(), pAbsoluteStruct->m_usAbsoluteSpreadSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Absolute设置队列数据
void LoadAbsoluteList(m_oLineSetupDataStruct* pLineSetupData, CXMLDOMElement* pElement)
{
	ASSERT(pLineSetupData != NULL);
	ASSERT(pElement != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll = 0;
	unsigned int uiShotPoint = 0;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 得到Absolute总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		// 得到ShotPoint号
		strKey = "ShotPoint";
		uiShotPoint = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);

		// 得到队列
		lpDispatch = pElement->get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oAbsoluteStruct oAbsoluteStruct;
		list<m_oAbsoluteStruct> olsAbsoluteStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			pElement->AttachDispatch(lpDispatch);
			LoadAbsolute(&oAbsoluteStruct, pElement);
			// 增加Absolute
			olsAbsoluteStruct.push_back(oAbsoluteStruct);
		}
		pLineSetupData->m_oAbsoluteStructMap.insert(
			map<unsigned int, list<m_oAbsoluteStruct>>::value_type (uiShotPoint, olsAbsoluteStruct));
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Absolute设置索引数据
void LoadAbsoluteMap(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Absolute设置区
		strKey = "AbsoluteSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Absolute总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadAbsoluteList(pLineSetupData, &oElement);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Absolute设置数据
void LoadAbsoluteSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Absolute
	OnResetAbsoluteMap(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Absolute设置队列数据
		LoadAbsoluteMap(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Absolute设置数据
void SaveAbsolute(m_oAbsoluteStruct* pAbsoluteStruct, CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		oVariant = (long)pAbsoluteStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Label";
		strConv = pAbsoluteStruct->m_pcLabel;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Spread";
		strConv = pAbsoluteStruct->m_pcAbsoluteSpread;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Absolute设置队列数据
void SaveAbsoluteList(m_oLineSetupDataStruct* pLineSetupData, CXMLDOMElement* pElement, 
	map<unsigned int, list<m_oAbsoluteStruct>>::iterator iter, unsigned int uiTabCount)
{
	ASSERT(pLineSetupData != NULL);
	ASSERT(pElement != NULL);
	CString strKey;
	string strConv = "";
	CString str = _T("");
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	LPDISPATCH lpDispatch;
	CXMLDOMElement oElementChild;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oAbsoluteStruct>::iterator iterList;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		strKey = "Count";
		oVariant = (long)iter->second.size();
		pElement->setAttribute(strKey, oVariant);
		strKey = "TabCount";
		oVariant = (long)uiTabCount;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ShotPoint";
		oVariant = (long)iter->first;
		pElement->setAttribute(strKey, oVariant);

		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		for (iterList = iter->second.begin(); iterList != iter->second.end(); iterList++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			pElement->appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveAbsolute(&(*iterList), &oElementChild);
			pElement->appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		pElement->appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Absolute设置索引数据
void SaveAbsoluteMap(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	CString str = _T("");
	int iCount = 0;
	map<unsigned int, list<m_oAbsoluteStruct>>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Absolute设置区
		strKey = "AbsoluteSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);
		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Absolute总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_oAbsoluteStructMap.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_oAbsoluteStructMap.begin();
			iter != pLineSetupData->m_oAbsoluteStructMap.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			iCount++;
			str.Format(_T("AbsoluteSetup%d"), iCount);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(str);
			oElementChild.AttachDispatch(lpDispatch);
			SaveAbsoluteList(pLineSetupData, &oElementChild, iter, uiTabCount + 1);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Absolute设置数据
void SaveAbsoluteSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Absolute设置索引数据
		SaveAbsoluteMap(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Absolute设置数据
void SetAbsoluteSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oAbsoluteStruct oAbsoluteStruct;
	list<m_oAbsoluteStruct> olsAbsoluteStruct;
	int iCount = 0;
	unsigned int uiShotPoint = 0;
	unsigned int uiPos = 0;
	OnResetAbsoluteMap(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&uiShotPoint, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&iCount, &pChar[uiPos], 4);
		uiPos += 4;
		olsAbsoluteStruct.clear();
		for (int i=0; i<iCount; i++)
		{
			memcpy(&oAbsoluteStruct.m_uiNb, &pChar[uiPos], 4);
			uiPos += 4;
			memcpy(&oAbsoluteStruct.m_usLabelSize, &pChar[uiPos], 2);
			uiPos += 2;
			oAbsoluteStruct.m_pcLabel = new char[oAbsoluteStruct.m_usLabelSize];
			memcpy(oAbsoluteStruct.m_pcLabel, &pChar[uiPos], oAbsoluteStruct.m_usLabelSize);
			uiPos += oAbsoluteStruct.m_usLabelSize;
			memcpy(&oAbsoluteStruct.m_usAbsoluteSpreadSize, &pChar[uiPos], 2);
			uiPos += 2;
			oAbsoluteStruct.m_pcAbsoluteSpread = new char[oAbsoluteStruct.m_usAbsoluteSpreadSize];
			memcpy(oAbsoluteStruct.m_pcAbsoluteSpread, &pChar[uiPos], oAbsoluteStruct.m_usAbsoluteSpreadSize);
			uiPos += oAbsoluteStruct.m_usAbsoluteSpreadSize;
			olsAbsoluteStruct.push_back(oAbsoluteStruct);
		}
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_oAbsoluteStructMap.insert(
			map<unsigned int, list<m_oAbsoluteStruct>>::value_type (uiShotPoint, olsAbsoluteStruct));
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveAbsoluteSetupData(pLineSetupData);
	}
}
// 加载Generic设置数据
void LoadGeneric(m_oGenericStruct* pGenericStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		pGenericStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Label";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pGenericStruct->m_usLabelSize = (unsigned short)(strConv.size() + 1);
		pGenericStruct->m_pcLabel = new char[pGenericStruct->m_usLabelSize];
		memcpy(pGenericStruct->m_pcLabel, strConv.c_str(), pGenericStruct->m_usLabelSize);
		strKey = "Line";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pGenericStruct->m_usLineSize = (unsigned short)(strConv.size() + 1);
		pGenericStruct->m_pcLine = new char[pGenericStruct->m_usLineSize];
		memcpy(pGenericStruct->m_pcLine, strConv.c_str(), pGenericStruct->m_usLineSize);
		strKey = "Spread";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pGenericStruct->m_usSpreadSize = (unsigned short)(strConv.size() + 1);
		pGenericStruct->m_pcSpread = new char[pGenericStruct->m_usSpreadSize];
		memcpy(pGenericStruct->m_pcSpread, strConv.c_str(), pGenericStruct->m_usSpreadSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Generic设置队列数据
void LoadGenericList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Generic设置区
		strKey = "GenericSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到BlastMachine总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oGenericStruct oGenericStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadGeneric(&oGenericStruct, &oElement);
			// 增加Generic
			pLineSetupData->m_olsGenericStruct.push_back(oGenericStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Generic设置数据
void LoadGenericSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Generic
	OnResetGenericList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Generic设置队列数据
		LoadGenericList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Generic设置数据
void SaveGeneric(m_oGenericStruct* pGenericStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		oVariant = (long)pGenericStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Label";
		strConv = pGenericStruct->m_pcLabel;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Line";
		strConv = pGenericStruct->m_pcLine;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Spread";
		strConv = pGenericStruct->m_pcSpread;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Generic设置队列数据
void SaveGenericList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oGenericStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Generic设置区
		strKey = "GenericSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置BlastMachine总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsGenericStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsGenericStruct.begin();
			iter != pLineSetupData->m_olsGenericStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveGeneric(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Generic设置数据
void SaveGenericSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Generic设置队列数据
		SaveGenericList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Generic设置数据
void SetGenericSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oGenericStruct oGenericStruct;
	unsigned int uiPos = 0;
	OnResetGenericList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oGenericStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oGenericStruct.m_usLabelSize, &pChar[uiPos], 2);
		uiPos += 2;
		oGenericStruct.m_pcLabel = new char[oGenericStruct.m_usLabelSize];
		memcpy(oGenericStruct.m_pcLabel, &pChar[uiPos], oGenericStruct.m_usLabelSize);
		uiPos += oGenericStruct.m_usLabelSize;
		memcpy(&oGenericStruct.m_usLineSize, &pChar[uiPos], 2);
		uiPos += 2;
		oGenericStruct.m_pcLine = new char[oGenericStruct.m_usLineSize];
		memcpy(oGenericStruct.m_pcLine, &pChar[uiPos], oGenericStruct.m_usLineSize);
		uiPos += oGenericStruct.m_usLineSize;
		memcpy(&oGenericStruct.m_usSpreadSize, &pChar[uiPos], 2);
		uiPos += 2;
		oGenericStruct.m_pcSpread = new char[oGenericStruct.m_usSpreadSize];
		memcpy(oGenericStruct.m_pcSpread, &pChar[uiPos], oGenericStruct.m_usSpreadSize);
		uiPos += oGenericStruct.m_usSpreadSize;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsGenericStruct.push_back(oGenericStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveGenericSetupData(pLineSetupData);
	}
}
// 加载Look设置数据
void LoadLook(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Look设置区
		strKey = "LookSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "AutoLook";
		pLineSetupData->m_oLook.m_uiAutoLook = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		strKey = "Resistance";
		pLineSetupData->m_oLook.m_uiResistance = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		strKey = "Tilt";
		pLineSetupData->m_oLook.m_uiTilt = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
		strKey = "Leakage";
		pLineSetupData->m_oLook.m_uiLeakage = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Look设置数据
void LoadLookSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Look设置数据
		LoadLook(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Look设置数据
void SaveLook(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Look设置区
		strKey = "LookSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "AutoLook";
		oVariant = (long)pLineSetupData->m_oLook.m_uiAutoLook;
		oElement.setAttribute(strKey, oVariant);
		strKey = "Resistance";
		oVariant = (long)pLineSetupData->m_oLook.m_uiResistance;
		oElement.setAttribute(strKey, oVariant);
		strKey = "Tilt";
		oVariant = (long)pLineSetupData->m_oLook.m_uiTilt;
		oElement.setAttribute(strKey, oVariant);
		strKey = "Leakage";
		oVariant = (long)pLineSetupData->m_oLook.m_uiLeakage;
		oElement.setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Look设置数据
void SaveLookSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Look设置数据
		SaveLook(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Look设置数据
void SetLookSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	unsigned int uiPos = 0;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	while(uiPos < uiSize)
	{
		memcpy(&pLineSetupData->m_oLook.m_uiAutoLook, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&pLineSetupData->m_oLook.m_uiResistance, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&pLineSetupData->m_oLook.m_uiTilt, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&pLineSetupData->m_oLook.m_uiLeakage, &pChar[uiPos], 4);
		uiPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	if (bSave == true)
	{
		SaveLookSetupData(pLineSetupData);
	}
}
// 加载LAULeakage设置数据
void LoadLAULeakage(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到LAULeakage设置区
		strKey = "LAULeakageSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "Limit";
		pLineSetupData->m_oLAULeakage.m_uiLimit = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载LAULeakage设置数据
void LoadLAULeakageSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载LAULeakage设置数据
		LoadLAULeakage(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存LAULeakage设置数据
void SaveLAULeakage(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到LAULeakage设置区
		strKey = "LAULeakageSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "Limit";
		oVariant = (long)pLineSetupData->m_oLAULeakage.m_uiLimit;
		oElement.setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存LAULeakage设置数据
void SaveLAULeakageSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存LAULeakage设置数据
		SaveLAULeakage(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置LAULeakage设置数据
void SetLAULeakageSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	unsigned int uiPos = 0;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	while(uiPos < uiSize)
	{
		memcpy(&pLineSetupData->m_oLAULeakage.m_uiLimit, &pChar[uiPos], 4);
		uiPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	if (bSave == true)
	{
		SaveLAULeakageSetupData(pLineSetupData);
	}
}
// 加载FormLine设置数据
void LoadFormLine(m_oFormLineStruct* pFormLineStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	try
	{
		strKey = "Nb";
		pFormLineStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "BoxType";
		pFormLineStruct->m_uiBoxType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SN";
		pFormLineStruct->m_uiSN = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载FormLine设置队列数据
void LoadFormLineList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到FormLine设置区
		strKey = "FormLineSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到FormLine总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oFormLineStruct oFormLineStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadFormLine(&oFormLineStruct, &oElement);
			// 增加FormLine
			pLineSetupData->m_olsFormLineStruct.push_back(oFormLineStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载FormLine设置数据
void LoadFormLineSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置FormLine
	OnResetFormLineList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载FormLine设置队列数据
		LoadFormLineList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存FormLine设置数据
void SaveFormLine(m_oFormLineStruct* pFormLineStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		oVariant = (long)pFormLineStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "BoxType";
		oVariant = (long)pFormLineStruct->m_uiBoxType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SN";
		oVariant = (long)pFormLineStruct->m_uiSN;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存FormLine设置队列数据
void SaveFormLineList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oFormLineStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到FormLine设置区
		strKey = "FormLineSetup";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);

		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置FormLine总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsFormLineStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsFormLineStruct.begin();
			iter != pLineSetupData->m_olsFormLineStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveFormLine(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存FormLine设置数据
void SaveFormLineSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存FormLine设置队列数据
		SaveFormLineList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置FormLine设置数据
void SetFormLineSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oFormLineStruct oFormLineStruct;
	unsigned int uiPos = 0;
	OnResetFormLineList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oFormLineStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oFormLineStruct.m_uiBoxType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oFormLineStruct.m_uiSN, &pChar[uiPos], 4);
		uiPos += 4;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsFormLineStruct.push_back(oFormLineStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveFormLineSetupData(pLineSetupData);
	}
}
// 加载Instrument_SensorTestBase设置数据
void LoadInstrument_SensorTestBase(Instrument_SensorTestBase_Struct* pInstrument_SensorTestBaseStruct,
	CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "TestAim";
		pInstrument_SensorTestBaseStruct->m_uiTestAim = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Nb";
		pInstrument_SensorTestBaseStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Descr";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pInstrument_SensorTestBaseStruct->m_usDescrSize = (unsigned short)(strConv.size() + 1);
		pInstrument_SensorTestBaseStruct->m_pcDescr = new char[pInstrument_SensorTestBaseStruct->m_usDescrSize];
		memcpy(pInstrument_SensorTestBaseStruct->m_pcDescr, strConv.c_str(), pInstrument_SensorTestBaseStruct->m_usDescrSize);
		strKey = "TestType";
		pInstrument_SensorTestBaseStruct->m_uiTestType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "ADC";
		pInstrument_SensorTestBaseStruct->m_uiADC = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Gain";
		pInstrument_SensorTestBaseStruct->m_uiGain = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "DAC";
		pInstrument_SensorTestBaseStruct->m_uiDAC = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Filter";
		pInstrument_SensorTestBaseStruct->m_uiFilter = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SamplingRate";
		pInstrument_SensorTestBaseStruct->m_uiSamplingRate = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "SamplingLength";
		pInstrument_SensorTestBaseStruct->m_uiSamplingLength = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Instrument_SensorTestBase设置队列数据
void LoadInstrument_SensorTestBaseList(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Instrument_SensorTestBase设置区
		if (bInstrument == true)
		{
			strKey = "InstrumentTestBase";
		}
		else
		{
			strKey = "SensorTestBase";
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Instrument_SensorTestBase总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		Instrument_SensorTestBase_Struct oInstrument_SensorTestBaseStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadInstrument_SensorTestBase(&oInstrument_SensorTestBaseStruct, &oElement);
			// 增加Instrument_SensorTestBase
			if (bInstrument == true)
			{
				pLineSetupData->m_olsInstrumentTestBaseStruct.push_back(oInstrument_SensorTestBaseStruct);
			}
			else
			{
				pLineSetupData->m_olsSensorTestBaseStruct.push_back(oInstrument_SensorTestBaseStruct);
			}
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Instrument_SensorTestBase设置数据
void LoadInstrument_SensorTestBaseSetupData(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Instrument_SensorTestBase
	OnResetInstrument_SensorTestBaseList(bInstrument, pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Instrument_SensorTestBase设置队列数据
		LoadInstrument_SensorTestBaseList(bInstrument, pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Instrument_SensorTestBase设置数据
void SaveInstrument_SensorTestBase(Instrument_SensorTestBase_Struct* pInstrument_SensorTestBaseStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "TestAim";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiTestAim;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Nb";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Descr";
		strConv = pInstrument_SensorTestBaseStruct->m_pcDescr;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestType";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiTestType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "ADC";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiADC;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Gain";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiGain;
		pElement->setAttribute(strKey, oVariant);
		strKey = "DAC";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiDAC;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Filter";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiFilter;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SamplingRate";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiSamplingRate;
		pElement->setAttribute(strKey, oVariant);
		strKey = "SamplingLength";
		oVariant = (long)pInstrument_SensorTestBaseStruct->m_uiSamplingLength;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Instrument_SensorTestBase设置队列数据
void SaveInstrument_SensorTestBaseList(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oInstrumentTestBaseStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Instrument_SensorTestBase设置区
		if (bInstrument == true)
		{
			strKey = "InstrumentTestBase";
		}
		else
		{
			strKey = "SensorTestBase";
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);
		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Instrument_SensorTestBase总数
		strKey = "Count";
		if (bInstrument == true)
		{
			oVariant = (long)pLineSetupData->m_olsInstrumentTestBaseStruct.size();
		}
		else
		{
			oVariant = (long)pLineSetupData->m_olsSensorTestBaseStruct.size();
		}
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		if (bInstrument == true)
		{
			for (iter = pLineSetupData->m_olsInstrumentTestBaseStruct.begin();
				iter != pLineSetupData->m_olsInstrumentTestBaseStruct.end(); iter++)
			{
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
				oElementParent.appendChild(lpDispatch);
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
				oElementChild.AttachDispatch(lpDispatch);
				SaveInstrument_SensorTestBase(&(*iter), &oElementChild);
				oElementParent.appendChild(lpDispatch);		
			}
		}
		else
		{
			for (iter = pLineSetupData->m_olsSensorTestBaseStruct.begin();
				iter != pLineSetupData->m_olsSensorTestBaseStruct.end(); iter++)
			{
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
				oElementParent.appendChild(lpDispatch);
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
				oElementChild.AttachDispatch(lpDispatch);
				SaveInstrument_SensorTestBase(&(*iter), &oElementChild);
				oElementParent.appendChild(lpDispatch);		
			}
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Instrument_SensorTestBase设置数据
void SaveInstrument_SensorTestBaseSetupData(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Instrument_SensorTestBase设置队列数据
		SaveInstrument_SensorTestBaseList(bInstrument, pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Instrument_SensorTestBase设置数据
void SetInstrument_SensorTestBaseSetupData(char* pChar, unsigned int uiSize, bool bInstrument, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oInstrumentTestBaseStruct oInstrument_SensorTestBaseStruct;
	unsigned int uiPos = 0;
	OnResetInstrument_SensorTestBaseList(bInstrument, pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiTestAim, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_usDescrSize, &pChar[uiPos], 2);
		uiPos += 2;
		oInstrument_SensorTestBaseStruct.m_pcDescr = new char[oInstrument_SensorTestBaseStruct.m_usDescrSize];
		memcpy(oInstrument_SensorTestBaseStruct.m_pcDescr, &pChar[uiPos], oInstrument_SensorTestBaseStruct.m_usDescrSize);
		uiPos += oInstrument_SensorTestBaseStruct.m_usDescrSize;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiTestType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiADC, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiGain, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiDAC, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiFilter, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiSamplingRate, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestBaseStruct.m_uiSamplingLength, &pChar[uiPos], 4);
		uiPos += 4;
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		if (bInstrument == true)
		{
			pLineSetupData->m_olsInstrumentTestBaseStruct.push_back(oInstrument_SensorTestBaseStruct);
		}
		else
		{
			pLineSetupData->m_olsSensorTestBaseStruct.push_back(oInstrument_SensorTestBaseStruct);
		}
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveInstrument_SensorTestBaseSetupData(bInstrument, pLineSetupData);
	}
}
// 加载Instrument_SensorTestLimit设置数据
void LoadInstrument_SensorTestLimit(Instrument_SensorTestLimit_Struct* pInstrument_SensorTestLimitStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "Nb";
		pInstrument_SensorTestLimitStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Descr";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pInstrument_SensorTestLimitStruct->m_usDescrSize = (unsigned short)(strConv.size() + 1);
		pInstrument_SensorTestLimitStruct->m_pcDescr = new char[pInstrument_SensorTestLimitStruct->m_usDescrSize];
		memcpy(pInstrument_SensorTestLimitStruct->m_pcDescr, strConv.c_str(), pInstrument_SensorTestLimitStruct->m_usDescrSize);
		strKey = "Unit";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pInstrument_SensorTestLimitStruct->m_usUnitSize = (unsigned short)(strConv.size() + 1);
		pInstrument_SensorTestLimitStruct->m_pcUnit = new char[pInstrument_SensorTestLimitStruct->m_usUnitSize];
		memcpy(pInstrument_SensorTestLimitStruct->m_pcUnit, strConv.c_str(), pInstrument_SensorTestLimitStruct->m_usUnitSize);
		strKey = "TestAim";
		pInstrument_SensorTestLimitStruct->m_uiTestAim = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "TestType";
		pInstrument_SensorTestLimitStruct->m_uiTestType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Limit";
		pInstrument_SensorTestLimitStruct->m_fLimit = CXMLDOMTool::GetElementAttributeFloat(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Instrument_SensorTestLimit设置队列数据
void LoadInstrument_SensorTestLimitList(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Instrument_SensorTestLimit设置区
		if (bInstrument == true)
		{
			strKey = "InstrumentTestLimit";
		}
		else
		{
			strKey = "SensorTestLimit";
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Instrument_SensorTestLimit总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		Instrument_SensorTestLimit_Struct oInstrument_SensorTestLimitStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadInstrument_SensorTestLimit(&oInstrument_SensorTestLimitStruct, &oElement);
			// 增加Instrument_SensorTestLimit
			if (bInstrument == true)
			{
				pLineSetupData->m_olsInstrumentTestLimitStruct.push_back(oInstrument_SensorTestLimitStruct);
			}
			else
			{
				pLineSetupData->m_olsSensorTestLimitStruct.push_back(oInstrument_SensorTestLimitStruct);
			}
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Instrument_SensorTestLimit设置数据
void LoadInstrument_SensorTestLimitSetupData(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Instrument_SensorTestLimit
	OnResetInstrument_SensorTestLimitList(bInstrument, pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Instrument_SensorTestLimit设置队列数据
		LoadInstrument_SensorTestLimitList(bInstrument, pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Instrument_SensorTestLimit设置数据
void SaveInstrument_SensorTestLimit(Instrument_SensorTestLimit_Struct* pInstrument_SensorTestLimitStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "TestAim";
		oVariant = (long)pInstrument_SensorTestLimitStruct->m_uiTestAim;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Nb";
		oVariant = (long)pInstrument_SensorTestLimitStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Descr";
		strConv = pInstrument_SensorTestLimitStruct->m_pcDescr;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Unit";
		strConv = pInstrument_SensorTestLimitStruct->m_pcUnit;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestType";
		oVariant = (long)pInstrument_SensorTestLimitStruct->m_uiTestType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Limit";
		oVariant = (long)pInstrument_SensorTestLimitStruct->m_fLimit;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Instrument_SensorTestLimit设置队列数据
void SaveInstrument_SensorTestLimitList(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oInstrumentTestLimitStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Instrument_SensorTestLimit设置区
		if (bInstrument == true)
		{
			strKey = "InstrumentTestLimit";
		}
		else
		{
			strKey = "SensorTestLimit";
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);
		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Instrument_SensorTestBase总数
		strKey = "Count";
		if (bInstrument == true)
		{
			oVariant = (long)pLineSetupData->m_olsInstrumentTestLimitStruct.size();
		}
		else
		{
			oVariant = (long)pLineSetupData->m_olsSensorTestLimitStruct.size();
		}
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		if (bInstrument == true)
		{
			for (iter = pLineSetupData->m_olsInstrumentTestLimitStruct.begin();
				iter != pLineSetupData->m_olsInstrumentTestLimitStruct.end(); iter++)
			{
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
				oElementParent.appendChild(lpDispatch);
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
				oElementChild.AttachDispatch(lpDispatch);
				SaveInstrument_SensorTestLimit(&(*iter), &oElementChild);
				oElementParent.appendChild(lpDispatch);		
			}
		}
		else
		{
			for (iter = pLineSetupData->m_olsSensorTestLimitStruct.begin();
				iter != pLineSetupData->m_olsSensorTestLimitStruct.end(); iter++)
			{
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
				oElementParent.appendChild(lpDispatch);
				lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
				oElementChild.AttachDispatch(lpDispatch);
				SaveInstrument_SensorTestLimit(&(*iter), &oElementChild);
				oElementParent.appendChild(lpDispatch);		
			}
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Instrument_SensorTestLimit设置数据
void SaveInstrument_SensorTestLimitSetupData(bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Instrument_SensorTestLimit设置队列数据
		SaveInstrument_SensorTestLimitList(bInstrument, pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Instrument_SensorTestLimit设置数据
void SetInstrument_SensorTestLimitSetupData(char* pChar, unsigned int uiSize, bool bInstrument, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oInstrumentTestLimitStruct oInstrument_SensorTestLimitStruct;
	unsigned int uiPos = 0;
	OnResetInstrument_SensorTestLimitList(bInstrument, pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oInstrument_SensorTestLimitStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestLimitStruct.m_usDescrSize, &pChar[uiPos], 2);
		uiPos += 2;
		oInstrument_SensorTestLimitStruct.m_pcDescr = new char[oInstrument_SensorTestLimitStruct.m_usDescrSize];
		memcpy(oInstrument_SensorTestLimitStruct.m_pcDescr, &pChar[uiPos], oInstrument_SensorTestLimitStruct.m_usDescrSize);
		uiPos += oInstrument_SensorTestLimitStruct.m_usDescrSize;
		memcpy(&oInstrument_SensorTestLimitStruct.m_usUnitSize, &pChar[uiPos], 2);
		uiPos += 2;
		oInstrument_SensorTestLimitStruct.m_pcUnit = new char[oInstrument_SensorTestLimitStruct.m_usUnitSize];
		memcpy(oInstrument_SensorTestLimitStruct.m_pcUnit, &pChar[uiPos], oInstrument_SensorTestLimitStruct.m_usUnitSize);
		uiPos += oInstrument_SensorTestLimitStruct.m_usUnitSize;
		memcpy(&oInstrument_SensorTestLimitStruct.m_uiTestAim, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestLimitStruct.m_uiTestType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrument_SensorTestLimitStruct.m_fLimit, &pChar[uiPos], 4);
		uiPos += 4;

		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		if (bInstrument == true)
		{
			pLineSetupData->m_olsInstrumentTestLimitStruct.push_back(oInstrument_SensorTestLimitStruct);
		}
		else
		{
			pLineSetupData->m_olsSensorTestLimitStruct.push_back(oInstrument_SensorTestLimitStruct);
		}
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveInstrument_SensorTestLimitSetupData(bInstrument, pLineSetupData);
	}
}
// 加载Instrument Test设置数据
void LoadInstrumentTest(m_oInstrumentTestStruct* pInstrumentTestStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "TestNb";
		pInstrumentTestStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "TestType";
		pInstrumentTestStruct->m_uiTestType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Gain";
		pInstrumentTestStruct->m_uiGain = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "RecordLength";
		pInstrumentTestStruct->m_uiRecordLength = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Recorded";
		pInstrumentTestStruct->m_uiRecorded = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "AuxiliaryDescr";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pInstrumentTestStruct->m_usAuxiliaryDescrSize = (unsigned short)(strConv.size() + 1);
		pInstrumentTestStruct->m_pcAuxiliaryDescr = new char[pInstrumentTestStruct->m_usAuxiliaryDescrSize];
		memcpy(pInstrumentTestStruct->m_pcAuxiliaryDescr, strConv.c_str(), pInstrumentTestStruct->m_usAuxiliaryDescrSize);
		strKey = "AbsoluteSpread";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pInstrumentTestStruct->m_usAbsoluteSpreadSize = (unsigned short)(strConv.size() + 1);
		pInstrumentTestStruct->m_pcAbsoluteSpread = new char[pInstrumentTestStruct->m_usAbsoluteSpreadSize];
		memcpy(pInstrumentTestStruct->m_pcAbsoluteSpread, strConv.c_str(), pInstrumentTestStruct->m_usAbsoluteSpreadSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Instrument Test设置队列数据
void LoadInstrumentTestList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Instrument Test设置区
		strKey = "InstrumentTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Instrument Test总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oInstrumentTestStruct oInstrumentTestStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadInstrumentTest(&oInstrumentTestStruct, &oElement);
			// 增加Instrument Test
			pLineSetupData->m_olsInstrumentTestStruct.push_back(oInstrumentTestStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Instrument Test设置数据
void LoadInstrumentTestSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Instrument Test
	OnResetInstrumentTestList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Instrument Test设置队列数据
		LoadInstrumentTestList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Instrument Test设置数据
void SaveInstrumentTest(m_oInstrumentTestStruct* pInstrumentTestStruct, CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "TestNb";
		oVariant = (long)pInstrumentTestStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Recorded";
		oVariant = (long)pInstrumentTestStruct->m_uiRecorded;
		pElement->setAttribute(strKey, oVariant);
		strKey = "RecordLength";
		oVariant = (long)pInstrumentTestStruct->m_uiRecordLength;
		pElement->setAttribute(strKey, oVariant);
		strKey = "AuxiliaryDescr";
		strConv = pInstrumentTestStruct->m_pcAuxiliaryDescr;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "AbsoluteSpread";
		strConv = pInstrumentTestStruct->m_pcAbsoluteSpread;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestType";
		oVariant = (long)pInstrumentTestStruct->m_uiTestType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Gain";
		oVariant = (long)pInstrumentTestStruct->m_uiGain;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Instrument Test设置队列数据
void SaveInstrumentTestList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oInstrumentTestStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Instrument Test设置区
		strKey = "InstrumentTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);
		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Instrument Test总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsInstrumentTestStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsInstrumentTestStruct.begin();
			iter != pLineSetupData->m_olsInstrumentTestStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveInstrumentTest(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Instrument Test设置数据
void SaveInstrumentTestSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Instrument Test设置队列数据
		SaveInstrumentTestList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Instrument Test设置数据
void SetInstrumentTestSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oInstrumentTestStruct oInstrumentTestStruct;
	unsigned int uiPos = 0;
	OnResetInstrumentTestList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oInstrumentTestStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrumentTestStruct.m_uiTestType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrumentTestStruct.m_uiGain, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrumentTestStruct.m_uiRecordLength, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrumentTestStruct.m_uiRecorded, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oInstrumentTestStruct.m_usAuxiliaryDescrSize, &pChar[uiPos], 2);
		uiPos += 2;
		oInstrumentTestStruct.m_pcAuxiliaryDescr = new char[oInstrumentTestStruct.m_usAuxiliaryDescrSize];
		memcpy(oInstrumentTestStruct.m_pcAuxiliaryDescr, &pChar[uiPos], oInstrumentTestStruct.m_usAuxiliaryDescrSize);
		uiPos += oInstrumentTestStruct.m_usAuxiliaryDescrSize;
		memcpy(&oInstrumentTestStruct.m_usAbsoluteSpreadSize, &pChar[uiPos], 2);
		uiPos += 2;
		oInstrumentTestStruct.m_pcAbsoluteSpread = new char[oInstrumentTestStruct.m_usAbsoluteSpreadSize];
		memcpy(oInstrumentTestStruct.m_pcAbsoluteSpread, &pChar[uiPos], oInstrumentTestStruct.m_usAbsoluteSpreadSize);
		uiPos += oInstrumentTestStruct.m_usAbsoluteSpreadSize;

		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsInstrumentTestStruct.push_back(oInstrumentTestStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveInstrumentTestSetupData(pLineSetupData);
	}
}
// 加载Sensor Test设置数据
void LoadSensorTest(m_oSensorTestStruct* pSensorTestStruct,CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "TestNb";
		pSensorTestStruct->m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "TestType";
		pSensorTestStruct->m_uiTestType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Recorded";
		pSensorTestStruct->m_uiRecorded = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "AbsoluteSpread";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		pSensorTestStruct->m_usAbsoluteSpreadSize = (unsigned short)(strConv.size() + 1);
		pSensorTestStruct->m_pcAbsoluteSpread = new char[pSensorTestStruct->m_usAbsoluteSpreadSize];
		memcpy(pSensorTestStruct->m_pcAbsoluteSpread, strConv.c_str(), pSensorTestStruct->m_usAbsoluteSpreadSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Sensor Test设置队列数据
void LoadSensorTestList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Sensor Test设置区
		strKey = "SensorTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Sensor Test总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oSensorTestStruct oSensorTestStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadSensorTest(&oSensorTestStruct, &oElement);
			// 增加Sensor Test
			pLineSetupData->m_olsSensorTestStruct.push_back(oSensorTestStruct);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Sensor Test设置数据
void LoadSensorTestSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Sensor Test
	OnResetSensorTestList(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Sensor Test设置队列数据
		LoadSensorTestList(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Sensor Test设置数据
void SaveSensorTest(m_oSensorTestStruct* pSensorTestStruct, CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "TestNb";
		oVariant = (long)pSensorTestStruct->m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Recorded";
		oVariant = (long)pSensorTestStruct->m_uiRecorded;
		pElement->setAttribute(strKey, oVariant);
		strKey = "AbsoluteSpread";
		strConv = pSensorTestStruct->m_pcAbsoluteSpread;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestType";
		oVariant = (long)pSensorTestStruct->m_uiTestType;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Sensor Test设置队列数据
void SaveSensorTestList(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oSensorTestStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Sensor Test设置区
		strKey = "SensorTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);
		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Sensor Test总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_olsSensorTestStruct.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_olsSensorTestStruct.begin();
			iter != pLineSetupData->m_olsSensorTestStruct.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveSensorTest(&(*iter), &oElementChild);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Sensor Test设置数据
void SaveSensorTestSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Sensor Test设置队列数据
		SaveSensorTestList(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Sensor Test设置数据
void SetSensorTestSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oSensorTestStruct oSensorTestStruct;
	unsigned int uiPos = 0;
	OnResetSensorTestList(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oSensorTestStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorTestStruct.m_uiTestType, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorTestStruct.m_uiRecorded, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oSensorTestStruct.m_usAbsoluteSpreadSize, &pChar[uiPos], 2);
		uiPos += 2;
		oSensorTestStruct.m_pcAbsoluteSpread = new char[oSensorTestStruct.m_usAbsoluteSpreadSize];
		memcpy(oSensorTestStruct.m_pcAbsoluteSpread, &pChar[uiPos], oSensorTestStruct.m_usAbsoluteSpreadSize);
		uiPos += oSensorTestStruct.m_usAbsoluteSpreadSize;

		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_olsSensorTestStruct.push_back(oSensorTestStruct);
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveSensorTestSetupData(pLineSetupData);
	}
}
// 加载Multiple Test设置数据
void LoadMultipleTest(m_oMultipleTestTaskStruct* pMultipleTestTaskStruct, CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	try
	{
		strKey = "LineNb";
		pMultipleTestTaskStruct->m_uiLineNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "TestType";
		pMultipleTestTaskStruct->m_uiTestType = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "Gain";
		pMultipleTestTaskStruct->m_uiGain = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "LoopLineNb";
		pMultipleTestTaskStruct->m_uiLoopLineNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "NbLoops";
		pMultipleTestTaskStruct->m_uiNbLoops = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 加载Multiple Test设置队列数据
void LoadMultipleTestList(m_oLineSetupDataStruct* pLineSetupData, CXMLDOMElement* pElement)
{
	ASSERT(pLineSetupData != NULL);
	ASSERT(pElement != NULL);
	CString strKey;
	CString str = _T("");
	string strConv = "";
	CXMLDOMNodeList oNodeList;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll = 0;
	m_oMultipleTestKeyStruct oMultipleTestKeyStruct;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "TestNb";
		oMultipleTestKeyStruct.m_uiNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "TestName";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		oMultipleTestKeyStruct.m_usTestNameSize = (unsigned short)(strConv.size() + 1);
		oMultipleTestKeyStruct.m_pcTestName = new char[oMultipleTestKeyStruct.m_usTestNameSize];
		memcpy(oMultipleTestKeyStruct.m_pcTestName, strConv.c_str(), oMultipleTestKeyStruct.m_usTestNameSize);
		strKey = "AuxiliaryDescr";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		oMultipleTestKeyStruct.m_usAuxiliaryDescrSize = (unsigned short)(strConv.size() + 1);
		oMultipleTestKeyStruct.m_pcAuxiliaryDescr = new char[oMultipleTestKeyStruct.m_usAuxiliaryDescrSize];
		memcpy(oMultipleTestKeyStruct.m_pcAuxiliaryDescr, strConv.c_str(), oMultipleTestKeyStruct.m_usAuxiliaryDescrSize);
		strKey = "AbsoluteSpread";
		str = CXMLDOMTool::GetElementAttributeString(pElement, strKey);
		strConv = (CStringA)str;
		oMultipleTestKeyStruct.m_usAbsoluteSpreadSize = (unsigned short)(strConv.size() + 1);
		oMultipleTestKeyStruct.m_pcAbsoluteSpread = new char[oMultipleTestKeyStruct.m_usAbsoluteSpreadSize];
		memcpy(oMultipleTestKeyStruct.m_pcAbsoluteSpread, strConv.c_str(), oMultipleTestKeyStruct.m_usAbsoluteSpreadSize);
		strKey = "DelayBetweenTest";
		oMultipleTestKeyStruct.m_uiDelayBetweenTest = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "RecordResults";
		oMultipleTestKeyStruct.m_uiRecordResults = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "RecordLength";
		oMultipleTestKeyStruct.m_uiRecordLength = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		strKey = "TestFileNb";
		oMultipleTestKeyStruct.m_uiTestFileNb = CXMLDOMTool::GetElementAttributeUnsignedInt(pElement, strKey);
		// 得到队列
		lpDispatch = pElement->get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		m_oMultipleTestTaskStruct oMultipleTestTaskStruct;
		list<m_oMultipleTestTaskStruct> olsMultipleTestTaskStruct;
		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			pElement->AttachDispatch(lpDispatch);
			LoadMultipleTest(&oMultipleTestTaskStruct, pElement);
			// 增加
			olsMultipleTestTaskStruct.push_back(oMultipleTestTaskStruct);
		}
		pLineSetupData->m_oMultpleTestStructMap.insert(
			map<m_oMultipleTestKeyStruct, 
			list<m_oMultipleTestTaskStruct>>::value_type (oMultipleTestKeyStruct, olsMultipleTestTaskStruct));
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Multiple Test设置索引数据
void LoadMultipleTestMap(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int uiCountAll;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Multiple Test设置区
		strKey = "MultipleTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);
		// 得到Multiple Test总数
		strKey = "Count";
		uiCountAll = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElement, strKey);

		// 得到队列
		lpDispatch = oElement.get_childNodes();
		oNodeList.AttachDispatch(lpDispatch);

		for(unsigned int i = 0; i < uiCountAll; i++)
		{
			lpDispatch = oNodeList.get_item(i);
			oElement.AttachDispatch(lpDispatch);
			LoadMultipleTestList(pLineSetupData, &oElement);
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载Multiple Test设置数据
void LoadMultipleTestSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置Multiple Test
	OnResetMultipleTestMap(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载Multiple Test设置队列数据
		LoadMultipleTestMap(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Multiple Test设置数据
void SaveMultipleTest(m_oMultipleTestTaskStruct* pMultipleTestTaskStruct, CXMLDOMElement* pElement)
{
	CString strKey = _T("");
	COleVariant oVariant;
	CString str = _T("");
	string strConv = "";
	try
	{
		strKey = "LineNb";
		oVariant = (long)pMultipleTestTaskStruct->m_uiLineNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestType";
		oVariant = (long)pMultipleTestTaskStruct->m_uiTestType;
		pElement->setAttribute(strKey, oVariant);
		strKey = "Gain";
		oVariant = (long)pMultipleTestTaskStruct->m_uiGain;
		pElement->setAttribute(strKey, oVariant);
		strKey = "LoopLineNb";
		oVariant = (long)pMultipleTestTaskStruct->m_uiLoopLineNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "NbLoops";
		oVariant = (long)pMultipleTestTaskStruct->m_uiNbLoops;
		pElement->setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
}
// 保存Multiple Test设置队列数据
void SaveMultipleTestList(m_oLineSetupDataStruct* pLineSetupData, CXMLDOMElement* pElement, 
	map<m_oMultipleTestKeyStruct, list<m_oMultipleTestTaskStruct>>::iterator iter, unsigned int uiTabCount)
{
	ASSERT(pLineSetupData != NULL);
	ASSERT(pElement != NULL);
	CString strKey;
	string strConv = "";
	CString str = _T("");
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	LPDISPATCH lpDispatch;
	CXMLDOMElement oElementChild;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	list<m_oMultipleTestTaskStruct>::iterator iterList;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		strKey = "Count";
		oVariant = (long)iter->second.size();
		pElement->setAttribute(strKey, oVariant);
		strKey = "TabCount";
		oVariant = (long)uiTabCount;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestNb";
		oVariant = (long)iter->first.m_uiNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestName";
		strConv = iter->first.m_pcTestName;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "DelayBetweenTest";
		oVariant = (long)iter->first.m_uiDelayBetweenTest;
		pElement->setAttribute(strKey, oVariant);
		strKey = "RecordResults";
		oVariant = (long)iter->first.m_uiRecordResults;
		pElement->setAttribute(strKey, oVariant);
		strKey = "RecordLength";
		oVariant = (long)iter->first.m_uiRecordLength;
		pElement->setAttribute(strKey, oVariant);
		strKey = "TestFileNb";
		oVariant = (long)iter->first.m_uiTestFileNb;
		pElement->setAttribute(strKey, oVariant);
		strKey = "AuxiliaryDescr";
		strConv = iter->first.m_pcAuxiliaryDescr;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strKey = "AbsoluteSpread";
		strConv = iter->first.m_pcAbsoluteSpread;
		str = strConv.c_str();
		oVariant = str;
		pElement->setAttribute(strKey, oVariant);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		for (iterList = iter->second.begin(); iterList != iter->second.end(); iterList++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			pElement->appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("Record"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveMultipleTest(&(*iterList), &oElementChild);
			pElement->appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		pElement->appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Multiple Test设置索引数据
void SaveMultipleTestMap(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;
	unsigned int uiTabCount = 0;
	CString strTabChild = _T("");
	CString strTabParent = _T("");
	map<m_oMultipleTestKeyStruct, list<m_oMultipleTestTaskStruct>>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到Multiple Test设置区
		strKey = "MultipleTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElementParent.AttachDispatch(lpDispatch);
		// 得到Tab键数量
		strKey = "TabCount";
		uiTabCount = CXMLDOMTool::GetElementAttributeUnsignedInt(&oElementParent, strKey);
		strTabChild = _T("\r\n");
		strTabParent = _T("\r\n");
		for(unsigned int i = 0; i < uiTabCount; i++)
		{
			strTabChild += _T("\t");
			if (i < (uiTabCount - 1))
			{
				strTabParent += _T("\t");
			}
		}
		// 设置Multiple Test总数
		strKey = "Count";
		oVariant = (long)pLineSetupData->m_oMultpleTestStructMap.size();
		oElementParent.setAttribute(strKey, oVariant);
		// 删除所有子节点
		while(TRUE == oElementParent.hasChildNodes())
		{
			lpDispatch = oElementParent.get_firstChild();
			oElementParent.removeChild(lpDispatch);
		}
		for (iter = pLineSetupData->m_oMultpleTestStructMap.begin();
			iter != pLineSetupData->m_oMultpleTestStructMap.end(); iter++)
		{
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabChild);
			oElementParent.appendChild(lpDispatch);
			lpDispatch = pLineSetupData->m_oXMLDOMDocument.createElement(_T("TestSetup"));
			oElementChild.AttachDispatch(lpDispatch);
			SaveMultipleTestList(pLineSetupData, &oElementChild, iter, uiTabCount + 1);
			oElementParent.appendChild(lpDispatch);		
		}
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.createTextNode(strTabParent);
		oElementParent.appendChild(lpDispatch);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存Multiple Test设置数据
void SaveMultipleTestSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存Multiple Test设置索引数据
		SaveMultipleTestMap(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置Multiple Test设置数据
void SetMultipleTestSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	m_oMultipleTestKeyStruct oMultipleTestKeyStruct;
	m_oMultipleTestTaskStruct oMultipleTestTaskStruct;
	list<m_oMultipleTestTaskStruct> olsMultipleTestTaskStruct;
	int iCount = 0;
	unsigned int uiPos = 0;
	OnResetMultipleTestMap(pLineSetupData);
	while(uiPos < uiSize)
	{
		memcpy(&oMultipleTestKeyStruct.m_uiNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMultipleTestKeyStruct.m_usTestNameSize, &pChar[uiPos], 2);
		uiPos += 2;
		oMultipleTestKeyStruct.m_pcTestName = new char[oMultipleTestKeyStruct.m_usTestNameSize];
		memcpy(oMultipleTestKeyStruct.m_pcTestName, &pChar[uiPos], oMultipleTestKeyStruct.m_usTestNameSize);
		uiPos += oMultipleTestKeyStruct.m_usTestNameSize;
		memcpy(&oMultipleTestKeyStruct.m_usAuxiliaryDescrSize, &pChar[uiPos], 2);
		uiPos += 2;
		oMultipleTestKeyStruct.m_pcAuxiliaryDescr = new char[oMultipleTestKeyStruct.m_usAuxiliaryDescrSize];
		memcpy(oMultipleTestKeyStruct.m_pcAuxiliaryDescr, &pChar[uiPos], oMultipleTestKeyStruct.m_usAuxiliaryDescrSize);
		uiPos += oMultipleTestKeyStruct.m_usAuxiliaryDescrSize;
		memcpy(&oMultipleTestKeyStruct.m_usAbsoluteSpreadSize, &pChar[uiPos], 2);
		uiPos += 2;
		oMultipleTestKeyStruct.m_pcAbsoluteSpread = new char[oMultipleTestKeyStruct.m_usAbsoluteSpreadSize];
		memcpy(oMultipleTestKeyStruct.m_pcAbsoluteSpread, &pChar[uiPos], oMultipleTestKeyStruct.m_usAbsoluteSpreadSize);
		uiPos += oMultipleTestKeyStruct.m_usAbsoluteSpreadSize;
		memcpy(&oMultipleTestKeyStruct.m_uiDelayBetweenTest, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMultipleTestKeyStruct.m_uiRecordResults, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMultipleTestKeyStruct.m_uiRecordLength, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&oMultipleTestKeyStruct.m_uiTestFileNb, &pChar[uiPos], 4);
		uiPos += 4;
		memcpy(&iCount, &pChar[uiPos], 4);
		uiPos += 4;
		olsMultipleTestTaskStruct.clear();
		for (int i=0; i<iCount; i++)
		{
			memcpy(&oMultipleTestTaskStruct.m_uiLineNb, &pChar[uiPos], 4);
			uiPos += 4;
			memcpy(&oMultipleTestTaskStruct.m_uiTestType, &pChar[uiPos], 4);
			uiPos += 4;
			memcpy(&oMultipleTestTaskStruct.m_uiGain, &pChar[uiPos], 4);
			uiPos += 4;
			memcpy(&oMultipleTestTaskStruct.m_uiLoopLineNb, &pChar[uiPos], 4);
			uiPos += 4;
			memcpy(&oMultipleTestTaskStruct.m_uiNbLoops, &pChar[uiPos], 4);
			uiPos += 4;
			olsMultipleTestTaskStruct.push_back(oMultipleTestTaskStruct);
		}
		EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
		pLineSetupData->m_oMultpleTestStructMap.insert(
			map<m_oMultipleTestKeyStruct, list<m_oMultipleTestTaskStruct>>::value_type (oMultipleTestKeyStruct, olsMultipleTestTaskStruct));
		LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	}
	if (bSave == true)
	{
		SaveMultipleTestSetupData(pLineSetupData);
	}
}
// 加载SeisMonitor设置数据
void LoadSeisMonitor(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	CString str = _T("");
	string strConv = "";
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到SeisMonitor Test设置区
		strKey = "SeisMonitorTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "AbsoluteSpread";
		str = CXMLDOMTool::GetElementAttributeString(&oElement, strKey);
		strConv = (CStringA)str;
		pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize = (unsigned short)(strConv.size() + 1);
		pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread = new char[pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize];
		memcpy(pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread, strConv.c_str(), 
			pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 加载SeisMonitor设置数据
void LoadSeisMonitorSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 重置SeisMonitor
	OnResetSeisMonitor(pLineSetupData);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 加载SeisMonitor设置队列数据
		LoadSeisMonitor(pLineSetupData);
	}
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存SeisMonitor设置数据
void SaveSeisMonitor(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	CString strKey;
	string strConv = "";
	CString str = _T("");
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	try
	{
		// 找到SeisMonitor Test设置区
		strKey = "SeisMonitorTest";
		lpDispatch = pLineSetupData->m_oXMLDOMDocument.getElementsByTagName(strKey);
		oNodeList.AttachDispatch(lpDispatch);
		// 找到入口
		lpDispatch = oNodeList.get_item(0);
		oElement.AttachDispatch(lpDispatch);

		strKey = "AbsoluteSpread";
		strConv = pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread;
		str = strConv.c_str();
		oVariant = str;
		oElement.setAttribute(strKey, oVariant);
	}
	catch (CMemoryException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_MEMORY_EXCEPTION);
	}
	catch (CFileException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_FILE_EXCEPTION);
	}
	catch (CException* e)
	{
		e->ReportError(MB_OK, IDS_ERR_OTHER_EXCEPTION);
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 保存SeisMonitor设置数据
void SaveSeisMonitorSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	COleVariant oVariant;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	// 打开程序配置文件
	if (TRUE == OpenLineClientXMLFile(pLineSetupData))
	{
		// 保存SeisMonitor设置数据
		SaveSeisMonitor(pLineSetupData);
	}
	oVariant = (CString)(pLineSetupData->m_strLineXMLFilePath.c_str());
	pLineSetupData->m_oXMLDOMDocument.save(oVariant);
	// 关闭程序配置文件
	CloseLineClientXMLFile(pLineSetupData);
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}
// 设置SeisMonitor设置数据
void SetSeisMonitorSetupData(char* pChar, unsigned int uiSize, m_oLineSetupDataStruct* pLineSetupData, bool bSave)
{
	unsigned int uiPos = 0;
	OnResetSeisMonitor(pLineSetupData);
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	while(uiPos < uiSize)
	{
		memcpy(&pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize, &pChar[uiPos], 2);
		uiPos += 2;
		pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread = new char[pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize];
		memcpy(pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread, &pChar[uiPos], 
			pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize);
		uiPos += pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	if (bSave == true)
	{
		SaveSeisMonitorSetupData(pLineSetupData);
	}
}
// 加载测线客户端程序设置数据
void LoadLineAppSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	// 加载Survery设置数据
	LoadSurverySetupData(pLineSetupData);
	// 加载Point Code设置数据
	LoadPointCodeSetupData(pLineSetupData);
	// 加载Sensor设置数据
	LoadSensorSetupData(pLineSetupData);
	// 加载Marker设置数据
	LoadMarkerSetupData(pLineSetupData);
	// 加载Aux设置数据
	LoadAuxSetupData(pLineSetupData);
	// 加载Detour设置数据
	LoadDetourSetupData(pLineSetupData);
	// 加载Mute设置数据
	LoadMuteSetupData(pLineSetupData);
	// 加载BlastMachine设置数据
	LoadBlastMachineSetupData(pLineSetupData);
	// 加载Absolute设置数据
	LoadAbsoluteSetupData(pLineSetupData);
	// 加载Generic设置数据
	LoadGenericSetupData(pLineSetupData);
	// 加载Look设置数据
	LoadLookSetupData(pLineSetupData);
	// 加载LAULeakage设置数据
	LoadLAULeakageSetupData(pLineSetupData);
	// 加载FormLine设置数据
	LoadFormLineSetupData(pLineSetupData);
	// 加载InstrumentTestBase设置数据
	LoadInstrument_SensorTestBaseSetupData(true, pLineSetupData);
	// 加载SensorTestBase设置数据
	LoadInstrument_SensorTestBaseSetupData(false, pLineSetupData);
	// 加载InstrumentTestLimit设置数据
	LoadInstrument_SensorTestLimitSetupData(true, pLineSetupData);
	// 加载SensorTestLimit设置数据
	LoadInstrument_SensorTestLimitSetupData(false, pLineSetupData);
	// 加载Instrument Test设置数据
	LoadInstrumentTestSetupData(pLineSetupData);
	// 加载Sensor Test设置数据
	LoadSensorTestSetupData(pLineSetupData);
	// 加载Multiple Test设置数据
	LoadMultipleTestSetupData(pLineSetupData);
	// 加载SeisMonitor设置数据
	LoadSeisMonitorSetupData(pLineSetupData);
}

// 保存测线客户端程序设置数据
void SaveLineAppSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	ASSERT(pLineSetupData != NULL);
	// 保存Survery设置数据
	SaveSurverySetupData(pLineSetupData);
	// 保存Point Code设置数据
	SavePointCodeSetupData(pLineSetupData);
	// 保存Sensor设置数据
	SaveSensorSetupData(pLineSetupData);
	// 保存Marker设置数据
	SaveMarkerSetupData(pLineSetupData);
	// 保存Aux设置数据
	SaveAuxSetupData(pLineSetupData);
	// 保存Detour设置数据
	SaveDetourSetupData(pLineSetupData);
	// 保存Mute设置数据
	SaveMuteSetupData(pLineSetupData);
	// 保存BlastMachine设置数据
	SaveBlastMachineSetupData(pLineSetupData);
	// 保存Absolute设置数据
	SaveAbsoluteSetupData(pLineSetupData);
	// 保存Generic设置数据
	SaveGenericSetupData(pLineSetupData);
	// 保存Look设置数据
	SaveLookSetupData(pLineSetupData);
	// 保存LAULeakage设置数据
	SaveLAULeakageSetupData(pLineSetupData);
	// 保存FormLine设置数据
	SaveFormLineSetupData(pLineSetupData);
	// 保存InstrumentTestBase设置数据
	SaveInstrument_SensorTestBaseSetupData(true, pLineSetupData);
	// 保存SensorTestBase设置数据
	SaveInstrument_SensorTestBaseSetupData(false, pLineSetupData);
	// 保存InstrumentTestLimit设置数据
	SaveInstrument_SensorTestLimitSetupData(true, pLineSetupData);
	// 保存SensorTestLimit设置数据
	SaveInstrument_SensorTestLimitSetupData(false, pLineSetupData);
	// 保存Instrument Test设置数据
	SaveInstrumentTestSetupData(pLineSetupData);
	// 保存Sensor Test设置数据
	SaveSensorTestSetupData(pLineSetupData);
	// 保存Multiple Test设置数据
	SaveMultipleTestSetupData(pLineSetupData);
	// 保存SeisMonitor设置数据
	SaveSeisMonitorSetupData(pLineSetupData);
}

// 得到测线接收区域
void GetLineRevSection(unsigned int& uiLineNum, unsigned int& uiColumnNum, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oSurveryStruct>::iterator iter;
	string str = "";
	CString cstr = _T("");
	CString cstrType = _T("");
	unsigned int uiBegin = 0;
	unsigned int uiEnd = 0;
	unsigned int uiType = 0;
	uiLineNum = 0;
	uiColumnNum = 0;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsSurveryStruct.begin();
		iter != pLineSetupData->m_olsSurveryStruct.end(); iter++)
	{
		if (iter->m_uiLine > uiLineNum)
		{
			uiLineNum = iter->m_uiLine;
		}
		str = iter->m_pcReceiverSection;
		cstr = str.c_str();
		_stscanf_s(cstr, _T("%u-%up%u"), &uiBegin, &uiEnd, &uiType);
		if ((uiEnd + 1) > (uiColumnNum + uiBegin))
		{
			uiColumnNum = uiEnd - uiBegin + 1;
		}
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 SurveyXML 文件信息
void QuerySurverySetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oSurveryStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsSurveryStruct.begin();
		iter != pLineSetupData->m_olsSurveryStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiLine, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usReceiverSectionSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcReceiverSection, iter->m_usReceiverSectionSize);
		iPos += iter->m_usReceiverSectionSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 PointCode XML文件信息
void QueryPointCodeSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oPointCodeStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsPointCodeStruct.begin();
		iter != pLineSetupData->m_olsPointCodeStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usLabelSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcLabel, iter->m_usLabelSize);
		iPos += iter->m_usLabelSize;
		memcpy(&cProcBuf[iPos], &iter->m_usSensorTypeSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcSensorType, iter->m_usSensorTypeSize);
		iPos += iter->m_usSensorTypeSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Sensor XML文件信息
void QuerySensorSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oSensorStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsSensorStruct.begin();
		iter != pLineSetupData->m_olsSensorStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usLabelSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcLabel, iter->m_usLabelSize);
		iPos += iter->m_usLabelSize;
		memcpy(&cProcBuf[iPos], &iter->m_fContinuityMin, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_fContinuityMax, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_fTilt, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_fNoise, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_fLeakage, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiSEGDCode, 4);
		iPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Marker XML文件信息
void QueryMarkerSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oMarkerStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsMarkerStruct.begin();
		iter != pLineSetupData->m_olsMarkerStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiBoxType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiSN, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiLineName, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiPointNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiChannelNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiMarkerIncr, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiReversed, 4);
		iPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Aux XML文件信息
void QueryAuxSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oAuxStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsAuxStruct.begin();
		iter != pLineSetupData->m_olsAuxStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usLabelSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcLabel, iter->m_usLabelSize);
		iPos += iter->m_usLabelSize;
		memcpy(&cProcBuf[iPos], &iter->m_uiBoxType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiSN, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiChannelNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiGain, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiDpgNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usCommentsSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcComments, iter->m_usCommentsSize);
		iPos += iter->m_usCommentsSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Detour XML文件信息
void QueryDetourSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oDetourStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsDetourStruct.begin();
		iter != pLineSetupData->m_olsDetourStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiLowBoxType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiLowSN, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiLowChanNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiHighBoxType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiHighSN, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiHighChanNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiStopMarking, 4);
		iPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Mute XML文件信息
void QueryMuteSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oMuteStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsMuteStruct.begin();
		iter != pLineSetupData->m_olsMuteStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiLineName, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiPointNb, 4);
		iPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 BlastMachine XML文件信息
void QueryBlastMachineSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oBlastMachineStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsBlastMachineStruct.begin();
		iter != pLineSetupData->m_olsBlastMachineStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usLabelSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcLabel, iter->m_usLabelSize);
		iPos += iter->m_usLabelSize;
		memcpy(&cProcBuf[iPos], &iter->m_uiBoxType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiSN, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiChannelNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiGain, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiDpgNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usCommentsSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcComments, iter->m_usCommentsSize);
		iPos += iter->m_usCommentsSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Absolute XML文件信息
void QueryAbsoluteSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	map<unsigned int, list<m_oAbsoluteStruct>>::iterator iterMap;
	list<m_oAbsoluteStruct>::iterator iter;
	unsigned int uiSize = 0;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iterMap = pLineSetupData->m_oAbsoluteStructMap.begin();
		iterMap != pLineSetupData->m_oAbsoluteStructMap.end(); iterMap++)
	{
		memcpy(&cProcBuf[iPos], &iterMap->first, 4);
		iPos += 4;
		uiSize = iterMap->second.size();
		memcpy(&cProcBuf[iPos], &uiSize, 4);
		iPos += 4;
		for (iter = iterMap->second.begin(); iter != iterMap->second.end(); iter++)
		{
			memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
			iPos += 4;
			memcpy(&cProcBuf[iPos], &iter->m_usLabelSize, 2);
			iPos += 2;
			memcpy(&cProcBuf[iPos], iter->m_pcLabel, iter->m_usLabelSize);
			iPos += iter->m_usLabelSize;
			memcpy(&cProcBuf[iPos], &iter->m_usAbsoluteSpreadSize, 2);
			iPos += 2;
			memcpy(&cProcBuf[iPos], iter->m_pcAbsoluteSpread, iter->m_usAbsoluteSpreadSize);
			iPos += iter->m_usAbsoluteSpreadSize;
		}
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Generic XML文件信息
void QueryGenericSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oGenericStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsGenericStruct.begin();
		iter != pLineSetupData->m_olsGenericStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usLabelSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcLabel, iter->m_usLabelSize);
		iPos += iter->m_usLabelSize;
		memcpy(&cProcBuf[iPos], &iter->m_usLineSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcLine, iter->m_usLineSize);
		iPos += iter->m_usLineSize;
		memcpy(&cProcBuf[iPos], &iter->m_usSpreadSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcSpread, iter->m_usSpreadSize);
		iPos += iter->m_usSpreadSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Look XML文件信息
void QueryLookSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	memcpy(&cProcBuf[iPos], &pLineSetupData->m_oLook.m_uiAutoLook, 4);
	iPos += 4;
	memcpy(&cProcBuf[iPos], &pLineSetupData->m_oLook.m_uiResistance, 4);
	iPos += 4;
	memcpy(&cProcBuf[iPos], &pLineSetupData->m_oLook.m_uiTilt, 4);
	iPos += 4;
	memcpy(&cProcBuf[iPos], &pLineSetupData->m_oLook.m_uiLeakage, 4);
	iPos += 4;
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 LAULeakage XML文件信息
void QueryLAULeakageSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	memcpy(&cProcBuf[iPos], &pLineSetupData->m_oLAULeakage.m_uiLimit, 4);
	iPos += 4;
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 FormLine XML文件信息
void QueryFormLineSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oFormLineStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsFormLineStruct.begin();
		iter != pLineSetupData->m_olsFormLineStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiBoxType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiSN, 4);
		iPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 Instrument_SensorTestBase XML文件信息
void QueryInstrument_SensorTestBaseSetupData(char* cProcBuf, int& iPos, bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oInstrumentTestBaseStruct>::iterator iter;
	list<m_oInstrumentTestBaseStruct>* pList = NULL;
	if (bInstrument == true)
	{
		pList = &pLineSetupData->m_olsInstrumentTestBaseStruct;
	}
	else
	{
		pList = &pLineSetupData->m_olsSensorTestBaseStruct;
	}
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pList->begin();iter != pList->end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usDescrSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcDescr, iter->m_usDescrSize);
		iPos += iter->m_usDescrSize;
		memcpy(&cProcBuf[iPos], &iter->m_uiTestType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiADC, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiGain, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiDAC, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiFilter, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiSamplingRate, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiSamplingLength, 4);
		iPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 InstrumentTestLimit XML文件信息
void QueryInstrument_SensorTestLimitSetupData(char* cProcBuf, int& iPos, bool bInstrument, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oInstrumentTestLimitStruct>::iterator iter;
	list<m_oInstrumentTestLimitStruct>* pList = NULL;
	if (bInstrument == true)
	{
		pList = &pLineSetupData->m_olsInstrumentTestLimitStruct;
	}
	else
	{
		pList = &pLineSetupData->m_olsSensorTestLimitStruct;
	}
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pList->begin(); iter != pList->end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usDescrSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcDescr, iter->m_usDescrSize);
		iPos += iter->m_usDescrSize;
		memcpy(&cProcBuf[iPos], &iter->m_usUnitSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcUnit, iter->m_usUnitSize);
		iPos += iter->m_usUnitSize;
		memcpy(&cProcBuf[iPos], &iter->m_uiTestAim, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiTestType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_fLimit, 4);
		iPos += 4;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 InstrumentTest XML文件信息
void QueryInstrumentTestSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oInstrumentTestStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsInstrumentTestStruct.begin();
		iter != pLineSetupData->m_olsInstrumentTestStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiTestType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiGain, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiRecordLength, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiRecorded, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usAuxiliaryDescrSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcAuxiliaryDescr, iter->m_usAuxiliaryDescrSize);
		iPos += iter->m_usAuxiliaryDescrSize;
		memcpy(&cProcBuf[iPos], &iter->m_usAbsoluteSpreadSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcAbsoluteSpread, iter->m_usAbsoluteSpreadSize);
		iPos += iter->m_usAbsoluteSpreadSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 SensorTest XML文件信息
void QuerySensorTestSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	list<m_oSensorTestStruct>::iterator iter;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pLineSetupData->m_olsSensorTestStruct.begin();
		iter != pLineSetupData->m_olsSensorTestStruct.end(); iter++)
	{
		memcpy(&cProcBuf[iPos], &iter->m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiTestType, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_uiRecorded, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iter->m_usAbsoluteSpreadSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iter->m_pcAbsoluteSpread, iter->m_usAbsoluteSpreadSize);
		iPos += iter->m_usAbsoluteSpreadSize;
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 MultipleTest XML文件信息
void QueryMultipleTestSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	map<m_oMultipleTestKeyStruct, list<m_oMultipleTestTaskStruct>>::iterator iterMap;
	list<m_oMultipleTestTaskStruct>::iterator iter;
	unsigned int uiSize = 0;
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iterMap = pLineSetupData->m_oMultpleTestStructMap.begin();
		iterMap != pLineSetupData->m_oMultpleTestStructMap.end(); iterMap++)
	{
		memcpy(&cProcBuf[iPos], &iterMap->first.m_uiNb, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iterMap->first.m_usTestNameSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iterMap->first.m_pcTestName, iterMap->first.m_usTestNameSize);
		iPos += iterMap->first.m_usTestNameSize;
		memcpy(&cProcBuf[iPos], &iterMap->first.m_usAuxiliaryDescrSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iterMap->first.m_pcAuxiliaryDescr, iterMap->first.m_usAuxiliaryDescrSize);
		iPos += iterMap->first.m_usAuxiliaryDescrSize;
		memcpy(&cProcBuf[iPos], &iterMap->first.m_usAbsoluteSpreadSize, 2);
		iPos += 2;
		memcpy(&cProcBuf[iPos], iterMap->first.m_pcAbsoluteSpread, iterMap->first.m_usAbsoluteSpreadSize);
		iPos += iterMap->first.m_usAbsoluteSpreadSize;
		memcpy(&cProcBuf[iPos], &iterMap->first.m_uiDelayBetweenTest, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iterMap->first.m_uiRecordResults, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iterMap->first.m_uiRecordLength, 4);
		iPos += 4;
		memcpy(&cProcBuf[iPos], &iterMap->first.m_uiTestFileNb, 4);
		iPos += 4;
		uiSize = iterMap->second.size();
		memcpy(&cProcBuf[iPos], &uiSize, 4);
		iPos += 4;
		for (iter = iterMap->second.begin(); iter != iterMap->second.end(); iter++)
		{
			memcpy(&cProcBuf[iPos], &iter->m_uiLineNb, 4);
			iPos += 4;
			memcpy(&cProcBuf[iPos], &iter->m_uiTestType, 4);
			iPos += 4;
			memcpy(&cProcBuf[iPos], &iter->m_uiGain, 4);
			iPos += 4;
			memcpy(&cProcBuf[iPos], &iter->m_uiLoopLineNb, 4);
			iPos += 4;
			memcpy(&cProcBuf[iPos], &iter->m_uiNbLoops, 4);
			iPos += 4;
		}
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 查询 SeisMonitorTest XML文件信息
void QuerySeisMonitorSetupData(char* cProcBuf, int& iPos, m_oLineSetupDataStruct* pLineSetupData)
{
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	memcpy(&cProcBuf[iPos], &pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize, 2);
	iPos += 2;
	memcpy(&cProcBuf[iPos], 
		pLineSetupData->m_oSeisMonitor.m_pcAbsoluteSpread, 
		pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize);
	iPos += pLineSetupData->m_oSeisMonitor.m_usAbsoluteSpreadSize;
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
}

// 从XML配置文件得到测试数据限制值
float QueryTestDataLimit(bool bInstrument, string str, m_oLineSetupDataStruct* pLineSetupData)
{
	float fReturn = 0;
	list<m_oInstrumentTestLimitStruct>::iterator iter;
	list<m_oInstrumentTestLimitStruct>* pList = NULL;
	if (bInstrument == true)
	{
		pList = &pLineSetupData->m_olsInstrumentTestLimitStruct;
	}
	else
	{
		pList = &pLineSetupData->m_olsSensorTestLimitStruct;
	}
	EnterCriticalSection(&pLineSetupData->m_oSecCommInfo);
	for (iter = pList->begin(); iter != pList->end(); iter++)
	{
		if (0 == strcmp(iter->m_pcDescr, str.c_str()))
		{
			fReturn = iter->m_fLimit;
			break;
		}
	}
	LeaveCriticalSection(&pLineSetupData->m_oSecCommInfo);
	return fReturn;
}

// 释放测线客户端参数设置信息结构体缓冲区
void OnFreeLineXMLSetupData(m_oLineSetupDataStruct* pLineSetupData)
{
	// 重置测线客户端信息
	OnResetLineClientXMLSetupData(pLineSetupData);
	DeleteCriticalSection(&pLineSetupData->m_oSecCommInfo);
	delete pLineSetupData;
	pLineSetupData = NULL;
}