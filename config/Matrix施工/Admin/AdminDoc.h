// AdminDoc.h : CAdminDoc 类的接口
//


#pragma once


class CAdminDoc : public CDocument
{
protected: // 仅从序列化创建
	CAdminDoc();
	DECLARE_DYNCREATE(CAdminDoc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CAdminDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};


