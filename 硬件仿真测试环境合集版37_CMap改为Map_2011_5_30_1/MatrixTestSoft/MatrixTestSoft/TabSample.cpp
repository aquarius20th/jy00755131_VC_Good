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
	, m_bHeartBeat(TRUE)
	, m_uiADCFileLength(0)
	, m_uiSendPort(0)
	, m_pLogFile(NULL)
	, m_uiTailTimeCount(0)
	, m_uiTailFrameTimerCount(0)
	, m_uiIPSource(0)
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
	ON_BN_CLICKED(IDC_BUTTON_SETBYHAND, &CTabSample::OnBnClickedButtonSetbyhand)
	ON_MESSAGE(WM_ADCSETSOCKET, &CTabSample::OnADCSetSocket)
	ON_MESSAGE(WM_SYSTIMESOCKET, &CTabSample::OnSysTimeSocket)
	ON_MESSAGE(WM_TAILTIMESOCKET, &CTabSample::OnTailTimeSocket)
	ON_MESSAGE(WM_TAILFRAMESOCKET, &CTabSample::OnTailFrameSocket)
	ON_MESSAGE(WM_IPSETSOCKET, &CTabSample::OnIPSetSocket)
	ON_MESSAGE(WM_HEADFRAMESOCKET, &CTabSample::OnHeadFrameSocket)
	ON_MESSAGE(WM_ADCGRAPHSOCKET, &CTabSample::OnADCGraphSocket)
	ON_MESSAGE(WM_DLGADCGRAPHSHOW, &CTabSample::OnShowADCGraph)
	ON_BN_CLICKED(IDC_BUTTON_GRAPHEXE, &CTabSample::OnBnClickedButtonGraphexe)
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
	CString str = _T("");
	// 得到本机IP地址
	// @@@@@@@@ cxm 2011.11.25 提供给用户设定死本机IP
	char* localIP = IPSourceAddr;
// 	PHOSTENT	hostinfo; 
// 	char		name[256]; 
// 	for (int i=0; i<InstrumentNum; i++)
// 	{
// 		m_dlgADCGraphShow[i].m_pADCDataRecThread = &m_oThreadManage.m_oADCDataRecThread;
// 	}
// 
// 	if(   gethostname(name, sizeof(name)) == 0) 
// 	{ 
// 		if((hostinfo = gethostbyname(name)) != NULL) 
// 		{ 
// 			localIP = inet_ntoa(*(struct in_addr*)*hostinfo-> h_addr_list); 
// 		} 
// 	} 
	// CString转换为const char*
	m_uiIPSource = inet_addr(localIP);
	m_oThreadManage.m_oHeadFrame.m_uiIPSource = m_uiIPSource;
	m_oThreadManage.m_oTailFrame.m_uiIPSource = m_uiIPSource;
	m_oThreadManage.m_oTailTimeFrame.m_uiIPSource = m_uiIPSource;
	m_oThreadManage.m_oSysTime.m_uiIPSource = m_uiIPSource;
	m_oThreadManage.m_oADCDataRecThread.m_uiIPSource = m_uiIPSource;
	m_oThreadManage.m_oHeartBeatThread.m_uiIPSource = m_uiIPSource;
	m_oThreadManage.m_oADCSet.m_uiIPSource = m_uiIPSource;

	str.Format(_T("0x%04x"), m_uiSendPort);
	GetDlgItem(IDC_EDIT_SENDPORT)->SetWindowText(str);

	str.Format(_T("%d"), m_uiADCFileLength);
	GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->SetWindowText(str);
	m_oThreadManage.m_oInstrumentList.m_pWnd = this;
	m_oThreadManage.m_oIPSet.m_pWnd = this;
	m_oThreadManage.m_oHeadFrame.m_pWnd = this;
	m_oThreadManage.m_oSysTime.m_pWnd = this;
	m_oThreadManage.m_oTailTimeFrame.m_pWnd = this;
	m_oThreadManage.m_oInstrumentMonitorThread.m_pWnd = this;
	m_oThreadManage.m_oADCSet.m_pWnd = this;
	m_oThreadManage.m_pLogFile = m_pLogFile;
	m_oThreadManage.m_oInstrumentList.m_pInstrumentGraph = &m_oInstrumentGraph;
	m_oInstrumentGraph.m_pWnd = this;

	m_oThreadManage.OnInit();
	OnOpen();
	// 监测尾包定时器
//	SetTimer(TabSampleTailMonitorTimerNb, TabSampleTailMonitorTimerSet, NULL);
	OnDisableButtons();
	// 加入设备成员绘图
	OnDrawInstrumentGraph();
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
	_stscanf_s(str,_T("%x"), &m_uiSendPort);
	unsigned short usSendPort = static_cast<unsigned short>(m_uiSendPort);
	// 填充套接字地址结构
	OnSetSocketAddr(&m_oThreadManage.m_oHeartBeatThread.m_SendToAddr, usSendPort, IPBroadcastAddr);
	OnSetSocketAddr(&m_oThreadManage.m_oHeadFrame.m_SendToAddr, usSendPort, m_uiIPSource);
	OnSetSocketAddr(&m_oThreadManage.m_oTailFrame.m_SendToAddr, usSendPort, m_uiIPSource);
	OnSetSocketAddr(&m_oThreadManage.m_oTailTimeFrame.m_SendToAddr, usSendPort, m_uiIPSource);
	OnSetSocketAddr(&m_oThreadManage.m_oSysTime.m_SendToAddr, usSendPort, m_uiIPSource);
	OnSetSocketAddr(&m_oThreadManage.m_oADCSet.m_SendToAddr, usSendPort, m_uiIPSource);
	OnSetSocketAddr(&m_oThreadManage.m_oADCDataRecThread.m_SendToAddr, usSendPort, m_uiIPSource);
	OnSetSocketAddr(&m_oThreadManage.m_oADCDataRecThread.m_SendADCGraphToAddr, ADCGraphShowPort, IPBroadcastAddr);


	GetDlgItem(IDC_EDIT_SENDPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_HEARTBEAT)->EnableWindow(TRUE);
	m_oThreadManage.OnOpen();

	// 设置异步套接字
	WSAAsyncSelect(m_oThreadManage.m_oADCSet.m_ADCSetSocket, this->m_hWnd, 
		WM_ADCSETSOCKET, FD_READ);
	WSAAsyncSelect(m_oThreadManage.m_oSysTime.m_SysTimeSocket, this->m_hWnd, 
		WM_SYSTIMESOCKET, FD_READ);
	WSAAsyncSelect(m_oThreadManage.m_oTailTimeFrame.m_TailTimeSocket, this->m_hWnd, 
		WM_TAILTIMESOCKET, FD_READ);
	WSAAsyncSelect(m_oThreadManage.m_oTailFrame.m_TailFrameSocket, this->m_hWnd, 
		WM_TAILFRAMESOCKET, FD_READ);
	WSAAsyncSelect(m_oThreadManage.m_oIPSet.m_IPSetSocket, this->m_hWnd, 
		WM_IPSETSOCKET, FD_READ);
	WSAAsyncSelect(m_oThreadManage.m_oHeadFrame.m_HeadFrameSocket, this->m_hWnd, 
		WM_HEADFRAMESOCKET, FD_READ);
	WSAAsyncSelect(m_oThreadManage.m_oADCDataRecThread.m_ADCGraphShowSocket, this->m_hWnd, 
		WM_ADCGRAPHSOCKET, FD_READ);
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
	GetDlgItem(IDC_EDIT_SENDPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_HEARTBEAT)->EnableWindow(FALSE);
	m_oThreadManage.OnStop();
}
void CTabSample::OnBnClickedCheckMatrixnetwork()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// 得到CHECK控件当前工作状态
	BOOL bStatus = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_MATRIXNETWORK)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
	if (bStatus == TRUE)
	{
		OnOpen();
		m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedCheckMatrixnetwork"), _T("开启测网建立！"), SuccessStatus);
	}
	else
	{
		OnStop();
		m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedCheckMatrixnetwork"), _T("关闭测网建立！"), SuccessStatus);
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
	BOOL bSelect = FALSE;
	// hash_map迭代器
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	int iSelect = 0;
	UpdateData(TRUE);
	// 得到当前Check控件状态
	for(iter=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.begin(); iter!=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if ((iter->second->m_bIPSetOK == true) && (iter->second->m_uiInstrumentType == InstrumentTypeFDU))
			{
				iSelect = BST_CHECKED;
				iter->second->m_iSelectObject = iSelect;
				if (iSelect == BST_CHECKED)
				{
					bSelect = TRUE;	
				}
				iSelect = BST_UNCHECKED;
				iter->second->m_iSelectObjectNoise = iSelect;
			}
		}
	}
	m_oThreadManage.m_oADCDataSaveToFile.m_uiSampleInstrumentNum = 0;

	for(iter=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.begin(); iter!=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if ((iter->second->m_bIPSetOK == true) 
				&& (iter->second->m_uiInstrumentType == InstrumentTypeFDU)
				&& (iter->second->m_iSelectObject == BST_CHECKED))
			{
				m_oThreadManage.m_oADCDataSaveToFile.m_uiSampleInstrumentNum++;
				iter->second->m_uiUnCheckedNum = CalLocationGraph(iter->second->m_uiFDUIndex);
			}
		}
	}
	// 至少有一个仪器被选中
	if (bSelect == TRUE)
	{
		CString str = _T("");
		CString strTemp = _T("");
		SYSTEMTIME  sysTime;
		m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedButtotStartsample"), _T("开始ADC数据采样！"), SuccessStatus);

		for (int i=0; i<InstrumentMaxCount; i++)
		{
			m_dlgADCGraphShow[i].m_bStopSample = FALSE;
		}

		m_Sec_Sample.Lock();
		m_oThreadManage.m_oADCDataRecThread.OnReset();
		m_Sec_Sample.Unlock();

		m_oThreadManage.m_oSysTime.m_uiSysTimeCount = 0;

		// 创建本次ADC数据采集数据存储文件夹
		GetLocalTime(&sysTime);
		strTemp = m_csSaveFilePath;
		strTemp += _T("\\ADC数据备份");
		str.Format(_T("_%04d_%02d_%02d_%02d_%02d_%02d_%03d"), sysTime.wYear,sysTime.wMonth,sysTime.wDay,
			sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
		strTemp += str;
		// 按日期创建保存ADC采样数据的文件夹
		CreateDirectory(strTemp,NULL);
		GetDlgItem(IDC_EDIT_ADCSAVEFRAMENB)->GetWindowText(str);
		m_uiADCFileLength = _tstoi(str);
		m_oThreadManage.m_oADCFrameInfo.m_csSaveFilePath = strTemp;
		m_oThreadManage.m_oADCDataSaveToFile.m_csSaveFilePath = strTemp;
		m_oThreadManage.m_oADCDataSaveToFile.m_uiADCFileLength = m_uiADCFileLength * ReceiveDataSize;
		for (int i=0; i<InstrumentMaxCount; i++)
		{
			m_dlgADCGraphShow[i].m_csSaveFilePath = strTemp;
			m_dlgADCGraphShow[i].m_uiADCFileLength = m_uiADCFileLength * ReceiveDataSize;
		}
		// 创建并打开ADC数据帧信息文件
		m_oThreadManage.m_oADCFrameInfo.OnOpenFile();
		// 发送开始采样操作命令帧
		OnSendCmdFrame();
		GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(FALSE);
	}
}

void CTabSample::OnBnClickedButtonStopsample()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	for (int i=0; i<InstrumentMaxCount; i++)
	{
		m_dlgADCGraphShow[i].m_bStopSample = TRUE;
	}

	// 发送采样结束操作命令帧
	m_oThreadManage.m_oADCSet.OnADCSampleStop();
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
//		ProcessMessages();
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
//		ProcessMessages();
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
//		ProcessMessages();
		if (dSampleData[i] < minvalue)
		{
			minvalue = dSampleData[i];
		}
	}
	return minvalue;
}

// 求采样数据的均方差
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
	double dbaverage = 0.0;
	for (unsigned int i=0; i<uiLength; i++)
	{
		dbaverage += dSampleData[i];
	}
	dbaverage = dbaverage / uiLength;
	for (unsigned int i=0; i<uiLength; i++)
	{
//		ProcessMessages();
		rmsvalue += pow(dSampleData[i] - dbaverage, 2);
	}
	// 幂运算用pow(X,Y)函数，x^y以X为底的Y次幂，需要math.h
	rmsvalue = rmsvalue / uiLength;
	return rmsvalue;
}

void CTabSample::OnBnClickedButtonSavefilepath()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// 文件夹选择对话框

	wchar_t szDir[MAX_PATH];
//	char szDir[MAX_PATH];
	BROWSEINFO bi;
	ITEMIDLIST* pidl;

	bi.hwndOwner = this->m_hWnd; // 指定父窗口，在对话框显示期间，父窗口将被禁用 
	bi.pidlRoot = NULL; // 如果指定NULL，就以“桌面”为根 
	bi.pszDisplayName = szDir; 
	bi.lpszTitle = _T("请选择采样数据存储目录"); // 这一行将显示在对话框的顶端 
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
		CString str = _T("");
		m_csSaveFilePath = szDir;
		str = _T("选择ADC数据采集文件存储路径为") + m_csSaveFilePath;
		m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedButtonSavefilepath"), str, SuccessStatus);
	}
}

void CTabSample::OnBnClickedButtonReset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	for (int i=0; i<InstrumentMaxCount; i++)
	{
		m_dlgADCGraphShow[i].m_bStopSample = FALSE;
	}
	m_Sec_Sample.Lock();
	m_oThreadManage.m_oADCDataRecThread.OnReset();
	m_Sec_Sample.Unlock();
	m_oThreadManage.m_oSysTime.m_uiSysTimeCount = 0;
	m_uiADCFileLength = 0;
	m_oInstrumentGraph.m_bLButtonSelect = false;
	UpdateData(FALSE);
	m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedButtonReset"), _T("ADC数据采样重置！"), SuccessStatus);
}
// 发送数据采集命令帧
//************************************
// Method:    OnSendCmdFrame
// FullName:  CTabSample::OnSendCmdFrame
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int* pSelectObject
//************************************
void CTabSample::OnSendCmdFrame(void)
{
//	m_oThreadManage.m_oADCSet.OnADCStartSample(m_oThreadManage.m_oTailFrame.m_uiSysTime);
	m_oThreadManage.m_oSysTime.MakeCollectSysTimeFrameData();
 	m_oThreadManage.m_oSysTime.SendCollectSysTimeFrameToSocket();
}

void CTabSample::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	// 开始采样
	if (nIDEvent == TabSampleStartSampleTimerNb)
	{
		KillTimer(TabSampleStartSampleTimerNb);
		m_Sec_Sample.Lock();
		m_oThreadManage.m_oADCDataRecThread.m_bStartSample = TRUE;
		m_Sec_Sample.Unlock();
// 		// 调试用，每1秒发送一次误码查询
		SetTimer(TabSampleQueryErrorCountTimerNb, TabSampleQueryErrorCountTimerSet, NULL);
		m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedButtonSetbyhand"), _T("开始发送误码查询命令（采集站广播，交叉站按IP）！"), SuccessStatus);
	}
	// 停止采样
	else if (nIDEvent == TabSampleStopSampleTimerNb)
	{
		KillTimer(TabSampleStopSampleTimerNb);
		m_Sec_Sample.Lock();
		// 保存剩余的ADC数据文件
		m_oThreadManage.m_oADCDataRecThread.OnSaveADCToFile(true);
		m_oThreadManage.m_oADCDataRecThread.m_bStartSample = FALSE;
		m_Sec_Sample.Unlock();
		m_oThreadManage.m_oADCFrameInfo.OnCloseFile();
		GetDlgItem(IDC_BUTTOT_STARTSAMPLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(FALSE);
	}
	// 查询硬件设备出错计数
	else if (nIDEvent == TabSampleQueryErrorCountTimerNb)
	{
		// @@@@@@@先不进行误码查询
//		m_oThreadManage.m_oADCSet.OnQueryErrorCode();
	}
	// 监测尾包
// 	else if (nIDEvent == TabSampleTailMonitorTimerNb)
// 	{
// 		// @@@@打开尾包监测则Release版在此出问题
// 		KillTimer(TabSampleTailMonitorTimerNb);
// 		if (m_oThreadManage.m_oTailFrame.m_bTailRecLAUX == TRUE)
// 		{
// 			CString str = _T("");
// 			m_uiTailFrameTimerCount++;
// 			str.Format(_T("尾包监测定时器计数%d"), m_uiTailFrameTimerCount);
// 			m_pLogFile->OnWriteLogFile(_T("CTabSample::OnTimer"), str, SuccessStatus);
// 			if (m_uiTailFrameTimerCount > m_oThreadManage.m_oTailFrame.m_uiTailRecLAUXCount)
// 			{
// 				if ((m_uiTailFrameTimerCount - m_oThreadManage.m_oTailFrame.m_uiTailRecLAUXCount) > TailFrameLAUXCount)
// 				{
// 					str.Format(_T("无交叉站尾包，此时定时器计数%d，尾包计数%d，删除所有仪器，停止采集！")
// 						, m_uiTailFrameTimerCount, m_oThreadManage.m_oTailFrame.m_uiTailRecLAUXCount);
// 					m_pLogFile->OnWriteLogFile(_T("CTabSample::OnTimer"), str, WarningStatus);
// 					m_oThreadManage.m_oTailFrame.m_bTailRecLAUX = FALSE;
// 					m_oThreadManage.m_oTailFrame.m_uiTailRecLAUXCount = 0;
// 					m_uiTailFrameTimerCount = 0;
// 					// 连续5秒未收到尾包则删除所有仪器
// 					m_oThreadManage.m_oInstrumentList.DeleteAllInstrument();
// 					// 删除所有仪器后不能再发送停止采样的命令!
// //					OnBnClickedButtonStopsample();
// 					OnDisableButtons();
// 					KillTimer(TabSampleQueryErrorCountTimerNb);
// 				}
// 			}
// 		}
// 		SetTimer(TabSampleTailMonitorTimerNb, TabSampleTailMonitorTimerSet, NULL);
// 	}
	// 监测IP地址设置应答帧
	else if (nIDEvent == TabSampleIPSetMonitorTimerNb)
	{
		KillTimer(TabSampleIPSetMonitorTimerNb);
		// 所有仪器的IP地址设置都完成后
		if (true == m_oThreadManage.m_oHeadFrame.OnMonitorIPSetReturn())
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
		m_oThreadManage.m_oTailTimeFrame.OnSetTimeDelay(0, 0);
		SetTimer(TabSampleTimeCalTimerNb, TabSampleTimeCalTimerSet, NULL);
	}
	// 做时统
	else if (nIDEvent == TabSampleTimeCalTimerNb)
	{
		CString str = _T("");
		m_uiTailTimeCount++;
		if (m_uiTailTimeCount > TailTimeFrameCount)
		{
			str.Format(_T("已有%d次未得到完整的尾包时刻查询应答帧"), TailTimeFrameCount);
			m_pLogFile->OnWriteLogFile(_T("CTabSample::OnTimer"), str, ErrorStatus);
			m_uiTailTimeCount = 0;
			KillTimer(TabSampleTimeCalTimerNb);
		} 
		else
		{
			str.Format(_T("第%d次发送尾包时刻查询"), m_uiTailTimeCount);
			if (m_uiTailTimeCount == 1)
			{
				m_pLogFile->OnWriteLogFile(_T("CTabSample::OnTimer"), str, SuccessStatus);
			}
			else
			{
				m_pLogFile->OnWriteLogFile(_T("CTabSample::OnTimer"), str, WarningStatus);
			}
			// 重置尾包时刻查询结果
			m_oThreadManage.m_oInstrumentList.ClearExperiedTailTimeResult();
			// 发送尾包时刻查询帧
			m_oThreadManage.m_oTailFrame.SendTailTimeFrame();
		}
	}
	// ADC参数设置
	else if (nIDEvent == TabSampleADCSetTimerNb)
	{
		m_uiTailTimeCount = 0;
		// 得到当前Check控件状态
		// hash_map迭代器
		hash_map<unsigned int, CInstrument*>::iterator  iter;
		for(iter=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.begin(); iter!=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.end(); iter++)
		{
			if (NULL != iter->second)
			{
				if ((iter->second->m_bIPSetOK == true) && (iter->second->m_uiInstrumentType == InstrumentTypeFDU))
				{
					iter->second->m_iSelectObject = BST_CHECKED;
				}
			}
		}
		KillTimer(TabSampleADCSetTimerNb);
		m_oThreadManage.m_oADCSet.OnADCSet();
	}
	// 零漂矫正
	else if (nIDEvent == TabSampleADCZeroDriftTimerNb)
	{
		KillTimer(TabSampleADCZeroDriftTimerNb);
		m_oThreadManage.m_oADCSet.OnADCZeroDrift();
	}
	// ADC命令设置应答监视
	else if (nIDEvent == TabSampleADCSetReturnTimerNb)
	{
		CString str = _T("");
		// 在指定时间内未收全命令设置应答帧，重发该命令
		if (m_oThreadManage.m_oADCSet.m_uiADCSetOptCount >= ADCSetCmdReSendNum)
		{
			str.Format(_T("在指定时间内未收全ADC命令序号为%d的应答，重发该指令%d次，跳过该指令！"),
				m_oThreadManage.m_oADCSet.m_uiADCSetOperationNb, m_oThreadManage.m_oADCSet.m_uiADCSetOptCount);
			m_pLogFile->OnWriteLogFile(_T("CTabSample::OnTimer"), str, WarningStatus);
			m_oThreadManage.m_oADCSet.m_uiADCSetOptCount = 0;
			m_oThreadManage.m_oADCSet.m_uiADCSetOptPreviousNb = 0;
			m_oThreadManage.m_oADCSet.OnADCSetNextOpt();
		}
		else
		{
			m_oThreadManage.m_oADCSet.OnResetADCOperationNb();
			m_oThreadManage.m_oADCSet.OnSendADCSetCmd();
			str.Format(_T("在指定时间内未收全ADC命令序号为%d的应答，重发该指令！"), m_oThreadManage.m_oADCSet.m_uiADCSetOperationNb);
			m_pLogFile->OnWriteLogFile(_T("CTabSample::OnTimer"), str, WarningStatus);
		}
	}
	CDialog::OnTimer(nIDEvent);
}
void CTabSample::OnBnClickedCheckHeartbeat()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// 得到CHECK控件的状态
	BOOL bStatus = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_HEARTBEAT)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
	if (bStatus == TRUE)
	{
		m_oThreadManage.m_oHeartBeatThread.ResumeThread();
		m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedCheckHeartbeat"), _T("开启心跳！"), SuccessStatus);
	}
	else
	{
		m_oThreadManage.m_oHeartBeatThread.SuspendThread();
		m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedCheckHeartbeat"), _T("停止心跳！"), SuccessStatus);
	}
}

void CTabSample::OnBnClickedButtonSetadc()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë	
	// 以广播方式发送ADC参数设置
	m_oThreadManage.m_oADCSet.OnADCSet();
}

void CTabSample::OnBnClickedButtonTimecal()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// 发送尾包时刻查询帧以完成时统
	m_oThreadManage.m_oTailFrame.SendTailTimeFrame();
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
	GetDlgItem(IDC_BUTTON_STOPSAMPLE)->EnableWindow(FALSE);
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


// void CTabSample::OnBnClickedButtonFdu48()
// {
// 	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
// 	OnShowADCGraph(48);
// }

// 图形化显示ADC数据
//************************************
// Method:    OnShowADCGraph
// FullName:  CTabSample::OnShowADCGraph
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: unsigned int uiADCGraphNb
//************************************
LRESULT CTabSample::OnShowADCGraph(WPARAM wParam, LPARAM lParam)
{	
	m_dlgADCGraphShow[lParam].m_uiADCGraphNb = lParam;
	m_dlgADCGraphShow[lParam].m_uiADCGraphIP = wParam;
	// 如果窗体已经存在则先销毁
	if ( ::IsWindow( m_dlgADCGraphShow[lParam].m_hWnd ) )
	{
		m_dlgADCGraphShow[lParam].DestroyWindow();
	}
	// 创建窗体
	if ( m_dlgADCGraphShow[lParam].Create( IDD_DIALOG_GRAPHSHOW ) )
	{
		m_dlgADCGraphShow[lParam].ShowWindow( SW_SHOW );
		m_dlgADCGraphShow[lParam].UpdateWindow();
	}
	return TRUE;
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
	// hash_map迭代器
	m_oThreadManage.m_oInstrumentList.OnGetFduIndex();
	m_oInstrumentGraph.m_bLButtonSelect = true;
	SetTimer(TabSampleInitSysTimeTimerNb, TabSampleInitSysTimeTimerSet, NULL);
	GetDlgItem(IDC_BUTTON_SETBYHAND)->EnableWindow(FALSE);
// 	// 调试用，每1秒发送一次误码查询
// 	SetTimer(TabSampleQueryErrorCountTimerNb, TabSampleQueryErrorCountTimerSet, NULL);
// 	m_pLogFile->OnWriteLogFile(_T("CTabSample::OnBnClickedButtonSetbyhand"), _T("开始发送误码查询命令（采集站广播，交叉站按IP）！"), SuccessStatus);
}
// 本地时间查询的异步套接字响应函数
LRESULT CTabSample::OnSysTimeSocket(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case FD_READ:
		{
			m_oThreadManage.m_oSysTime.OnReceive();
		}
		break;
	}
	return TRUE;
}
// ADC参数设置的异步套接字响应函数
LRESULT CTabSample::OnADCSetSocket(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case FD_READ:
		{
			m_oThreadManage.m_oADCSet.OnReceive();
		}
		break;
	}
	return TRUE;
}
// 尾包时刻查询的异步套接字响应函数
LRESULT CTabSample::OnTailTimeSocket(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case FD_READ:
		{
			m_oThreadManage.m_oTailTimeFrame.OnReceive();
		}
		break;
	}
	return TRUE;
}
// 尾包帧的异步套接字响应函数
LRESULT CTabSample::OnTailFrameSocket(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case FD_READ:
		{
			m_oThreadManage.m_oTailFrame.OnReceive();
		}
		break;
	}
	return TRUE;
}
// IP地址设置应答帧的异步套接字响应函数
LRESULT CTabSample::OnIPSetSocket(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case FD_READ:
		{
			m_oThreadManage.m_oIPSet.OnReceive();
		}
		break;
	}
	return TRUE;
}
// 首包帧的异步套接字响应函数
LRESULT CTabSample::OnHeadFrameSocket(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case FD_READ:
		{
			m_oThreadManage.m_oHeadFrame.OnReceive();
		}
		break;
	}
	return TRUE;
}
// ADC数据帧图形化显示接收设置的异步套接字响应函数
LRESULT CTabSample::OnADCGraphSocket(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case FD_READ:
		{
			m_oThreadManage.m_oADCDataRecThread.OnReceive();
		}
		break;
	}
	return TRUE;
}
// 填充套接字地址结构
void CTabSample::OnSetSocketAddr(sockaddr_in* addr, u_short hostshort, ULONG Saddr)
{
	addr->sin_family = AF_INET;											// 填充套接字地址结构
	addr->sin_port = htons(hostshort);
	addr->sin_addr.S_un.S_addr = Saddr;
}

void CTabSample::OnBnClickedButtonGraphexe()
{
	// TODO: 在此添加控件通知处理程序代码
	// 得到当前路径
	wchar_t cSaveToFilePath[MAX_PATH];
	// 得到当前路径
	GetCurrentDirectory(MAX_PATH, cSaveToFilePath);

	wchar_t s[1000];
	swprintf_s(s,_T("%s\\绘图程序.exe"),cSaveToFilePath);
	char pach[1000];
	//保存成ANSI格式的文件
	string mstring = WideCharToMultiChar( (LPCTSTR)s );
	strcpy_s( pach, sizeof(pach), mstring.c_str() );
	// 调用其他应用程序的四种方法：
	// 以下是4种调用外部程序的方法，其中：WinExec调用成功后才会执行之后的代码，ShellExecute和CreateProcess
	// 调用后立即执行之后的代码，system用于调用Dos程序和command命令

	// (1) WinExec，例如：WinExec(pach,1);
	// (2) ShellExecute，例如：ShellExecute(this->m_hWnd, _T("open"), s, NULL, NULL, SW_SHOWNORMAL);
	// (3) CreateProcess，例如：
// 	CRect Screen;
// 	GetWindowRect(Screen);
// 	STARTUPINFO     StartupInfo; 
// 	ZeroMemory(   &StartupInfo,   sizeof(STARTUPINFO)); 
// 	StartupInfo.cb   =   sizeof(STARTUPINFO); 
// 	PROCESS_INFORMATION   ProcessInfo; 
// 	if(CreateProcess(NULL, s, NULL,   NULL, 
// 		TRUE,   NORMAL_PRIORITY_CLASS,   NULL,   NULL, 
// 		&StartupInfo,   &ProcessInfo)) 
// 	{ 
// 		//   we   must   close   the   handles   returned   in   ProcessInfo.hProcess 
// 		//   we   can   close   the   handle   at   any   time,   might   as   well   close   it   now 
// 		// 如果还要关闭打开的进程则需保存打开进程的进程句柄
// 		// HANDLE	hWordWeb=ProcessInfo.hProcess; 
// 		// 关闭进程
// 		// TerminateProcess(hWordWeb,   0); 
// 		CloseHandle(ProcessInfo.hProcess); 
// 		CloseHandle(ProcessInfo.hThread); 
// 	} 
	// (4) system：函数执行DOS和Command.com中的命令。包括各种DOS命令，以及其他command中的例行程序。 
	// 执行该程序时，被调用程序被挂起状态（当然要插入中断程序），而该程序执行。当该程序执行完毕，就会调用原程序。 
	// 例如：system("c:\\a.exe ");
	WinExec(pach,1);
}
// 计算绘图序号
unsigned int CTabSample::CalLocationGraph(unsigned int uiLocation)
{
	hash_map<unsigned int, CInstrument*>::iterator  iter;
	unsigned int uiUnCheckedNum = 0;
	for(iter=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.begin(); iter!=m_oThreadManage.m_oInstrumentList.m_oInstrumentIPMap.end(); iter++)
	{
		if (NULL != iter->second)
		{
			if ((iter->second->m_bIPSetOK == true) && (iter->second->m_uiInstrumentType == InstrumentTypeFDU))
			{
				if (iter->second->m_uiFDUIndex < uiLocation)
				{
					if (iter->second->m_iSelectObject == BST_UNCHECKED)
					{
						uiUnCheckedNum++;
					}
				}
			}
		}
	}
	return uiUnCheckedNum;
}
// 绘制设备连接图
void CTabSample::OnDrawInstrumentGraph(void)
{
	CRect oRect = 0;
	GetDlgItem(IDC_STATIC_GRAPH)->GetWindowRect(oRect);
	ScreenToClient(oRect);
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW) ;
	m_oInstrumentGraph.CreateEx(WS_EX_STATICEDGE, 
		className, NULL, WS_CHILD | WS_VISIBLE, 
		oRect.left, oRect.top, oRect.Width(), oRect.Height(),
		this->GetSafeHwnd(), NULL);
}
