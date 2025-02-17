#pragma once


// CInstrumentGraph

class CInstrumentGraph : public CWnd
{
	DECLARE_DYNAMIC(CInstrumentGraph)

public:
	CInstrumentGraph();
	virtual ~CInstrumentGraph();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CRect  m_rectClient;
	CBrush m_brushBack;

	CDC     m_dcGraph;
	CBitmap *m_pbitmapOldGrid;
	CBitmap m_bitmapGrid;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// 绘制仪器图形
	void OnDrawInstrumentGraph(void);
	afx_msg void OnPaint();

	// 绘制所有仪器
	void DrawUnitAll(CDC* pDC);
	// 绘制仪器单元（包含连接线）
	void DrawUnit(CDC* pDC, int iUnitIndex, int iLineNum, unsigned int uiLineDirection, unsigned int uiType);
private:
	// 开始绘制仪器时X轴坐标
	int m_iPosX;
	// 开始绘制仪器时Y轴坐标
	int m_iPosY;
	// 仪器图元结构体
	typedef struct InstrumentGraph
	{
		// 仪器绘图区域
		CRect oRect;
		// 点号
		int iUnitIndex;
		// 测线号
		int iLineNum;
	}m_oInstrumentGraph;
	// 存储仪器图元绘图区
	CList <m_oInstrumentGraph, m_oInstrumentGraph> m_oInstrumentGraphRectList;
	// 鼠标移动记录显示区坐标
	CRect m_oRectMove;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
private:
	// 设备绘图单元X方向尺寸
	unsigned int m_uiGridX;
	// 设备Y方向绘图尺寸
	unsigned int m_uiGridY;
	// 连线X方向绘图尺寸
	unsigned int m_uiGridLineX;
	// 连线Y方向绘图尺寸
	unsigned int m_uiGridLineY;
	// 显示仪器的线号和点号
	BOOL m_bShowInstrumentLabel;
	// 显示仪器坐标（包含测线号和点号）
	void OnShowInstrumentAxisPoint(CPoint point);
};


