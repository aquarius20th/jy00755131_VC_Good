#include "StdAfx.h"
#include "SensorList.h"

CSensorList::CSensorList(void)
{
}

CSensorList::~CSensorList(void)
{
}

/**
* 初始化
* @param void
* @return void
*/
void CSensorList::OnInit()
{
	m_uiCountAll = 0;	// 检波器总数	
	ParseXML();	// 解析配置文件
}

/**
* 重新加载
* @param void
* @return void
*/
void CSensorList::OnReload(void)
{
	m_uiCountAll = 0;	// 检波器总数
	m_olsSensor.RemoveAll();	// 移除队列
	ParseXML();	// 解析配置文件
}

/**
* 关闭
* @param void
* @return void
*/
void CSensorList::OnClose()
{
	m_uiCountAll = 0;	// 检波器总数
	m_olsSensor.RemoveAll();	// 移除队列
}

/**
* 解析配置文件
* @param void
* @return void
*/
void CSensorList::ParseXML()
{
	CString strKey;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElement;
	LPDISPATCH lpDispatch;
	unsigned int i;

	// 找到检波器设置区
	strKey = "SensorSetup";
	lpDispatch = m_pXMLDOMDocument->getElementsByTagName(strKey);
	oNodeList.AttachDispatch(lpDispatch);
	// 找到入口
	lpDispatch = oNodeList.get_item(0);
	oElement.AttachDispatch(lpDispatch);
	// 得到检波器总数
	strKey = "Count";
	m_uiCountAll = CXMLDOMTool::GetElementAttributeInt(&oElement, strKey);
	// 得到Tab键数量
	strKey = "TabCount";
	m_uiTabCount = CXMLDOMTool::GetElementAttributeInt(&oElement, strKey);

	m_strTabChild = "\r\n";
	m_strTabParent = "\r\n";
	for(i = 0; i < m_uiTabCount; i++)
	{
		m_strTabChild = m_strTabChild + "\t";
	}
	for(i = 0; i < m_uiTabCount - 1; i++)
	{
		m_strTabParent = m_strTabParent + "\t";
	}

	// 得到队列
	lpDispatch = oElement.get_childNodes();
	oNodeList.AttachDispatch(lpDispatch);

	CSensorData oSensor;
	for(i = 0; i < m_uiCountAll; i++)
	{
		lpDispatch = oNodeList.get_item(i);
		oElement.AttachDispatch(lpDispatch);
		oSensor.ParseXML(&oElement);
		// 增加检波器
		m_olsSensor.AddTail(oSensor);
	}
}

/**
* 保存配置文件
* @param void
* @return void
*/
void CSensorList::OnSave()
{
	CString strKey;
	COleVariant oVariant;
	CXMLDOMNodeList oNodeList;
	CXMLDOMElement oElementParent, oElementChild;
	LPDISPATCH lpDispatch;

	// 找到检波器设置区
	strKey = "SensorSetup";
	lpDispatch = m_pXMLDOMDocument->getElementsByTagName(strKey);
	oNodeList.AttachDispatch(lpDispatch);
	// 找到入口
	lpDispatch = oNodeList.get_item(0);
	oElementParent.AttachDispatch(lpDispatch);
	// 设置总数
	strKey = "Count";
	oVariant = (long)m_uiCountAll;
	oElementParent.setAttribute(strKey, oVariant);

	// 删除所有子节点
	while(TRUE == oElementParent.hasChildNodes())
	{
		lpDispatch = oElementParent.get_firstChild();
		oElementParent.removeChild(lpDispatch);
	}
	// 增加新节点
	CSensorData oSensor;
	for(unsigned int i = 0; i < m_uiCountAll; i++)
	{
		oSensor = m_olsSensor.RemoveHead();

		lpDispatch = m_pXMLDOMDocument->createTextNode(m_strTabChild);
		oElementParent.appendChild(lpDispatch);

		lpDispatch = m_pXMLDOMDocument->createElement("Record");
		oElementChild.AttachDispatch(lpDispatch);
		oSensor.AddToXML(&oElementChild);
		oElementParent.appendChild(lpDispatch);

		if(i == (m_uiCountAll - 1))
		{
			lpDispatch = m_pXMLDOMDocument->createTextNode(m_strTabParent);
			oElementParent.appendChild(lpDispatch);
		}		
	}
}
