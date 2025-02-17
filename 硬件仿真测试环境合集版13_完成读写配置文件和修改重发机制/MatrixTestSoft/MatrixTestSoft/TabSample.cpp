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
END_MESSAGE_MAP()


// 初始化
void CTabSample::OnInit(void)
{
	// 控件选择
	for (int k=0; k<GraphViewNum; k++)
	{
		ProcessMessages();
		m_iSelectObject[k] = 0;
		m_iSelectObjectNoise[k] = 0;
		m_iButtonIDFDU[k] = IDC_BUTTON_FDU1 + k;
		m_iCheckIDInstrumentFDU[k] = IDC_CHECK_GRAPHVIEW1 + k;
		m_iCheckIDNoiseFDU[k] = IDC_CHECK_NOISE1 + k;
	}

	CString strtmp = _T("");
	for (int i=0; i<GraphViewNum; i++)
	{
		ProcessMessages();
		strtmp.Format(_T("仪器%d"), i+1);
		char* pchar = strtmp.GetBuffer(0); 
		strcpy_s(m_cSelectObjectName[i],pchar);
	}
	
	// 得到当前路径
	char cSaveToFilePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, cSaveToFilePath);
	m_csSaveFilePath = cSaveToFilePath;
	
	m_oThreadManage.m_oADCDataRecThread.m_pOScopeCtrl = &m_dlgADCData.m_OScopeCtrl;
	m_dlgADCData.m_ADCDataRecThread = &m_oThreadManage.m_oADCDataRecThread;

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
	m_oThreadManage.m_oADCDataRecThread.m_csIPSource = ip;

	m_oThreadManage.m_oSysTimeSocket.m_pADCSet = &m_oADCSet;
	m_oThreadManage.m_IPSetSocket.m_pADCSet = &m_oADCSet;


	CString str;
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

	
	m_oThreadManage.m_oSysTimeSocket.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_oADCDataRecThread.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_IPSetSocket.m_pSelectObject = m_iSelectObject;
	m_oThreadManage.m_oADCDataRecThread.m_pSelectObjectNoise = m_iSelectObjectNoise;
	for(int i=0; i<GraphViewNum; i++)
	{
		m_oThreadManage.m_oADCDataRecThread.m_cSelectObjectName[i] = m_cSelectObjectName[i];
		m_oThreadManage.m_oInstrumentList.m_iButtonIDFDU[i] = m_iButtonIDFDU[i];
		m_oThreadManage.m_IPSetSocket.m_iButtonIDFDU[i] = m_iButtonIDFDU[i];
		m_oThreadManage.m_oInstrumentList.m_iCheckIDInstrumentFDU[i] = m_iCheckIDInstrumentFDU[i];
		m_oThreadManage.m_IPSetSocket.m_iCheckIDInstrumentFDU[i] = m_iCheckIDInstrumentFDU[i];
	}
	CButton* iconbutton = NULL;
	CStatic* iconstatic = NULL;
	CButton* pButton = NULL;
	
	iconstatic =(CStatic*)this->GetDlgItem(IDC_STATIC_LAUX);
	iconstatic->SetIcon(m_iconLAUXDisconnected);
	iconstatic = NULL;
	
	for (int i=0; i<GraphViewNum; i++)
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
	SetTimer(4, 1000, NULL);
	OnDisableButtons();
}


// 打开
void CTabSample::OnOpen(void)
{
	CString str;

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
void CTabSample::OnStop(void)
{
	CButton* iconbutton = NULL;
	CStatic* iconstatic = NULL;
	CButton* pButton = NULL;

	iconstatic =(CStatic*)this->GetDlgItem(IDC_STATIC_LAUX);
	iconstatic->SetIcon(m_iconLAUXDisconnected);
	iconstatic = NULL;

	for (int i=0; i<GraphViewNum; i++)
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

		CButton* pButton = NULL;
		for (int i=0; i<GraphViewNum; i++)
		{
			ProcessMessages();
			pButton = (CButton*)GetDlgItem(m_iCheckIDInstrumentFDU[i]);
			m_iSelectObject[i] = pButton->GetCheck();
			pButton = NULL;
			pButton = (CButton*)GetDlgItem(m_iCheckIDNoiseFDU[i]);
			m_iSelectObjectNoise[i] = pButton->GetCheck();
			pButton = NULL;
		}
		delete pButton;

		for (int i=0; i<GraphViewNum; i++)
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

			m_bStartSample = TRUE;
			m_dlgADCData.m_bStopSample = FALSE;

			m_Sec.Lock();
			for (int i=0; i<GraphViewNum; i++)
			{
				ProcessMessages();
				
				m_oThreadManage.m_oADCDataRecThread.m_dSampleTemp[i].clear();
				m_oThreadManage.m_oADCDataRecThread.m_dADCSave[i].clear();
				m_oThreadManage.m_oADCDataRecThread.m_uiADCDataFrameCount[i] = 0;
				m_oThreadManage.m_oADCDataRecThread.m_uiADCDataFrameRecCount[i] = 0;
				m_oThreadManage.m_oADCDataRecThread.m_dADCDataShow[i] = 0;
			}
			m_oThreadManage.m_oADCDataRecThread.m_uiADCGraphIP = 0;
			m_oThreadManage.m_oADCDataRecThread.m_bStartSample = FALSE;
			m_oThreadManage.m_oADCDataRecThread.m_uiADCSaveFileNum = 0;
			m_oThreadManage.m_oADCDataRecThread.m_uiADCDataToSaveNum = 0;
			m_Sec.Unlock();


			// 创建本次ADC数据采集数据存储文件夹
		
			GetLocalTime(&sysTime);
			str = _T("ADC数据备份");
			m_oThreadManage.m_oADCDataRecThread.m_csSaveFilePath.Format("%s\\%s_%04d_%02d_%02d_%02d_%02d_%02d_%04d", m_csSaveFilePath, str, sysTime.wYear,sysTime.wMonth,sysTime.wDay,
				sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
			// 按日期创建保存ADC采样数据的文件夹
			CreateDirectory(m_oThreadManage.m_oADCDataRecThread.m_csSaveFilePath,NULL);
			
			GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->GetWindowText(str);
			m_uiADCFileLength = atoi(str);

			m_oThreadManage.m_oADCDataRecThread.m_uiADCFileLength = m_uiADCFileLength * ReceiveDataSize;
			m_dlgADCData.m_uiADCFileLength = m_uiADCFileLength * ReceiveDataSize;

			// 先做零漂矫正
			unsigned int uiIPAim = 0;
			uiIPAim = 0xffffffff;
			OnADCZeroDrift(uiIPAim);

			// 发送开始采样操作命令帧
			OnSendCmdFrame(m_iSelectObject);
			SetTimer(1, 1500, NULL);
//			SetTimer(3, 1800, NULL);
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
	m_dlgADCData.m_bStopSample = TRUE;
	m_bStartSample = FALSE;

	// 发送采样结束操作命令帧

	unsigned int uiIPAim;
	uiIPAim = 0xffffffff;
	OnADCSampleStop(uiIPAim);

	SetTimer(2, 1000, NULL);
	KillTimer(3);
}
// 求采样数据的平均值
double CTabSample::SampleDataAverage(vector<double> dSampleData)
{
	int length = 0;
	double average = 0.0;

	length = dSampleData.size();
	for (int i=0; i<length; i++)
	{
		ProcessMessages();
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
		ProcessMessages();
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
		ProcessMessages();
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
}

void CTabSample::OnBnClickedButtonReset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	CButton* pButton = NULL;


	for (int i=0; i<GraphViewNum; i++)
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
		m_Sec.Unlock();

		pButton = (CButton*)GetDlgItem(m_iCheckIDInstrumentFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;
		pButton = (CButton*)GetDlgItem(m_iCheckIDNoiseFDU[i]);
		pButton->SetCheck(0);
		pButton = NULL;
	}
	delete pButton;

	m_dlgADCData.m_bStopSample = FALSE;
	m_bStartSample = FALSE;
	m_dlgADCData.m_dSampleData.clear();

	m_Sec.Lock();
	m_oThreadManage.m_oADCDataRecThread.m_uiADCGraphIP = 0;
	m_oThreadManage.m_oADCDataRecThread.m_bStartSample = FALSE;
	m_oThreadManage.m_oADCDataRecThread.m_uiADCSaveFileNum = 0;
	m_oThreadManage.m_oADCDataRecThread.m_uiADCDataToSaveNum = 0;
	m_Sec.Unlock();
	m_uiADCFileLength = 0;
	m_uiTailRecCount = 0;
	UpdateData(FALSE);
}
// 计算仪器最大的采样长度
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
	unsigned int uiKey;					// 索引键	

	// 开始采样
	if (nIDEvent == 1)
	{
		KillTimer(1);
		m_Sec.Lock();
		m_oThreadManage.m_oADCDataRecThread.m_bStartSample = TRUE;
		m_Sec.Unlock();
	}
	// 停止采样
	else if (nIDEvent == 2)
	{
		KillTimer(2);
	
		unsigned int uiADCDataNeedToSave = 0;
		m_Sec.Lock();
		// 计算仪器最大的采样长度
		uiADCDataNeedToSave = OnSampleDataMaxLength(GraphViewNum, m_oThreadManage.m_oADCDataRecThread.m_dADCSave);
		// 保存剩余的ADC数据文件
		m_oThreadManage.m_oADCDataRecThread.OnSaveRemainADCData(uiADCDataNeedToSave);
		m_Sec.Unlock();
	}
	// ADC数据重发机制
// 	else if (nIDEvent == 3)
// 	{
// 		KillTimer(3);
// 		m_oThreadManage.m_oADCDataRecThread.OnADCDataRetransmission();
// 		SetTimer(3, 200, NULL);
// 	}
	// 监测尾包
	else if (nIDEvent == 4)
	{
		KillTimer(4);
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
		SetTimer(4, 1000, NULL);
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
			ProcessMessages();
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
		SetTimer(7, 2000, NULL);
	}
	// 做时统
	else if (nIDEvent == 7)
	{
		KillTimer(7);
		m_oThreadManage.m_TailFrameSocket.SendTailTimeFrame();
		SetTimer(8, 1000, NULL);
	}
	// ADC参数设置
	else if (nIDEvent == 8)
	{
		KillTimer(8);
		unsigned int uiIPAim = 0;
		uiIPAim = 0xffffffff;
		OnADCSet(uiIPAim);
		SetTimer(9, 2000, NULL);
	}
	// 零漂矫正
	else if (nIDEvent == 9)
	{
		KillTimer(9);
		unsigned int uiIPAim = 0;
		uiIPAim = 0xffffffff;
		OnADCZeroDrift(uiIPAim);
		OnEnableButtons();
	}
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
	uiIPAim = 0xffffffff;
	OnADCSet(uiIPAim);
}

// ADC参数设置
void CTabSample::OnADCSet(unsigned int uiIPAim)
{
	m_oADCSet.ADCSetFrameHead(uiIPAim, true, 0x0001, ADSetReturnPort);
	m_oADCSet.OnSetSine();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnOpenPowerTBLow();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnOpenPowerTBHigh();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnOpenSwitchTBLow();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnOpenSwitchTBHigh();
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
	m_oADCSet.ADCSetFrameHead(uiIPAim, true, 0x0001, ADSetReturnPort);

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
void CTabSample::OnShowADCGraph(unsigned int uiADCGraphNb)
{	
	m_dlgADCData.m_uiADCGraphNb = uiADCGraphNb;
	m_dlgADCData.DoModal();
}
// ADC零漂矫正
void CTabSample::OnADCZeroDrift(unsigned int uiIPAim)
{
	m_oADCSet.ADCSetFrameHead(uiIPAim, true, 0x0001, ADSetReturnPort);
	m_oADCSet.OnOpenPowerZeroDrift();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCRegisterWrite();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	m_oADCSet.OnADCRegisterRead();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.ADCSetFrameHead(uiIPAim, true, 0x0002, CollectSysTimePort);
	m_oADCSet.OnADCRegisterQuery();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.ADCSetFrameHead(uiIPAim, true, 0x0001, ADSetReturnPort);
	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCSampleSynchronization();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCReadContinuous(true);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);
	
	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCZeroDriftCorrection();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCReadContinuous(true);
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnStopSample();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);

	m_oADCSet.OnADCRegisterRead();
	m_oThreadManage.m_oSysTimeSocket.SendTo(m_oADCSet.m_cFrameData, 128, m_oThreadManage.m_oSysTimeSocket.m_uiSendPort, IPBroadcastAddr);

	Sleep(100);
}

// 防止程序在循环中运行无法响应消息
void CTabSample::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

