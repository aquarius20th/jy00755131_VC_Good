// TabUartToUdp.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabUartToUdp.h"


// CTabUartToUdp 对话框

IMPLEMENT_DYNAMIC(CTabUartToUdp, CDialog)

CTabUartToUdp::CTabUartToUdp(CWnd* pParent /*=NULL*/)
	: CDialog(CTabUartToUdp::IDD, pParent)
	, m_usUartBufCount(0)
	, m_uiUartCount(0)
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
void CTabUartToUdp::OnInit(void)
{
	//端口设置
	m_comboSerialPortCom.SetWindowText( _T(COM0) );		// 设置控件初始化显示
	m_comboSerialPortCom.InsertString( 0, _T(COM0) );	// 插入下拉菜单选项
	m_comboSerialPortCom.InsertString( 1, _T(COM1) );
	m_comboSerialPortCom.InsertString( 2, _T(COM2) );
	m_comboSerialPortCom.InsertString( 3, _T(COM3) );
	m_comboSerialPortCom.InsertString( 4, _T(COM4) );
	m_comboSerialPortCom.InsertString( 5, _T(COM5) );
	m_comboSerialPortCom.InsertString( 6, _T(COM6) );
	m_comboSerialPortCom.InsertString( 7, _T(COM7) );
	m_comboSerialPortCom.InsertString( 8, _T(COM8) );
	m_comboSerialPortCom.InsertString( 9, _T(COM16) );

	//波特率设置
	m_comboSerialPortBaud.SetWindowText( _T(BAUD11) );	// 设置控件初始化显示
	m_comboSerialPortBaud.InsertString( 0, _T(BAUD0) );	// 插入下拉菜单选项
	m_comboSerialPortBaud.InsertString( 1, _T(BAUD1) );
	m_comboSerialPortBaud.InsertString( 2, _T(BAUD2) );
	m_comboSerialPortBaud.InsertString( 3, _T(BAUD3) );
	m_comboSerialPortBaud.InsertString( 4, _T(BAUD4) );
	m_comboSerialPortBaud.InsertString( 5, _T(BAUD5) );
	m_comboSerialPortBaud.InsertString( 6, _T(BAUD6) );
	m_comboSerialPortBaud.InsertString( 7, _T(BAUD7) );
	m_comboSerialPortBaud.InsertString( 8, _T(BAUD8) );
	m_comboSerialPortBaud.InsertString( 9, _T(BAUD9) );
	m_comboSerialPortBaud.InsertString( 10, _T(BAUD10) );
	m_comboSerialPortBaud.InsertString( 11, _T(BAUD11) );

	// 串口通讯设置
	m_ctrlMSComm1.put_InBufferCount(0);		// 清空接收缓存
	m_ctrlMSComm1.put_OutBufferCount(0);	// 清空发送缓存
	m_ctrlMSComm1.put_InputMode(1);         // 二进制模式
	CString str;
	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->SetWindowText("127.0.0.1");
	str.Format("0x%04x",0x9001);
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->SetWindowText(str);	// 0x9001
	str.Format("0x%04x",0x9002);
	GetDlgItem(IDC_EDIT_PORT_REC)->SetWindowText(str);			// 0x9002

	m_usUartBufCount = 0;
	m_uiUartCount = 0;
}

// 得到当前选择的串口号

int CTabUartToUdp::OnGetSerialPortCom(void)
{
	int iCurrentCom = 0;
	CString str;
	m_comboSerialPortCom.GetWindowText(str);	
	if (str == _T(COM0))
	{
		iCurrentCom = 1;
	}
	else if (str == _T(COM1))
	{
		iCurrentCom = 2;
	}
	else if (str == _T(COM2))
	{
		iCurrentCom = 3;
	}
	else if (str == _T(COM3))
	{
		iCurrentCom = 4;
	}
	else if (str == _T(COM4))
	{
		iCurrentCom = 5;
	}
	else if (str == _T(COM5))
	{
		iCurrentCom = 6;
	}
	else if (str == _T(COM6))
	{
		iCurrentCom = 7;
	}
	else if (str == _T(COM7))
	{
		iCurrentCom = 8;
	}
	else if (str == _T(COM8))
	{
		iCurrentCom = 9;
	}
	else if (str == _T(COM16))
	{
		iCurrentCom = 17;
	}
	else
	{
		AfxMessageBox(_T("Input Serial Port Com Nb is Wrong!"));
	}
	return iCurrentCom;
}

// 得到当前选择的波特率
int CTabUartToUdp::OnGetSerialPortBaud(void)
{
	int iCurrentBaud = 0;
	CString str;
	m_comboSerialPortBaud.GetWindowText(str);	
	if (str == _T(BAUD0))
	{
		iCurrentBaud = 300;
	}
	else if (str == _T(BAUD1))
	{
		iCurrentBaud = 600;
	}
	else if (str == _T(BAUD2))
	{
		iCurrentBaud = 1200;
	}
	else if (str == _T(BAUD3))
	{
		iCurrentBaud = 2400;
	}
	else if (str == _T(BAUD4))
	{
		iCurrentBaud = 4800;
	}
	else if (str == _T(BAUD5))
	{
		iCurrentBaud = 9600;
	}
	else if (str == _T(BAUD6))
	{
		iCurrentBaud = 19200;
	}
	else if (str == _T(BAUD7))
	{
		iCurrentBaud = 38400;
	}
	else if (str == _T(BAUD8))
	{
		iCurrentBaud = 43000;
	}
	else if (str == _T(BAUD9))
	{
		iCurrentBaud = 56000;
	}
	else if (str == _T(BAUD10))
	{
		iCurrentBaud = 57600;
	}	
	else if (str == _T(BAUD11))
	{
		iCurrentBaud = 115200;
	}
	else
	{
		AfxMessageBox(_T("Input Serial Port Baud is Wrong!"));
	}

	return iCurrentBaud;
}

// 从UDP发送数据
void CTabUartToUdp::OnSendFromUDP(unsigned char* TXDATA, unsigned int uiCount)
{
	unsigned int uiFrameCount = 0;
	unsigned char ucTxdata[256];
	CString str;
	CString strIP;
	int iSendPort;
	uiFrameCount = uiCount/256;
	if (uiFrameCount == 0)
	{
		return;
	}
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->GetWindowText(str);
	sscanf_s(str,"%x", &iSendPort);
	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->GetWindowText(str);
	strIP = str;
	for (unsigned int i=0; i<uiFrameCount; i++)
	{
		memcpy(&ucTxdata, &TXDATA[i*256], 256);
		m_socketUartToUdp.SendTo(ucTxdata, 256, iSendPort, strIP);
	}
}


void CTabUartToUdp::OnBnClickedButtonSerialportOpen()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	// 先关闭已经打开的端口
	if(m_ctrlMSComm1.get_PortOpen())
	{
		m_ctrlMSComm1.put_PortOpen(FALSE);
		m_socketUartToUdp.Close();
	}
	UpdateData(TRUE);
	int iPort;
	int iBaud;
	CString str;
	int iRecPort;
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
		sscanf_s(str,"%x", &iRecPort);
		// 创建Socket
		m_socketUartToUdp.Create(iRecPort,SOCK_DGRAM);
		m_socketUartToUdp.SetSockOpt(NULL,&m_socketUartToUdp,65536,SOL_SOCKET);
		ControlDisable();
		AfxMessageBox(_T("Open Serial Port Successful!"));
	}
	KillTimer(1);
	SetTimer(1,5,NULL);
}

void CTabUartToUdp::OnBnClickedButtonSerialportClose()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_ctrlMSComm1.put_PortOpen(FALSE);
	m_socketUartToUdp.Close();
	ControlEnable();
	if (!m_ctrlMSComm1.get_PortOpen())
	{
		AfxMessageBox(_T("Close Serial Port Successful!"));
	}
}

void CTabUartToUdp::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	if (nIDEvent == 1)
	{
		VARIANT vResponse;
		int len;
		unsigned char* Rxdata = NULL;

		len = m_ctrlMSComm1.get_InBufferCount(); //接收到的字符数目

		if(len > 0) 
		{
			// 设置需要读取的输入缓冲区大小，为了不丢帧
			m_ctrlMSComm1.put_InputLen(len);
			vResponse=m_ctrlMSComm1.get_Input(); //read
			Rxdata = (unsigned char*) vResponse.parray->pvData;
			if (len >1024)
			{
				AfxMessageBox(_T("Receive data from Uart leaked!"));
				return;
			} 

			for (int i=0; i<len; i++)
			{
				switch(m_uiUartCount)
				{
				case 0:
					if (Rxdata[i] == 0x11)
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
					if (Rxdata[i] == 0x22)
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
					if (Rxdata[i] == 0x33)
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
					if (Rxdata[i] == 0x44)
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
					m_ucUartBuf[m_usUartBufCount][m_uiUartCount] = Rxdata[i];
					m_uiUartCount++;
					if (m_uiUartCount == 256)
					{
						m_uiUartCount = 0;
						OnSendFromUDP(m_ucUartBuf[m_usUartBufCount], 256);
						m_usUartBufCount += 1;
						m_usUartBufCount = m_usUartBufCount % 8;
					}
					break;
				}
			}
		} // 接收到字符，MSComm控件发送事件 
	}

	CDialog::OnTimer(nIDEvent);
}

// 使能控件
void CTabUartToUdp::ControlEnable(void)
{
	GetDlgItem(IDC_COMBO_SERIALPORT_COM)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_SERIALPORT_BAUD)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT_REC)->EnableWindow(TRUE);
}

// 禁用控件
void CTabUartToUdp::ControlDisable(void)
{
	GetDlgItem(IDC_COMBO_SERIALPORT_COM)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_SERIALPORT_BAUD)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS_UARTTOUDP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT_UARTTOUDP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT_REC)->EnableWindow(FALSE);
}
