// 绘图程序Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "绘图程序.h"
#include "绘图程序Dlg.h"
#include <math.h>
#include <algorithm>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// C绘图程序Dlg 对话框




C绘图程序Dlg::C绘图程序Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(C绘图程序Dlg::IDD, pParent)
	, m_currentDuration(0)
	, m_dateRange(0)
	, m_minDuration(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
C绘图程序Dlg::~C绘图程序Dlg()
{
	delete m_ChartViewer.getChart();
}
void C绘图程序Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ChartViewer, m_ChartViewer);
	DDX_Control(pDX, IDC_PointerPB, m_PointerPB);
	DDX_Control(pDX, IDC_XZoomPB, m_XZoomPB);
	DDX_Control(pDX, IDC_HScrollBar, m_HScrollBar);
	DDX_Control(pDX, IDC_VScrollBar, m_VScrollBar);
}

BEGIN_MESSAGE_MAP(C绘图程序Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PointerPB, &C绘图程序Dlg::OnBnClickedPointerpb)
	ON_BN_CLICKED(IDC_ZoomInPB, &C绘图程序Dlg::OnBnClickedZoominpb)
	ON_BN_CLICKED(IDC_ZoomOutPB, &C绘图程序Dlg::OnBnClickedZoomoutpb)
	ON_BN_CLICKED(IDC_XZoomPB, &C绘图程序Dlg::OnBnClickedXzoompb)
	ON_BN_CLICKED(IDC_XYZoomPB, &C绘图程序Dlg::OnBnClickedXyzoompb)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_CONTROL(CVN_ViewPortChanged, IDC_ChartViewer, OnViewPortChanged)
	ON_BN_CLICKED(IDC_ChartViewer, OnBnClickedChartViewer)
END_MESSAGE_MAP()


// C绘图程序Dlg 消息处理程序

BOOL C绘图程序Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// Load icons to mouse usage buttons
	loadButtonIcon(IDC_PointerPB, IDI_PointerPB, 100, 20);  
	loadButtonIcon(IDC_ZoomInPB, IDI_ZoomInPB, 100, 20);    
	loadButtonIcon(IDC_ZoomOutPB, IDI_ZoomOutPB, 100, 20);

	// Load icons to zoom/scroll direction control buttons
	loadButtonIcon(IDC_XZoomPB, IDI_XZoomPB, 105, 20);
	loadButtonIcon(IDC_XYZoomPB, IDI_XYZoomPB, 105, 20);

	//
	// Initialize member variables
	//
	m_extBgColor = getDefaultBgColor();     // Default background color
	m_minValue = m_maxValue = 0;            // y axes ranges

	LoadData();

	setLicenseCode("R5MNGUVV3UXWFT2CC44B1D7E"); //破解chartdirector

	// Earliest date is the first timestamp
	m_minDate = m_timeStamps[0];
	// Duration is the seconds elapsed of the last timestamp from the earliest date
	m_dateRange = m_timeStamps[m_timeStamps.len - 1] - m_minDate;

	m_minDuration = 10;
	// Set up the ChartViewer to reflect the visible and minimum duration
	m_ChartViewer.setZoomInWidthLimit(m_minDuration / m_dateRange);
	m_ChartViewer.setViewPortWidth(m_currentDuration / m_dateRange);
	m_ChartViewer.setViewPortLeft(1 - m_ChartViewer.getViewPortWidth());

	// Initially set the mouse to drag to scroll mode in horizontal direction.
	m_PointerPB.SetCheck(1);
	m_XZoomPB.SetCheck(1);
	m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);

	// Can update chart now
	m_ChartViewer.updateViewPort(true, true);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void C绘图程序Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void C绘图程序Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR C绘图程序Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 载入一个图标资源到按钮
void C绘图程序Dlg::loadButtonIcon(int buttonId, int iconId, int width, int height)
{
	GetDlgItem(buttonId)->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)::LoadImage(
		AfxGetResourceHandle(), MAKEINTRESOURCE(iconId), IMAGE_ICON, width, height, 
		LR_DEFAULTCOLOR));  
}

// 得到默认的背景颜色
int C绘图程序Dlg::getDefaultBgColor(void)
{
	LOGBRUSH LogBrush; 
	HBRUSH hBrush = (HBRUSH)SendMessage(WM_CTLCOLORDLG, (WPARAM)CClientDC(this).m_hDC, 
		(LPARAM)m_hWnd); 
	::GetObject(hBrush, sizeof(LOGBRUSH), &LogBrush); 
	int ret = LogBrush.lbColor;
	return ((ret & 0xff) << 16) | (ret & 0xff00) | ((ret & 0xff0000) >> 16);
}

// 载入数据
void C绘图程序Dlg::LoadData(void)
{
	// The data for the line chart
	for (int i=0; i<2000; i++)
	{
		timeTemp[i] = i;
		data0[i] = 1 + sin(3.1415926*i/2000);
		data1[i] = 2 + sin(3.1415926*i/2000);
		data2[i] = 3 + sin(3.1415926*i/2000);
	}

	// The initial view port is to show 1 year of data.
	m_currentDuration = 100;
	//
	// Get the data and stores them in a memory buffer for fast scrolling / zooming. In 
	// this demo, we just use a random number generator. In practice, you may get the data
	// from a database or XML or by other means.

	// Read random data into the data arrays
	m_timeStamps = DoubleArray(timeTemp, 2000);
	m_dataSeriesA = DoubleArray(data0, 2000);
	m_dataSeriesB = DoubleArray(data1, 2000);
	m_dataSeriesC = DoubleArray(data2, 2000);
}

void C绘图程序Dlg::OnBnClickedPointerpb()
{
	// TODO: 在此添加控件通知处理程序代码
	 m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);
}

void C绘图程序Dlg::OnBnClickedZoominpb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomIn);
}

void C绘图程序Dlg::OnBnClickedZoomoutpb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomOut);
}

void C绘图程序Dlg::OnBnClickedXzoompb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setZoomDirection(Chart::DirectionHorizontal); 
	m_ChartViewer.setScrollDirection(Chart::DirectionHorizontal);

	// Viewport is always unzoomed as y-axis is auto-scaled
	m_ChartViewer.setViewPortTop(0);
	m_ChartViewer.setViewPortHeight(1);

	// Update chart to auto-scale axis
	m_ChartViewer.updateViewPort(true, true);
}

void C绘图程序Dlg::OnBnClickedXyzoompb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setZoomDirection(Chart::DirectionHorizontalVertical); 
	m_ChartViewer.setScrollDirection(Chart::DirectionHorizontalVertical);  
}

void C绘图程序Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode != SB_ENDSCROLL)
	{
		// User is still scrolling

		// Set the view port based on the scroll bar
		m_ChartViewer.setViewPortLeft(moveScrollBar(nSBCode, nPos, pScrollBar));
		// Update the chart image only, but no need to update the image map.
		m_ChartViewer.updateViewPort(true, false);
	}
	else
		// Scroll bar has stoped moving. Can update image map now.
		m_ChartViewer.updateViewPort(false, true);
}

void C绘图程序Dlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode != SB_ENDSCROLL)
	{
		// User is still scrolling

		// Set the view port based on the scroll bar
		m_ChartViewer.setViewPortTop(moveScrollBar(nSBCode, nPos, pScrollBar));
		// Update the chart image only, but no need to update the image map.
		m_ChartViewer.updateViewPort(true, false);
	}
	else
		// Scroll bar has stoped moving. Can update image map now.
		m_ChartViewer.updateViewPort(false, true);
}
//
// CChartViewer ViewPortChanged event
//
void C绘图程序Dlg::OnViewPortChanged()
{
	//
	// Set up the scroll bars to reflect the current position and size of the view port
	//
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_ALL;
	info.nMin = 0;
	info.nMax = 0x1fffffff;

	m_HScrollBar.EnableWindow(m_ChartViewer.getViewPortWidth() < 1);
	if (m_ChartViewer.getViewPortWidth() < 1)
	{
		info.nPage = (int)ceil(m_ChartViewer.getViewPortWidth() * (info.nMax - info.nMin));
		info.nPos = (int)(0.5 + m_ChartViewer.getViewPortLeft() * (info.nMax - info.nMin))
			+ info.nMin;
		m_HScrollBar.SetScrollInfo(&info);
	}

	m_VScrollBar.EnableWindow(m_ChartViewer.getViewPortHeight() < 1);
	if (m_ChartViewer.getViewPortHeight() < 1)
	{
		info.nPage = (int)ceil(m_ChartViewer.getViewPortHeight() * (info.nMax - info.nMin));
		info.nPos = (int)(0.5 + m_ChartViewer.getViewPortTop() * (info.nMax - info.nMin))
			+ info.nMin;
		m_VScrollBar.SetScrollInfo(&info);
	}

	// Compute the start date (in chartTime) and duration (in seconds) of the view port
	m_currentDuration = (int)(0.5 + m_ChartViewer.getViewPortWidth() * m_dateRange);
	//
	// Update chart and image map if necessary
	//
	if (m_ChartViewer.needUpdateChart())
		drawChart(&m_ChartViewer);
	if (m_ChartViewer.needUpdateImageMap())
		updateImageMap(&m_ChartViewer);
}
//
// User clicks on the CChartViewer
//
void C绘图程序Dlg::OnBnClickedChartViewer() 
{
	ImageMapHandler *handler = m_ChartViewer.getImageMapHandler();
	if (0 != handler)
	{
		//
		// Query the ImageMapHandler to see if the mouse is on a clickable hot spot. We 
		// consider the hot spot as clickable if its href ("path") parameter is not empty.
		//
		const char *path = handler->getValue("path");
		if ((0 != path) && (0 != *path))
		{
			// In this sample code, we just show all hot spot parameters.
			// @@@@@@@@
// 			CHotSpotDlg hs;
// 			hs.SetData(handler);
// 			hs.DoModal();
		}
	}
}

//
// Handle scroll bar events
//
double C绘图程序Dlg::moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//
	// Get current scroll bar position
	//
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_ALL;
	pScrollBar->GetScrollInfo(&info);

	//
	// Compute new position based on the type of scroll bar events
	//
	int newPos = info.nPos;
	switch (nSBCode)
	{
	case SB_LEFT:
		newPos = info.nMin;
		break;
	case SB_RIGHT:
		newPos = info.nMax;
		break;
	case SB_LINELEFT:
		newPos -= (info.nPage > 10) ? info.nPage / 10 : 1;
		break;
	case SB_LINERIGHT:
		newPos += (info.nPage > 10) ? info.nPage / 10 : 1;
		break;
	case SB_PAGELEFT:
		newPos -= info.nPage;
		break;
	case SB_PAGERIGHT:
		newPos += info.nPage;
		break;
	case SB_THUMBTRACK:
		newPos = info.nTrackPos;
		break;
	}
	if (newPos < info.nMin) newPos = info.nMin;
	if (newPos > info.nMax) newPos = info.nMax;

	// Update the scroll bar with the new position
	pScrollBar->SetScrollPos(newPos);

	// Returns the position of the scroll bar as a ratio of its total length
	return ((double)(newPos - info.nMin)) / (info.nMax - info.nMin);
}
//
// Draw the chart and display it in the given viewer
//
void C绘图程序Dlg::drawChart(CChartViewer *viewer)
{
	// @@@@@@
	//
	// In this demo, we copy the visible part of the data to a separate buffer for chart
	// plotting. 
	//
	// Note that if you only have a small amount of data (a few hundred data points), it
	// may be easier to just plot all data in any case (so the following copying code is 
	// not needed), and let ChartDirector "clip" the chart to the plot area. 
	//

	// Using ViewPortLeft and ViewPortWidth, get the start and end dates of the view port.
	double viewPortStartDate = m_minDate + (__int64)(viewer->getViewPortLeft() * 
		m_dateRange + 0.5);
	double viewPortEndDate = viewPortStartDate + (__int64)(viewer->getViewPortWidth() * 
		m_dateRange + 0.5);

	// Get the starting index of the array using the start date
	int startIndex = (int)(std::lower_bound(m_timeStamps.data, m_timeStamps.data + m_timeStamps.len,
		viewPortStartDate) - m_timeStamps.data);
	if ((startIndex > 0) && (m_timeStamps[startIndex] != viewPortStartDate)) 
		--startIndex;

	// Get the ending index of the array using the end date
	int endIndex = (int)(std::upper_bound(m_timeStamps.data, m_timeStamps.data + m_timeStamps.len, 
		viewPortEndDate) - m_timeStamps.data);
	if (endIndex >= m_timeStamps.len - 1)
		endIndex = m_timeStamps.len - 1;

	// Get the length
	int noOfPoints = endIndex - startIndex + 1;

	// We copy the visible data from the main arrays to separate data arrays
	double* viewPortTimeStamps = new double[noOfPoints];
	double* viewPortDataSeriesA = new double[noOfPoints];
	double* viewPortDataSeriesB = new double[noOfPoints];
	double* viewPortDataSeriesC = new double[noOfPoints];
	int arraySizeInBytes = noOfPoints * sizeof(double);
	memcpy(viewPortTimeStamps, m_timeStamps.data + startIndex, arraySizeInBytes);
	memcpy(viewPortDataSeriesA, m_dataSeriesA.data + startIndex, arraySizeInBytes);
	memcpy(viewPortDataSeriesB, m_dataSeriesB.data + startIndex, arraySizeInBytes);
	memcpy(viewPortDataSeriesC, m_dataSeriesC.data + startIndex, arraySizeInBytes);

	if (noOfPoints >= 520)
	{
		//
		// Zoomable chart with high zooming ratios often need to plot many thousands of 
		// points when fully zoomed out. However, it is usually not needed to plot more
		// data points than the resolution of the chart. Plotting too many points may cause
		// the points and the lines to overlap. So rather than increasing resolution, this 
		// reduces the clarity of the chart. So it is better to aggregate the data first if
		// there are too many points.
		//
		// In our current example, the chart only has 520 pixels in width and is using a 2
		// pixel line width. So if there are more than 520 data points, we aggregate the 
		// data using the ChartDirector aggregation utility method.
		//
		// If in your real application, you do not have too many data points, you may 
		// remove the following code altogether.
		//

		// Set up an aggregator to aggregate the data based on regular sized slots
		ArrayMath m(DoubleArray(viewPortTimeStamps, noOfPoints));
		m.selectRegularSpacing(noOfPoints / 260);

		// For the timestamps, take the first timestamp on each slot
		int aggregatedNoOfPoints = m.aggregate(DoubleArray(viewPortTimeStamps, noOfPoints), 
			Chart::AggregateFirst).len;

		// For the data values, aggregate by taking the averages
		m.aggregate(DoubleArray(viewPortDataSeriesA, noOfPoints), Chart::AggregateAvg);
		m.aggregate(DoubleArray(viewPortDataSeriesB, noOfPoints), Chart::AggregateAvg);
		m.aggregate(DoubleArray(viewPortDataSeriesC, noOfPoints), Chart::AggregateAvg);

		noOfPoints = aggregatedNoOfPoints;
	}

	//
	// Now we have obtained the data, we can plot the chart. 
	//

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 1 - Configure overall chart appearance. 
	///////////////////////////////////////////////////////////////////////////////////////

	// Create an XYChart object 600 x 300 pixels in size, with pale blue (0xf0f0ff) 
	// background, black (000000) border, 1 pixel raised effect, and with a rounded frame.
	XYChart *c = new XYChart(300, 550, 0xf0f0ff, 0, 1);
	c->setRoundedFrame(m_extBgColor);

	// Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white
	// background. Turn on both horizontal and vertical grid lines with light grey
	// color (0xcccccc). Set clipping mode to clip the data lines to the plot area.
	c->setPlotArea(30, 45, 195, 450, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);
	c->setClipping();

	// Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 9
	// pts Arial Bold font. Set the background and border color to Transparent.
	c->addLegend(225, 45, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);

	// Add a title box to the chart using 15 pts Times Bold Italic font, on a light
	// blue (CCCCFF) background with glass effect. white (0xffffff) on a dark red
	// (0x800000) background, with a 1 pixel 3D border.
	c->addTitle("Hitech Matrix428", "timesbi.ttf", 12)->setBackground(
		0xccccff, 0x000000, Chart::glassEffect());

// 	// Swap the x and y axis to become a rotated chart
 	c->swapXY();
	 
	// Set the y axis on the top side (right + rotated = top)
	c->setYAxisOnRight(false);

	// Reverse the x axis so it is pointing downwards
	c->xAxis()->setReverse();

	// Set the labels on the x axis.
	//	c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));


	// Display 1 out of 3 labels on the x-axis.
//	c->xAxis()->setLabelStep(3);

	// 	// Add a title to the x axis
	// 	c->xAxis()->setTitle("Jun 12, 2006");

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 2 - Add data to chart
	///////////////////////////////////////////////////////////////////////////////////////

	// 
	// In this example, we represent the data by lines. You may modify the code below if 
	// you want to use other representations (areas, scatter plot, etc).
	//

	// Add a line layer to the chart
	LineLayer *layer = c->addLineLayer();

	// Set the default line width to 2 pixels
	layer->setLineWidth(2);

	// Set the axes width to 2 pixels
	c->xAxis()->setWidth(2);
	c->yAxis()->setWidth(2);

	// Now we add the 3 data series to a line layer, using the color red (ff0000), green
	// (00cc00) and blue (0000ff)
	layer->setXData(DoubleArray(viewPortTimeStamps, noOfPoints));
	layer->addDataSet(DoubleArray(viewPortDataSeriesA, noOfPoints), 0xff0000, "FDU #1");
	layer->addDataSet(DoubleArray(viewPortDataSeriesB, noOfPoints), 0x00cc00, "FDU #2");
	layer->addDataSet(DoubleArray(viewPortDataSeriesC, noOfPoints), 0x0000ff, "FDU #3");

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 3 - Set up x-axis scale
	///////////////////////////////////////////////////////////////////////////////////////

	// Set x-axis date scale to the view port date range. 
	c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

	// Configure the x-axis to auto-scale with at least 75 pixels between major tick and 
	// 15  pixels between minor ticks. This shows more minor grid lines on the chart.
//	c->xAxis()->setTickDensity(80, 8);

	//
	// In the current demo, the x-axis range can be from a few years to a few days. We can 
	// let ChartDirector auto-determine the date/time format. However, for more beautiful 
	// formatting, we set up several label formats to be applied at different conditions. 
	//

// 	// If all ticks are yearly aligned, then we use "yyyy" as the label format.
// 	c->xAxis()->setFormatCondition("align", 360 * 86400);
// 	c->xAxis()->setLabelFormat("{value|yyyy}");
// 
// 	// If all ticks are monthly aligned, then we use "mmm yyyy" in bold font as the first 
// 	// label of a year, and "mmm" for other labels.
// 	c->xAxis()->setFormatCondition("align", 30 * 86400);
// 	c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "<*font=bold*>{value|mmm yyyy}", 
// 		Chart::AllPassFilter(), "{value|mmm}");
// 
// 	// If all ticks are daily algined, then we use "mmm dd<*br*>yyyy" in bold font as the 
// 	// first label of a year, and "mmm dd" in bold font as the first label of a month, and
// 	// "dd" for other labels.
// 	c->xAxis()->setFormatCondition("align", 86400);
// 	c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), 
// 		"<*block,halign=left*><*font=bold*>{value|mmm dd<*br*>yyyy}", 
// 		Chart::StartOfMonthFilter(), "<*font=bold*>{value|mmm dd}");
// 	c->xAxis()->setMultiFormat(Chart::AllPassFilter(), "{value|dd}");
// 
// 	// For all other cases (sub-daily ticks), use "hh:nn<*br*>mmm dd" for the first label of
// 	// a day, and "hh:nn" for other labels.
// 	c->xAxis()->setFormatCondition("else");
// 	c->xAxis()->setMultiFormat(Chart::StartOfDayFilter(), 
// 		"<*font=bold*>{value|hh:nn<*br*>mmm dd}", Chart::AllPassFilter(), "{value|hh:nn}");

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 4 - Set up y-axis scale
	///////////////////////////////////////////////////////////////////////////////////////

	if ((viewer->getZoomDirection() == Chart::DirectionHorizontal) || 
		((m_minValue == 0) && (m_maxValue == 0)))
	{
		// y-axis is auto-scaled - save the chosen y-axis scaled to support xy-zoom mode
		c->layout();
		m_minValue = c->yAxis()->getMinValue();
		m_maxValue = c->yAxis()->getMaxValue();
	}
	else
	{
		// xy-zoom mode - compute the actual axis scale in the view port 
		double axisLowerLimit =  m_maxValue - (m_maxValue - m_minValue) * 
			(viewer->getViewPortTop() + viewer->getViewPortHeight());
		double axisUpperLimit =  m_maxValue - (m_maxValue - m_minValue) * 
			viewer->getViewPortTop();
		// *** use the following formula if you are using a log scale axis ***
		// double axisLowerLimit = m_maxValue * pow(m_minValue / m_maxValue, 
		//  viewer->getViewPortTop() + viewer->getViewPortHeight());
		// double axisUpperLimit = m_maxValue * pow(m_minValue / m_maxValue, 
		//  viewer->getViewPortTop());

		// use the zoomed-in scale
		c->yAxis()->setLinearScale(axisLowerLimit, axisUpperLimit);
		c->yAxis()->setRounding(false, false);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 5 - Display the chart
	///////////////////////////////////////////////////////////////////////////////////////

	// Set the chart image to the WinChartViewer
	delete m_ChartViewer.getChart();
	m_ChartViewer.setChart(c);
	// 	// Output the chart
	// 	c->makeChart("Matrix428.bmp");
	// 
	// 	//free up resources
	// 	delete c;

	// Free resources
	delete[] viewPortTimeStamps;
	delete[] viewPortDataSeriesA;
	delete[] viewPortDataSeriesB;
	delete[] viewPortDataSeriesC;
}
//
// Update the image map
//
void C绘图程序Dlg::updateImageMap(CChartViewer *viewer)
{
	if (0 == viewer->getImageMapHandler())
	{
		// no existing image map - creates a new one
		viewer->setImageMap(viewer->getChart()->getHTMLImageMap("clickable", "",
			"title='[{dataSetName}] {x|1}: Volt {value|9}'"));
	}
}