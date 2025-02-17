// GPSAnalysisDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GPSAnalysis.h"
#include "GPSAnalysisDlg.h"

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


// CGPSAnalysisDlg 对话框




CGPSAnalysisDlg::CGPSAnalysisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGPSAnalysisDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CGPSAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FRAMESELECT, m_ctrlcomboFrameSelect);
	DDX_Control(pDX, IDC_EDIT_SENDFRAME, m_editSendFrame);
}

BEGIN_MESSAGE_MAP(CGPSAnalysisDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_FRAMESELECT, &CGPSAnalysisDlg::OnCbnSelchangeComboFrameselect)
	ON_BN_CLICKED(IDC_BUTTON_SENDGPSFRAME, &CGPSAnalysisDlg::OnBnClickedButtonSendgpsframe)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGPSAnalysisDlg 消息处理程序

BOOL CGPSAnalysisDlg::OnInitDialog()
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
	m_ctrlcomboFrameSelect.SetWindowText(_T("$GPRMC"));
	m_ctrlcomboFrameSelect.InsertString(0, _T("$GPRMC"));
	m_ctrlcomboFrameSelect.InsertString(1, _T("$GPGLL"));
	m_editSendFrame.SetWindowText(_T("$GPRMC,074529.82,A,2429.6717,N,11804.6973,E,12.623,32.122,010806,,W,A*08"));
	m_RecThread.Init();
	m_RecThread.CreateThread();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGPSAnalysisDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGPSAnalysisDlg::OnPaint()
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
HCURSOR CGPSAnalysisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGPSAnalysisDlg::OnCbnSelchangeComboFrameselect()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	int iPos = m_ctrlcomboFrameSelect.GetCurSel();	// 当前选中的行

	if (0 == iPos)
	{
		m_editSendFrame.SetWindowText(_T("$GPRMC,074529.82,A,2429.6717,N,11804.6973,E,12.623,32.122,010806,,W,A*08"));
	}
	else if (1 == iPos)
	{
		m_editSendFrame.SetWindowText(_T("$GPGLL,4250.5589,S,14718.5084,E,092204.999,A*2D"));
	}
	else
	{	
		m_editSendFrame.SetWindowText(_T(""));
	}

}

void CGPSAnalysisDlg::OnBnClickedButtonSendgpsframe()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CString strSend = _T("");
	m_editSendFrame.GetWindowText(strSend);
	int iLength = 0;
	iLength = strSend.GetLength();
	if (iLength == 0)
	{
		return;
	}
	char* pBuf=(char*)strSend.GetBuffer(iLength); 
	MakeSendFrame(pBuf, iLength);
	m_RecThread.m_Socket.SendTo(m_pSendFrameData, 256, RecGPSPort, IPSource);
}

// 生成帧头
void CGPSAnalysisDlg::MakeSendFrame(char* buf, int ilength)
{
	unsigned int	uiIPSource;
	unsigned int	uiIPAim;
	unsigned short	usPortAim;
	unsigned short	usCommand;

	m_pSendFrameData[0] = 0x11;
	m_pSendFrameData[1] = 0x22;
	m_pSendFrameData[2] = 0x33;
	m_pSendFrameData[3] = 0x44;
	m_pSendFrameData[4] = 0x00;
	m_pSendFrameData[5] = 0x00;
	m_pSendFrameData[6] = 0x00;
	m_pSendFrameData[7] = 0x00;
	m_pSendFrameData[8] = 0x00;
	m_pSendFrameData[9] = 0x00;
	m_pSendFrameData[10] = 0x00;
	m_pSendFrameData[11] = 0x00;
	m_pSendFrameData[12] = 0x00;
	m_pSendFrameData[13] = 0x00;
	m_pSendFrameData[14] = 0x00;
	m_pSendFrameData[15] = 0x00;

	// 源IP地址
	uiIPSource	=	inet_addr(IPSource);
	// 目标IP地址
	uiIPAim		=	0xFFFFFFFF;
	// 目标端口号
	usPortAim	=	RecGPSPort;
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	usCommand	=	1;

	// 源IP地址
	memcpy(&m_pSendFrameData[16], &uiIPSource, 4);
	// 目标IP地址
	memcpy(&m_pSendFrameData[20], &uiIPAim, 4);
	// 目标端口号
	memcpy(&m_pSendFrameData[24], &usPortAim, 2);
	// 命令号 1-设置命令应答；2-查询命令应答；3-AD采样数据重发
	memcpy(&m_pSendFrameData[26], &usCommand, 2);
	
	for (int i=0; i<ilength; i++)
	{
		m_pSendFrameData[28+i] = buf[i];
	}
	m_pSendFrameData[28+ilength] = 0x0d;
	m_pSendFrameData[29+ilength] = 0x0a;
}

void CGPSAnalysisDlg::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	m_RecThread.OnClose();
	
	int iResult = 0;
	iResult = ::WaitForSingleObject(m_RecThread.m_hRecThreadClose,500);
	if (iResult != WAIT_OBJECT_0)
	{
		AfxMessageBox(_T("GPS接收线程在100ms内未能正常结束！"));
	}
	
	CDialog::OnClose();
}
