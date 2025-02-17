#pragma once

#include "DetourData.h"

/**
*@brief 迂回道数据队列类
*/
class CDetourList
{
public:
	CDetourList();
	~CDetourList();

public: //属性
	/** XMLDOM文件指针*/
	CXMLDOMDocument* m_pXMLDOMDocument;
	/** 迂回道总数*/
	unsigned int m_uiCountAll;
	/** 迂回道队列*/
	CList<CDetourData*, CDetourData*> m_olsDetour;

	/** 迂回道数据指针*/
	CDetourData* m_pDetourData;

public: //方法
	// 初始化
	void OnInit();
	// 重新加载
	void OnReload();
	// 关闭
	void OnClose();

	// 解析配置文件
	void ParseXML();
	// 根据索引号，得到迂回道
	CDetourData* GetDetour(unsigned int uiIndex);
};
