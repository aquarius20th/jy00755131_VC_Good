// TabUartToUdp.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabUartToUdp.h"


// CTabUartToUdp 对话框
// HANDLE	hCom;				// 准备打开的串口的句柄
// HANDLE	hCommWatchThread;	// 辅助线程的全局函数
// UINT CommWatchProc(CTabUartToUdp *pwnd);	// 线程函数

IMPLEMENT_DYNAMIC(CTabUartToUdp, CDialog)

CTabUartToUdp::CTabUartToUdp(CWnd* pParent /*=NULL*/)
	: CDialog(CTabUartToUdp::IDD, pParent)
	, m_usUartBufCount(0)
	, m_uiUartCount(0)
	, m_uiSerialPortComCurSel(0)
	, m_uiSerialPortBaudCurSel(0)
	, m_uiSendPort(0)
	, m_uiRecPort(0)
	, m_csIPAddress(_T(""))
{

}

CTabUartToUdp::~CTabUartToUdp()
{
}

void CTabUartToUdp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SERIALPORT_COM, m_comboSerialPortCom);
	DDX_Control(pDX, IDC_COMBO_SERIALPORT_BAUD, m_comboSerialPortBaud);
	DDX_Control(pDX, IDC_MSCOMM1, m_ctrlMSComm1);
}


BEGIN_MESSAGE_MAP(CTabUartToUdp, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SERIALPORT_OPEN, &CTabUartToUdp::OnBnClickedButtonSerialportOpen)
	ON_BN_CLICKED(IDC_BUTTON_SERIALPORT_CLOSE, &CTabUartToUdp::OnBnClickedButtonSerialportClose)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// 初始化设置
//************************************
// Method:    OnInit
// FullName:  CTabUartToUdp::OnInit
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabUartToUdp::OnInit(void)
{
	//端口设置
	CString str = _T("");
	for (int i=0; i<COMNum; i++)
	{
		str.Format(_T("COM%d"), i+1);
		m_comboSerialPortCom.InsertString(i, str);	// 插入下拉菜单选项
	}
	m_comboSerialPortCom.SetCurSel(m_uiSerialPortComCurSel);					// 设置控件初始化显示

	//波特率设置
	str.Format(_T("%d"), BAUD0);
	m_comboSerialPortBaud.InsertString( 0, str );	// 插入下拉菜单选项
	str.Format(_T("%d"), BAUD1);
	m_comboSerialPortBaud.InsertString( 1, str );
	str.Format(_T("%d"), BAUD2);
	m_comboSerialPortBaud.InsertString( 2, str );
	str.Format(_T("%d"), BAUD3);
	m_comboSerialPortBaud.InsertString( 3, str );
	str.Format(_T("%d"), BAUD4);
	m_comboSerialPortBaud.InsertString( 4, str );
	str.Format(_T("%d"), BAUD5);
	m_comboSerialPortBaud.InsertString( 5, str );
	str.Format(_T("%d"), BAUD6);
	m_comboSerialPortBaud.InsertString( 6, str );
	str.Format(_T("%d"), BAUD7);
	m_comboSerialPortBaud.InsertString( 7, str );
	str.Format(_T("%d"), BAUD8);
	m_comboSerialPortBaud.InsertString( 8, str );
	str.Format(_T("%d"), BAUD9);
	m_comboSerialPortBaud.InsertString( 9, str );
	str.Format(_T("%d"), BAUD10);
	m_comboSerialPortBaud.InsertString( 10, str );
	str.Format(_T("%d"), BAUD11);
	m_comboSerialPortBaud.InsertString( 11, str );
	m_comboSerialPortBaud.SetCurSel(m_uiSerialPortBaudCurSel);					// 设置控件初始化显示

	// 串口通讯设置
	m_ctrlMSComm1.put_InBufferCount(0);		// 清空接收缓存
	m_ctrlMSComm1.put_OutBufferCount(0);	// 清空发送缓存
	m_ctrlMSComm1.put_InputMode(1);         // 二进制模式

	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->SetWindowText(m_csIPAddress);
	str.Format(_T("0x%04x"),m_uiSendPort);
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->SetWindowText(str);	// 0x9001
	str.Format(_T("0x%04x"),m_uiRecPort);
	GetDlgItem(IDC_EDIT_PORT_REC)->SetWindowText(str);			// 0x9002

	m_usUartBufCount = 0;
	m_uiUartCount = 0;
}

// 得到当前选择的串口号
//************************************
// Method:    OnGetSerialPortCom
// FullName:  CTabUartToUdp::OnGetSerialPortCom
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: void
//************************************
int CTabUartToUdp::OnGetSerialPortCom(void)
{
	int iCurrentCom = 0;
	CString str = _T("");
	CString strTemp = _T("");
	m_comboSerialPortCom.GetWindowText(str);	
	for (int i=0; i<COMNum; i++)
	{
		strTemp.Format(_T("COM%d"), i+1);
		if (str == strTemp)
		{
			iCurrentCom = i+1;
			break;
		}
	}
	if (iCurrentCom == 0)
	{
		AfxMessageBox(_T("Input Serial Port Com Nb is Wrong!"));
	}
	return iCurrentCom;
}

// 得到当前选择的波特率
//************************************
// Method:    OnGetSerialPortBaud
// FullName:  CTabUartToUdp::OnGetSerialPortBaud
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: void
//************************************
int CTabUartToUdp::OnGetSerialPortBaud(void)
{
	int iCurrentBaud = 0;
	CString str = _T("");
	CString strTemp = _T("");
	m_comboSerialPortBaud.GetWindowText(str);	
	strTemp.Format(_T("%d"), BAUD0);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD0;
	}
	strTemp.Format(_T("%d"), BAUD1);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD1;
	}
	strTemp.Format(_T("%d"), BAUD2);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD2;
	}
	strTemp.Format(_T("%d"), BAUD3);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD3;
	}
	strTemp.Format(_T("%d"), BAUD4);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD4;
	}
	strTemp.Format(_T("%d"), BAUD5);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD5;
	}
	strTemp.Format(_T("%d"), BAUD6);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD6;
	}
	strTemp.Format(_T("%d"), BAUD7);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD7;
	}
	strTemp.Format(_T("%d"), BAUD8);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD8;
	}
	strTemp.Format(_T("%d"), BAUD9);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD9;
	}
	strTemp.Format(_T("%d"), BAUD10);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD10;
	}	
	strTemp.Format(_T("%d"), BAUD11);
	if (str == strTemp)
	{
		iCurrentBaud = BAUD11;
	}
	if (iCurrentBaud == 0)
	{
		AfxMessageBox(_T("Input Serial Port Baud is Wrong!"));
	}
	return iCurrentBaud;
}

// 从UDP发送数据
//************************************
// Method:    OnSendFromUDP
// FullName:  CTabUartToUdp::OnSendFromUDP
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: unsigned char * TXDATA
// Parameter: unsigned int uiCount
//************************************
void CTabUartToUdp::OnSendFromUDP(unsigned char* TXDATA, unsigned int uiCount)
{
	unsigned int uiFrameCount = 0;
	unsigned char ucTxdata[RcvFrameSize];
	CString str = _T("");
	CString strIP = _T("");
	int iSendPort = 0;
	unsigned int uiFrameSize = RcvFrameSize;
	uiFrameCount = uiCount/uiFrameSize;
	if (uiFrameCount == 0)
	{
		return;
	}
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->GetWindowText(str);
	sscanf_s(str, "%x", &iSendPort);
	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->GetWindowText(str);
	strIP = str;
	for (unsigned int i=0; i<uiFrameCount; i++)
	{
		ProcessMessages();
		memcpy(&ucTxdata, &TXDATA[i*uiFrameSize], uiFrameSize);
		m_socketUartToUdp.SendTo(ucTxdata, uiFrameSize, iSendPort, strIP);
	}
}
// UINT CommWatchProc(CTabUartToUdp *pwnd)
// {
// 	unsigned char lpBuffer[16384];
// 	unsigned long datacount;	//接收数据的数量
// 
// 	OVERLAPPED wol;
// 	wol.Internal=0;
// 	wol.InternalHigh=0;
// 	wol.Offset=0;
// 	wol.OffsetHigh=0;
// 	wol.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
// 
// 	while(1)
// 	{
// 		if (pwnd->m_bUartThreadclose == TRUE)
// 		{
// 			break;
// 		}
// 		ReadFile(hCom, lpBuffer, 16384, &datacount, &wol);	
// 		if (datacount!=0)			//收到有效数据
// 		{							//把接收的数据作为参数传给消息相应函数
// 			pwnd->OnUartReceiveData(lpBuffer, datacount);
// 		}
// 		if (pwnd->m_bUartThreadclose == TRUE)
// 		{
// 			break;
// 		}
// 		Sleep(500);
// 	}
// 	return 1;
// }

void CTabUartToUdp::OnBnClickedButtonSerialportOpen()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// 打开串口
	OnOpenCom();
//	m_bUartThreadclose = FALSE;
// 	DWORD threadID;							// 启动USB口监视线程
// 	if (NULL != hCommWatchThread)
// 	{
// 		CloseHandle(hCommWatchThread);
// 	}
// 	hCommWatchThread = ::CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,
// 		(LPTHREAD_START_ROUTINE)CommWatchProc,this, 0, &threadID);//hDevice  
// 	if (hCommWatchThread == NULL)
// 	{
// 		::AfxMessageBox(_T("创建接收线程失败!"));
// 	}
	GetDlgItem(IDC_BUTTON_SERIALPORT_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERIALPORT_CLOSE)->EnableWindow(TRUE);
}

void CTabUartToUdp::OnBnClickedButtonSerialportClose()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

// 	OnCloseCom();
// //	m_bUartThreadclose = TRUE;
// 	m_socketUartToUdp.Close();
// 	ControlEnable();
// 	AfxMessageBox(_T("Close Serial Port Successful!"));
	m_ctrlMSComm1.put_PortOpen(FALSE);
	m_socketUartToUdp.Close();
	ControlEnable();
	if (!m_ctrlMSComm1.get_PortOpen())
	{
		AfxMessageBox(_T("Close Serial Port Successful!"));
	}
	GetDlgItem(IDC_BUTTON_SERIALPORT_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERIALPORT_CLOSE)->EnableWindow(FALSE);
}

// 使能控件
//************************************
// Method:    ControlEnable
// FullName:  CTabUartToUdp::ControlEnable
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabUartToUdp::ControlEnable(void)
{
	GetDlgItem(IDC_COMBO_SERIALPORT_COM)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_SERIALPORT_BAUD)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT_REC)->EnableWindow(TRUE);
}

// 禁用控件
//************************************
// Method:    ControlDisable
// FullName:  CTabUartToUdp::ControlDisable
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabUartToUdp::ControlDisable(void)
{
	GetDlgItem(IDC_COMBO_SERIALPORT_COM)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_SERIALPORT_BAUD)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT_REC)->EnableWindow(FALSE);
}

// 打开串口
//************************************
// Method:    OnOpenCom
// FullName:  CTabUartToUdp::OnOpenCom
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabUartToUdp::OnOpenCom(void)
{
// 	UpdateData(TRUE);
// 	CString str;
// 	m_comboSerialPortCom.GetWindowText(str);
// 	hCom =CreateFile( str, GENERIC_READ | GENERIC_WRITE/*允许读写*/,
// 		0/*此项必须为0*/,NULL/*no security attrs*/,
// 		OPEN_EXISTING/*设置产生方式*/,
// 		0/*使用同步通信*/,
// 		NULL );
// 
// 	if(hCom!=INVALID_HANDLE_VALUE)			// 检测打开串口操作是否成功
// 	{
// 		AfxMessageBox(_T("Open Serial Port Successful!"));
// 	}
// 	else
// 	{
// 		AfxMessageBox(_T("Serial Port COM Nb is Invalid!"));
// 		return;
// 	}
// 
// 
// 	SetupComm( hCom, 1024,1024);					// 设置输入、输出缓冲区的大小
// 
// 	PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR| PURGE_RXCLEAR );	// 清干净输入、输出缓冲区
// 
// 	LPCOMMTIMEOUTS lpCommTimeouts = new COMMTIMEOUTS;//(LPCOMMTIMEOUTS)malloc(sizeof(LPCOMMTIMEOUTS));
// 	lpCommTimeouts->ReadIntervalTimeout=MAXDWORD; // 读间隔超时
// 	lpCommTimeouts->ReadTotalTimeoutMultiplier=0; // 读时间系数
// 	lpCommTimeouts->ReadTotalTimeoutConstant=0; // 读时间常量 
// 	lpCommTimeouts->WriteTotalTimeoutMultiplier=50; // 写时间系数
// 	lpCommTimeouts->WriteTotalTimeoutConstant=2000; // 写时间常量
// 	//加入读取和写入延时以避免同步通信过程中的线程阻塞情况
// 
// 	SetCommTimeouts(hCom, lpCommTimeouts);
// 
// 	DCB	dcb ; // 定义数据控制块结构
// 
// 	GetCommState(hCom, &dcb ) ; //读串口原来的参数设置
// 
// 	dcb.BaudRate = OnGetSerialPortBaud(); 
// 	dcb.ByteSize = 8; 
// 	dcb.Parity = NOPARITY;
// 	dcb.StopBits = ONESTOPBIT ;
// 	dcb.fBinary = TRUE ;
// 	dcb.fParity = FALSE;
// 
// 	SetCommState(hCom, &dcb ) ; //串口参数配置
// 
// 	int iRecPort;
// 	GetDlgItem(IDC_EDIT_PORT_REC)->GetWindowText(str);			// 0x9002
// 	sscanf_s(str,"%x", &iRecPort);
// 	// 创建Socket
// 	m_socketUartToUdp.Create(iRecPort,SOCK_DGRAM);
// 	m_socketUartToUdp.SetSockOpt(SO_RCVBUF,&m_socketUartToUdp,UartToUDPBufSize,SOL_SOCKET);
// 	m_socketUartToUdp.SetSockOpt(SO_SNDBUF,&m_socketUartToUdp,UartToUDPBufSize,SOL_SOCKET);

	int iPort = 0;
	int iBaud = 0;
	CString str = _T("");
	if(m_ctrlMSComm1.get_PortOpen())
	{
		m_ctrlMSComm1.put_PortOpen(FALSE);
		m_socketUartToUdp.Close();
	}
	UpdateData(TRUE);
	iPort = OnGetSerialPortCom();
	iBaud = OnGetSerialPortBaud();
	m_ctrlMSComm1.put_CommPort(iPort);
	str.Format(_T("%d,n,8,1"), iBaud);
	m_ctrlMSComm1.put_Settings(str);

	if(!m_ctrlMSComm1.get_PortOpen())
	{
		m_ctrlMSComm1.put_PortOpen(TRUE);
		m_socketUartToUdp.m_pctrlMSComm1 = &m_ctrlMSComm1;
		GetDlgItem(IDC_EDIT_PORT_REC)->GetWindowText(str);			// 0x9002
		sscanf_s(str,"%x", &m_uiRecPort);
		m_uiSerialPortComCurSel = m_comboSerialPortCom.GetCurSel();
		m_uiSerialPortBaudCurSel = m_comboSerialPortBaud.GetCurSel();
	
		BOOL bReturn = FALSE; 
		// 创建Socket
		bReturn =  m_socketUartToUdp.Create(m_uiRecPort,SOCK_DGRAM);
		if (bReturn == FALSE)
		{
			AfxMessageBox(_T("UDP转串口端口创建失败！"));
		}
		else
		{
			bReturn = m_socketUartToUdp.SetSockOpt(SO_SNDBUF,&m_socketUartToUdp,UartToUDPBufSize,SOL_SOCKET);
			if (bReturn == FALSE)
			{
				AfxMessageBox(_T("UDP转串口端口设置发送缓冲区设置失败！"));
			}
			bReturn = m_socketUartToUdp.SetSockOpt(SO_RCVBUF,&m_socketUartToUdp,UartToUDPBufSize,SOL_SOCKET);
			if (bReturn == FALSE)
			{
				AfxMessageBox(_T("UDP转串口端口设置接收缓冲区设置失败！"));
			}
			DWORD dwBytesReturned = 0;
			BOOL bNewBehavior = FALSE;
			DWORD status = 0;
			status = WSAIoctl(m_socketUartToUdp, SIO_UDP_CONNRESET ,
				&bNewBehavior, sizeof(bNewBehavior),
				NULL, 0, &dwBytesReturned,
				NULL, NULL);
		}
		ControlDisable();
		AfxMessageBox(_T("Open Serial Port Successful!"));
	}
	KillTimer(TabUartToUdpRecTimerNb);
	SetTimer(TabUartToUdpRecTimerNb, TabUartToUdpRecTimerSet, NULL);
}

// 关闭串口
// void CTabUartToUdp::OnCloseCom(void)
// {
// 	if(hCom != NULL)
// 	{
// 		CloseHandle(hCom);
// 	}
// }

// 串口接收数据
void CTabUartToUdp::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	if (nIDEvent == TabUartToUdpRecTimerNb)
	{
		KillTimer(TabUartToUdpRecTimerNb);
		VARIANT vResponse;
		int len = 0;
		unsigned char* Rxdata = NULL;

		len = m_ctrlMSComm1.get_InBufferCount(); //接收到的字符数目

		if(len > 0) 
		{
			// 设置需要读取的输入缓冲区大小，为了不丢帧
			m_ctrlMSComm1.put_InputLen(len);
			vResponse=m_ctrlMSComm1.get_Input(); //read
			Rxdata = (unsigned char*) vResponse.parray->pvData;
// 			if (len >16384)
// 			{
// 				AfxMessageBox(_T("Receive data from Uart leaked!"));
// 				return;
// 			} 

			for (int i=0; i<len; i++)
			{
				ProcessMessages();
				switch(m_uiUartCount)
				{
				case 0:
					if (Rxdata[i] == FrameHeadCheck0)
					{
						m_ucUartBuf[m_usUartBufCount][0] = Rxdata[i];
						m_uiUartCount++;
						break;
					}
					else
					{
						m_uiUartCount = 0;
						break;
					}

				case 1:
					if (Rxdata[i] == FrameHeadCheck1)
					{
						m_ucUartBuf[m_usUartBufCount][1] = Rxdata[i];
						m_uiUartCount++;
						break;
					}
					else
					{
						m_uiUartCount = 0;
						break;
					}
				case 2:
					if (Rxdata[i] == FrameHeadCheck2)
					{
						m_ucUartBuf[m_usUartBufCount][2] = Rxdata[i];
						m_uiUartCount++;
						break;
					}
					else
					{
						m_uiUartCount = 0;
						break;
					}
				case 3:
					if (Rxdata[i] == FrameHeadCheck3)
					{
						m_ucUartBuf[m_usUartBufCount][3] = Rxdata[i];
						m_uiUartCount++;
						break;
					}
					else
					{
						m_uiUartCount = 0;
						break;
					}
				default:
					if (m_uiUartCount <  RcvFrameSize)
					{
						m_ucUartBuf[m_usUartBufCount][m_uiUartCount] = Rxdata[i];
						m_uiUartCount++;
						if (m_uiUartCount == RcvFrameSize)
						{
							m_uiUartCount = 0;
							OnSendFromUDP(m_ucUartBuf[m_usUartBufCount], RcvFrameSize);
							m_usUartBufCount += 1;
							m_usUartBufCount = m_usUartBufCount % RcvBufNum;
						}
					}
					break;
				}
			}
		} // 接收到字符，MSComm控件发送事件 
		SetTimer(TabUartToUdpRecTimerNb, TabUartToUdpRecTimerSet, NULL);
	}
	
	CDialog::OnTimer(nIDEvent);
}

// 关闭
//************************************
// Method:    OnClose
// FullName:  CTabUartToUdp::OnClose
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabUartToUdp::OnClose(void)
{
//	OnCloseCom();
	if(m_ctrlMSComm1.get_PortOpen())
	{
		m_ctrlMSComm1.put_PortOpen(FALSE);
		m_socketUartToUdp.Close();
	}
	KillTimer(TabUartToUdpRecTimerNb);
//	m_bUartThreadclose = TRUE;
}

// 防止程序在循环中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CTabUartToUdp::ProcessMessages
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabUartToUdp::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
