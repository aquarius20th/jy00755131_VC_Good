// GraphView.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabSample.h"
#include <math.h>
#include "GraphShowDlg.h"

// CGraphView 对话框

IMPLEMENT_DYNAMIC(CTabSample, CDialog)

CTabSample::CTabSample(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSample::IDD, pParent)
	, m_csSaveFilePath(_T(""))
	, m_bMatrixNetwork(TRUE)
	, m_bStartSample(FALSE)
	, m_bHeartBeat(TRUE)
	, m_uiTailRecCount(0)
	, m_uiADCFileLength(0)
	, m_uiSendPort(0)
	, m_bTailCountStart(false)
{

}

CTabSample::~CTabSample()
{

}

void CTabSample::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_MATRIXNETWORK, m_bMatrixNetwork);
	DDX_Check(pDX, IDC_CHECK_HEARTBEAT, m_bHeartBeat);
}


BEGIN_MESSAGE_MAP(CTabSample, CDialog)
	ON_BN_CLICKED(IDC_CHECK_MATRIXNETWORK, &CTabSample::OnBnClickedCheckMatrixnetwork)
	ON_BN_CLICKED(IDC_BUTTOT_STARTSAMPLE, &CTabSample::OnBnClickedButtotStartsample)
	ON_BN_CLICKED(IDC_BUTTON_STOPSAMPLE, &CTabSample::OnBnClickedButtonStopsample)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILEPATH2, &CTabSample::OnBnClickedButtonSavefilepath)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CTabSample::OnBnClickedButtonReset)
	ON_WM_TIMER()
//	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_HEARTBEAT, &CTabSample::OnBnClickedCheckHeartbeat)
	ON_BN_CLICKED(IDC_BUTTON_SETADC, &CTabSample::OnBnClickedButtonSetadc)
	ON_BN_CLICKED(IDC_BUTTON_TIMECAL, &CTabSample::OnBnClickedButtonTimecal)
	ON_BN_CLICKED(IDC_BUTTON_FDU1, &CTabSample::OnBnClickedButtonFdu1)
	ON_BN_CLICKED(IDC_BUTTON_FDU2, &CTabSample::OnBnClickedButtonFdu2)
	ON_BN_CLICKED(IDC_BUTTON_FDU3, &CTabSample::OnBnClickedButtonFdu3)
	ON_BN_CLICKED(IDC_BUTTON_FDU4, &CTabSample::OnBnClickedButtonFdu4)
	ON_BN_CLICKED(IDC_BUTTON_FDU5, &CTabSample::OnBnClickedButtonFdu5)
	ON_BN_CLICKED(IDC_BUTTON_FDU6, &CTabSample::OnBnClickedButtonFdu6)
	ON_BN_CLICKED(IDC_BUTTON_FDU7, &CTabSample::OnBnClickedButtonFdu7)
	ON_BN_CLICKED(IDC_BUTTON_FDU8, &CTabSample::OnBnClickedButtonFdu8)
	ON_BN_CLICKED(IDC_BUTTON_FDU9, &CTabSample::OnBnClickedButtonFdu9)
	ON_BN_CLICKED(IDC_BUTTON_FDU10, &CTabSample::OnBnClickedButtonFdu10)
	ON_BN_CLICKED(IDC_BUTTON_FDU11, &CTabSample::OnBnClickedButtonFdu11)
	ON_BN_CLICKED(IDC_BUTTON_FDU12, &CTabSample::OnBnClickedButtonFdu12)
	ON_BN_CLICKED(IDC_BUTTON_FDU13, &CTabSample::OnBnClickedButtonFdu13)
	ON_BN_CLICKED(IDC_BUTTON_FDU14, &CTabSample::OnBnClickedButtonFdu14)
	ON_BN_CLICKED(IDC_BUTTON_FDU15, &CTabSample::OnBnClickedButtonFdu15)
	ON_BN_CLICKED(IDC_BUTTON_FDU16, &CTabSample::OnBnClickedButtonFdu16)
	ON_BN_CLICKED(IDC_BUTTON_FDU17, &CTabSample::OnBnClickedButtonFdu17)
	ON_BN_CLICKED(IDC_BUTTON_FDU18, &CTabSample::OnBnClickedButtonFdu18)
	ON_BN_CLICKED(IDC_BUTTON_FDU19, &CTabSample::OnBnClickedButtonFdu19)
	ON_BN_CLICKED(IDC_BUTTON_FDU20, &CTabSample::OnBnClickedButtonFdu20)
	ON_BN_CLICKED(IDC_BUTTON_FDU21, &CTabSample::OnBnClickedButtonFdu21)
	ON_BN_CLICKED(IDC_BUTTON_FDU22, &CTabSample::OnBnClickedButtonFdu22)
	ON_BN_CLICKED(IDC_BUTTON_FDU23, &CTabSample::OnBnClickedButtonFdu23)
	ON_BN_CLICKED(IDC_BUTTON_FDU24, &CTabSample::OnBnClickedButtonFdu24)
	ON_BN_CLICKED(IDC_BUTTON_FDU25, &CTabSample::OnBnClickedButtonFdu25)
	ON_BN_CLICKED(IDC_BUTTON_FDU26, &CTabSample::OnBnClickedButtonFdu26)
	ON_BN_CLICKED(IDC_BUTTON_FDU27, &CTabSample::OnBnClickedButtonFdu27)
	ON_BN_CLICKED(IDC_BUTTON_FDU28, &CTabSample::OnBnClickedButtonFdu28)
	ON_BN_CLICKED(IDC_BUTTON_FDU29, &CTabSample::OnBnClickedButtonFdu29)
	ON_BN_CLICKED(IDC_BUTTON_FDU30, &CTabSample::OnBnClickedButtonFdu30)
	ON_BN_CLICKED(IDC_BUTTON_FDU31, &CTabSample::OnBnClickedButtonFdu31)
	ON_BN_CLICKED(IDC_BUTTON_FDU32, &CTabSample::OnBnClickedButtonFdu32)
	ON_BN_CLICKED(IDC_BUTTON_FDU33, &CTabSample::OnBnClickedButtonFdu33)
	ON_BN_CLICKED(IDC_BUTTON_FDU34, &CTabSample::OnBnClickedButtonFdu34)
	ON_BN_CLICKED(IDC_BUTTON_FDU35, &CTabSample::OnBnClickedButtonFdu35)
	ON_BN_CLICKED(IDC_BUTTON_FDU36, &CTabSample::OnBnClickedButtonFdu36)
	ON_BN_CLICKED(IDC_BUTTON_FDU37, &CTabSample::OnBnClickedButtonFdu37)
	ON_BN_CLICKED(IDC_BUTTON_FDU38, &CTabSample::OnBnClickedButtonFdu38)
	ON_BN_CLICKED(IDC_BUTTON_FDU39, &CTabSample::OnBnClickedButtonFdu39)
	ON_BN_CLICKED(IDC_BUTTON_FDU40, &CTabSample::OnBnClickedButtonFdu40)
	ON_BN_CLICKED(IDC_BUTTON_FDU41, &CTabSample::OnBnClickedButtonFdu41)
	ON_BN_CLICKED(IDC_BUTTON_FDU42, &CTabSample::OnBnClickedButtonFdu42)
	ON_BN_CLICKED(IDC_BUTTON_FDU43, &CTabSample::OnBnClickedButtonFdu43)
	ON_BN_CLICKED(IDC_BUTTON_FDU44, &CTabSample::OnBnClickedButtonFdu44)
	ON_BN_CLICKED(IDC_BUTTON_FDU45, &CTabSample::OnBnClickedButtonFdu45)
	ON_BN_CLICKED(IDC_BUTTON_FDU46, &CTabSample::OnBnClickedButtonFdu46)
	ON_BN_CLICKED(IDC_BUTTON_FDU47, &CTabSample::OnBnClickedButtonFdu47)
	ON_BN_CLICKED(IDC_BUTTON_FDU48, &CTabSample::OnBnClickedButtonFdu48)
	ON_BN_CLICKED(IDC_BUTTON_SETBYHAND, &CTabSample::OnBnClickedButtonSetbyhand)
END_MESSAGE_MAP()


// 初始化
//************************************
// Method:    OnInit
// FullName:  CTabSample::OnInit
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabSample::OnInit(void)
{
	CString strtmp = _T("");
	// 得到当前路径
	char cSaveToFilePath[MAX_PATH];
	// 得到本机IP地址
	char		name[256]; 
	CString ip = _T(""); 
	PHOSTENT	hostinfo; 
	CString str = _T("");
	CButton* iconbutton = NULL;
	CStatic* iconstatic = NULL;
	CButton* pButton = NULL;

	// 控件选择
	for (int k=0; k<InstrumentNum; k++)
	{
		ProcessMessages();
		m_iSelectObject[k] = 0;
		m_iSelectObjectNoise[k] = 0;
		m_iButtonIDFDU[k] = IDC_BUTTON_FDU1 + k;
		m_iCheckIDInstrumentFDU[k] = IDC_CHECK_GRAPHVIEW1 + k;
		m_iCheckIDNoiseFDU[k] = IDC_CHECK_NOISE1 + k;
	}

	for (int i=0; i<InstrumentNum; i++)
	{
		ProcessMessages();
		strtmp.Format(_T("仪器%d"), i+1);
		char* pchar = strtmp.GetBuffer(0); 
		strcpy_s(m_cSelectObjectName[i],pchar);
		pchar = NULL;
		delete pchar;
	}

	GetCurrentDirectory(MAX_PATH, cSaveToFilePath);
	m_csSaveFilePath = cSaveToFilePath;
	
	for (int i=0; i<InstrumentNum; i++)
	{
		m_dlgADCData[i].m_ADCDataRecThread = &m_oThreadManage.m_oADCDataRecThread;
	}

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
	m_oThreadManage.m_oADCDataRecThread.m_csIPSource = ip;
	m_oThreadManage.m_oHeartBeatThread.m_csIPSource = ip;
	m_oADCSet.m_csIPSource = ip;
	m_oThreadManage.m_oSysTimeSocket.m_pADCSet = &m_oADCSet;
	m_oThreadManage.m_IPSetSocket.m_pADCSet = &m_oADCSet;

	str.Format("0x%04x", m_uiSendPort);
	GetDlgItem(IDC_EDIT_SENDPORT)->SetWindowText(str);

	str.Format("%d", m_uiADCFileLength);
	GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->SetWindowText(str);

	m_iconFDUDisconnected = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_iconFDUConnected = AfxGetApp()->LoadIcon(IDI_ICON2);
	m_iconLAUXDisconnected = AfxGetApp()->LoadIcon(IDI_ICON3);
	m_iconLAUXConnected = AfxGetApp()->LoadIcon(IDI_ICON4);
	
	m_oThreadManage.m_oInstrumentList.m_iconFDUDisconnected = m_iconFDUDisconnected;
	m_oThreadManage.m_oInstrumentList.m_iconLAUXDisconnected = m_iconLAUXDisconnected;
	m_oThreadManage.m_oInstrumentList.m_pwnd = this;
	m_oThreadManage.m_IPSetSocket.m_iconFDUConnected = m_iconFDUConnected;
	m_oThreadManage.m_IPSetSocket.m_iconLAUXConnected= m_iconLAUXConnected;
	m_oThreadManage.m_IPSetSocket.m_pwnd = this;
	m_oThreadManage.m_HeadFrameSocket.m_pwnd = this;
	m_oThreadManage.m_oSysTimeSocket.m_pwnd = this;
	
	m_oThreadManage.m_oSysTimeSocket.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_oADCDataRecThread.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_IPSetSocket.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_oADCDataRecThread.m_pSelectObjectNoise = m_iSelectObjectNoise;
	for(int i=0; i<InstrumentNum; i++)
	{
		m_oThreadManage.m_oADCDataRecThread.m_cSelectObjectName[i] = m_cSelectObjectName[i];
		m_oThreadManage.m_oInstrumentList.m_iButtonIDFDU[i] = m_iButtonIDFDU[i];
		m_oThreadManage.m_IPSetSocket.m_iButtonIDFDU[i] = m_iButtonIDFDU[i];
		m_oThreadManage.m_oInstrumentList.m_iCheckIDInstrumentFDU[i] = m_iCheckIDInstrumentFDU[i];
		m_oThreadManage.m_IPSetSocket.m_iCheckIDInstrumentFDU[i] = m_iCheckIDInstrumentFDU[i];
	}
	
	iconstatic =(CStatic*)this->GetDlgItem(IDC_STATIC_LAUX);
	iconstatic->SetIcon(m_iconLAUXDisconnected);
	iconstatic = NULL;
	
	for (int i=0; i<InstrumentNum; i++)
	{
		ProcessMessages();
		iconbutton = (CButton*)this->GetDlgItem(m_iButtonIDFDU[i]);
		iconbutton->SetIcon(m_iconFDUDisconnected);
		iconbutton = NULL;
		pButton = (CButton*)GetDlgItem(m_iCheckIDInstrumentFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;
		pButton = (CButton*)GetDlgItem(m_iCheckIDNoiseFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;
	}
	delete iconstatic;
	delete iconbutton;
	delete pButton;

	m_oThreadManage.OnInit();
	OnOpen();
	// 监测尾包定时器
	SetTimer(TabSampleTailMonitorTimerNb, TabSampleTailMonitorTimerSet, NULL);
	OnDisableButtons();
}


// 打开
//************************************
// Method:    OnOpen
// FullName:  CTabSample::OnOpen
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabSample::OnOpen(void)
{
	CString str = _T("");

	GetDlgItem(IDC_EDIT_SENDPORT)->GetWindowText(str);
	sscanf_s(str,"%x", &m_uiSendPort);
	m_oThreadManage.m_oHeartBeatThread.m_uiSendPort = m_uiSendPort;
	m_oThreadManage.m_HeadFrameSocket.m_uiSendPort = m_uiSendPort;
	m_oThreadManage.m_TailFrameSocket.m_uiSendPort = m_uiSendPort;
	m_oThreadManage.m_TailTimeFrameSocket.m_uiSendPort = m_uiSendPort;
	m_oThreadManage.m_oSysTimeSocket.m_uiSendPort = m_uiSendPort;
	m_oThreadManage.m_oADCDataRecThread.m_uiSendPort = m_uiSendPort;
	m_oThreadManage.m_IPSetSocket.m_uiSendPort = m_uiSendPort;
	GetDlgItem(IDC_EDIT_SENDPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_HEARTBEAT)->EnableWindow(TRUE);

	m_oThreadManage.OnOpen();
}

// 停止
//************************************
// Method:    OnStop
// FullName:  CTabSample::OnStop
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabSample::OnStop(void)
{
	CButton* iconbutton = NULL;
	CStatic* iconstatic = NULL;
	CButton* pButton = NULL;

	iconstatic =(CStatic*)this->GetDlgItem(IDC_STATIC_LAUX);
	iconstatic->SetIcon(m_iconLAUXDisconnected);
	iconstatic = NULL;

	for (int i=0; i<InstrumentNum; i++)
	{
		ProcessMessages();
		iconbutton = (CButton*)this->GetDlgItem(m_iButtonIDFDU[i]);
		iconbutton->SetIcon(m_iconFDUDisconnected);
		iconbutton = NULL;
		pButton = (CButton*)GetDlgItem(m_iCheckIDInstrumentFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;
		pButton = (CButton*)GetDlgItem(m_iCheckIDNoiseFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;
	}
	delete iconstatic;
	delete iconbutton;
	delete pButton;

	GetDlgItem(IDC_EDIT_SENDPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_HEARTBEAT)->EnableWindow(FALSE);
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

//************************************
// Method:    OnClose
// FullName:  CTabSample::OnClose
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
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
		CButton* pButton = NULL;
	
		UpdateData(TRUE);
		for (int i=0; i<InstrumentNum; i++)
		{
			ProcessMessages();
			pButton = (CButton*)GetDlgItem(m_iCheckIDInstrumentFDU[i]);
			m_iSelectObject[i] = pButton->GetCheck();
			pButton = NULL;
			pButton = (CButton*)GetDlgItem(m_iCheckIDNoiseFDU[i]);
			m_iSelectObjectNoise[i] = pButton->GetCheck();
			pButton = NULL;
		}
		pButton = NULL;
		delete pButton;

		for (int i=0; i<InstrumentNum; i++)
		{
			ProcessMessages();
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
			unsigned int uiIPAim = 0;

			m_bStartSample = TRUE;
			for (int i=0; i<InstrumentNum; i++)
			{
				m_dlgADCData[i].m_bStopSample = FALSE;
			}

			m_Sec.Lock();
			for (int i=0; i<InstrumentNum; i++)
			{
				ProcessMessages();
				m_oThreadManage.m_oADCDataRecThread.m_dSampleTemp[i].clear();
				m_oThreadManage.m_oADCDataRecThread.m_dADCSave[i].clear();
				m_oThreadManage.m_oADCDataRecThread.m_uiADCDataFrameCount[i] = 0;
				m_oThreadManage.m_oADCDataRecThread.m_uiADCDataFrameRecCount[i] = 0;
				m_oThreadManage.m_oADCDataRecThread.m_dADCDataShow[i] = 0;
				m_oThreadManage.m_oADCDataRecThread.m_oADCLostMap[i].RemoveAll();
				m_oThreadManage.m_oADCDataRecThread.m_uiADCGraphIP[i] = 0;
			}
			m_oThreadManage.m_oADCDataRecThread.m_bStartSample = FALSE;
			m_oThreadManage.m_oADCDataRecThread.m_uiADCSaveFileNum = 0;
			m_oThreadManage.m_oADCDataRecThread.m_uiADCDataToSaveNum = 0;
			m_Sec.Unlock();

			m_oThreadManage.m_oSysTimeSocket.m_uiSysTimeCount = 0;
			// 创建本次ADC数据采集数据存储文件夹
		
			GetLocalTime(&sysTime);
			m_oThreadManage.m_oADCDataRecThread.m_csSaveFilePath = _T("");
			m_oThreadManage.m_oADCDataRecThread.m_csSaveFilePath += m_csSaveFilePath;
			str = _T("\\ADC数据备份");
			m_oThreadManage.m_oADCDataRecThread.m_csSaveFilePath += str;
			str.Format("_%04d_%02d_%02d_%02d_%02d_%02d_%03d", sysTime.wYear,sysTime.wMonth,sysTime.wDay,
				sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
			m_oThreadManage.m_oADCDataRecThread.m_csSaveFilePath += str;
			// 按日期创建保存ADC采样数据的文件夹
			CreateDirectory(m_oThreadManage.m_oADCDataRecThread.m_csSaveFilePath,NULL);
			
			GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->GetWindowText(str);
			m_uiADCFileLength = atoi(str);

			m_oThreadManage.m_oADCDataRecThread.m_uiADCFileLength = m_uiADCFileLength * ReceiveDataSize;
			
			for (int i=0; i<InstrumentNum; i++)
			{
				m_dlgADCData[i].m_uiADCFileLength = m_uiADCFileLength * ReceiveDataSize;
			}

			// 先做零漂矫正
			uiIPAim = BroadCastPort;
			OnADCZeroDrift(uiIPAim);

			// 发送开始采样操作命令帧
			OnSendCmdFrame(m_iSelectObject);
		}
	}
}

void CTabSample::OnBnClickedButtonStopsample()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	unsigned int uiIPAim = 0;;
	if (m_bStartSample == FALSE)
	{
		return;
	}
	for (int i=0; i<InstrumentNum; i++)
	{
		m_dlgADCData[i].m_bStopSample = TRUE;
	}
	m_bStartSample = FALSE;

	// 发送采样结束操作命令帧
	uiIPAim = BroadCastPort;
	OnADCSampleStop(uiIPAim);

	SetTimer(TabSampleStopSampleTimerNb, TabSampleStopSampleTimerSet, NULL);
	KillTimer(TabSampleQueryErrorCountTimerNb);
}
// 求采样数据的平均值
//************************************
// Method:    SampleDataAverage
// FullName:  CTabSample::SampleDataAverage
// Access:    public 
// Returns:   double
// Qualifier:
// Parameter: double * dSampleData
// Parameter: unsigned int uiLength
//************************************
double CTabSample::SampleDataAverage(double* dSampleData, unsigned int uiLength)
{
	double average = 0.0;
	for (unsigned int i=0; i<uiLength; i++)
	{
		ProcessMessages();
		average += dSampleData[i];
	}
	average = average/uiLength;
	return average;
}

// 求采样数据的最大值
//************************************
// Method:    SampleDataMax
// FullName:  CTabSample::SampleDataMax
// Access:    public 
// Returns:   double
// Qualifier:
// Parameter: double * dSampleData
// Parameter: unsigned int uiLength
//************************************
double CTabSample::SampleDataMax(double* dSampleData, unsigned int uiLength)
{
	double maxvalue = 0.0;
	maxvalue = dSampleData[0];
	for (unsigned int i=0; i<uiLength;i++)
	{
		ProcessMessages();
		if (dSampleData[i] > maxvalue)
		{
			maxvalue = dSampleData[i];
		}
	}

	return maxvalue;
}

// 求采样数据的最小值
//************************************
// Method:    SampleDataMin
// FullName:  CTabSample::SampleDataMin
// Access:    public 
// Returns:   double
// Qualifier:
// Parameter: double * dSampleData
// Parameter: unsigned int uiLength
//************************************
double CTabSample::SampleDataMin(double* dSampleData, unsigned int uiLength)
{
	double minvalue = 0.0;
	minvalue = dSampleData[0];
	for (unsigned int i=0; i<uiLength;i++)
	{
		ProcessMessages();
		if (dSampleData[i] < minvalue)
		{
			minvalue = dSampleData[i];
		}
	}
	return minvalue;
}

// 求采样数据的均方根
//************************************
// Method:    SampleDataRms
// FullName:  CTabSample::SampleDataRms
// Access:    public 
// Returns:   double
// Qualifier:
// Parameter: double * dSampleData
// Parameter: unsigned int uiLength
//************************************
double CTabSample::SampleDataRms(double* dSampleData, unsigned int uiLength)
{
	double rmsvalue = 0.0;
	for (unsigned int i=0; i<uiLength; i++)
	{
		ProcessMessages();
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
	pidl = NULL;
	delete pidl;
}

void CTabSample::OnBnClickedButtonReset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CButton* pButton = NULL;
	for (int i=0; i<InstrumentNum; i++)
	{
		ProcessMessages();
		m_iSelectObject[i] = 0;
		m_iSelectObjectNoise[i] = 0;

		m_Sec.Lock();
		m_oThreadManage.m_oADCDataRecThread.m_dSampleTemp[i].clear();
		m_oThreadManage.m_oADCDataRecThread.m_dADCSave[i].clear();
		m_oThreadManage.m_oADCDataRecThread.m_uiADCDataFrameCount[i] = 0;
		m_oThreadManage.m_oADCDataRecThread.m_uiADCDataFrameRecCount[i] = 0;
		m_oThreadManage.m_oADCDataRecThread.m_dADCDataShow[i] = 0;
		m_oThreadManage.m_oADCDataRecThread.m_oADCLostMap[i].RemoveAll();
		m_oThreadManage.m_oADCDataRecThread.m_uiADCGraphIP[i] = 0;
		m_Sec.Unlock();

		pButton = (CButton*)GetDlgItem(m_iCheckIDInstrumentFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;
		pButton = (CButton*)GetDlgItem(m_iCheckIDNoiseFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;

		m_dlgADCData[i].m_bStopSample = FALSE;
	}
	delete pButton;

	m_bStartSample = FALSE;

	m_Sec.Lock();

	m_oThreadManage.m_oADCDataRecThread.m_bStartSample = FALSE;
	m_oThreadManage.m_oADCDataRecThread.m_uiADCSaveFileNum = 0;
	m_oThreadManage.m_oADCDataRecThread.m_uiADCDataToSaveNum = 0;
	m_Sec.Unlock();
	m_oThreadManage.m_oSysTimeSocket.m_uiSysTimeCount = 0;
	m_uiADCFileLength = 0;
	m_uiTailRecCount = 0;
	UpdateData(FALSE);
}
// 计算仪器最大的采样长度
//************************************
// Method:    OnSampleDataMaxLength
// FullName:  CTabSample::OnSampleDataMaxLength
// Access:    public 
// Returns:   unsigned int
// Qualifier:
// Parameter: int iGraphViewNum
// Parameter: vector<double> * dSampleData
//************************************
unsigned int CTabSample::OnSampleDataMaxLength(int iGraphViewNum, vector<double>* dSampleData)
{
	unsigned int iMaxLength = 0;

	for (int i=0; i<iGraphViewNum; i++)
	{
		ProcessMessages();
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
//************************************
// Method:    OnSendCmdFrame
// FullName:  CTabSample::OnSendCmdFrame
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int * pSelectObject
//************************************
void CTabSample::OnSendCmdFrame(int* pSelectObject)
{
	m_oThreadManage.MakeCollectSysTimeFrameData(pSelectObject);
	m_oThreadManage.SendCollectSysTimeFrameToSocket();
}

void CTabSample::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	CInstrument* pInstrument = NULL;	// 仪器对象指针
	POSITION pos = NULL;				// 位置	
	unsigned int uiKey = 0;					// 索引键	

	// 开始采样
	if (nIDEvent == TabSampleStartSampleTimerNb)
	{
		KillTimer(TabSampleStartSampleTimerNb);
		m_Sec.Lock();
		m_oThreadManage.m_oADCDataRecThread.m_bStartSample = TRUE;
		m_Sec.Unlock();
	}
	// 停止采样
	else if (nIDEvent == TabSampleStopSampleTimerNb)
	{
		KillTimer(TabSampleStopSampleTimerNb);
	
		unsigned int uiADCDataNeedToSave = 0;
		// @@@@@@@@@需要改进
		m_Sec.Lock();
		// 计算仪器最大的采样长度
		uiADCDataNeedToSave = OnSampleDataMaxLength(InstrumentNum, m_oThreadManage.m_oADCDataRecThread.m_dADCSave);
		// 保存剩余的ADC数据文件
		m_oThreadManage.m_oADCDataRecThread.OnSaveRemainADCData(uiADCDataNeedToSave);
		m_Sec.Unlock();
	}
	// 查询硬件设备出错计数
	else if (nIDEvent == TabSampleQueryErrorCountTimerNb)
	{
		unsigned int uiIPAim = 0;
		int iPos = 0;
		// 广播查询采集站
		uiIPAim = BroadCastPort;
		iPos = m_oADCSet.ADCSetFrameHead(uiIPAim, true, SendQueryCmd, QueryErrorCodePort);
		m_oADCSet.OnQueryErrorCodeFdu(iPos);
		m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
		// 按IP地址查询交叉站
		uiIPAim = IPSetAddrStart;
		iPos = m_oADCSet.ADCSetFrameHead(uiIPAim, true, SendQueryCmd, QueryErrorCodePort);
		m_oADCSet.OnQueryErrorCodeLAUX(iPos);
		m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
// 		// 每1秒发送一次误码查询
	}
	// 监测尾包
	else if (nIDEvent == TabSampleTailMonitorTimerNb)
	{
		if (m_oThreadManage.m_TailFrameSocket.m_bTailRec == TRUE)
		{
			m_oThreadManage.m_TailFrameSocket.m_bTailRec = FALSE;
			m_uiTailRecCount = 0;
			m_bTailCountStart = true;
		}
		if(m_bTailCountStart == true)
		{
			m_uiTailRecCount++;
			if (m_uiTailRecCount == TailFrameCount)
			{
				// 连续5秒未收到尾包则删除所有仪器
				m_oThreadManage.m_oInstrumentList.DeleteAllInstrument();
				m_bTailCountStart = false;
				OnBnClickedButtonStopsample();
				m_uiTailRecCount = 0;
				OnDisableButtons();
				KillTimer(TabSampleQueryErrorCountTimerNb);
			}
		}
	}
	// 监测IP地址设置应答帧
	else if (nIDEvent == TabSampleIPSetMonitorTimerNb)
	{
		KillTimer(TabSampleIPSetMonitorTimerNb);
		bool	bIPSetAllOk = true;
		unsigned int icount = m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetCount();
		if (icount == 0)
		{
			return;
		}
		pos = m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetStartPosition();	// 得到索引表起始位置
		while(NULL != pos)
		{
			ProcessMessages();
			pInstrument = NULL;		
			m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetNextAssoc(pos, uiKey, pInstrument);	// 得到仪器对象
			if(NULL != pInstrument)	
			{
				if (pInstrument->m_bIPSetOK == false)
				{
					if (pInstrument->m_uiIPAddress != 0)
					{
						if (pInstrument->m_uiIPResetTimes < IPAddrResetTimes)
						{
							// 广播设置IP地址
							m_oThreadManage.m_HeadFrameSocket.MakeIPSetFrame(pInstrument, TRUE);
							m_oThreadManage.m_HeadFrameSocket.SendIPSetFrame();
							// 按IP地址查询应答
							m_oThreadManage.m_HeadFrameSocket.MakeIPSetFrame(pInstrument, FALSE);
							m_oThreadManage.m_HeadFrameSocket.SendIPSetFrame();
							// 设备IP地址重设次数加一
							pInstrument->m_uiIPResetTimes++;
							//						SetTimer(5, 10000, NULL);
							bIPSetAllOk = false;
						}
						else
						{
							CString strtmp = _T("");
							bIPSetAllOk = false;
							strtmp.Format(_T("设备SN为%04x已连续设置%d次IP地址失败！"), pInstrument->m_uiSN, pInstrument->m_uiIPResetTimes);
							AfxMessageBox(strtmp);
						}
					}
				}
			}
		}
		// 所有仪器的IP地址设置都完成后
		if (bIPSetAllOk == true)
		{
			GetDlgItem(IDC_BUTTON_SETBYHAND)->EnableWindow(TRUE);
		}
		else
		{
			SetTimer(TabSampleIPSetMonitorTimerNb, TabSampleIPSetMonitorTimerSet, NULL);
		}
	}
	// 先将设备的本地时间设为0
	else if (nIDEvent == TabSampleInitSysTimeTimerNb)
	{
		KillTimer(TabSampleInitSysTimeTimerNb);
		pos = m_oThreadManage.m_oInstrumentList.m_oInstrumentMap.GetStartPosition();	// 得到索引表起始位置
		while(NULL != pos)
		{
			ProcessMessages();

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
		SetTimer(TabSampleTimeCalTimerNb, TabSampleTimeCalTimerSet, NULL);
	}
	// 做时统
	else if (nIDEvent == TabSampleTimeCalTimerNb)
	{
		KillTimer(TabSampleTimeCalTimerNb);
		m_oThreadManage.m_TailFrameSocket.SendTailTimeFrame();
		SetTimer(TabSampleADCSetTimerNb, TabSampleADCSetTimerSet, NULL);
	}
	// ADC参数设置
	else if (nIDEvent == TabSampleADCSetTimerNb)
	{
		unsigned int uiIPAim = 0;
		KillTimer(TabSampleADCSetTimerNb);
		uiIPAim = BroadCastPort;
		OnADCSet(uiIPAim);
		SetTimer(TabSampleADCZeroDriftTimerNb, TabSampleADCZeroDriftTimerSet, NULL);
	}
	// 零漂矫正
	else if (nIDEvent == TabSampleADCZeroDriftTimerNb)
	{
		unsigned int uiIPAim = 0;
		KillTimer(TabSampleADCZeroDriftTimerNb);
		uiIPAim = BroadCastPort;
		OnADCZeroDrift(uiIPAim);
		OnEnableButtons();
	}
	pInstrument = NULL;
	delete pInstrument;
	CDialog::OnTimer(nIDEvent);
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

void CTabSample::OnBnClickedButtonSetadc()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	
	// 以广播方式发送ADC参数设置
	// 目标IP地址
	unsigned int uiIPAim = 0;
	uiIPAim = BroadCastPort;
	OnADCSet(uiIPAim);
}

// ADC参数设置
//************************************
// Method:    OnADCSet
// FullName:  CTabSample::OnADCSet
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned int uiIPAim
//************************************
void CTabSample::OnADCSet(unsigned int uiIPAim)
{
	int iPos = 0;
	iPos = m_oADCSet.ADCSetFrameHead(uiIPAim, true, SendSetCmd, ADSetReturnPort);
	m_oADCSet.OnSetSine(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
	
	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnOpenPowerTBLow(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnOpenPowerTBHigh(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
	
	Sleep(ADCOperationSleepTime);
	
	m_oADCSet.OnOpenSwitchTBLow(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);
	
	m_oADCSet.OnOpenSwitchTBHigh(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCRegisterWrite(iPos, false);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCRegisterRead(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCSetReturn(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
}

// ADC数据采集停止
//************************************
// Method:    OnADCSampleStop
// FullName:  CTabSample::OnADCSampleStop
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned int uiIPAim
//************************************
void CTabSample::OnADCSampleStop(unsigned int uiIPAim)
{
	int iPos = 0;
	iPos = m_oADCSet.ADCSetFrameHead(uiIPAim, true, SendSetCmd, ADSetReturnPort);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
}

void CTabSample::OnBnClickedButtonTimecal()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oThreadManage.m_TailFrameSocket.SendTailTimeFrame();
}

// 使能button控件
//************************************
// Method:    OnEnableButtons
// FullName:  CTabSample::OnEnableButtons
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabSample::OnEnableButtons(void)
{
	GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(TRUE);
}

// 禁用Button控件
//************************************
// Method:    OnDisableButtons
// FullName:  CTabSample::OnDisableButtons
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabSample::OnDisableButtons(void)
{
	GetDlgItem(IDC_BUTTON_SETBYHAND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_TIMECAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SETADC)->EnableWindow(FALSE);
}

void CTabSample::OnBnClickedButtonFdu1()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(1);
}

void CTabSample::OnBnClickedButtonFdu2()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(2);
}

void CTabSample::OnBnClickedButtonFdu3()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(3);
}

void CTabSample::OnBnClickedButtonFdu4()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(4);
}

void CTabSample::OnBnClickedButtonFdu5()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(5);
}

void CTabSample::OnBnClickedButtonFdu6()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(6);
}

void CTabSample::OnBnClickedButtonFdu7()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(7);
}

void CTabSample::OnBnClickedButtonFdu8()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(8);
}

void CTabSample::OnBnClickedButtonFdu9()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(9);
}

void CTabSample::OnBnClickedButtonFdu10()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(10);
}

void CTabSample::OnBnClickedButtonFdu11()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(11);
}

void CTabSample::OnBnClickedButtonFdu12()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(12);
}

void CTabSample::OnBnClickedButtonFdu13()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(13);
}

void CTabSample::OnBnClickedButtonFdu14()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(14);
}

void CTabSample::OnBnClickedButtonFdu15()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(15);
}

void CTabSample::OnBnClickedButtonFdu16()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(16);
}

void CTabSample::OnBnClickedButtonFdu17()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(17);
}

void CTabSample::OnBnClickedButtonFdu18()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(18);
}

void CTabSample::OnBnClickedButtonFdu19()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(19);
}

void CTabSample::OnBnClickedButtonFdu20()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(20);
}

void CTabSample::OnBnClickedButtonFdu21()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(21);
}

void CTabSample::OnBnClickedButtonFdu22()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(22);
}

void CTabSample::OnBnClickedButtonFdu23()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(23);
}

void CTabSample::OnBnClickedButtonFdu24()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(24);
}

void CTabSample::OnBnClickedButtonFdu25()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(25);
}

void CTabSample::OnBnClickedButtonFdu26()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(26);
}

void CTabSample::OnBnClickedButtonFdu27()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(27);
}

void CTabSample::OnBnClickedButtonFdu28()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(28);
}

void CTabSample::OnBnClickedButtonFdu29()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(29);
}

void CTabSample::OnBnClickedButtonFdu30()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(30);
}

void CTabSample::OnBnClickedButtonFdu31()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(31);
}

void CTabSample::OnBnClickedButtonFdu32()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(32);
}

void CTabSample::OnBnClickedButtonFdu33()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(33);
}

void CTabSample::OnBnClickedButtonFdu34()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(34);
}

void CTabSample::OnBnClickedButtonFdu35()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(35);
}

void CTabSample::OnBnClickedButtonFdu36()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(36);
}

void CTabSample::OnBnClickedButtonFdu37()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(37);
}

void CTabSample::OnBnClickedButtonFdu38()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(38);
}

void CTabSample::OnBnClickedButtonFdu39()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(39);
}

void CTabSample::OnBnClickedButtonFdu40()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(40);
}

void CTabSample::OnBnClickedButtonFdu41()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(41);
}

void CTabSample::OnBnClickedButtonFdu42()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(42);
}

void CTabSample::OnBnClickedButtonFdu43()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(43);
}

void CTabSample::OnBnClickedButtonFdu44()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(44);
}

void CTabSample::OnBnClickedButtonFdu45()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(45);
}

void CTabSample::OnBnClickedButtonFdu46()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(46);
}

void CTabSample::OnBnClickedButtonFdu47()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(47);
}

void CTabSample::OnBnClickedButtonFdu48()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	OnShowADCGraph(48);
}

// 图形化显示ADC数据
//************************************
// Method:    OnShowADCGraph
// FullName:  CTabSample::OnShowADCGraph
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned int uiADCGraphNb
//************************************
void CTabSample::OnShowADCGraph(unsigned int uiADCGraphNb)
{	
	m_dlgADCData[uiADCGraphNb - 1].m_uiADCGraphNb = uiADCGraphNb;
//	m_dlgADCData.DoModal();

	if ( ::IsWindow( m_dlgADCData[uiADCGraphNb - 1].m_hWnd ) )
	{
		m_dlgADCData[uiADCGraphNb - 1].DestroyWindow();
	}

	if ( m_dlgADCData[uiADCGraphNb - 1].Create( IDD_DIALOG_GRAPHSHOW ) )
	{
		m_dlgADCData[uiADCGraphNb - 1].ShowWindow( SW_SHOW );
		m_dlgADCData[uiADCGraphNb - 1].UpdateWindow();
	}
}
// ADC零漂矫正
//************************************
// Method:    OnADCZeroDrift
// FullName:  CTabSample::OnADCZeroDrift
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned int uiIPAim
//************************************
void CTabSample::OnADCZeroDrift(unsigned int uiIPAim)
{
	int iPos = 0;
	iPos = m_oADCSet.ADCSetFrameHead(uiIPAim, true, SendSetCmd, ADSetReturnPort);
	m_oADCSet.OnOpenPowerZeroDrift(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
	
	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);
	
	// 新版硬件程序
/*	m_oADCSet.OnADCRegisterWrite(iPos, true);*/
	// 旧版硬件程序
	m_oADCSet.OnADCRegisterWrite(iPos, false);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnADCRegisterRead(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	iPos = m_oADCSet.ADCSetFrameHead(uiIPAim, true, SendQueryCmd, CollectSysTimePort);
	m_oADCSet.OnADCRegisterQuery(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	iPos = m_oADCSet.ADCSetFrameHead(uiIPAim, true, SendSetCmd, ADSetReturnPort);
	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCSampleSynchronization(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCReadContinuous(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
	
	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCZeroDriftCorrection(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCReadContinuous(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnStopSample(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);

	m_oADCSet.OnADCRegisterRead(iPos);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_ucFrameData, SndFrameSize, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(ADCOperationSleepTime);
}

// 防止程序在循环中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CTabSample::ProcessMessages
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabSample::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}


void CTabSample::OnBnClickedButtonSetbyhand()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(TabSampleInitSysTimeTimerNb, TabSampleInitSysTimeTimerSet, NULL);
	// 调试用，每1秒发送一次误码查询
	SetTimer(TabSampleQueryErrorCountTimerNb, TabSampleQueryErrorCountTimerSet, NULL);
}
