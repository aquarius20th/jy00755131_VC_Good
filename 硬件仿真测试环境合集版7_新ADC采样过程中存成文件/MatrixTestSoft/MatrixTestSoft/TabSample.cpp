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
	, m_uiTailRecCount(0)
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
	ON_BN_CLICKED(IDC_BUTTON_SETADC, &CTabSample::OnBnClickedButtonSetadc)
	ON_BN_CLICKED(IDC_BUTTON_TIMECAL, &CTabSample::OnBnClickedButtonTimecal)
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
	m_oThreadManage.m_HeadFrameSocket.m_csIPSource = ip;

	m_oThreadManage.m_TailFrameSocket.m_csIPSource = ip;
	m_oThreadManage.m_TailTimeFrameSocket.m_csIPSource = ip;
	m_oThreadManage.m_oSysTimeSocket.m_csIPSource = ip;
	m_oThreadManage.m_ADCDataSocket.m_csIPSource = ip;

	m_oThreadManage.m_oSysTimeSocket.m_pADCSet = &m_oADCSet;


	CString str;
	str.Format("0x%04x", 0x8202);
	GetDlgItem(IDC_EDIT_SENDPORT)->SetWindowText(str);

	str.Format("%d", 406);
	GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->SetWindowText(str);

	m_iconDisconnected = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_iconConnected = AfxGetApp()->LoadIcon(IDI_ICON2);
	
	m_oThreadManage.m_oInstrumentList.m_iconDisconnected = m_iconDisconnected;
	m_oThreadManage.m_oInstrumentList.m_pwnd = this;
	m_oThreadManage.m_IPSetSocket.m_iconConnected = m_iconConnected;
	m_oThreadManage.m_IPSetSocket.m_pwnd = this;
//	m_oThreadManage.m_ADCDataSocket.m_pwnd = this;

	m_oThreadManage.m_ADCDataSocket.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_ADCDataSocket.m_pSelectObjectNoise = m_iSelectObjectNoise;
	for(int i=0; i<GraphViewNum; i++)
	{
		m_oThreadManage.m_ADCDataSocket.m_pSampleData[i] = &m_dSampleData[i];
		m_oThreadManage.m_ADCDataSocket.m_pOScopeCtrl[i] = &m_OScopeCtrl[i];
		m_oThreadManage.m_ADCDataSocket.m_cSelectObjectName[i] = m_cSelectObjectName[i];
	}


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
	// 监测尾包定时器
	SetTimer(4, 1000, NULL);
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
//	m_HeartBeatSocket.m_uiSendPort = itemp;
	m_oThreadManage.m_oHeartBeatThread.m_uiSendPort = itemp;
	m_oThreadManage.m_HeadFrameSocket.m_uiSendPort = itemp;
	m_oThreadManage.m_TailFrameSocket.m_uiSendPort = itemp;
	m_oThreadManage.m_TailTimeFrameSocket.m_uiSendPort = itemp;
	m_oThreadManage.m_oSysTimeSocket.m_uiSendPort = itemp;
	m_oThreadManage.m_ADCDataSocket.m_uiSendPort = itemp;
	GetDlgItem(IDC_EDIT_SENDPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_HEARTBEAT)->EnableWindow(TRUE);

// 	m_HeartBeatSocket.Close();
// 	BOOL bReturn = FALSE;
// 	// 生成网络端口，接收发送命令应答帧，create函数写入第三个参数IP地址则接收固定IP地址发送的帧，不写则全网接收
// 	bReturn = m_HeartBeatSocket.Create(HeartBeatRec, SOCK_DGRAM);
// 	if (bReturn == FALSE)
// 	{
// 		AfxMessageBox(_T("命令广播端口创建失败！"));
// 	}
// 
// 	//设置广播模式
// 	int	iOptval, iOptlen;
// 	iOptlen = sizeof(int);
// 	iOptval = 1;
// 	m_HeartBeatSocket.SetSockOpt(SO_BROADCAST, &iOptval, iOptlen, SOL_SOCKET);
// 	m_HeartBeatSocket.MakeHeartBeatFrame();
// 	KillTimer(3);
// 	SetTimer(3, 250, NULL);

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
// 	KillTimer(3);
// 	m_HeartBeatSocket.Close();
// 	m_oThreadManage.OnStop();
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
// 	KillTimer(3);
// 	m_HeartBeatSocket.Close();
	m_oThreadManage.OnClose();
}

void CTabSample::OnBnClickedButtotStartsample()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	if (m_bStartSample == FALSE)
	{
		BOOL bSelect = FALSE;
	
		if (m_uiSampleNb != 0)
		{
			bSelect = TRUE;
		}
		// 至少有一个仪器被选中
		if (bSelect == TRUE)
		{
			m_bStartSample = TRUE;
			m_bStopSample = FALSE;

			for (int i=0; i<GraphViewNum; i++)
			{
				m_oThreadManage.m_ADCDataSocket.m_uiADCDataFrameCount[i] = 0;
				m_oThreadManage.m_ADCDataSocket.m_uiADCRecCompleteCount[i] = 0;
			}
			m_oThreadManage.m_ADCDataSocket.m_bStartSample = FALSE;

			// 创建本次ADC数据采集数据存储文件夹
			CString str = "";
			SYSTEMTIME  sysTime;
			GetLocalTime(&sysTime);
			str = _T("ADC数据备份");
			m_oThreadManage.m_ADCDataSocket.m_csSaveFilePath.Format("%s\\%s_%04d_%02d_%02d_%02d_%02d_%02d_%04d", m_csSaveFilePath, str, sysTime.wYear,sysTime.wMonth,sysTime.wDay,
				sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
			// 按日期创建保存ADC采样数据的文件夹
			CreateDirectory(m_oThreadManage.m_ADCDataSocket.m_csSaveFilePath,NULL);
			m_oThreadManage.m_ADCDataSocket.m_uiADCSaveFileNum = 0;
			m_oThreadManage.m_ADCDataSocket.m_uiADCDataToSaveNum = 0;
			// 发送开始采样操作命令帧
			OnSendCmdFrame(m_uiSampleNb);
			SetTimer(1, 1500, NULL);
// 			// 开启采样输出随机数定时器
// 			SetTimer(1,ReceiveDataSize,NULL);

//			// 开启采样时间定时器
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

	// 发送采样结束操作命令帧
	//OnSendCmdFrame(StartSampleCmd, 0, m_uiSampleNb);

	unsigned int uiIPAim;
	for (int i=0; i<GraphViewNum; i++)
	{
		if ((m_uiSampleNb & (0x01<<i)) != 0)
		{
			uiIPAim	= 81 + 10*i;
			/*OnADCSampleStop(uiIPAim);*/
		}
	}
	OnADCSampleStop(uiIPAim);
	

	// 		// 关闭采样输出随机数定时器
	// 		KillTimer(1);

	SetTimer(2, 1000, NULL);
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
			for (int i=0; i<GraphViewNum; i++)
			{
				m_oThreadManage.m_ADCDataSocket.m_uiADCDataFrameCount[i] = 0;
				m_oThreadManage.m_ADCDataSocket.m_uiADCRecCompleteCount[i] = 0;
			}
			m_oThreadManage.m_ADCDataSocket.m_bStartSample = FALSE;
			OnSendCmdFrame(m_uiNoiseNb);
			SetTimer(1, 1500, NULL);
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
	unsigned int uiIPAim;
	for (int i=0; i<GraphViewNum; i++)
	{
		if ((m_uiNoiseNb & (0x01<<i)) != 0)
		{
			uiIPAim	= 81 + 10*i;
			/*OnADCSampleStop(uiIPAim);*/
		}
	}
	OnADCSampleStop(uiIPAim);
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
	// 将ADC采样数据及运算结果保存成ANSI格式的文件
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
			strTemp.Format("%s\t\t",cppSelectObjectName[i]);
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
							strOutput += "\t\t";
							continue;
						}
						if (iDataLength > i)
						{	
							strTemp.Format("%2.9f\t",dSampleData[j][i]);
							strOutput += strTemp;
						}
						else
						{
							strOutput += "\t\t";
						}
					}
					else
					{
						strOutput += "\t\t";
					}
				}
				else
				{
					strOutput += "\t\t";
				}
			}
			strOutput += "\r\n";
		}

		strOutput += "\r\n数据处理结果：\r\n";

		// 输出仪器标签
		for (int i=0; i<iGraphViewNum; i++)
		{
			strTemp.Format("%s\t\t",cppSelectObjectName[i]);
			strOutput += strTemp;
		}
		strOutput += "\r\n";

		for (int i=0; i<iMathValueIdNum; i++)
		{
			if (i == 0)
			{
				strOutput += "平均值：\r\n";
			}
			else if (i == 1)
			{
				strOutput += "均方根：\r\n";
			}
			else if (i == 2)
			{
				strOutput += "最大值：\r\n";
			}
			else if (i == 3)
			{
				strOutput += "最小值：\r\n";
			}
			for (int j=0; j<iGraphViewNum; j++)
			{
				if (m_iSelectObject[j] == 1)
				{
					if (m_iSelectObjectNoise[j] == 0)
					{

						iDataLength = dSampleData[j].size()-iMathValueIdNum;
						if (iDataLength < 0)
						{
							strOutput += "\t\t";
							continue;
						}
						strTemp.Format("%2.9f\t",dSampleData[j][iDataLength+i]);
						strOutput += strTemp;
					}
					else
					{
						strOutput += "\t\t";
					}
				}
				else
				{
					strOutput += "\t\t";
				}
			}
			strOutput += "\r\n\r\n";
		}
		fprintf(savefile,"%s", strOutput); 
	}
	fclose(savefile); 
	//	数据采集结束后存ADC采样文件
	OnSaveADCDataToFile(iMathValueIdNum, iGraphViewNum, cppSelectObjectName, dSampleData, sysTime);
}
void CTabSample::OnSaveADCDataToFile(int iMathValueIdNum, int iGraphViewNum, char** cppSelectObjectName, vector <double>* dSampleData, SYSTEMTIME systime)
{
	CString strFileName = "";
	CString str = "";
	unsigned int uiSaveADCFileNum = 0;	// 保存ADC数据文件个数
	unsigned int uiADCFileLength = 0;	// 每个文件单道ADC数据个数
	GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->GetWindowText(str);
	uiADCFileLength = atoi(str);
	uiADCFileLength = uiADCFileLength * ReceiveDataSize;
	uiSaveADCFileNum = (m_iMaxLength - iMathValueIdNum)/uiADCFileLength;
	// 如果不足一个文件
	if(((m_iMaxLength - iMathValueIdNum)%uiADCFileLength) != 0)
	{
		uiSaveADCFileNum += 1;	
	}
	for (unsigned int k=0; k<uiSaveADCFileNum; k++)
	{
		strFileName.Format("%s\\%04d%02d%02d%02d%02d%02d%04d_%d.text", m_csSaveFilePath, systime.wYear, systime.wMonth, systime.wDay,
			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, k);
		// 将ADC采样数据及运算结果保存成ANSI格式的文件
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
				strTemp.Format("%s\t\t",cppSelectObjectName[i]);
				strOutput += strTemp;
			}
			strOutput += "\r\n";
			if (((m_iMaxLength - iMathValueIdNum)-k*uiADCFileLength)<uiADCFileLength)
			{
				// 输出各仪器采样数据
				for (unsigned int i=k*uiADCFileLength; i<(m_iMaxLength - iMathValueIdNum); i++)
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
									strOutput += "\t\t";
									continue;
								}
								if (iDataLength > i)
								{	
									strTemp.Format("%2.9f\t",dSampleData[j][i]);
									strOutput += strTemp;
								}
								else
								{
									strOutput += "\t\t";
								}
							}
							else
							{
								strOutput += "\t\t";
							}
						}
						else
						{
							strOutput += "\t\t";
						}
					}
					strOutput += "\r\n";
				}
			}
			else
			{
				// 输出各仪器采样数据
				for (unsigned int i=k*uiADCFileLength; i<(k+1)*uiADCFileLength; i++)
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
									strOutput += "\t\t";
									continue;
								}
								if (iDataLength > i)
								{	
									strTemp.Format("%2.9f\t",dSampleData[j][i]);
									strOutput += strTemp;
								}
								else
								{
									strOutput += "\t\t";
								}
							}
							else
							{
								strOutput += "\t\t";
							}
						}
						else
						{
							strOutput += "\t\t";
						}
					}
					strOutput += "\r\n";
				}
			}

			fprintf(savefile,"%s", strOutput); 
		}
		fclose(savefile); 
	}
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

// 发送数据采集命令帧
void CTabSample::OnSendCmdFrame(unsigned int uiTestNb)
{
	m_oThreadManage.m_oSysTimeSocket.m_uiTestNb = uiTestNb;

	m_oThreadManage.MakeCollectSysTimeFrameData(uiTestNb);
	m_oThreadManage.SendCollectSysTimeFrameToSocket();
}

void CTabSample::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	// 达到采样时间停止采样
// 	if (nIDEvent == 3)
// 	{
// 		m_HeartBeatSocket.SendHeartBeatFrame();
// 	}
	if (nIDEvent == 10)
	{
		// 关闭采样时间定时器
		KillTimer(10);
		OnBnClickedButtonStopsample();
	}
	// 监测尾包
	else if (nIDEvent == 4)
	{
		if (m_oThreadManage.m_TailFrameSocket.m_bTailRec == TRUE)
		{
			m_oThreadManage.m_TailFrameSocket.m_bTailRec = FALSE;
			m_uiTailRecCount = 0;
		}
		else
		{
			m_uiTailRecCount++;
			if (m_uiTailRecCount == TailFrameCount)
			{
				// 连续5秒未收到尾包则删除所有仪器
				m_oThreadManage.m_oInstrumentList.DeleteAllInstrument();
				m_uiTailRecCount = 0;
			}
		}
	}
	// 开始采样
	else if (nIDEvent == 1)
	{
		KillTimer(1);
		m_oThreadManage.m_ADCDataSocket.m_bStartSample = TRUE;
	}
	// 停止采样
	else if (nIDEvent == 2)
	{
		KillTimer(2);
	
		unsigned int uiADCDataNeedToSave = 0;
		// 计算仪器最大的采样长度
		uiADCDataNeedToSave = OnSampleDataMaxLength(GraphViewNum, m_oThreadManage.m_ADCDataSocket.m_dADCSave);
		// 保存剩余的ADC数据文件
		m_oThreadManage.m_ADCDataSocket.OnSaveRemainADCData(uiADCDataNeedToSave);

		CString str;
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
					str.Format("Avg = %2.6f",davg);
					GetDlgItem(m_iMathId[k][0])->SetWindowText(str);

					drms = SampleDataRms(m_dSampleData[k]);
					str.Format("RMS = %2.6f",drms);
					GetDlgItem(m_iMathId[k][1])->SetWindowText(str);

					dmax = SampleDataMax(m_dSampleData[k]);
					str.Format("Max = %2.6f",dmax);
					GetDlgItem(m_iMathId[k][2])->SetWindowText(str);

					dmin = SampleDataMin(m_dSampleData[k]);
					str.Format("Min = %2.6f",dmin);
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
	CDialog::OnTimer(nIDEvent);
}
// 滑动条消息响应函数，实现图形界面滚动条效果，nPos只能达到32k
void   CTabSample::OnHScroll(UINT   nSBCode,   UINT   nPos,   CScrollBar*   pScrollBar) 
{  
	int iPos = 0;
	int iPageMove = 0;
	UINT uiMovePos = 0;
	UINT uiMove = 0;
	unsigned int uiMoveTimes = 0;
	unsigned int uiXAxisPointNum = 0;
	int minpos = 0;
	int maxpos = 0;
	BOOL btest = FALSE;
	if (m_bStopSample == FALSE)
	{
		return;
	}
	if( IDC_SCROLLBAR_GRAPHVIEW == pScrollBar->GetDlgCtrlID())
	{
		pScrollBar->GetScrollRange(&minpos, &maxpos); 
		maxpos = pScrollBar->GetScrollLimit();

		// Get the current position of scroll box.
		int curpos = pScrollBar->GetScrollPos(); 
		switch( nSBCode ) 
		{
		case SB_PAGELEFT: 
			// Get the page size
			iPos = curpos;
			if (curpos > minpos)
			{
				curpos = max(minpos, curpos - 30);
				uiMove = iPos - curpos;
				OnMoveScrollBarToLeft(uiMove);
			}
			break;
		case SB_PAGERIGHT:
			// Get the page size. 
			iPos = curpos;
			if (curpos < maxpos)
			{
				curpos = min(maxpos, curpos + 30);
				uiMove = curpos - iPos;
				OnMoveScrollBarToRight(uiMove);
			}
			break;
		case SB_LINELEFT:
			if ((curpos - 5) > minpos)
			{
				OnMoveScrollBarToLeft(5);
				curpos   -=   5; 
			}
			else if (curpos > minpos)
			{
				OnMoveScrollBarToLeft(1);
				curpos--;
			}
			break;
		case  SB_LINERIGHT:
			if ((curpos + 5) < maxpos)
			{
				OnMoveScrollBarToRight(5);
				curpos   +=   5; 
			}
			else if (curpos < maxpos)
			{
				curpos++;
			}
			break;

			// 滚动条拖动到指定位置
		case SB_THUMBTRACK:
			// 突破nPos 32K的限制（16位），实现32位
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_TRACKPOS;
			pScrollBar->GetScrollInfo(&info);
			nPos = info.nTrackPos;

			if (nPos > maxpos)
			{
				nPos = maxpos;
			}
			if (nPos < minpos)
			{
				nPos = minpos;
			}
			if (curpos > nPos)
			{
				uiMovePos = curpos - nPos;
			}
			else
			{
				uiMovePos = nPos - curpos;
			}
			uiXAxisPointNum = m_OScopeCtrl[0].XAxisPointNum;

			// 每次移动横坐标最大尺寸的一半
			uiMoveTimes = 2*uiMovePos/uiXAxisPointNum;

			if (2*uiMovePos%uiXAxisPointNum != 0)
			{
				uiMoveTimes += 1;
			}
			for (unsigned int i=0; i<uiMoveTimes; i++)
			{
				unsigned int uitmp = uiMovePos - (uiXAxisPointNum/2)*i;
				if (uitmp < uiXAxisPointNum/2)
				{
					uiMove = uitmp;
				}
				else
				{
					uiMove = uiXAxisPointNum/2;
				}
				if (curpos > nPos)
				{
					OnMoveScrollBarToLeft(uiMove);
				}
				else
				{
					OnMoveScrollBarToRight(uiMove);
				}
			} 
			curpos   =   nPos;
			break;
		default: 
			return; 
		} 
		pScrollBar-> SetScrollPos(   curpos,   TRUE   ); 
	
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

} 
void CTabSample::OnBnClickedCheckHeartbeat()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	BOOL bStatus = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_HEARTBEAT)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
	if (bStatus == TRUE)
	{
		m_oThreadManage.m_oHeartBeatThread.ResumeThread();
		m_oThreadManage.m_oHeartBeatThread.OnOpen();
// 		KillTimer(3);
// 		SetTimer(3, 250, NULL);
	}
	else
	{
		m_oThreadManage.m_oHeartBeatThread.SuspendThread();
		m_oThreadManage.m_oHeartBeatThread.OnStop();
//		KillTimer(3);
	}
}

void CTabSample::OnBnClickedButtonSetadc()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
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
			}
		}
	}
	// 目标IP地址
	unsigned int uiIPAim = 0;
	for (int i=0; i<GraphViewNum; i++)
	{
		if ((m_uiSampleNb & (0x01<<i)) != 0)
		{
 			uiIPAim	= 81 + 10*i;
			/*OnADCSet(uiIPAim);*/
			/*Sleep(500);*/
		}
	}
	OnADCSet(uiIPAim);
}

// ADC参数设置
void CTabSample::OnADCSet(unsigned int uiIPAim)
{
	m_oADCSet.ADCSetFrameHead(uiIPAim);
	m_oADCSet.OnSetSine();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnOpenPower();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnOpenPowerTB();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnOpenSwitch();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnOpenSwitchTB();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCRegisterWrite();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCRegisterRead();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCSetReturn();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
}

// ADC数据采集停止
void CTabSample::OnADCSampleStop(unsigned int uiIPAim)
{
	m_oADCSet.ADCSetFrameHead(uiIPAim);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
}

void CTabSample::OnBnClickedButtonTimecal()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oThreadManage.m_TailFrameSocket.SendTailTimeFrame();
}

// 滚动条左移
void CTabSample::OnMoveScrollBarToRight(unsigned int uiMovePos)
{
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
				m_OScopeCtrl[i].LeftDrawPoint(m_dSampleData[i], MathValueIdNum, uiMovePos);
			}
		}
	}
}

// 滚动条右移
void CTabSample::OnMoveScrollBarToLeft(unsigned int uiMovePos)
{
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
				m_OScopeCtrl[i].RightDrawPoint(m_dSampleData[i], MathValueIdNum, uiMovePos);
			}
		}
	}
}
