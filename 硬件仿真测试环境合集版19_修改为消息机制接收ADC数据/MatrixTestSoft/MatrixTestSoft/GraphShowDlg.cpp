// GraphShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "GraphShowDlg.h"
#include  <io.h>

// CGraphShowDlg 对话框

IMPLEMENT_DYNAMIC(CGraphShowDlg, CDialog)

CGraphShowDlg::CGraphShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphShowDlg::IDD, pParent)
	, m_uiADCGraphNb(0)
	, m_uiScrollBarMax(0)
	, m_uiADCFileLength(0)
	, m_bStopSample(FALSE)
	, m_pSampleData(NULL)
	, m_uiSampleDataNum(0)
	, m_pADCDataSaveToFileThread(NULL)
	, m_pADCDataProcessThread(NULL)
{

}

CGraphShowDlg::~CGraphShowDlg()
{
	
}

void CGraphShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR_GRAPHVIEW, m_ctrlScrollBar);
}


BEGIN_MESSAGE_MAP(CGraphShowDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CGraphShowDlg 消息处理程序

BOOL CGraphShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_uiADCFileLength > 0)
	{
		m_pSampleData = new double[m_uiADCFileLength];
	}

	// 创建绘图控件并设置相关参数
 	CreateGraph(IDC_STATIC_GRAPHSHOW, &m_OScopeCtrl);
	// customize the control
	SetRange(-2.5, 2.5, DecimalPlaces, &m_OScopeCtrl) ;
 	SetYUnits("Y", &m_OScopeCtrl) ;
 	SetXUnits("X", &m_OScopeCtrl) ;
 	SetBackgroundColor(RGB(0, 0, 64), &m_OScopeCtrl) ;
 	SetGridColor(RGB(192, 192, 255), &m_OScopeCtrl) ;
 	SetPlotColor(RGB(255, 255, 255), &m_OScopeCtrl) ;
	unsigned int uiLength = 0;
	m_Sec.Lock();
//	uiLength = m_pADCDataProcessThread->m_uiADCDataFrameProcNum[m_uiADCGraphNb - 1] * ReceiveDataSize;
	uiLength = m_pADCDataSaveToFileThread->m_uiADCFrameSaved[m_uiADCGraphNb - 1] * ReceiveDataSize;
	m_Sec.Unlock();
	if (uiLength > m_OScopeCtrl.m_uiXAxisPointNum)
	{
		m_OScopeCtrl.m_uiXAxisLabelMax = uiLength;
	}
	else
	{
		m_OScopeCtrl.m_uiXAxisLabelMax = m_OScopeCtrl.m_uiXAxisPointNum;
	}
	m_OScopeCtrl.m_uiXAxisLabelMin = m_OScopeCtrl.m_uiXAxisLabelMax - m_OScopeCtrl.m_uiXAxisPointNum;
	m_OScopeCtrl.Reset(false);
	m_OScopeCtrl.m_dPreviousPosition = 0.0;
	m_OScopeCtrl.m_uiPointNum = uiLength;	// 绘制了的点的个数

	OnSetHScrollBarRange();

	m_Sec.Lock();
	m_pADCDataProcessThread->m_uiADCGraphIP = 71 + 10 * m_uiADCGraphNb;
	m_Sec.Unlock();
	m_OScopeCtrl.m_bStopSample = m_bStopSample;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// 创建图形显示模板
void CGraphShowDlg::CreateGraph(int iGraphViewId, COScopeCtrl* oScopeCtrl)
{
	CRect rect;
	
	GetDlgItem(iGraphViewId)->GetWindowRect(rect) ;
	ScreenToClient(rect) ;
	// create the control
	oScopeCtrl->Create(WS_VISIBLE | WS_CHILD, rect, this) ; 
}
// 设置采样取值范围
void CGraphShowDlg::SetRange(double dLower, double dUpper, int nDecimalPlaces, COScopeCtrl* oScopeCtrl)
{
	oScopeCtrl->SetRange(dLower, dUpper, nDecimalPlaces) ;	
}

// 设置Y轴标签
void CGraphShowDlg::SetYUnits(CString str, COScopeCtrl* oScopeCtrl)
{
	oScopeCtrl->SetYUnits(str);
}

// 设置X轴标签
void CGraphShowDlg::SetXUnits(CString str, COScopeCtrl* oScopeCtrl)
{
	oScopeCtrl->SetXUnits(str);
}

// 设置背景颜色
void CGraphShowDlg::SetBackgroundColor(COLORREF color, COScopeCtrl* oScopeCtrl)
{
	oScopeCtrl->SetBackgroundColor(color);
}

// 设置网格颜色
void CGraphShowDlg::SetGridColor(COLORREF color, COScopeCtrl* oScopeCtrl)
{
	oScopeCtrl->SetGridColor(color);
}

// 设置画点颜色
void CGraphShowDlg::SetPlotColor(COLORREF color, COScopeCtrl* oScopeCtrl)
{
	oScopeCtrl->SetPlotColor(color);
}

void CGraphShowDlg::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ

	m_Sec.Lock();
	m_pADCDataProcessThread->m_uiADCGraphIP = 0;
	m_Sec.Unlock();
	m_uiADCGraphNb = 0;
	if(m_pSampleData != NULL)
	{
		m_pSampleData = NULL;
		delete m_pSampleData;
	}
	CDialog::OnClose();
}
// 滑动条消息响应函数，实现图形界面滚动条效果，nPos只能达到32k
void CGraphShowDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
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
// 读ADC数据文件
void CGraphShowDlg::OnReadADCDataFile(CString csPathName, unsigned int uiPos, unsigned int uiPosNum)
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
			ProcessMessages();
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
						ProcessMessages();
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
						if (m_uiADCGraphNb-1 == j)
						{
							db = atof(cstmp);
							m_pSampleData[m_uiSampleDataNum] = db;
							m_uiSampleDataNum++;
						}
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
// 滚动条拖动到坐标点读取文件绘图
void CGraphShowDlg::OnMoveScrollBarToPos(unsigned int uiPos)
{
	CString strtmp = "";
	CString csPathName = "";
	unsigned int uiReadADCFileNb = 0;
	unsigned int uiDrawDataNum = 0;
	double dDrawData;

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
	m_Sec.Lock();
	csPathName = m_pADCDataSaveToFileThread->m_csSaveFilePath + strtmp;
	m_Sec.Unlock();
	if (uiPos == 0)
	{
		OnReadADCDataFile(csPathName, 0, m_OScopeCtrl.m_uiXAxisPointNum);
	}
	else
	{
		if ((uiPos + m_OScopeCtrl.m_uiXAxisPointNum) > uiReadADCFileNb*m_uiADCFileLength)
		{
			OnReadADCDataFile(csPathName, uiPos - (uiReadADCFileNb - 1)*m_uiADCFileLength - 1, (uiReadADCFileNb*m_uiADCFileLength - uiPos + 1));
			strtmp.Format("\\%d.text", uiReadADCFileNb + 1);
			m_Sec.Lock();
			csPathName = m_pADCDataSaveToFileThread->m_csSaveFilePath + strtmp;
			m_Sec.Unlock();
			OnReadADCDataFile(csPathName, 0, m_OScopeCtrl.m_uiXAxisPointNum + 1 - (uiReadADCFileNb*m_uiADCFileLength - uiPos + 1));
		}
		else
		{
			OnReadADCDataFile(csPathName, uiPos - (uiReadADCFileNb - 1)*m_uiADCFileLength - 1, m_OScopeCtrl.m_uiXAxisPointNum + 1);
		}
	}

	// 绘图
	uiDrawDataNum = m_uiSampleDataNum;
	if (uiDrawDataNum == 0)
	{
		m_OScopeCtrl.m_uiXAxisLabelMin = 0;
	}
	// 初始化绘图界面
	m_OScopeCtrl.Reset(true);
	// 绘图
	if (uiDrawDataNum > 0)
	{
		m_OScopeCtrl.m_uiXAxisLabelMin = uiPos;
		m_OScopeCtrl.m_uiXAxisLabelMax = uiPos + m_OScopeCtrl.m_uiXAxisPointNum;
		m_OScopeCtrl.m_uiPointNum = 0;
		if (uiPos == 0)
		{
			m_OScopeCtrl.m_dPreviousPosition = 0.0;
		}
		else 
		{
			m_OScopeCtrl.m_dPreviousPosition = m_pSampleData[0];
		}

		for (unsigned int j=1; j<uiDrawDataNum; j++)
		{
			ProcessMessages();
			dDrawData = m_pSampleData[j];
			m_OScopeCtrl.DrawPoint(dDrawData);
		}
		m_uiSampleDataNum = 0;
	}

}
// 设置横向滚动条范围
void CGraphShowDlg::OnSetHScrollBarRange(void)
{
	unsigned int uiLength = 0;
	m_Sec.Lock();
//	uiLength = m_pADCDataProcessThread->m_uiADCDataFrameProcNum[m_uiADCGraphNb - 1] * ReceiveDataSize;
	uiLength = m_pADCDataSaveToFileThread->m_uiADCFrameSaved[m_uiADCGraphNb - 1] * ReceiveDataSize;
	m_Sec.Unlock();
	if (uiLength > m_OScopeCtrl.m_uiXAxisPointNum)
	{
		m_uiScrollBarMax = uiLength - m_OScopeCtrl.m_uiXAxisPointNum; 
	}
	else
	{
		m_uiScrollBarMax = uiLength;
	}
	CScrollBar*pSB =(CScrollBar*)GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW); 
	pSB->SetScrollRange(0, m_uiScrollBarMax); 
	pSB->SetScrollPos(m_uiScrollBarMax, TRUE);
	if (m_bStopSample == TRUE)
	{
		OnMoveScrollBarToPos(m_uiScrollBarMax);
	}
	// 向滚动条发送消息
//	SendMessage(WM_HSCROLL,MAKELONG(SB_THUMBPOSITION, m_uiScrollBarMax),(LPARAM)GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW)->GetSafeHwnd());
}
// 防止程序在循环中运行无法响应消息
void CGraphShowDlg::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
