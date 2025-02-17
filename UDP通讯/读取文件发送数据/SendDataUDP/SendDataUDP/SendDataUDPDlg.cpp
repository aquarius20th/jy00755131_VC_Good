// SendDataUDPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SendDataUDP.h"
#include "SendDataUDPDlg.h"
//extern void ReadSeletedFile(vector <UCHAR> Buf);

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


// CSendDataUDPDlg 对话框




CSendDataUDPDlg::CSendDataUDPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendDataUDPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSendDataUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSendDataUDPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSendDataUDPDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CSendDataUDPDlg 消息处理程序

BOOL CSendDataUDPDlg::OnInitDialog()
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
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	m_SendData.reserve(1000);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSendDataUDPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSendDataUDPDlg::OnPaint()
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
HCURSOR CSendDataUDPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
DWORD CSendDataUDPDlg::StringIPToDWORDIP(CString& strIP)
{	
	DWORD dwIP;
	DWORD  by1,by2,by3,by4;
	strIP.Replace(_T("."),_T(" "));
	_stscanf_s(LPCTSTR(strIP),_T("%d %d %d %d"),&by1,&by2,&by3,&by4);
	dwIP = by4;
	dwIP = dwIP<<8 | by3;
	dwIP = dwIP<<8 | by2;
	dwIP = dwIP<<8 | by1;
	return dwIP;
}
//读取文件内容到缓冲区
void CSendDataUDPDlg::ReadSeletedFile()
{

	CString sPathName;	//读取文件的绝对路径
	CFile savefile;
	CString strtemp;
	const wchar_t pszFilter[] = _T("TXT Document (*.txt)|*.txt||");
	CFileDialog dlg(TRUE, _T("EC"), _T("*.txt"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("Text Document(*.txt)|*.txt")); 

	if ( dlg.DoModal() != IDOK )
		return;
	sPathName=dlg.GetPathName();
	if ((_waccess(sPathName,0)) != -1)
	{

		wchar_t* chartemp;
		CStdioFile   file(sPathName,CFile::modeRead);	//只读   
		while(file.ReadString(strtemp))					// 每读完一行，指向下一行，如果读到文件末尾，退出循环   
		{ 
			chartemp = strtemp.GetBuffer(strtemp.GetLength()+1); 

			for (int i = 0;i < strtemp.GetLength();i++)
			{
				m_SendData.push_back((char)chartemp[i]);
			}
			m_SendData.push_back('\r');
			m_SendData.push_back('\n');
		}
	}
	else
	{
		AfxMessageBox(_T("文件不存在！"));
		return;
	}
}
//将数据缓冲区中的数据发送出去
void CSendDataUDPDlg::SendData()
{
	unsigned long SendDataLength = 1;
	SendDataLength = m_SendData.size();
	if( !(SendDataLength > 0) )
	{
		AfxMessageBox(_T("文件格式错误或无内容！"));
		return;
	}

	// 	// 设置发送缓冲区大小
	// 	if (setsockopt(m_UDPSendDataSocket, SOL_SOCKET, SO_SNDBUF, (char*)&pBuf, SendDataLength) == SOCKET_ERROR) 
	// 	{
	// 		closesocket(m_UDPSendDataSocket);
	// 		m_UDPSendDataSocket = NULL;
	// 		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	// 	}

	SOCKET m_UDPSendDataSocket;											//定义套接字句柄
	m_UDPSendDataSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);		//创建UDP套接字
	sockaddr_in SendDataAddr;								//套接字地址结构变量
	u_short m_UDPSendDataPort = 0x5010;									//目标机端口
	CString m_csUDPSendDataIP = _T("192.168.1.56");						//目标机IP地址
	DWORD m_dwUDPSendDataIP = StringIPToDWORDIP(m_csUDPSendDataIP);		

	SendDataAddr.sin_family = AF_INET;
	SendDataAddr.sin_port = htons(m_UDPSendDataPort);
	SendDataAddr.sin_addr.s_addr = m_dwUDPSendDataIP;
	//	bind(m_UDPSendDataSocket, (sockaddr*)&SendDataAddr,sizeof(SendDataAddr));	//绑定套接字，由于不是TCP通讯，不需要listen，所以可以不用将套接字与地址结构信息相关联

	unsigned long ulTimes = 0; 
	unsigned char tBuf[256];
	while (SendDataLength > 256)
	{
		for (int i=0;i<256;i++)
		{
			tBuf[i] = m_SendData[ulTimes*256+i];
		}
		sendto(m_UDPSendDataSocket,(char*)tBuf,256,0,(sockaddr*)&SendDataAddr,sizeof(sockaddr_in));
		SendDataLength -=256;
		ulTimes++;

	}
	if (SendDataLength > 0)
	{
		for (unsigned long i=0;i<SendDataLength;i++)
		{
			tBuf[i] = m_SendData[ulTimes*256+i];
		}
		sendto(m_UDPSendDataSocket,(char*)tBuf,SendDataLength,0,(sockaddr*)&SendDataAddr,sizeof(sockaddr_in));
	}
	m_SendData.clear();
}
void CSendDataUDPDlg::OnBnClickedButtonSend()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	ReadSeletedFile();
	SendData();

}
