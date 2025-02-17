#pragma once
#include "OScopeCtrl.h"
#include "ADCDataSocket.h"

// CGraphShowDlg 对话框

class CGraphShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CGraphShowDlg)

public:
	CGraphShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGraphShowDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_GRAPHSHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	// 图形模板
	COScopeCtrl m_OScopeCtrl;
protected:
	// 创建图形显示模板
	void CreateGraph(int iGraphViewId, COScopeCtrl* oScopeCtrl);
	// 设置采样取值范围
	void SetRange(double dLower, double dUpper, int nDecimalPlaces, COScopeCtrl* oScopeCtrl);
	// 设置Y轴标签
	void SetYUnits(CString str, COScopeCtrl* oScopeCtrl);
	// 设置X轴标签
	void SetXUnits(CString str, COScopeCtrl* oScopeCtrl);
	// 设置背景颜色
	void SetBackgroundColor(COLORREF color, COScopeCtrl* oScopeCtrl);
	// 设置网格颜色
	void SetGridColor(COLORREF color, COScopeCtrl* oScopeCtrl);
	// 设置画点颜色
	void SetPlotColor(COLORREF color, COScopeCtrl* oScopeCtrl);
	// 滚动条拖动到坐标点读取文件绘图
	void OnMoveScrollBarToPos(unsigned int uiPos);
public:
	// 显示ADC数据设备的序号
	unsigned int m_uiADCGraphNb;
	afx_msg void OnClose();
	// ADC数据接收Socket
	CADCDataSocket* m_ADCDataSocket;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 横向滚动条的最大值
	unsigned int m_uiScrollBarMax;
	// 设置横向滚动条范围
	void OnSetHScrollBarRange(void);
	// 读ADC数据文件
	void OnReadADCDataFile(CString csPathName, unsigned int uiPos, unsigned int uiPosNum);
	// 采集到的数据
	vector <double> m_dSampleData;
	// ADC数据自动存储文件数据点个数
	unsigned int m_uiADCFileLength;
	// 采样停止标志位
	BOOL m_bStopSample;
};
