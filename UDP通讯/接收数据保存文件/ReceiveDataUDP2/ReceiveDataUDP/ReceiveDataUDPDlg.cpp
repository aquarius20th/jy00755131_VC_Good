// ReceiveDataUDPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ReceiveDataUDP.h"
#include "ReceiveDataUDPDlg.h"

CReceiveDataUDPDlg* mwnd;
int iCount = 0;
extern DWORD WINAPI ReceiveData(HWND hWnd);


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


// CReceiveDataUDPDlg 对话框




CReceiveDataUDPDlg::CReceiveDataUDPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReceiveDataUDPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReceiveDataUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CReceiveDataUDPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_RECEIVE, &CReceiveDataUDPDlg::OnBnClickedButtonReceive)
END_MESSAGE_MAP()


// CReceiveDataUDPDlg 消息处理程序

BOOL CReceiveDataUDPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mwnd = this;

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

	m_Receive.OnInit();

	HANDLE hComThread = NULL;	
	DWORD threadID;							// 启动USB口监视线程
	if (NULL != hComThread)
	{
		CloseHandle(hComThread);
	}
	hComThread = ::CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,
		(LPTHREAD_START_ROUTINE)ReceiveData,AfxGetMainWnd()->m_hWnd, 0, &threadID);//hDevice  
	if (hComThread == NULL)
	{
		::AfxMessageBox(_T("创建接收线程失败!"));
		::PostQuitMessage(0);
	}


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CReceiveDataUDPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CReceiveDataUDPDlg::OnPaint()
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
HCURSOR CReceiveDataUDPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CReceiveDataUDPDlg::OnBnClickedButtonReceive()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

}
DWORD WINAPI ReceiveData(HWND hWnd)
{
	unsigned char		pBuf[256];
	WORD				wFrmLen = 256;		// 帧长: 初始化为缓冲区的大小，被ReceiveFrom函数使用
	int				nSize;
	sockaddr_in sockClient;
	sockClient.sin_family = AF_INET;
	nSize = sizeof(sockClient);
	int iFrameCount = 0;
	int iSurplusCount = 0;

	// 接收来自客户端的数据

	while( TRUE )
	{
		iCount = mwnd->m_Receive.GetFrameCount();
		if (iCount > 0)
		{
			iFrameCount = iCount/256;
			if (iFrameCount>0)
			{
				for( int i=0; i<iFrameCount; i++ )
				{
					mwnd->m_Receive.Receive(pBuf, 256,0);
					for(int i=0; i<256; i++)
					{
						mwnd->m_Receive.m_ReceiveData.push_back(pBuf[i]);
					}
				}	
			}
			iSurplusCount = iCount - iFrameCount*256;
			if (iSurplusCount > 0)
			{
				mwnd->m_Receive.Receive(pBuf, iSurplusCount, 0);
				for (int i=0; i<iSurplusCount; i++)
				{
					mwnd->m_Receive.m_ReceiveData.push_back(pBuf[i]);
				}
			}
		}
		if ((iCount == 0)&&((iFrameCount != 0)||(iSurplusCount != 0)))
		{
			iFrameCount = 0;
			iSurplusCount = 0;
			mwnd->m_Receive.SaveReceiveDataToFile(mwnd->m_Receive.m_ReceiveData);
			mwnd->m_Receive.m_ReceiveData.clear();
		}
	}

	return 1;
}

