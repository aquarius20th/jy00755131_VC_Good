// GraphView.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabSample.h"
#include <math.h>

// CGraphView 对话框

IMPLEMENT_DYNAMIC(CTabSample, CDialog)

CTabSample::CTabSample(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSample::IDD, pParent)
	, m_csSaveFilePath(_T(""))
	, m_uiScrollBarMax(100)
	, m_uiScrollBarMin(0)
	, m_bMatrixNetwork(TRUE)
	, m_bStartSample(FALSE)
	, m_dSampleTime(0)
	, m_iSelectObject1(0)
	, m_iSelectObject2(0)
	, m_iSelectObject3(0)
	, m_iSelectObject4(0)
	, m_iSelectObjectNoise1(0)
	, m_iSelectObjectNoise2(0)
	, m_iSelectObjectNoise3(0)
	, m_iSelectObjectNoise4(0)
	, m_bStartNoise(FALSE)
	, m_uiSampleNb(0)
	, m_uiNoiseNb(0)
	, m_iMaxLength(0)
	, m_bStopSample(FALSE)
	, m_bHeartBeat(TRUE)
{

}

CTabSample::~CTabSample()
{
	for (int i=0; i<GraphViewNum; i++)
	{
		m_dSampleData[i].clear();
	}
	for (int j=0; j<GraphViewId; j++)
	{
		if (m_cSelectObjectName[j] != NULL)
		{
			m_cSelectObjectName[j] = NULL;
			delete m_cSelectObjectName[j];
		}
	}
}

void CTabSample::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_MATRIXNETWORK, m_bMatrixNetwork);
	DDX_Text(pDX, IDC_EDIT_SAMPLETIME, m_dSampleTime);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW1, m_iSelectObject1);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW2, m_iSelectObject2);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW3, m_iSelectObject3);
	DDX_Check(pDX, IDC_CHECK_GRAPHVIEW4, m_iSelectObject4);
	DDX_Check(pDX, IDC_CHECK_NOISE1, m_iSelectObjectNoise1);
	DDX_Check(pDX, IDC_CHECK_NOISE2, m_iSelectObjectNoise2);
	DDX_Check(pDX, IDC_CHECK_NOISE3, m_iSelectObjectNoise3);
	DDX_Check(pDX, IDC_CHECK_NOISE4, m_iSelectObjectNoise4);
	DDX_Check(pDX, IDC_CHECK_HEARTBEAT, m_bHeartBeat);
}


BEGIN_MESSAGE_MAP(CTabSample, CDialog)
	ON_BN_CLICKED(IDC_CHECK_MATRIXNETWORK, &CTabSample::OnBnClickedCheckMatrixnetwork)
	ON_BN_CLICKED(IDC_BUTTOT_STARTSAMPLE, &CTabSample::OnBnClickedButtotStartsample)
	ON_BN_CLICKED(IDC_BUTTON_STOPSAMPLE, &CTabSample::OnBnClickedButtonStopsample)
	ON_BN_CLICKED(IDC_BUTTON_STARTNOISE, &CTabSample::OnBnClickedButtonStartnoise)
	ON_BN_CLICKED(IDC_BUTTON_STOPNOISE, &CTabSample::OnBnClickedButtonStopnoise)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILEPATH2, &CTabSample::OnBnClickedButtonSavefilepath)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CTabSample::OnBnClickedButtonReset)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_HEARTBEAT, &CTabSample::OnBnClickedCheckHeartbeat)
END_MESSAGE_MAP()


// 初始化

void CTabSample::OnInit(void)
{
	m_iGraphViewId[0] = IDC_GRAPHSHOW1;
	m_iGraphViewId[1] = IDC_GRAPHSHOW2;
	m_iGraphViewId[2] = IDC_GRAPHSHOW3;
	m_iGraphViewId[3] = IDC_GRAPHSHOW4;

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

	// 控件选择
	for (int k=0; k<GraphViewNum; k++)
	{
		m_iSelectObject[k] = 0;
		m_iSelectObjectNoise[k] = 0;
	}
	m_cSelectObjectName[0] = _T("仪器一");
	m_cSelectObjectName[1] = _T("仪器二");
	m_cSelectObjectName[2] = _T("仪器三");
	m_cSelectObjectName[3] = _T("仪器四");

	// 得到当前路径
	char cSaveToFilePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, cSaveToFilePath);
	m_csSaveFilePath = cSaveToFilePath;

	CScrollBar*pSB =(CScrollBar*)GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW); 
	pSB->SetScrollRange(m_uiScrollBarMin,m_uiScrollBarMax); 

	// 创建绘图控件并设置相关参数
	CreateGraph(GraphViewNum, m_iGraphViewId, m_OScopeCtrl);
	// customize the control
	SetRange(-2.5, 2.5, 1, GraphViewNum, m_OScopeCtrl) ;
	SetYUnits("Y", GraphViewNum, m_OScopeCtrl) ;
	SetXUnits("X", GraphViewNum, m_OScopeCtrl) ;
	SetBackgroundColor(RGB(0, 0, 64), GraphViewNum, m_OScopeCtrl) ;
	SetGridColor(RGB(192, 192, 255), GraphViewNum, m_OScopeCtrl) ;
	SetPlotColor(RGB(255, 255, 255), GraphViewNum, m_OScopeCtrl) ;

	// 得到本机IP地址
	char		name[255]; 
	CString		ip; 
	PHOSTENT	hostinfo; 
	if(   gethostname   (name, sizeof(name)) == 0) 
	{ 
		if((hostinfo = gethostbyname(name)) != NULL) 
		{ 
			ip = inet_ntoa(*(struct in_addr*)*hostinfo-> h_addr_list); 
		} 
	} 
	m_oThreadManage.m_oHeadFrameThread.m_csIPSource = ip;
	m_oThreadManage.m_oTailFrameThread.m_csIPSource = ip;
	m_oThreadManage.m_oSysTimeSocket.m_csIPSource = ip;
	CString str;
	str.Format("0x%04x", 0x8202);
	GetDlgItem(IDC_EDIT_SENDPORT)->SetWindowText(str);

	m_iconDisconnected = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_iconConnected = AfxGetApp()->LoadIcon(IDI_ICON2);
	
	m_oThreadManage.m_oInstrumentList.m_iconDisconnected = m_iconDisconnected;
	m_oThreadManage.m_oInstrumentList.m_pwnd = this;
	m_oThreadManage.m_oHeadFrameThread.m_iconConnected = m_iconConnected;
	m_oThreadManage.m_oHeadFrameThread.m_pwnd = this;
	CStatic* icon;
	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT1);
	icon->SetIcon(m_iconDisconnected);

	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT2);
	icon->SetIcon(m_iconDisconnected);

	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT3);
	icon->SetIcon(m_iconDisconnected);

	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT4);
	icon->SetIcon(m_iconDisconnected);

	m_oThreadManage.OnInit();
	OnOpen();
}

// 创建图形显示模板
void CTabSample::CreateGraph(int iGraphViewNum, int* iGraphViewId, COScopeCtrl* oScopeCtrl)
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
void CTabSample::SetRange(double dLower, double dUpper, int nDecimalPlaces, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetRange(dLower, dUpper, nDecimalPlaces) ;
	}
}

// 设置Y轴标签
void CTabSample::SetYUnits(CString string, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetYUnits(string);
	}
}

// 设置X轴标签
void CTabSample::SetXUnits(CString string, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetXUnits(string);
	}
}

// 设置背景颜色
void CTabSample::SetBackgroundColor(COLORREF color, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetBackgroundColor(color);
	}
}

// 设置网格颜色
void CTabSample::SetGridColor(COLORREF color, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetGridColor(color);
	}
}

// 设置画点颜色
void CTabSample::SetPlotColor(COLORREF color, int iGraphViewNum, COScopeCtrl* oScopeCtrl)
{
	for (int i=0; i<iGraphViewNum; i++)
	{
		oScopeCtrl[i].SetPlotColor(color);
	}
}
// 打开
void CTabSample::OnOpen(void)
{
	CString str;
	int itemp;
	GetDlgItem(IDC_EDIT_SENDPORT)->GetWindowText(str);
	sscanf_s(str,"%x", &itemp);
	m_oThreadManage.m_oHeadFrameThread.m_uiSendPort = itemp;
	m_oThreadManage.m_oHeartBeatThread.m_uiSendPort = itemp;
	m_oThreadManage.m_oTailFrameThread.m_uiSendPort = itemp;
	m_oThreadManage.m_oSysTimeSocket.m_uiSendPort = itemp;
	GetDlgItem(IDC_EDIT_SENDPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_HEARTBEAT)->EnableWindow(TRUE);
	m_oThreadManage.OnOpen();
}

// 停止
void CTabSample::OnStop(void)
{
	CStatic* icon;
	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT1);
	icon->SetIcon(m_iconDisconnected);

	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT2);
	icon->SetIcon(m_iconDisconnected);

	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT3);
	icon->SetIcon(m_iconDisconnected);

	icon =(CStatic*)this->GetDlgItem(IDC_STATIC_INSTRUMENT4);
	icon->SetIcon(m_iconDisconnected);
	GetDlgItem(IDC_EDIT_SENDPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_HEARTBEAT)->EnableWindow(FALSE);
	m_oThreadManage.OnStop();
}
void CTabSample::OnBnClickedCheckMatrixnetwork()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	BOOL bStatus = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_MATRIXNETWORK)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
	if (bStatus == TRUE)
	{
		OnOpen();
	}
	else
	{
		OnStop();
	}
}

void CTabSample::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	m_oThreadManage.OnClose();
}

void CTabSample::OnBnClickedButtotStartsample()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (m_bStartSample == FALSE)
	{
		BOOL bSelect = FALSE;
		UpdateData(TRUE);

		m_iSelectObject[0] = m_iSelectObject1;
		m_iSelectObject[1] = m_iSelectObject2;
		m_iSelectObject[2] = m_iSelectObject3;
		m_iSelectObject[3] = m_iSelectObject4;

		m_iSelectObjectNoise[0] = m_iSelectObjectNoise1;
		m_iSelectObjectNoise[1] = m_iSelectObjectNoise2;
		m_iSelectObjectNoise[2] = m_iSelectObjectNoise3;
		m_iSelectObjectNoise[3] = m_iSelectObjectNoise4;

		m_uiSampleNb = 0;
		for (int i=0; i<GraphViewNum; i++)
		{
			if (m_iSelectObject[i] == 1)
			{
				if (m_iSelectObjectNoise[i] == 0)
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
			OnSendCmdFrame(m_uiSampleNb);

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

void CTabSample::OnBnClickedButtonStopsample()
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

	// 		// 关闭采样输出随机数定时器
	// 		KillTimer(1);

	// 关闭采样时间定时器
	// KillTimer(10);

	// 对选中的仪器进行数据处理
	for (int k=0; k<GraphViewNum; k++)
	{
		if (m_iSelectObject[k] == 1)
		{
			if (m_iSelectObjectNoise[k] == 0)
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
	m_uiScrollBarMax = m_iMaxLength; 
	if (m_iMaxLength > m_OScopeCtrl[0].XAxisPointNum)
	{
		m_uiScrollBarMin = m_OScopeCtrl[0].XAxisPointNum; 
	} 
	else
	{
		m_uiScrollBarMin = 0; 
	}
	CScrollBar*pSB =(CScrollBar*)GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW); 
	pSB->SetScrollRange(m_uiScrollBarMin, m_uiScrollBarMax); 
	pSB->SetScrollPos(m_uiScrollBarMax, TRUE);

	// 将采样数据保存到文件中，只有设定采样时间的数据才会被保存
	if (m_dSampleTime > 0)
	{
		OnSaveToFile(MathValueIdNum, GraphViewNum, m_cSelectObjectName,m_dSampleData);
	}
}

void CTabSample::OnBnClickedButtonStartnoise()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
		if (m_bStartNoise == FALSE)
	{
		BOOL bSelect = FALSE;
		UpdateData(TRUE);

		m_iSelectObject[0] = m_iSelectObject1;
		m_iSelectObject[1] = m_iSelectObject2;
		m_iSelectObject[2] = m_iSelectObject3;
		m_iSelectObject[3] = m_iSelectObject4;

		m_iSelectObjectNoise[0] = m_iSelectObjectNoise1;
		m_iSelectObjectNoise[1] = m_iSelectObjectNoise2;
		m_iSelectObjectNoise[2] = m_iSelectObjectNoise3;
		m_iSelectObjectNoise[3] = m_iSelectObjectNoise4;

		m_uiNoiseNb = 0;
		for (int i=0; i<GraphViewNum; i++)
		{
			if (m_iSelectObject[i] == 1)
			{
				if (m_iSelectObjectNoise[i] == 1)
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
		}
	}
}

void CTabSample::OnBnClickedButtonStopnoise()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (m_bStartNoise == FALSE)
	{
		return;
	}
	m_bStartNoise = FALSE;

	// 发送采样结束操作命令帧
	//OnSendCmdFrame(StartSampleCmd, 0, m_uiNoiseNb);
}

void CTabSample::OnBnClickedButtonSavefilepath()
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
		m_csSaveFilePath = szDir;
	}
}

void CTabSample::OnBnClickedButtonReset()
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
		m_iSelectObject[i] = 0;
		m_iSelectObjectNoise[i] = 0;
		GetDlgItem(m_iMathId[i][0])->SetWindowText("Avg =");
		GetDlgItem(m_iMathId[i][1])->SetWindowText("RMS =");
		GetDlgItem(m_iMathId[i][2])->SetWindowText("Max =");
		GetDlgItem(m_iMathId[i][3])->SetWindowText("Min =");
	}
	m_bStopSample = FALSE;
	m_bStartSample = FALSE;
	m_bStartNoise = FALSE;
	m_iSelectObject1 = 0;
	m_iSelectObject2 = 0;
	m_iSelectObject3 = 0;
	m_iSelectObject4 = 0;
	
	m_dSampleTime = 0;
	m_uiSampleNb = 0;
	m_uiNoiseNb = 0;
	m_iMaxLength = 0;
	UpdateData(FALSE);
}

// 将采集到的数据保存到文件中
void CTabSample::OnSaveToFile(int iMathValueIdNum, int iGraphViewNum, char** cppSelectObjectName, vector <double>* dSampleData)
{
	CString strFileName = "";
	SYSTEMTIME  sysTime;
	GetLocalTime(&sysTime);
	strFileName.Format("%s\\%04d%02d%02d%02d%02d%02d%04d.text", m_csSaveFilePath,sysTime.wYear,sysTime.wMonth,sysTime.wDay,
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
				if (m_iSelectObject[j] == 1)
				{
					if (m_iSelectObjectNoise[j] == 0)
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
				if (m_iSelectObject[j] == 1)
				{
					if (m_iSelectObjectNoise[j] == 0)
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
unsigned int CTabSample::OnSampleDataMaxLength(int iGraphViewNum, vector<double>* dSampleData)
{
	unsigned int iMaxLength = 0;

	for (int i=0; i<iGraphViewNum; i++)
	{
		if (m_iSelectObject[i] == 1)
		{
			if (m_iSelectObjectNoise[i] == 0)
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
// 求采样数据的平均值
double CTabSample::SampleDataAverage(vector<double> dSampleData)
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
double CTabSample::SampleDataMax(vector<double> dSampleData)
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
double CTabSample::SampleDataMin(vector<double> dSampleData)
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
double CTabSample::SampleDataRms(vector<double> dSampleData)
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
// 发送数据采集命令帧
void CTabSample::OnSendCmdFrame(unsigned int uiTestNb)
{
	m_oThreadManage.m_oSysTimeSocket.m_uiSampleNb = uiTestNb;
	m_oThreadManage.m_oADDataThread.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_oADDataThread.m_pSelectObjectNoise = m_iSelectObjectNoise;
	for(int i=0; i<GraphViewNum; i++)
	{
		m_oThreadManage.m_oADDataThread.m_pSampleData[i] = &m_dSampleData[i];
		m_oThreadManage.m_oADDataThread.m_pOScopeCtrl[i] = &m_OScopeCtrl[i];
	}
	m_oThreadManage.MakeCollectSysTimeFrameData(uiTestNb);
	m_oThreadManage.SendCollectSysTimeFrameToSocket();
}

void CTabSample::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	// 达到采样时间停止采样
	if (nIDEvent == 10)
	{
		OnBnClickedButtonStopsample();
	}
	CDialog::OnTimer(nIDEvent);
}
// 滑动条消息响应函数，实现图形界面滚动条效果
void   CTabSample::OnHScroll(UINT   nSBCode,   UINT   nPos,   CScrollBar*   pScrollBar) 
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
			if ((pos - nPos) >= (m_uiScrollBarMax-m_uiScrollBarMin))
			{
				uimovepos = (m_uiScrollBarMax-m_uiScrollBarMin)/4;
			}
			else
			{
				uimovepos = pos - nPos;
			}
			for (int i=0; i<GraphViewNum; i++)
			{
				if (m_iSelectObject[i] == 1)
				{
					if (m_iSelectObjectNoise[i] == 0)
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
			if ((nPos - pos) >= (m_uiScrollBarMax-m_uiScrollBarMin))
			{
				uimovepos = (m_uiScrollBarMax-m_uiScrollBarMin)/8;
			}
			else
			{
				uimovepos = nPos - pos;
			}
			for (int i=0; i<GraphViewNum; i++)
			{
				if (m_iSelectObject[i] == 1)
				{
					if (m_iSelectObjectNoise[i] == 0)
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
	if(pos <  m_uiScrollBarMin) 
	{
		pos = m_uiScrollBarMin; 
	}
	if (pos > m_uiScrollBarMax) 
	{
		pos = m_uiScrollBarMax; 
	}
	pScrollBar-> SetScrollPos(   pos,   TRUE   ); 


} 
void CTabSample::OnBnClickedCheckHeartbeat()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	BOOL bStatus = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_HEARTBEAT)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
	if (bStatus == TRUE)
	{
		m_oThreadManage.m_oHeartBeatThread.ResumeThread();
		m_oThreadManage.m_oHeartBeatThread.OnOpen();
	}
	else
	{
		m_oThreadManage.m_oHeartBeatThread.SuspendThread();
		m_oThreadManage.m_oHeartBeatThread.OnStop();
	}
}
