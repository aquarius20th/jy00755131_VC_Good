// 绘图程序Dlg.h : 头文件
//

#pragma once
#include "ChartViewer.h"
#include "afxwin.h"
#include "Parameter.h"
// C绘图程序Dlg 对话框
class C绘图程序Dlg : public CDialog
{

// 构造
public:
	C绘图程序Dlg(CWnd* pParent = NULL);	// 标准构造函数
	~C绘图程序Dlg();

// 对话框数据
	enum { IDD = IDD_MY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	
	int m_extBgColor;						// 默认的背景颜色
	double m_minData;						// 横坐标的最小值
	double m_maxData;					// 横坐标的最大值
	double m_dateRange;					// 横坐标的数值变化范围
	double m_maxValue;					// 纵坐标的最大值
	double m_minValue;					// 纵坐标的最小值
	unsigned int m_uiIntervalNum;	// 绘图区域左侧间隔个数

	DoubleArray m_timeStamps;		// 横坐标数值所在双数组
	DoubleArray m_dataSeriesA;		// 第一条曲线所在双数组
	DoubleArray m_dataSeriesB;		// 第二条曲线所在双数组
	DoubleArray m_dataSeriesC;		// 第三条曲线所在双数组
public:	
	CChartViewer	m_ChartViewer;	// 添加绘图控件的控制变量
	double m_currentDuration;			// 当前显示数据点的个数
	CButton m_PointerPB;				// 按键Pointer的控制变量
	CButton m_XZoomPB;				// 按键X Zoom/Y Auto的控制变量
	CScrollBar m_HScrollBar;			// 横向滚动条的控制变量
	CScrollBar m_VScrollBar;			// 纵向滚动条的控制变量
	CComboBox m_Duration;			// 绘图显示点数选项卡控制变量
	double timeTemp[2000];				// 横坐标数值数组
	double data0[2000];					// 第一条曲线数组	
	double data1[2000];					// 第二条曲线数组
	double data2[2000];					// 第三条曲线数组
	double m_minDuration;				// 最少显示点的个数
private:
	// 得到默认的背景颜色
	int getDefaultBgColor(void);
	// 载入数据
	void LoadData(void);
	// 载入一个图标资源到按钮
	void loadButtonIcon(int buttonId, int iconId, int width, int height);
	// 当用户选中时移动滚动条
	double moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 绘制图形
	void drawChart(CChartViewer *viewer);
	// 重绘图片
	void updateImageMap(CChartViewer *viewer);
	// 验证用户输入的显示点数
	void validateDuration(const CString &text);
public:
	afx_msg void OnBnClickedPointerpb();
	afx_msg void OnBnClickedZoominpb();
	afx_msg void OnBnClickedZoomoutpb();
	afx_msg void OnBnClickedXzoompb();
	afx_msg void OnBnClickedXyzoompb();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnViewPortChanged();
	afx_msg void OnBnClickedChartViewer();
	afx_msg void OnCbnSelchangeDuration();
	afx_msg void OnCbnKillfocusDuration();
};
