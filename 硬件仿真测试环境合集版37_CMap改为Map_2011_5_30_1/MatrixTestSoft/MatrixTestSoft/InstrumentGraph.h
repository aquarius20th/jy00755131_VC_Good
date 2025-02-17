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
	// 客户区尺寸
	CRect  m_rectClient;
	// 背景刷子
	CBrush m_brushBack;
	CBitmap* m_pbitmapOldGrid;
	// 界面指针
	CWnd* m_pWnd;
	// 仪器图元结构体
	typedef struct InstrumentGraph
	{
		// 仪器绘图区域
		CRect oRectInstrument;
		// 连接线绘图区域
		CRect oRectLine;
		// 点号
		int iUnitIndex;
		// 测线号
		int iLineIndex;
		// 采集站序号
		unsigned int uiFDUIndex;
		// SN号
		unsigned int uiSN;
		// 连接线方向
		unsigned int uiLineDirection;
		// 仪器类型
		unsigned int uiType;
		// 仪器操作
		unsigned int uiOpt;
		bool operator == (const InstrumentGraph& v2) const
		{
			return ((iUnitIndex == v2.iUnitIndex)
				&& (iLineIndex == v2.iLineIndex));
		}
	}m_oGraph;
	// 存储仪器图元绘图区
	CList <m_oGraph, m_oGraph> m_oInstrumentGraphRectList;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();

	// 绘制仪器单元（包含连接线）
	void DrawUnit(int iUnitIndex, int iLineIndex, unsigned int uiLineDirection, unsigned int uiType, 
		unsigned int uiSN, unsigned int uiOpt);
	// 删除仪器单元（包含连接线）
	void CInstrumentGraph::DelUnit(int iUnitIndex, int iLineIndex);
private:
	CDC m_dcGraph;
	// 绘图区尺寸
	CRect m_rectGraph;
	// 纵向滚动条CWnd指针
	CWnd* m_pWndVScr;
	// 横向滚动条CWnd指针
	CWnd* m_pWndHScr;
	// 静态控件CWnd指针
	CWnd* m_pWndStatic;
	//定义一个位图对象
	CBitmap m_oBmpFDU1;
	//定义一个位图对象
	CBitmap m_oBmpFDU2;
	//定义一个位图对象
	CBitmap m_oBmpLAUL1;
	//定义一个位图对象
	CBitmap m_oBmpLAUL2;
	//定义一个位图对象
	CBitmap m_oBmpLAUX1;
	//定义一个位图对象
	CBitmap m_oBmpLAUX2;
	//定义一个位图对象
	CBitmap m_oBmpLCI1;
	//定义一个位图对象
	CBitmap m_oBmpLCI2;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
private:
	// 设备绘图单元X方向尺寸
	unsigned int m_uiGridX;
	// 设备Y方向绘图尺寸
	unsigned int m_uiGridY;
	// 连线X方向绘图尺寸
	unsigned int m_uiGridLineX;
	// 连线Y方向绘图尺寸
	unsigned int m_uiGridLineY;
	// 判断界面仪器是否被选中
	BOOL OnSelectInstrument(CPoint oGraphPoint, m_oGraph &oInstrumentGraph);
	// 滚动条拖动
	int moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 将信息显示区域填充为背景色
	void OnFillMsgAreaBkColor(void);
public:
	// 填充背景颜色
	void OnFillBkColor(CDC* pDC, CRect oRect);
	// 清除所有仪器图形
	void OnClearAllInstrumentGraph(void);
private:
	// 坐标信息
	int m_iPosShowInterval;
	// 纵向滚动条宽度
	int m_iVScrBarInterval;
	// 横向滚动条宽度
	int m_iHScrBarInterval;
	// 主交叉站仪器起始位置X轴坐标
	int m_iLauxPosX;
	// 主交叉站仪器起始位置Y轴坐标
	int m_iLauxPosY;
public:
	// 右侧增加仪器移动图形DC的大小
	int m_iRightMovePos;
	// 左侧增加仪器移动图形DC的大小
	int m_iLeftMovePos;
	// 处理绘图区域
	void AddGraphView(unsigned int uiUnitNum, unsigned int uiLineDirection);
	// 在图形界面上输出文字信息
	void OnShowTextOut(CDC* pDC, CPoint point, CString str);
private:
	// 横向滚动条坐标
	int m_iHScrPos;
public:
	// 重置
	void OnReset(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	// 计算完采集站序号后才能在界面左键选择仪器
	bool m_bLButtonSelect;
	// 载入仪器图像
	void OnLoadInstrumentBmp(CBitmap* pBmp, int iBmpX, int iBmpY);
private:
	// 绘制设备图标志位
	bool m_bDrawUnit;
public:
	// 回收绘图区域资源
	void DelGraphView(unsigned int uiUnitNum, unsigned int uiLineDirection);
};