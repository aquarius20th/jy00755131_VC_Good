// GraphView.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabSample.h"
#include <math.h>
#include  <io.h>


// CGraphView 对话框

IMPLEMENT_DYNAMIC(CTabSample, CDialog)

CTabSample::CTabSample(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSample::IDD, pParent)
	, m_csSaveFilePath(_T(""))
	, m_uiScrollBarMax(100)
	, m_uiScrollBarMin(0)
	, m_bMatrixNetwork(TRUE)
	, m_bStartSample(FALSE)
	, m_iSelectObject1(0)
	, m_iSelectObject2(0)
	, m_iSelectObject3(0)
	, m_iSelectObject4(0)
	, m_iSelectObjectNoise1(0)
	, m_iSelectObjectNoise2(0)
	, m_iSelectObjectNoise3(0)
	, m_iSelectObjectNoise4(0)
	, m_uiSampleNb(0)
	, m_iMaxLength(0)
	, m_bStopSample(FALSE)
	, m_bHeartBeat(TRUE)
	, m_uiTailRecCount(0)
	, m_uiADCFileLength(0)
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
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILEPATH2, &CTabSample::OnBnClickedButtonSavefilepath)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CTabSample::OnBnClickedButtonReset)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_HEARTBEAT, &CTabSample::OnBnClickedCheckHeartbeat)
	ON_BN_CLICKED(IDC_BUTTON_SETADC, &CTabSample::OnBnClickedButtonSetadc)
	ON_BN_CLICKED(IDC_BUTTON_TIMECAL, &CTabSample::OnBnClickedButtonTimecal)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CTabSample::OnBnClickedButtonTest)
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
		m_dSampleData[i].reserve(1024);
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
	m_oThreadManage.m_IPSetSocket.m_pADCSet = &m_oADCSet;


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
	m_oThreadManage.m_HeadFrameSocket.m_pwnd = this;
//	m_oThreadManage.m_ADCDataSocket.m_pwnd = this;

	m_oThreadManage.m_ADCDataSocket.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_IPSetSocket.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_ADCDataSocket.m_pSelectObjectNoise = m_iSelectObjectNoise;
	for(int i=0; i<GraphViewNum; i++)
	{
//		m_oThreadManage.m_ADCDataSocket.m_pSampleData[i] = &m_dSampleData[i];
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
	OnDisableButtons();
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
	m_oThreadManage.m_IPSetSocket.m_uiSendPort = itemp;
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
	
		UpdateData(TRUE);

		m_iSelectObject[0] = m_iSelectObject1;
		m_iSelectObject[1] = m_iSelectObject2;
		m_iSelectObject[2] = m_iSelectObject3;
		m_iSelectObject[3] = m_iSelectObject4;

		m_iSelectObjectNoise[0] = m_iSelectObjectNoise1;
		m_iSelectObjectNoise[1] = m_iSelectObjectNoise2;
		m_iSelectObjectNoise[2] = m_iSelectObjectNoise3;
		m_iSelectObjectNoise[3] = m_iSelectObjectNoise4;

	
		for (int i=0; i<GraphViewNum; i++)
		{
			if (m_iSelectObject[i] == 1)
			{	
				bSelect = TRUE;	
			}
		}

		// 至少有一个仪器被选中
		if (bSelect == TRUE)
		{
			CString str = "";
			SYSTEMTIME  sysTime;

			m_bStartSample = TRUE;
			m_bStopSample = FALSE;

			for (int i=0; i<GraphViewNum; i++)
			{
				m_oThreadManage.m_ADCDataSocket.m_uiADCDataFrameCount[i] = 0;
				m_oThreadManage.m_ADCDataSocket.m_uiADCRecCompleteCount[i] = 0;
				// 初始化绘图界面
				m_OScopeCtrl[i].XAxisLabelMin = 0;
				m_OScopeCtrl[i].XAxisLabelMax = m_OScopeCtrl[i].XAxisPointNum;
				m_OScopeCtrl[i].Reset();
				m_OScopeCtrl[i].m_dPreviousPosition = 0.0;
				m_OScopeCtrl[i].PointNum = 0;			// 绘制了的点的个数初值为0
			}

			m_oThreadManage.m_ADCDataSocket.m_bStartSample = FALSE;

			// 创建本次ADC数据采集数据存储文件夹
		
			GetLocalTime(&sysTime);
			str = _T("ADC数据备份");
			m_oThreadManage.m_ADCDataSocket.m_csSaveFilePath.Format("%s\\%s_%04d_%02d_%02d_%02d_%02d_%02d_%04d", m_csSaveFilePath, str, sysTime.wYear,sysTime.wMonth,sysTime.wDay,
				sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
			// 按日期创建保存ADC采样数据的文件夹
			CreateDirectory(m_oThreadManage.m_ADCDataSocket.m_csSaveFilePath,NULL);
			m_oThreadManage.m_ADCDataSocket.m_uiADCSaveFileNum = 0;
			m_oThreadManage.m_ADCDataSocket.m_uiADCDataToSaveNum = 0;
			
			GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->GetWindowText(str);
			m_uiADCFileLength = atoi(str);
			m_uiADCFileLength = m_uiADCFileLength * ReceiveDataSize;
			m_oThreadManage.m_ADCDataSocket.m_uiADCFileLength = m_uiADCFileLength;
			

			// 发送开始采样操作命令帧
			OnSendCmdFrame(m_iSelectObject);
			SetTimer(1, 1500, NULL);
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

	unsigned int uiIPAim;
	uiIPAim = 0xffffffff;
	OnADCSampleStop(uiIPAim);

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

		m_oThreadManage.m_ADCDataSocket.m_dSampleTemp[i].clear();
		m_oThreadManage.m_ADCDataSocket.m_dADCSave[i].clear();
		m_oThreadManage.m_ADCDataSocket.m_uiADCDataFrameCount[i] = 0;
		m_oThreadManage.m_ADCDataSocket.m_uiADCRecCompleteCount[i] = 0;
	}
	m_bStopSample = FALSE;
	m_bStartSample = FALSE;

	m_oThreadManage.m_ADCDataSocket.m_bStartSample = FALSE;
	m_oThreadManage.m_ADCDataSocket.m_uiADCSaveFileNum = 0;
	m_oThreadManage.m_ADCDataSocket.m_uiADCDataToSaveNum = 0;
	m_iSelectObject1 = 0;
	m_iSelectObject2 = 0;
	m_iSelectObject3 = 0;
	m_iSelectObject4 = 0;
	m_uiADCFileLength = 0;
	m_uiSampleNb = 0;
	m_iMaxLength = 0;
	m_uiTailRecCount = 0;
	UpdateData(FALSE);
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
void CTabSample::OnSendCmdFrame(int* pSelectObject)
{
	m_oThreadManage.MakeCollectSysTimeFrameData(pSelectObject);
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
	CInstrument* pInstrument = NULL;	// 仪器对象指针
	POSITION pos = NULL;				// 位置	
	unsigned int uiKey;					// 索引键	

	// 定时采样
	if (nIDEvent == 10)
	{
		// 关闭采样时间定时器
		KillTimer(10);
		OnBnClickedButtonStopsample();
	}
	// 监测IP地址设置应答帧
	else if (nIDEvent == 5)
	{
		KillTimer(5);
		bool	bIPSetAllOk = true;
		unsigned int icount = m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetCount();
		if (icount == 0)
		{
			return;
		}
		pos = m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetStartPosition();	// 得到索引表起始位置
		while(NULL != pos)
		{
			pInstrument = NULL;		
			m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetNextAssoc(pos, uiKey, pInstrument);	// 得到仪器对象
			if(NULL != pInstrument)	
			{
				if (pInstrument->m_bIPSetOK == false)
				{
					m_oThreadManage.m_HeadFrameSocket.MakeIPSetFrame(pInstrument);
					m_oThreadManage.m_HeadFrameSocket.SendIPSetFrame();
					SetTimer(5, 10000, NULL);
					bIPSetAllOk = false;
				}
			}
		}
		// 所有仪器的IP地址设置都完成后
		if (bIPSetAllOk == true)
		{
			SetTimer(6, 1000, NULL);
		}
	}
	// 先将设备的本地时间设为0
	else if (nIDEvent == 6)
	{
		KillTimer(6);
		pos = m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetStartPosition();	// 得到索引表起始位置
		while(NULL != pos)
		{
			pInstrument = NULL;		
			m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetNextAssoc(pos, uiKey, pInstrument);	// 得到仪器对象
			if(NULL != pInstrument)	
			{
				if (pInstrument->m_bIPSetOK == true)
				{
					// 生成时统报文设置帧
					m_oThreadManage.m_TailTimeFrameSocket.MakeTimeDelayFrameData(pInstrument, 0, 0);
					// 发送时统报文设置帧
					m_oThreadManage.m_TailTimeFrameSocket.SendTimeDelayFrameData();
				}
			}
		}
		SetTimer(7, 2000, NULL);
	}
	// 做时统
	else if (nIDEvent == 7)
	{
		KillTimer(7);
		m_oThreadManage.m_TailFrameSocket.SendTailTimeFrame();
		SetTimer(8, 1000, NULL);
	}
	else if (nIDEvent == 8)
	{
		KillTimer(8);
		unsigned int uiIPAim = 0;
		uiIPAim = 0xffffffff;
		OnADCSet(uiIPAim);

		OnEnableButtons();

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
				OnDisableButtons();
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

		m_iMaxLength = 0;
		// 计算仪器最大的采样长度
		for (int i=0; i<GraphViewNum; i++)
		{
			if (m_iSelectObject[i] == 1)
			{
				if (m_iSelectObjectNoise[i] == 0)
				{
					if (m_iMaxLength < m_oThreadManage.m_ADCDataSocket.m_uiADCDataFrameCount[i])
					{
						m_iMaxLength = m_oThreadManage.m_ADCDataSocket.m_uiADCDataFrameCount[i];
					}
				}
			}
		}
		m_iMaxLength = m_iMaxLength * ReceiveDataSize;
		if (m_iMaxLength == 0)
		{
			return;
		}

		// 横向滚动条的最大和最小值
		if (m_iMaxLength > m_OScopeCtrl[0].XAxisPointNum)
		{
			m_uiScrollBarMax = m_iMaxLength - m_OScopeCtrl[0].XAxisPointNum; 
		}
		else
		{
			m_uiScrollBarMax = m_iMaxLength;
		}
		m_uiScrollBarMin = 0; 

		CScrollBar*pSB =(CScrollBar*)GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW); 
		pSB->SetScrollRange(m_uiScrollBarMin, m_uiScrollBarMax); 
		pSB->SetScrollPos(m_uiScrollBarMax, TRUE);
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
			if (curpos > minpos)
			{
				curpos = max(minpos, curpos - 30);
				OnMoveScrollBarToPos(curpos);
			}
			break;
		case SB_PAGERIGHT:
			// Get the page size. 
			if (curpos < maxpos)
			{
				curpos = min(maxpos, curpos + 30);
				OnMoveScrollBarToPos(curpos);
			}
			break;
		case SB_LINELEFT:
			if ((curpos - 5) > minpos)
			{
				curpos   -=   5; 
			}
			else if (curpos > minpos)
			{	
				curpos--;
			}
			OnMoveScrollBarToPos(curpos);
			break;
		case  SB_LINERIGHT:
			if ((curpos + 5) < maxpos)
			{
				curpos   +=   5; 
			}
			else if (curpos < maxpos)
			{
				curpos++;
			}
			OnMoveScrollBarToPos(curpos);
			break;

			// 滚动条拖动到指定位置
		case SB_THUMBPOSITION:
			// 突破nPos 32K的限制（16位），实现32位
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_TRACKPOS;
			pScrollBar->GetScrollInfo(&info);
			iPos = info.nTrackPos;

			if (iPos > maxpos)
			{
				iPos = maxpos;
			}
			if (iPos < minpos)
			{
				iPos = minpos;
			}
			curpos   =   iPos;
			OnMoveScrollBarToPos(curpos);
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
	
	// 以广播方式发送ADC参数设置
	// 目标IP地址
	unsigned int uiIPAim = 0;
	uiIPAim = 0xffffffff;
	OnADCSet(uiIPAim);
}

// ADC参数设置
void CTabSample::OnADCSet(unsigned int uiIPAim)
{
	m_oADCSet.ADCSetFrameHead(uiIPAim, true);
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
	m_oADCSet.ADCSetFrameHead(uiIPAim, true);

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

// 读ADC数据文件
void CTabSample::OnReadADCDataFile(CString csPathName, unsigned int uiPos, unsigned int uiPosNum)
{
	CString strtemp = "";
	CString cstmp = "";
	unsigned int uiRowCount = 0;
	unsigned int uicolCount = 0;
	double db;
	int iDirectionPrevious = 0;
	int iDirectionNow = 0;
	
	if ((_access(csPathName,0)) != -1)
	{
		CStdioFile   file(csPathName,CFile::modeRead);	//只读   
		while(file.ReadString(strtemp))					// 每读完一行，指向下一行，如果读到文件末尾，退出循环   
		{ 
			uiRowCount++;
			// 第4行开始为ADC采集数据
			if (uiRowCount > ADCFileHeadRow)
			{
				uicolCount = 0;
				iDirectionPrevious = 0;
				iDirectionNow = 0;
				if ((uiRowCount - ADCFileHeadRow) > (uiPos + uiPosNum))
				{
					break;
				}
				if ((uiRowCount - ADCFileHeadRow) > uiPos)
				{
					for (int j=0; j<GraphViewNum; j++)
					{
						uicolCount++;

						if (uicolCount > 1)
						{
							iDirectionPrevious+= 1;
						}
						iDirectionNow = strtemp.Find(_T("	"), iDirectionPrevious);
						cstmp = strtemp.Mid(iDirectionPrevious, iDirectionNow-iDirectionPrevious);
						iDirectionPrevious = iDirectionNow;
						if (cstmp == "")
						{
							iDirectionPrevious+= 1;
							continue;
						}
						db = atof(cstmp);
						m_dSampleData[j].push_back(db);
					}
				}
			}
		}
	}
	else
	{
		AfxMessageBox(_T("文件不存在！"));
		return;
	}
}

void CTabSample::OnBnClickedButtonTest()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
// 	CString csPathName = "";
// 	csPathName = m_csSaveFilePath + _T("\\数据备份\\1.text");
// 	OnReadADCDataFile(csPathName, 0);
}

// 滚动条拖动到坐标点读取文件绘图
void CTabSample::OnMoveScrollBarToPos(unsigned int uiPos)
{
	CString strtmp = "";
	CString csPathName = "";
	unsigned int uiReadADCFileNb = 0;
	unsigned int uiDrawDataNum = 0;
	double dDrawData;

//	uiPos += 1;
	uiReadADCFileNb = uiPos/m_uiADCFileLength;
	if ((uiPos%m_uiADCFileLength) != 0)
	{
		uiReadADCFileNb += 1;
	}
	if (uiPos == 0)
	{
		uiReadADCFileNb = 1;
	}
	strtmp.Format("\\%d.text", uiReadADCFileNb);
	csPathName = m_oThreadManage.m_ADCDataSocket.m_csSaveFilePath + strtmp;
	if (uiPos == 0)
	{
		OnReadADCDataFile(csPathName, 0, m_OScopeCtrl[0].XAxisPointNum);
	}
	else
	{
		if ((uiPos + m_OScopeCtrl[0].XAxisPointNum) > uiReadADCFileNb*m_uiADCFileLength)
		{
			OnReadADCDataFile(csPathName, uiPos - (uiReadADCFileNb - 1)*m_uiADCFileLength - 1, (uiReadADCFileNb*m_uiADCFileLength - uiPos + 1));
			strtmp.Format("\\%d.text", uiReadADCFileNb + 1);
			csPathName = m_oThreadManage.m_ADCDataSocket.m_csSaveFilePath + strtmp;
			OnReadADCDataFile(csPathName, 0, m_OScopeCtrl[0].XAxisPointNum + 1 - (uiReadADCFileNb*m_uiADCFileLength - uiPos + 1));
		}
		else
		{
			OnReadADCDataFile(csPathName, uiPos - (uiReadADCFileNb - 1)*m_uiADCFileLength - 1, m_OScopeCtrl[0].XAxisPointNum + 1);
		}
	}

	
	for (int i=0; i<GraphViewNum; i++)
	{
		// 绘图
		uiDrawDataNum = m_dSampleData[i].size();
		if (uiDrawDataNum == 0)
		{
			m_OScopeCtrl[i].XAxisLabelMin = 0;
		}
		// 初始化绘图界面
		m_OScopeCtrl[i].Reset();
		// 绘图
		if (uiDrawDataNum > 0)
		{
			m_OScopeCtrl[i].XAxisLabelMin = uiPos;
			m_OScopeCtrl[i].XAxisLabelMax = uiPos + m_OScopeCtrl[0].XAxisPointNum;
			m_OScopeCtrl[i].PointNum = 0;
			if (uiPos == 0)
			{
				m_OScopeCtrl[i].m_dPreviousPosition = 0.0;
			}
			else 
			{
				m_OScopeCtrl[i].m_dPreviousPosition = m_dSampleData[i][0];
			}

			for (unsigned int j=1; j<uiDrawDataNum; j++)
			{
				dDrawData = m_dSampleData[i][j];
				m_OScopeCtrl[i].DrawPoint(dDrawData);
			}
			m_dSampleData[i].clear();
		}
	}
}

// 使能button控件
void CTabSample::OnEnableButtons(void)
{
	GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(TRUE);
}

// 禁用Button控件
void CTabSample::OnDisableButtons(void)
{
	GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_TIMECAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SETADC)->EnableWindow(FALSE);
}
