// HardwareTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "模拟示波器图形显示.h"
#include "HardwareTestDlg.h"
#include <math.h>

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


// CHardwareTestDlg 对话框




CHardwareTestDlg::CHardwareTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHardwareTestDlg::IDD, pParent)
	, m_bStartSample(FALSE)
	, m_dSampleTime(0)
	, m_bSelectObject1(1)
	, m_bSelectObject2(1)
	, m_bSelectObject3(1)
	, m_bSelectObject4(1)
	, m_bSelectObject5(1)
	, m_bSelectObject6(1)
	, m_bSelectObject7(1)
	, m_bSelectObject8(1)
	, m_cSelectObjectName(NULL)
	, strFilePath("")
	, m_bStopSample(FALSE)
	, m_uiSampleNb(0)
	, m_bSelectObjectNoise1(0)
	, m_bSelectObjectNoise2(0)
	, m_bSelectObjectNoise3(0)
	, m_bSelectObjectNoise4(0)
	, m_bSelectObjectNoise5(0)
	, m_bSelectObjectNoise6(0)
	, m_bSelectObjectNoise7(0)
	, m_bSelectObjectNoise8(0)
	, m_iMaxLength(0)
	, m_bStartNoise(FALSE)
	, m_uiNoiseNb(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	 srand( (unsigned)time( NULL ) );
}
CHardwareTestDlg::~CHardwareTestDlg()
{
	for (int i=0; i<GraphViewNum; i++)
	{
		m_dSampleData[i].clear();
	}
	if (m_cSelectObjectName != NULL)
	{
		for (int j=0; j<GraphViewId; j++)
		{
			if (m_cSelectObjectName[j] != NULL)
			{
				m_cSelectObjectName[j] = NULL;
				delete m_cSelectObjectName[j];
			}
		}
		m_cSelectObjectName = NULL;
		delete m_cSelectObjectName;
	}
	if (m_SysTimeSocket != NULL)
	{
		m_SysTimeSocket = NULL;
		delete m_SysTimeSocket;
	}
}
void CHardwareTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SAMPLETIME, m_dSampleTime);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW1, m_bSelectObject1);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW2, m_bSelectObject2);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW3, m_bSelectObject3);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW4, m_bSelectObject4);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW5, m_bSelectObject5);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW6, m_bSelectObject6);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW7, m_bSelectObject7);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW8, m_bSelectObject8);
	DDX_Check(pDX, IDC_CHECK_NOISE1, m_bSelectObjectNoise1);
	DDX_Check(pDX, IDC_CHECK_NOISE2, m_bSelectObjectNoise2);
	DDX_Check(pDX, IDC_CHECK_NOISE3, m_bSelectObjectNoise3);
	DDX_Check(pDX, IDC_CHECK_NOISE4, m_bSelectObjectNoise4);
	DDX_Check(pDX, IDC_CHECK_NOISE5, m_bSelectObjectNoise5);
	DDX_Check(pDX, IDC_CHECK_NOISE6, m_bSelectObjectNoise6);
	DDX_Check(pDX, IDC_CHECK_NOISE7, m_bSelectObjectNoise7);
	DDX_Check(pDX, IDC_CHECK_NOISE8, m_bSelectObjectNoise8);
}

BEGIN_MESSAGE_MAP(CHardwareTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_HSCROLL() 
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTOT_STARTSAMPLE, &CHardwareTestDlg::OnBnClickedButtotStartSample)
	ON_BN_CLICKED(IDC_BUTTON_STOPSAMPLE, &CHardwareTestDlg::OnBnClickedButtonStopSample)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILEPATH, &CHardwareTestDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CHardwareTestDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_STARTNOISE, &CHardwareTestDlg::OnBnClickedButtonStartNoise)
	ON_BN_CLICKED(IDC_BUTTON_STOPNOISE, &CHardwareTestDlg::OnBnClickedButtonStopNoise)
	ON_BN_CLICKED(IDC_BUTTON_ADCINIT, &CHardwareTestDlg::OnBnClickedButtonADCInit)
	ON_BN_CLICKED(IDC_BUTTON_OFFSET, &CHardwareTestDlg::OnBnClickedButtonADCOffest)
	ON_BN_CLICKED(IDC_BUTTON_FULLSCALE, &CHardwareTestDlg::OnBnClickedButtonADCFullScale)
	ON_BN_CLICKED(IDC_BUTTON_ADCSTART, &CHardwareTestDlg::OnBnClickedButtonADCStart)
	ON_BN_CLICKED(IDC_BUTTON_ADCSTOP, &CHardwareTestDlg::OnBnClickedButtonADCStop)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CHardwareTestDlg 消息处理程序

BOOL CHardwareTestDlg::OnInitDialog()
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
	pHardwareTestDlg = this;

	m_iGraphViewId[0] = IDC_GRAPHSHOW1;
	m_iGraphViewId[1] = IDC_GRAPHSHOW2;
	m_iGraphViewId[2] = IDC_GRAPHSHOW3;
	m_iGraphViewId[3] = IDC_GRAPHSHOW4;
	m_iGraphViewId[4] = IDC_GRAPHSHOW5;
	m_iGraphViewId[5] = IDC_GRAPHSHOW6;
	m_iGraphViewId[6] = IDC_GRAPHSHOW7;
	m_iGraphViewId[7] = IDC_GRAPHSHOW8;

	for (int i=0; i<GraphViewNum;i++)
	{
		m_dSampleData[i].reserve(10240);
	}

	// 输入控件ID
	m_iMathId[0][0] = IDC_STATIC_AVERAGE1;
	m_iMathId[0][1] = IDC_STATIC_RMS1;
	m_iMathId[0][2] = IDC_STATIC_MAX1;
	m_iMathId[0][3] = IDC_STATIC_MIN1;

	m_iMathId[1][0] = IDC_STATIC_AVERAGE2;
	m_iMathId[1][1] = IDC_STATIC_RMS2;
	m_iMathId[1][2] = IDC_STATIC_MAX2;
	m_iMathId[1][3] = IDC_STATIC_MIN2;

	m_iMathId[2][0] = IDC_STATIC_AVERAGE3;
	m_iMathId[2][1] = IDC_STATIC_RMS3;
	m_iMathId[2][2] = IDC_STATIC_MAX3;
	m_iMathId[2][3] = IDC_STATIC_MIN3;

	m_iMathId[3][0] = IDC_STATIC_AVERAGE4;
	m_iMathId[3][1] = IDC_STATIC_RMS4;
	m_iMathId[3][2] = IDC_STATIC_MAX4;
	m_iMathId[3][3] = IDC_STATIC_MIN4;

	m_iMathId[4][0] = IDC_STATIC_AVERAGE5;
	m_iMathId[4][1] = IDC_STATIC_RMS5;
	m_iMathId[4][2] = IDC_STATIC_MAX5;
	m_iMathId[4][3] = IDC_STATIC_MIN5;

	m_iMathId[5][0] = IDC_STATIC_AVERAGE6;
	m_iMathId[5][1] = IDC_STATIC_RMS6;
	m_iMathId[5][2] = IDC_STATIC_MAX6;
	m_iMathId[5][3] = IDC_STATIC_MIN6;

	m_iMathId[6][0] = IDC_STATIC_AVERAGE7;
	m_iMathId[6][1] = IDC_STATIC_RMS7;
	m_iMathId[6][2] = IDC_STATIC_MAX7;
	m_iMathId[6][3] = IDC_STATIC_MIN7;

	m_iMathId[7][0] = IDC_STATIC_AVERAGE8;
	m_iMathId[7][1] = IDC_STATIC_RMS8;
	m_iMathId[7][2] = IDC_STATIC_MAX8;
	m_iMathId[7][3] = IDC_STATIC_MIN8;

	// 控件选择
	for (int k=0; k<GraphViewNum; k++)
	{
		m_bSelectObject[k] = 0;
		m_bSelectObjectNoise[k] = 0;
	}


	// 选中的仪器对象名称
	m_cSelectObjectName = new char*[GraphViewId];
	m_cSelectObjectName[0] = "仪器一";
	m_cSelectObjectName[1] = "仪器二";
	m_cSelectObjectName[2] = "仪器三";
	m_cSelectObjectName[3] = "仪器四";
	m_cSelectObjectName[4] = "仪器五";
	m_cSelectObjectName[5] = "仪器六";
	m_cSelectObjectName[6] = "仪器七";
	m_cSelectObjectName[7] = "仪器八";

	// 得到当前路径
	GetCurrentDirectory(MAX_PATH, cSaveToFilePath);
	strFilePath = cSaveToFilePath;

	// 横向滚动条的最大和最小值
	MAX_RANGE = 100; 
	MIN_RANGE = 0; 
	CScrollBar*pSB =(CScrollBar*)GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW); 
	pSB->SetScrollRange(MIN_RANGE,MAX_RANGE); 

	// 创建绘图控件并设置相关参数
	CreateGraph(GraphViewNum, m_iGraphViewId, m_OScopeCtrl);
	// customize the control
	SetRange(-2.5, 2.5, 1, GraphViewNum, m_OScopeCtrl) ;
	SetYUnits("Y", GraphViewNum, m_OScopeCtrl) ;
	SetXUnits("X", GraphViewNum, m_OScopeCtrl) ;
	SetBackgroundColor(RGB(0, 0, 64), GraphViewNum, m_OScopeCtrl) ;
	SetGridColor(RGB(192, 192, 255), GraphViewNum, m_OScopeCtrl) ;
	SetPlotColor(RGB(255, 255, 255), GraphViewNum, m_OScopeCtrl) ;

	m_oThreadManage.OnInit();
	m_SysTimeSocket = new CSysTimeSocket;
	// 创建AD采集命令广播端口
	OnCreateCollectSysTimeBroadcastSocket();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
 
void CHardwareTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHardwareTestDlg::OnPaint()
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
HCURSOR CHardwareTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 创建获取采集站本地时间端口
void CHardwareTestDlg::OnCreateCollectSysTimeBroadcastSocket(void)
{
	BOOL bReturn = FALSE;
	// 生成网络端口，接收发送命令应答帧，create函数写入第三个参数IP地址则接收固定IP地址发送的帧，不写则全网接收
	bReturn = m_SysTimeSocket->Create(ADSetReturnPort, SOCK_DGRAM);
	if (bReturn == FALSE)
	{
		AfxMessageBox(_T("命令广播端口创建失败！"));
	}

	//设置广播模式
	int	iOptval, iOptlen;
	iOptlen = sizeof(int);
	iOptval = 1;
	m_SysTimeSocket->SetSockOpt(SO_BROADCAST, &iOptval, iOptlen, SOL_SOCKET);
}

// 创建图形显示模板
void CHardwareTestDlg::CreateGraph(int iGraphViewNum, int* iGraphViewId, COScopeCtrl* oScopeCtrl)
{
	CRect rect;
	for (int i=0; i<iGraphViewNum; i++)
	{
		GetDlgItem(iGraphViewId[i])->GetWindowRect(rect) ;
		ScreenToClient(rect) ;
		// create the control
		oScopeCtrl[i].Create(WS_VISIBLE | WS_CHILD, rect, this) ; 
	}
}
// 设置采样取值范围
void CHardwareTestDlg::SetRange(double dLower, double dUpper, int nDecimalPlaces, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetRange(dLower, dUpper, nDecimalPlaces) ;
	}
}

// 设置Y轴标签
void CHardwareTestDlg::SetYUnits(CString string, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetYUnits(string);
	}
}

// 设置X轴标签
void CHardwareTestDlg::SetXUnits(CString string, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetXUnits(string);
	}
}

// 设置背景颜色
void CHardwareTestDlg::SetBackgroundColor(COLORREF color, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetBackgroundColor(color);
	}
}

// 设置网格颜色
void CHardwareTestDlg::SetGridColor(COLORREF color, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetGridColor(color);
	}
}

// 设置画点颜色
void CHardwareTestDlg::SetPlotColor(COLORREF color, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetPlotColor(color);
	}
}


// 至少有一个仪器被选中才会启动采样时间定时器和8个采样输出随机数定时器
void CHardwareTestDlg::OnBnClickedButtotStartSample()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (m_bStartSample == FALSE)
	{
		BOOL bSelect = FALSE;
		UpdateData(TRUE);

		m_bSelectObject[0] = m_bSelectObject1;
		m_bSelectObject[1] = m_bSelectObject2;
		m_bSelectObject[2] = m_bSelectObject3;
		m_bSelectObject[3] = m_bSelectObject4;
		m_bSelectObject[4] = m_bSelectObject5;
		m_bSelectObject[5] = m_bSelectObject6;
		m_bSelectObject[6] = m_bSelectObject7;
		m_bSelectObject[7] = m_bSelectObject8;

		m_bSelectObjectNoise[0] = m_bSelectObjectNoise1;
		m_bSelectObjectNoise[1] = m_bSelectObjectNoise2;
		m_bSelectObjectNoise[2] = m_bSelectObjectNoise3;
		m_bSelectObjectNoise[3] = m_bSelectObjectNoise4;
		m_bSelectObjectNoise[4] = m_bSelectObjectNoise5;
		m_bSelectObjectNoise[5] = m_bSelectObjectNoise6;
		m_bSelectObjectNoise[6] = m_bSelectObjectNoise7;
		m_bSelectObjectNoise[7] = m_bSelectObjectNoise8;

		m_uiSampleNb = 0;
		for (int i=0; i<GraphViewNum; i++)
		{
			if (m_bSelectObject[i] == 1)
			{
				if (m_bSelectObjectNoise[i] == 0)
				{
					m_uiSampleNb |= 1<<i;
					bSelect = TRUE;
				}
			}
		}
		// 至少有一个仪器被选中
		if (bSelect == TRUE)
		{
			m_bStartSample = TRUE;
			m_bStopSample = FALSE;

 			// 发送开始采样操作命令帧
 		/*	OnSendCmdStartSampleFrame(m_uiSampleNb);*/
			m_SysTimeSocket->OnADCStart(m_uiSampleNb);

			// 			// 开启采样输出随机数定时器
			// 			SetTimer(1,ReceiveDataSize,NULL);

			// 开启采样时间定时器
// 			m_dSampleTime = m_dSampleTime*1000;
// 			UINT iSampleTime = 0;
// 			iSampleTime = (int)m_dSampleTime;
// 			if (iSampleTime != 0)
// 			{
// 				SetTimer(10, iSampleTime, NULL);
// 			}
		}
	}
}

void CHardwareTestDlg::OnBnClickedButtonStopSample()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (m_bStartSample == FALSE)
	{
		return;
	}
	m_bStopSample = TRUE;
	m_bStartSample = FALSE;


	CString str;
	// 发送采样结束操作命令帧
	//OnSendCmdFrame(StartSampleCmd, 0, m_uiSampleNb);
	m_SysTimeSocket->OnADCStop(m_uiSampleNb);

	// 		// 关闭采样输出随机数定时器
	// 		KillTimer(1);

	// 关闭采样时间定时器
	// KillTimer(10);

	// 对选中的仪器进行数据处理
	for (int k=0; k<GraphViewNum; k++)
	{
		if (m_bSelectObject[k] == 1)
		{
			if (m_bSelectObjectNoise[k] == 0)
			{
				double davg = 0.0;
				double drms = 0.0;
				double dmax = 0.0;
				double dmin = 0.0;
				if (m_dSampleData[k].size() == 0)
				{
					continue;
				}
				davg = SampleDataAverage(m_dSampleData[k]);
				str.Format("Avg = %2.3f",davg);
				GetDlgItem(m_iMathId[k][0])->SetWindowText(str);

				drms = SampleDataRms(m_dSampleData[k]);
				str.Format("RMS = %2.3f",drms);
				GetDlgItem(m_iMathId[k][1])->SetWindowText(str);

				dmax = SampleDataMax(m_dSampleData[k]);
				str.Format("Max = %2.3f",dmax);
				GetDlgItem(m_iMathId[k][2])->SetWindowText(str);

				dmin = SampleDataMin(m_dSampleData[k]);
				str.Format("Min = %2.3f",dmin);
				GetDlgItem(m_iMathId[k][3])->SetWindowText(str);

				// 将数据处理结果加入到数组中
				m_dSampleData[k].push_back(davg);
				m_dSampleData[k].push_back(drms);
				m_dSampleData[k].push_back(dmax);
				m_dSampleData[k].push_back(dmin);
			}
		}
	}


	// 计算仪器最大的采样长度
	m_iMaxLength = OnSampleDataMaxLength(GraphViewNum, m_dSampleData);
	if (m_iMaxLength == 0)
	{
		return;
	}

	// 横向滚动条的最大和最小值
	MAX_RANGE = m_iMaxLength; 
	if (m_iMaxLength > m_OScopeCtrl[0].XAxisPointNum)
	{
		MIN_RANGE = m_OScopeCtrl[0].XAxisPointNum; 
	} 
	else
	{
		MIN_RANGE = 0; 
	}
	CScrollBar*pSB =(CScrollBar*)GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW); 
	pSB->SetScrollRange(MIN_RANGE, MAX_RANGE); 
	pSB->SetScrollPos(MAX_RANGE, TRUE);

	// 将采样数据保存到文件中，只有设定采样时间的数据才会被保存
	if (m_dSampleTime > 0)
	{
		OnSaveToFile(MathValueIdNum, GraphViewNum, m_cSelectObjectName,m_dSampleData);
	}
}
void CHardwareTestDlg::OnTimer(UINT nIDEvent) 
{
	// 达到采样时间停止采样
	if (nIDEvent == 10)
	{
		OnBnClickedButtonStopSample();
	}
	CDialog::OnTimer(nIDEvent);
}
// 求采样数据的平均值
double CHardwareTestDlg::SampleDataAverage(vector<double> dSampleData)
{
	int length = 0;
	double average = 0.0;

	length = dSampleData.size();
	for (int i=0; i<length; i++)
	{
		average += dSampleData[i];
	}
	average = average/length;
	return average;
}

// 求采样数据的最大值
double CHardwareTestDlg::SampleDataMax(vector<double> dSampleData)
{
	int length = 0;
	double maxvalue = 0.0;

	length = dSampleData.size();
	maxvalue = dSampleData[0];
	for (int i=0; i<length;i++)
	{
		if (dSampleData[i] > maxvalue)
		{
			maxvalue = dSampleData[i];
		}
	}

	return maxvalue;
}

// 求采样数据的最小值
double CHardwareTestDlg::SampleDataMin(vector<double> dSampleData)
{
	int length = 0;
	double minvalue = 0.0;

	length = dSampleData.size();
	minvalue = dSampleData[0];
	for (int i=0; i<length;i++)
	{
		if (dSampleData[i] < minvalue)
		{
			minvalue = dSampleData[i];
		}
	}
	return minvalue;
}

// 求采样数据的均方根
double CHardwareTestDlg::SampleDataRms(vector<double> dSampleData)
{
	int length = 0;
	double rmsvalue = 0.0;

	length = dSampleData.size();
	for (int i=0; i<length; i++)
	{
		rmsvalue += dSampleData[i]*dSampleData[i];
	}
	// 幂运算用pow(X,Y)函数，x^y以X为底的Y次幂，需要math.h
	rmsvalue = pow(rmsvalue,0.5);
	return rmsvalue;
}

// 将采集到的数据保存到文件中
void CHardwareTestDlg::OnSaveToFile(int iMathValueIdNum, int iGraphViewNum, char** cppSelectObjectName, vector <double>* dSampleData)
{
	CString strFileName = "";
	SYSTEMTIME  sysTime;
	GetLocalTime(&sysTime);
	strFileName.Format("%s\\%04d%02d%02d%02d%02d%02d%04d.text", strFilePath,sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	//保存成ANSI格式的文件
	errno_t err;
	FILE * savefile;
	CString strTemp = "";
	unsigned int iDataLength = 0;
	if((err = fopen_s(&savefile,strFileName,"w+"))==NULL)
	{
		CString strOutput = "";
		// 输出仪器标签
		for (int i=0; i<iGraphViewNum; i++)
		{
			strTemp.Format("%s\t",cppSelectObjectName[i]);
			strOutput += strTemp;
		}
		strOutput += "\r\n";

		// 输出各仪器采样数据
		for (unsigned int i=0; i<(m_iMaxLength-iMathValueIdNum); i++)
		{
			for (int j=0; j<iGraphViewNum; j++)
			{
				if (m_bSelectObject[j] == 1)
				{
					if (m_bSelectObjectNoise[j] == 0)
					{
						iDataLength = dSampleData[j].size()-iMathValueIdNum;
						if (iDataLength <= 0 )
						{
							strOutput += "\t";
							continue;
						}
						if (iDataLength > i)
						{	
							strTemp.Format("%2.3f\t",dSampleData[j][i]);
							strOutput += strTemp;
						}
						else
						{
							strOutput += "\t";
						}
					}
					else
					{
						strOutput += "\t";
					}
				}
				else
				{
					strOutput += "\t";
				}
			}
			strOutput += "\r\n";
		}

		strOutput += "数据处理结果：\r\n";

		// 输出仪器标签
		for (int i=0; i<iGraphViewNum; i++)
		{
			strTemp.Format("%s\t",cppSelectObjectName[i]);
			strOutput += strTemp;
		}
		strOutput += "\r\n";

		for (int i=0; i<iMathValueIdNum; i++)
		{
			for (int j=0; j<iGraphViewNum; j++)
			{
				if (m_bSelectObject[j] == 1)
				{
					if (m_bSelectObjectNoise[j] == 0)
					{

						iDataLength = dSampleData[j].size()-iMathValueIdNum;
						if (iDataLength < 0)
						{
							strOutput += "\t";
							continue;
						}
						strTemp.Format("%2.3f\t",dSampleData[j][iDataLength+i]);
						strOutput += strTemp;
					}
					else
					{
						strOutput += "\t";
					}
				}
				else
				{
					strOutput += "\t";
				}
			}
			strOutput += "\r\n";
		}
		fprintf(savefile,"%s", strOutput); 
	}
	fclose(savefile); 
}

// 计算仪器最大的采样长度
unsigned int CHardwareTestDlg::OnSampleDataMaxLength(int iGraphViewNum, vector<double>* dSampleData)
{
	unsigned int iMaxLength = 0;

	for (int i=0; i<iGraphViewNum; i++)
	{
		if (m_bSelectObject[i] == 1)
		{
			if (m_bSelectObjectNoise[i] == 0)
			{
				if (iMaxLength<dSampleData[i].size())
				{
					iMaxLength = dSampleData[i].size();
				}
			}
		}
	}
	return iMaxLength;
}
// 手动设定文件存储路径
void CHardwareTestDlg::OnBnClickedButtonBrowse()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// 文件夹选择对话框

	char szDir[MAX_PATH];

	BROWSEINFO bi;

	ITEMIDLIST *pidl;

	bi.hwndOwner = this->m_hWnd; // 指定父窗口，在对话框显示期间，父窗口将被禁用 


	bi.pidlRoot = NULL; // 如果指定NULL，就以“桌面”为根 

	bi.pszDisplayName = szDir; 

	bi.lpszTitle = "请选择采样数据存储目录"; // 这一行将显示在对话框的顶端 

	bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;

	bi.lpfn = NULL;

	bi.lParam = 0;

	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);

	if(pidl == NULL) 
	{
		return;
	}
	if(!SHGetPathFromIDList(pidl, szDir))
	{	
		return;
	}
	else
	{
		strFilePath = szDir;
	}

}
// 滑动条消息响应函数，实现图形界面滚动条效果
void   CHardwareTestDlg::OnHScroll(UINT   nSBCode,   UINT   nPos,   CScrollBar*   pScrollBar) 
{ 
	UINT pos; 
	UINT uimovepos = 0;
	if (m_bStopSample == FALSE)
	{
		return;
	}

	pos = pScrollBar->GetScrollPos(); 

	switch( nSBCode ) 
	{ 	
		// 	case   SB_TOP: 
		// 		pos   =   MIN_RANGE; 
		// 		break; 
		// 
		// 	case   SB_BOTTOM: 
		// 		pos   =   MAX_RANGE; 
		// 		break; 
	case   SB_THUMBTRACK:
		if (pos > nPos)
		{
			if ((pos - nPos) >= (MAX_RANGE-MIN_RANGE))
			{
				uimovepos = (MAX_RANGE-MIN_RANGE)/4;
			}
			else
			{
				uimovepos = pos - nPos;
			}
			for (int i=0; i<GraphViewNum; i++)
			{
				if (m_bSelectObject[i] == 1)
				{
					if (m_bSelectObjectNoise[i] == 0)
					{
						if (m_dSampleData[i].size() == 0)
						{
							continue;
						}
						m_OScopeCtrl[i].RightDrawPoint(m_dSampleData[i], MathValueIdNum, uimovepos);
					}
				}
			}
		}
		if (pos < nPos)
		{
			if ((nPos - pos) >= (MAX_RANGE-MIN_RANGE))
			{
				uimovepos = (MAX_RANGE-MIN_RANGE)/4;
			}
			else
			{
				uimovepos = nPos - pos;
			}
			for (int i=0; i<GraphViewNum; i++)
			{
				if (m_bSelectObject[i] == 1)
				{
					if (m_bSelectObjectNoise[i] == 0)
					{
						if (m_dSampleData[i].size() == 0)
						{
							continue;
						}
						m_OScopeCtrl[i].LeftDrawPoint(m_dSampleData[i], MathValueIdNum, uimovepos);
					}
				}
			}
		}
		pos   =   nPos; 
		break;
	default: 
		return; 
	} 
	if(pos <  MIN_RANGE) 
	{
		pos = MIN_RANGE; 
	}
	if (pos > MAX_RANGE) 
	{
		pos = MAX_RANGE; 
	}
	pScrollBar-> SetScrollPos(   pos,   TRUE   ); 


} 
void CHardwareTestDlg::OnBnClickedButtonReset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	for (int i=0; i<GraphViewNum; i++)
	{
		m_OScopeCtrl[i].XAxisLabelMin = 0;
		m_OScopeCtrl[i].XAxisLabelMax = m_OScopeCtrl[i].XAxisPointNum;
		m_OScopeCtrl[i].Reset();
		m_OScopeCtrl[i].m_dPreviousPosition = 0.0;
		m_OScopeCtrl[i].PointNum = 0;			// 绘制了的点的个数初值为0

		m_dSampleData[i].clear();
		m_bSelectObject[i] = 0;
		m_bSelectObjectNoise[i] = 0;
		GetDlgItem(m_iMathId[i][0])->SetWindowText("Avg =");
		GetDlgItem(m_iMathId[i][1])->SetWindowText("RMS =");
		GetDlgItem(m_iMathId[i][2])->SetWindowText("Max =");
		GetDlgItem(m_iMathId[i][3])->SetWindowText("Min =");
	}
	m_bStopSample = FALSE;
	m_bStartSample = FALSE;
	m_bStartNoise = FALSE;
	m_bSelectObject1 = 1;
	m_bSelectObject2 = 1;
	m_bSelectObject3 = 1;
	m_bSelectObject4 = 1;
	m_bSelectObject5 = 1;
	m_bSelectObject6 = 1;
	m_bSelectObject7 = 1;
	m_bSelectObject8 = 1;
	m_dSampleTime = 0;
	m_uiSampleNb = 0;
	m_uiNoiseNb = 0;
	m_iMaxLength = 0;
	UpdateData(FALSE);
}

// 接收采样数据
void CHardwareTestDlg::ReceiveSampleData(byte* pSampleData)
{
	// uiTestNb表明接收到帧所对应的仪器号
	UINT32	uiTestNb = 0;
	unsigned short	uisDataNb = 0; // 接收到帧的数据的序号
	int	iSampleData = 0;
	int pos = 0;
	double dSampleDataToV = 0.0;
	// 设m_pFrameData[16]到m_pFrameData[19]为源地址做为通道号
	memcpy(&uiTestNb, &pSampleData[pos], 4);
	pos += 4;
	// 设通道1,2,3,4分别对应四个仪器IP地址
	if (uiTestNb == 111)
	{
		uiTestNb = 1;
	}
	else if (uiTestNb == 101)
	{
		uiTestNb = 2;
	}
	else if (uiTestNb == 91)
	{
		uiTestNb = 4;
	}
	else if (uiTestNb == 81)
	{
		uiTestNb = 8;
	}
	// append the new value to the plot
	for (UINT32 i=0; i<GraphViewNum; i++)
	{
		if (uiTestNb == (UINT32)(1 << i))
		{
			// 接收数据缓存
			double m_dpReceiveData [ReceiveDataSize]; 
			pos += 8;
			// m_pFrameData[28]到m_pFrameData[29]做为采样个数的序号，暂时不用
			memcpy(&uisDataNb, &pSampleData[pos], 2);
			pos += 2;

			// 之后为数据区
			for (int j=0; j<ReceiveDataSize; j++)
			{
				memcpy(&iSampleData, &pSampleData[pos], 3);
				pos += 3;

				// 24位数转换为电压值
				dSampleDataToV = iSampleData;
				if (dSampleDataToV < 0x7FFFFF)
				{
					dSampleDataToV = dSampleDataToV/( 0x7FFFFE )*2.5;
				}
				else if (dSampleDataToV > 0x800000)
				{
					dSampleDataToV = -(0xFFFFFF - dSampleDataToV)/( 0x7FFFFE )*2.5;
				}
				else
				{
					CString str;
					str.Format("ADC采样数值溢出，溢出值为%d", dSampleDataToV);
					AfxMessageBox(str);
				}
				m_dpReceiveData[j] = dSampleDataToV;

				if (m_bSelectObject[i] == 1)
				{
					if (m_bSelectObjectNoise[i] == 0)
					{
						m_Sec.Lock();
						m_dSampleData[i].push_back(dSampleDataToV);
						m_Sec.Unlock();
					}
				}
			}
			m_Sec.Lock();
			m_OScopeCtrl[i].AppendPoint(m_dpReceiveData, ReceiveDataSize);
			m_Sec.Unlock();
		}
	}

}

void CHardwareTestDlg::OnBnClickedButtonStartNoise()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (m_bStartNoise == FALSE)
	{
		BOOL bSelect = FALSE;
		UpdateData(TRUE);

		m_bSelectObject[0] = m_bSelectObject1;
		m_bSelectObject[1] = m_bSelectObject2;
		m_bSelectObject[2] = m_bSelectObject3;
		m_bSelectObject[3] = m_bSelectObject4;
		m_bSelectObject[4] = m_bSelectObject5;
		m_bSelectObject[5] = m_bSelectObject6;
		m_bSelectObject[6] = m_bSelectObject7;
		m_bSelectObject[7] = m_bSelectObject8;

		m_bSelectObjectNoise[0] = m_bSelectObjectNoise1;
		m_bSelectObjectNoise[1] = m_bSelectObjectNoise2;
		m_bSelectObjectNoise[2] = m_bSelectObjectNoise3;
		m_bSelectObjectNoise[3] = m_bSelectObjectNoise4;
		m_bSelectObjectNoise[4] = m_bSelectObjectNoise5;
		m_bSelectObjectNoise[5] = m_bSelectObjectNoise6;
		m_bSelectObjectNoise[6] = m_bSelectObjectNoise7;
		m_bSelectObjectNoise[7] = m_bSelectObjectNoise8;

		m_uiNoiseNb = 0;
		for (int i=0; i<GraphViewNum; i++)
		{
			if (m_bSelectObject[i] == 1)
			{
				if (m_bSelectObjectNoise[i] == 1)
				{
					m_uiNoiseNb |= 1<<i;
					bSelect = TRUE;
				}
			}
		}
		// 至少有一个仪器被选中
		if (bSelect == TRUE)
		{
			m_bStartNoise = TRUE;
			// 发送开始采样操作命令帧
			// OnSendCmdFrame(StartSampleCmd, 1, m_uiNoiseNb);
			m_SysTimeSocket->OnADCStart(m_uiNoiseNb);

		}
	}
}

void CHardwareTestDlg::OnBnClickedButtonStopNoise()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (m_bStartNoise == FALSE)
	{
		return;
	}
	m_bStartNoise = FALSE;

	// 发送采样结束操作命令帧
	//OnSendCmdFrame(StartSampleCmd, 0, m_uiNoiseNb);
	m_SysTimeSocket->OnADCStop(m_uiNoiseNb);
}

void CHardwareTestDlg::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	m_oThreadManage.OnClose();
	CDialog::OnClose();
}
void CHardwareTestDlg::OnBnClickedButtonADCInit()
{
	m_SysTimeSocket->OnADCInit(m_uiSampleNb);
}
void CHardwareTestDlg::OnBnClickedButtonADCOffest()
{
	m_SysTimeSocket->OnADCOffset(m_uiSampleNb);
}
void CHardwareTestDlg::OnBnClickedButtonADCFullScale()
{
	m_SysTimeSocket->OnADCFullScale(m_uiSampleNb);
}
void CHardwareTestDlg::OnBnClickedButtonADCStart()
{
	m_SysTimeSocket->OnADCStart(m_uiSampleNb);
}
void CHardwareTestDlg::OnBnClickedButtonADCStop()
{
	m_SysTimeSocket->OnADCStop(m_uiSampleNb);
}