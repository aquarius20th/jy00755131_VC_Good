// BLASTER_SHOTPRODlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BLASTER_SHOTPRO.h"
#include "BLASTER_SHOTPRODlg.h"

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


// CBLASTER_SHOTPRODlg 对话框




CBLASTER_SHOTPRODlg::CBLASTER_SHOTPRODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBLASTER_SHOTPRODlg::IDD, pParent)
	, m_uiSP428XL(1)
	, m_uiSPShotPro(1)
	, m_dSL428XL(10.0)
	, m_dSLShotPro(10.0)
	, m_dSN428XL(102.0)
	, m_dSNShotPro(102.0)
	, m_uiSI428XL(8)
	, m_uiSIShotPro(6)
	, m_uiSEQ428XL(1)
	, m_uiSEQShotPro(1)
	, m_uiSTAShotPro(7)
	, m_dCTBShotPro(00.004)
	, m_dUHShotPro(000.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBLASTER_SHOTPRODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SP_428XL, m_uiSP428XL);
	DDV_MinMaxUInt(pDX, m_uiSP428XL, 1, 99999);
	DDX_Text(pDX, IDC_EDIT_SP_SHOTPRO, m_uiSPShotPro);
	DDV_MinMaxUInt(pDX, m_uiSPShotPro, 1, 99999);
	DDX_Text(pDX, IDC_EDIT_SL_428XL, m_dSL428XL);
	DDV_MinMaxDouble(pDX, m_dSL428XL, 0.0, 99999.9);
	DDX_Text(pDX, IDC_EDIT_SL_SHOTPRO, m_dSLShotPro);
	DDV_MinMaxDouble(pDX, m_dSLShotPro, 0.0, 99999.9);
	DDX_Text(pDX, IDC_EDIT_SN_428XL, m_dSN428XL);
	DDV_MinMaxDouble(pDX, m_dSN428XL, 0.0, 99999.9);
	DDX_Text(pDX, IDC_EDIT_SN_SHOTPRO, m_dSNShotPro);
	DDX_Text(pDX, IDC_EDIT_SI_428XL, m_uiSI428XL);
	DDV_MinMaxUInt(pDX, m_uiSI428XL, 1, 9);
	DDX_Text(pDX, IDC_EDIT_SI_SHOTPRO, m_uiSIShotPro);
	DDV_MinMaxUInt(pDX, m_uiSIShotPro, 1, 9);
	DDX_Text(pDX, IDC_EDIT_SEQ_428XL, m_uiSEQ428XL);
	DDV_MinMaxUInt(pDX, m_uiSEQ428XL, 01, 16);
	DDX_Text(pDX, IDC_EDIT_SEQ_SHOTPRO, m_uiSEQShotPro);
	DDV_MinMaxUInt(pDX, m_uiSEQShotPro, 01, 16);
	DDX_Text(pDX, IDC_EDIT_STA_SHOTPRO, m_uiSTAShotPro);
	DDV_MinMaxUInt(pDX, m_uiSTAShotPro, 0, 7);
	DDX_Text(pDX, IDC_EDIT_CTB_SHOTPRO, m_dCTBShotPro);
	DDV_MinMaxDouble(pDX, m_dCTBShotPro, 00.000, 33.000);
	DDX_Text(pDX, IDC_EDIT_UH_SHOTPRO, m_dUHShotPro);
	DDV_MinMaxDouble(pDX, m_dUHShotPro, 000.0, 999.5);
}

BEGIN_MESSAGE_MAP(CBLASTER_SHOTPRODlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND_428XL, &CBLASTER_SHOTPRODlg::OnBnClickedButtonSend428xl)
	ON_BN_CLICKED(IDC_BUTTON_SEND_SHOTPRO, &CBLASTER_SHOTPRODlg::OnBnClickedButtonSendShotpro)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBLASTER_SHOTPRODlg 消息处理程序

BOOL CBLASTER_SHOTPRODlg::OnInitDialog()
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

	BOOL bflag1 = FALSE; 
	bflag1 =  m_oRecSocket428XL.Create(RecPort428XL,SOCK_DGRAM);
	if (bflag1 == FALSE)
	{
		AfxMessageBox(_T("428XL端接收端口创建失败！"));
	}
	BOOL bflag2 = FALSE; 
	bflag2 =  m_oRecSocketShotPro.Create(RecPortShotPro,SOCK_DGRAM);
	if (bflag2 == FALSE)
	{
		AfxMessageBox(_T("爆炸机端接收端口创建失败！"));
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBLASTER_SHOTPRODlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBLASTER_SHOTPRODlg::OnPaint()
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
HCURSOR CBLASTER_SHOTPRODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBLASTER_SHOTPRODlg::OnBnClickedButtonSend428xl()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	MakeFrame428XL();
	m_oRecSocket428XL.SendTo(m_ucSendBuf428XL, 128, RecPortShotPro, IPAddressShotPro);
}

void CBLASTER_SHOTPRODlg::OnBnClickedButtonSendShotpro()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	MakeFrameShotPro();
	m_oRecSocketShotPro.SendTo(m_ucSendBufShotPro, 256, RecPort428XL, IPAddress428XL);
}

// 生成428XL发送帧
void CBLASTER_SHOTPRODlg::MakeFrame428XL(void)
{
	UpdateData(TRUE);
	unsigned int	uiIPSource;
	unsigned int	uiIPAim;
	unsigned short	usPortAim;
	unsigned short	usCommand;

	m_ucSendBuf428XL[0] = 0x11;
	m_ucSendBuf428XL[1] = 0x22;
	m_ucSendBuf428XL[2] = 0x33;
	m_ucSendBuf428XL[3] = 0x44;
	m_ucSendBuf428XL[4] = 0x00;
	m_ucSendBuf428XL[5] = 0x00;
	m_ucSendBuf428XL[6] = 0x00;
	m_ucSendBuf428XL[7] = 0x00;
	m_ucSendBuf428XL[8] = 0x00;
	m_ucSendBuf428XL[9] = 0x00;
	m_ucSendBuf428XL[10] = 0x00;
	m_ucSendBuf428XL[11] = 0x00;
	m_ucSendBuf428XL[12] = 0x00;
	m_ucSendBuf428XL[13] = 0x00;
	m_ucSendBuf428XL[14] = 0x00;
	m_ucSendBuf428XL[15] = 0x00;

	// 源IP地址
	uiIPSource	=	inet_addr(IPAddress428XL);
	// 目标IP地址
	uiIPAim		=	inet_addr(IPAddressShotPro);
	// 目标端口号
	usPortAim	=	RecPort428XL;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	usCommand	=	1;

	// 源IP地址
	memcpy(&m_ucSendBuf428XL[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_ucSendBuf428XL[20], &uiIPAim, 4);
	// 目标端口号
	memcpy(&m_ucSendBuf428XL[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_ucSendBuf428XL[26], &usCommand, 2);

	unsigned int uiLength = 0;
	CString strSend = "";
	CString strTemp = "";
	char* pbuf = NULL;

	strSend += "*SP#";
	strTemp.Format("%d", m_uiSP428XL);
	strSend += strTemp;

	strSend += "/SL#";
	strTemp.Format("%.1f", m_dSL428XL);
	strSend += strTemp;

	strSend += "/SN#";
	strTemp.Format("%.1f", m_dSN428XL);
	strSend += strTemp;

	strSend += "/SI#";
	strTemp.Format("%d", m_uiSI428XL);
	strSend += strTemp;

	strSend += "/SEQ#";
	strTemp.Format("%d", m_uiSEQ428XL);
	strSend += strTemp;

	GetDlgItem(IDC_EDIT_SENDSHOW_428XL)->SetWindowText(strSend);
	uiLength = strSend.GetLength();
	pbuf = (char*)strSend.GetBuffer(uiLength);
	for (unsigned int i=0; i<uiLength; i++)
	{
		m_ucSendBuf428XL[32+i] = pbuf[i];
	}
	m_ucSendBuf428XL[32+uiLength] = 0x0d;
	m_ucSendBuf428XL[33+uiLength] = 0x0a;
}

// 生成爆炸机端发送帧
void CBLASTER_SHOTPRODlg::MakeFrameShotPro(void)
{
	UpdateData(TRUE);
	unsigned int	uiIPSource;
	unsigned int	uiIPAim;
	unsigned short	usPortAim;
	unsigned short	usCommand;

	m_ucSendBufShotPro[0] = 0x11;
	m_ucSendBufShotPro[1] = 0x22;
	m_ucSendBufShotPro[2] = 0x33;
	m_ucSendBufShotPro[3] = 0x44;
	m_ucSendBufShotPro[4] = 0x00;
	m_ucSendBufShotPro[5] = 0x00;
	m_ucSendBufShotPro[6] = 0x00;
	m_ucSendBufShotPro[7] = 0x00;
	m_ucSendBufShotPro[8] = 0x00;
	m_ucSendBufShotPro[9] = 0x00;
	m_ucSendBufShotPro[10] = 0x00;
	m_ucSendBufShotPro[11] = 0x00;
	m_ucSendBufShotPro[12] = 0x00;
	m_ucSendBufShotPro[13] = 0x00;
	m_ucSendBufShotPro[14] = 0x00;
	m_ucSendBufShotPro[15] = 0x00;

	// 源IP地址
	uiIPSource	=	inet_addr(IPAddressShotPro);
	// 目标IP地址
	uiIPAim		=	inet_addr(IPAddress428XL);
	// 目标端口号
	usPortAim	=	RecPortShotPro;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	usCommand	=	1;

	// 源IP地址
	memcpy(&m_ucSendBufShotPro[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_ucSendBufShotPro[20], &uiIPAim, 4);
	// 目标端口号
	memcpy(&m_ucSendBufShotPro[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_ucSendBufShotPro[26], &usCommand, 2);

	unsigned int uiLength = 0;
	CString strSend = "";
	CString strTemp = "";
	char* pbuf = NULL;
	int iIntegerPart = 0;
	int iDecimalPart = 0;
	strSend += "*SHOTPRO SP#";
	strTemp.Format("%5d", m_uiSPShotPro);
	strSend += strTemp;

	strSend += "/SL#";
	iIntegerPart = (unsigned int)m_dSLShotPro;
	iDecimalPart = (unsigned int)((m_dSLShotPro - iIntegerPart)*10);
	strTemp.Format("%5d.%1d", iIntegerPart, iDecimalPart);
	strSend += strTemp;

	strSend += "/SN#";
	iIntegerPart = (unsigned int)m_dSNShotPro;
	iDecimalPart = (unsigned int)((m_dSNShotPro - iIntegerPart)*10);
	strTemp.Format("%5d.%1d", iIntegerPart, iDecimalPart);
	strSend += strTemp;

	strSend += "/SI#";
	strTemp.Format("%1d", m_uiSIShotPro);
	strSend += strTemp;

	strSend += "/SEQ#";
	strTemp.Format("%2d", m_uiSEQShotPro);
	strSend += strTemp;

	strSend += "/STA:";
	strTemp.Format("%1d", m_uiSTAShotPro);
	strSend += strTemp;

	strSend += "/CTB:";
	iIntegerPart = (unsigned int)m_dCTBShotPro;
	iDecimalPart = (unsigned int)((m_dCTBShotPro - iIntegerPart)*1000);
	strTemp.Format("%02d.%03d", iIntegerPart, iDecimalPart);
	strSend += strTemp;

	strSend += "/UH:";
	iIntegerPart = (unsigned int)m_dUHShotPro;
	iDecimalPart = (unsigned int)((m_dUHShotPro - iIntegerPart)*10);
	strTemp.Format("%03d.%1d", iIntegerPart,iDecimalPart);
	strSend += strTemp;

	GetDlgItem(IDC_EDIT_SENDSHOW_SHOTPRO)->SetWindowText(strSend);
	uiLength = strSend.GetLength();
	pbuf = (char*)strSend.GetBuffer(uiLength);
	for (unsigned int i=0; i<uiLength; i++)
	{
		m_ucSendBufShotPro[28+i] = pbuf[i];
	}
	m_ucSendBufShotPro[28+uiLength] = 0x0d;
	m_ucSendBufShotPro[29+uiLength] = 0x0a;
}

void CBLASTER_SHOTPRODlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	KillTimer(1);
	OnBnClickedButtonSendShotpro();
	CDialog::OnTimer(nIDEvent);
}
