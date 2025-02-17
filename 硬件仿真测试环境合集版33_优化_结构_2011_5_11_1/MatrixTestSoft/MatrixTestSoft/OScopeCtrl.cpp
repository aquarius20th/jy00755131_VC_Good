// OScopeCtrl.cpp : implementation file//

#include "stdafx.h"
#include "math.h"

#include "OScopeCtrl.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__ ;
#endif

/////////////////////////////////////////////////////////////////////////////
// COScopeCtrl
COScopeCtrl::COScopeCtrl()
: m_bLButtonDown(FALSE)
, m_bMouseDrag(FALSE)
, m_bStopSample(FALSE)
{
	// since plotting is based on a LineTo for each new point
	// we need a starting point (i.e. a "previous" point)
	// use 0.0 as the default first point.
	// these are public member variables, and can be changed outside
	// (after construction).  Therefore m_perviousPosition could be set to
	// a more appropriate value prior to the first call to SetPosition.
	m_dPreviousPosition =   0.0 ;

	// public variable for the number of decimal places on the y axis
	m_nYDecimals = 3 ;

	// set some initial values for the scaling until "SetRange" is called.
	// these are protected varaibles and must be set with SetRange
	// in order to ensure that m_dRange is updated accordingly
	m_dLowerLimit = -10.0 ;
	m_dUpperLimit =  10.0 ;
	m_dRange      =  m_dUpperLimit - m_dLowerLimit ;   // protected member variable

	m_uiXAxisLabelMin = 0;		// X轴坐标的最小值初值为0
	m_uiXAxisLabelMax = 200;	// X轴坐标的最大值初值为200
	m_uiPointNum = 0;			// 绘制了的点的个数初值为0
	m_uiXAxisPointNum = 200;	// X轴可同时容纳点的个数
	m_pParentWnd = NULL;
	m_iMousepointX = 0;
	m_dMousepointY = 0.0;		

	// m_nShiftPixels determines how much the plot shifts (in terms of pixels) 
	// with the addition of a new data point
	m_nShiftPixels     = 2 ;
	m_nHalfShiftPixels = m_nShiftPixels/2 ;                     // protected
	m_nPlotShiftPixels = m_nShiftPixels + m_nHalfShiftPixels ;  // protected

	// background, grid and data colors
	// these are public variables and can be set directly
	m_crBackColor  = RGB(  0,   0,   0) ;  // see also SetBackgroundColor
	m_crGridColor  = RGB(  0, 255, 255) ;  // see also SetGridColor
	m_crPlotColor  = RGB(255, 255, 255) ;  // see also SetPlotColor

	// protected variables
	m_penPlot.CreatePen(PS_SOLID, 0, m_crPlotColor) ;
	m_brushBack.CreateSolidBrush(m_crBackColor) ;

	// public member variables, can be set directly 
	m_strXUnitsString.Format(_T("Times")) ;  // can also be set with SetXUnits
	m_strYUnitsString.Format(_T("Sample")) ;  // can also be set with SetYUnits

	// protected bitmaps to restore the memory DC's
	m_pbitmapOldGrid = NULL ;
	m_pbitmapOldPlot = NULL ;

}  // COScopeCtrl

/////////////////////////////////////////////////////////////////////////////
COScopeCtrl::~COScopeCtrl()
{
	// just to be picky restore the bitmaps for the two memory dc's
	// (these dc's are being destroyed so there shouldn't be any leaks)
	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid) ;  
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot) ;  

} // ~COScopeCtrl


BEGIN_MESSAGE_MAP(COScopeCtrl, CWnd)
	//{{AFX_MSG_MAP(COScopeCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COScopeCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL COScopeCtrl::Create(DWORD dwStyle, const RECT& rect, 
						 CWnd* pParentWnd, UINT nID) 
{
	BOOL result ;
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW) ;

	m_pParentWnd = pParentWnd;
	result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 
		className, NULL, dwStyle, 
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID) ;
	if (result != 0)
		InvalidateCtrl(true) ;

	return result ;

} // Create

/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::SetRange(double dLower, double dUpper, int nDecimalPlaces)
{
	ASSERT(dUpper > dLower) ;

	m_dLowerLimit     = dLower ;
	m_dUpperLimit     = dUpper ;
	m_nYDecimals      = nDecimalPlaces ;
	m_dRange          = m_dUpperLimit - m_dLowerLimit ;
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange ; 

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl(false) ;
	if (m_bStopSample == TRUE)
	{
		unsigned int uiPos = 0;
		uiPos = m_pParentWnd->GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW)->GetScrollPos(SB_HORZ);
		// 向滚动条发送消息
		m_pParentWnd->SendMessage(WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, uiPos), 
			(LPARAM)m_pParentWnd->GetDlgItem(IDC_SCROLLBAR_GRAPHVIEW)->GetSafeHwnd());
	}

}  // SetRange


/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::SetXUnits(CString string)
{
	m_strXUnitsString = string ;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl(true) ;

}  // SetXUnits

/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::SetYUnits(CString string)
{
	m_strYUnitsString = string ;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl(true) ;

}  // SetYUnits

/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::SetGridColor(COLORREF color)
{
	m_crGridColor = color ;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl(true) ;

}  // SetGridColor


/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::SetPlotColor(COLORREF color)
{
	m_crPlotColor = color ;

	m_penPlot.DeleteObject() ;
	m_penPlot.CreatePen(PS_SOLID, 0, m_crPlotColor) ;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl(true) ;

}  // SetPlotColor


/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::SetBackgroundColor(COLORREF color)
{
	m_crBackColor = color ;

	m_brushBack.DeleteObject() ;
	m_brushBack.CreateSolidBrush(m_crBackColor) ;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl(true) ;

}  // SetBackgroundColor

/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::InvalidateCtrl(bool bXMaxLableDefault)
{
	// There is a lot of drawing going on here - particularly in terms of 
	// drawing the grid.  Don't panic, this is all being drawn (only once)
	// to a bitmap.  The result is then BitBlt'd to the control whenever needed.
	int i ;
	int nCharacters ;
	int nTopGridPix, nMidGridPix, nBottomGridPix ;

	CPen *oldPen ;
	CPen solidPen(PS_SOLID, 0, m_crGridColor) ;
	CFont axisFont, yUnitFont, *oldFont ;
	CString strTemp ;

	// in case we haven't established the memory dc's
	CClientDC dc(this) ;  

	// if we don't have one yet, set up a memory dc for the grid
	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc) ;
		m_bitmapGrid.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid) ;
	}

	m_dcGrid.SetBkColor (m_crBackColor) ;

	// fill the grid background
	m_dcGrid.FillRect(m_rectClient, &m_brushBack) ;

	// draw the plot rectangle:
	// determine how wide the y axis scaling values are
	nCharacters = abs((int)log10(fabs(m_dUpperLimit))) ;
	nCharacters = max(nCharacters, abs((int)log10(fabs(m_dLowerLimit)))) ;

	// add the units digit, decimal point and a minus sign, and an extra space
	// as well as the number of decimal places to display
	nCharacters = nCharacters + 4 + m_nYDecimals ;  

	// adjust the plot rectangle dimensions
	// assume 6 pixels per character (this may need to be adjusted)
	m_rectPlot.left = m_rectClient.left + 6*(nCharacters) ;
	m_nPlotWidth    = m_rectPlot.Width() ;

	// draw the plot rectangle
	oldPen = m_dcGrid.SelectObject (&solidPen) ; 
	m_dcGrid.MoveTo (m_rectPlot.left, m_rectPlot.top) ;
	m_dcGrid.LineTo (m_rectPlot.right, m_rectPlot.top) ;
	m_dcGrid.LineTo (m_rectPlot.right, m_rectPlot.bottom) ;
	m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.bottom) ;
	m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.top) ;
	m_dcGrid.SelectObject (oldPen) ; 

	// draw the dotted lines, 
	// use SetPixel instead of a dotted pen - this allows for a 
	// finer dotted line and a more "technical" look
	nMidGridPix    = (m_rectPlot.top + m_rectPlot.bottom)/2 ;
	nTopGridPix    = nMidGridPix - m_nPlotHeight/4 ;
	nBottomGridPix = nMidGridPix + m_nPlotHeight/4 ;

	for (i=m_rectPlot.left; i<m_rectPlot.right; i+=4)
	{
		ProcessMessages();
		m_dcGrid.SetPixel (i, nTopGridPix,    m_crGridColor) ;
		m_dcGrid.SetPixel (i, nMidGridPix,    m_crGridColor) ;
		m_dcGrid.SetPixel (i, nBottomGridPix, m_crGridColor) ;
	}

	// create some fonts (horizontal and vertical)
	// use a height of 14 pixels and 300 weight 
	// (these may need to be adjusted depending on the display)
	axisFont.CreateFont (14, 0, 0, 0, 300,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, "Arial") ;
	yUnitFont.CreateFont (14, 0, 900, 0, 300,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, "Arial") ;

	// grab the horizontal font
	oldFont = m_dcGrid.SelectObject(&axisFont) ;

	// y max
	m_dcGrid.SetTextColor (m_crGridColor) ;
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP) ;
	strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dUpperLimit) ;
	m_dcGrid.TextOut (m_rectPlot.left-4, m_rectPlot.top, strTemp) ;

	// y min
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_BASELINE) ;
	strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dLowerLimit) ;
	m_dcGrid.TextOut (m_rectPlot.left-4, m_rectPlot.bottom, strTemp) ;

	// x min
	m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP) ;
	strTemp.Format (_T("%d"), m_uiXAxisLabelMin) ; ;
	m_dcGrid.TextOut (m_rectPlot.left, m_rectPlot.bottom+4, strTemp) ;

	// x max
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP) ;
	if (bXMaxLableDefault == true)
	{
		m_uiXAxisLabelMax = m_nPlotWidth/m_nShiftPixels;
	}
	m_uiXAxisPointNum = m_nPlotWidth/m_nShiftPixels;
	strTemp.Format (_T("%d"), m_uiXAxisLabelMax) ; 
	m_dcGrid.TextOut (m_rectPlot.right, m_rectPlot.bottom+4, strTemp) ;

	// x units
	m_dcGrid.SetTextAlign (TA_CENTER|TA_TOP) ;
	m_dcGrid.TextOut ((m_rectPlot.left+m_rectPlot.right)/2, 
		m_rectPlot.bottom+4, m_strXUnitsString) ;

	// restore the font
	m_dcGrid.SelectObject(oldFont) ;

	// y units
	oldFont = m_dcGrid.SelectObject(&yUnitFont) ;
	m_dcGrid.SetTextAlign (TA_CENTER|TA_BASELINE) ;
	m_dcGrid.TextOut ((m_rectClient.left+m_rectPlot.left)/2, 
		(m_rectPlot.bottom+m_rectPlot.top)/2, m_strYUnitsString) ;
	m_dcGrid.SelectObject(oldFont) ;

	// at this point we are done filling the the grid bitmap, 
	// no more drawing to this bitmap is needed until the setting are changed

	// if we don't have one yet, set up a memory dc for the plot
	if (m_dcPlot.GetSafeHdc() == NULL)
	{
		m_dcPlot.CreateCompatibleDC(&dc) ;
		m_bitmapPlot.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot) ;
	}

	// make sure the plot bitmap is cleared
	m_dcPlot.SetBkColor (m_crBackColor) ;
	m_dcPlot.FillRect(m_rectClient, &m_brushBack) ;

	// finally, force the plot area to redraw
	InvalidateRect(m_rectClient) ;

} // InvalidateCtrl


/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::AppendPoint(double* dpNewPoint, UINT uiNewPointSize)
{
	// append a data point to the plot
	// return the previous point
	CRect rectCleanUp ;
	UINT uiPicturemove;
	if ((m_uiPointNum + uiNewPointSize) > m_uiXAxisLabelMax)
	{
		if (m_uiPointNum <= m_uiXAxisLabelMax)
		{

			uiPicturemove = m_uiXAxisLabelMax - m_uiPointNum;
			for (UINT i=0; i<uiPicturemove; i++)
			{
			//	ProcessMessages();
				DrawPoint(dpNewPoint[i], 0) ;
			}
			m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top, 
				m_nPlotWidth, m_nPlotHeight, &m_dcPlot, 
				m_rectPlot.left+m_nShiftPixels*(uiNewPointSize - uiPicturemove), m_rectPlot.top, 
				SRCCOPY) ;

			// establish a rectangle over the right side of plot
			// which now needs to be cleaned up proir to adding the new point
			rectCleanUp = m_rectPlot ;
			rectCleanUp.left  = rectCleanUp.right - m_nShiftPixels*(uiNewPointSize - uiPicturemove) ;
			// fill the cleanup area with the background
			m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;
			for (UINT j=uiPicturemove; j<uiNewPointSize; j++)
			{
			//	ProcessMessages();
				DrawPoint(dpNewPoint[j], (uiNewPointSize - j)) ;
			}
		} 
		else
		{
			m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top, 
				m_nPlotWidth, m_nPlotHeight, &m_dcPlot, 
				m_rectPlot.left+m_nShiftPixels*uiNewPointSize, m_rectPlot.top, 
				SRCCOPY) ;

			// establish a rectangle over the right side of plot
			// which now needs to be cleaned up proir to adding the new point
			rectCleanUp = m_rectPlot ;
			rectCleanUp.left  = rectCleanUp.right - m_nShiftPixels*uiNewPointSize ;
			// fill the cleanup area with the background
			m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;
			for (UINT i=0; i<uiNewPointSize; i++)
			{
			//	ProcessMessages();
				DrawPoint(dpNewPoint[i], (uiNewPointSize - i)) ;
			}
		}
	}
	else
	{
		for (UINT i=0; i<uiNewPointSize;i++)
		{
		//	ProcessMessages();
			DrawPoint(dpNewPoint[i], 0) ;
		}
	}
	
	InvalidateRect(m_rectClient) ;


} // AppendPoint

////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::OnPaint() 
{
	CPaintDC dc(this) ;  // device context for painting
	CDC memDC ;
	CBitmap memBitmap ;
	CBitmap* oldBitmap ; // bitmap originally found in CMemDC

	// no real plotting work is performed here, 
	// just putting the existing bitmaps on the client

	// to avoid flicker, establish a memory dc, draw to it 
	// and then BitBlt it to the client
	memDC.CreateCompatibleDC(&dc) ;
	memBitmap.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the grid on the memory dc
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
			&m_dcGrid, 0, 0, SRCCOPY) ;
		// now add the plot on top as a "pattern" via SRCPAINT.
		// works well with dark background and a light plot
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
			&m_dcPlot, 0, 0, SRCPAINT) ;  //SRCPAINT
		// 绘制鼠标拖动放大矩形框
		if(m_bMouseDrag == 1)
		{
			OnDrawMouseDragRect(&memDC, m_pointStart, m_pointMouseDragCur);
		}
		// finally send the result to the display
		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
			&memDC, 0, 0, SRCCOPY) ;
	}

	memDC.SelectObject(oldBitmap) ;

} // OnPaint

/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::DrawPoint(double dNewPoint, UINT uiDrawPointMove)
{
	// this does the work of "scrolling" the plot to the left
	// and appending a new data point all of the plotting is 
	// directed to the memory based bitmap associated with m_dcPlot
	// the will subsequently be BitBlt'd to the client in OnPaint
	
	m_dCurrentPosition = dNewPoint ;

	m_uiPointNum++;
	int currX, prevX, currY, prevY ;

	CPen *oldPen ;
	CRect rectCleanUp ;

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		// shift the plot by BitBlt'ing it to itself 
		// note: the m_dcPlot covers the entire client
		//       but we only shift bitmap that is the size 
		//       of the plot rectangle
		// grab the right side of the plot (exluding m_nShiftPixels on the left)
		// move this grabbed bitmap to the left by m_nShiftPixels
		if (m_uiPointNum > m_uiXAxisLabelMax)
		{
			// 动态显示X轴标签
			CString strTemp ;
			CFont axisFont, *oldFont ;
			axisFont.CreateFont (14, 0, 0, 0, 300,
				FALSE, FALSE, 0, ANSI_CHARSET,
				OUT_DEFAULT_PRECIS, 
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, 
				DEFAULT_PITCH|FF_SWISS, "Arial") ;

			// grab the horizontal font
			oldFont = m_dcGrid.SelectObject(&axisFont) ;
			// x min
			m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP) ;
			m_uiXAxisLabelMin = m_uiXAxisLabelMin+m_uiPointNum-m_uiXAxisLabelMax;
			strTemp.Format (_T("%d"), m_uiXAxisLabelMin) ; 
			m_dcGrid.TextOut (m_rectPlot.left, m_rectPlot.bottom+4, strTemp) ;

			// x max
			m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP) ;
			m_uiXAxisLabelMax = m_uiPointNum;
			strTemp.Format (_T("%d"), m_uiXAxisLabelMax) ; 
			m_dcGrid.TextOut (m_rectPlot.right, m_rectPlot.bottom+4, strTemp) ;

			// restore the font
			m_dcGrid.SelectObject(oldFont) ;


			
			// grab the plotting pen
			oldPen = m_dcPlot.SelectObject(&m_penPlot) ;

			// move to the previous point
			prevX = m_rectPlot.right - m_nPlotShiftPixels - (uiDrawPointMove - 1)* m_nShiftPixels ;
			prevY = m_rectPlot.bottom - 
				(long)((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor) ;
			m_dcPlot.MoveTo (prevX, prevY) ;

			// draw to the current point
			currX = m_rectPlot.right - m_nHalfShiftPixels - (uiDrawPointMove - 1)* m_nShiftPixels ;
			currY = m_rectPlot.bottom -
				(long)((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor) ;
			m_dcPlot.LineTo (currX, currY) ;

			// restore the pen 
			m_dcPlot.SelectObject(oldPen) ;

		}
		else
		{
			// draw the next line segement
			// grab the plotting pen
			oldPen = m_dcPlot.SelectObject(&m_penPlot) ;

			// move to the previous point
			prevX = m_rectPlot.left+m_nHalfShiftPixels+(m_uiPointNum - 1)* m_nShiftPixels;
			prevY = m_rectPlot.bottom - 
				(long)((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor) ;
			m_dcPlot.MoveTo (prevX, prevY) ;

			// draw to the current point
			currX = m_rectPlot.left+m_nPlotShiftPixels+(m_uiPointNum - 1)* m_nShiftPixels ;
			currY = m_rectPlot.bottom -
				(long)((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor) ;
			m_dcPlot.LineTo (currX, currY) ;

			// restore the pen 
			m_dcPlot.SelectObject(oldPen) ;
		}
		// 确保绘图过程中数据不溢出
		if ((prevY <= m_rectPlot.top) || (currY <= m_rectPlot.top))
			m_dcPlot.FillRect(CRect(prevX, m_rectClient.top, currX+1, m_rectPlot.top+1), &m_brushBack) ;
		if ((prevY >= m_rectPlot.bottom) || (currY >= m_rectPlot.bottom))
			m_dcPlot.FillRect(CRect(prevX, m_rectPlot.bottom+1, currX+1, m_rectClient.bottom+1), &m_brushBack) ;

		m_dPreviousPosition = m_dCurrentPosition;
	}

} // end DrawPoint

void COScopeCtrl::DrawPoint(double dNewPoint)
{
	m_dCurrentPosition = dNewPoint ;
	// this does the work of "scrolling" the plot to the left
	// and appending a new data point all of the plotting is 
	// directed to the memory based bitmap associated with m_dcPlot
	// the will subsequently be BitBlt'd to the client in OnPaint

	int currX, prevX, currY, prevY ;

	CPen *oldPen ;
	CRect rectCleanUp ;

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		// shift the plot by BitBlt'ing it to itself 
		// note: the m_dcPlot covers the entire client
		//       but we only shift bitmap that is the size 
		//       of the plot rectangle
		// grab the right side of the plot (exluding m_nShiftPixels on the left)
		// move this grabbed bitmap to the left by m_nShiftPixels

		m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top+1, 
			m_nPlotWidth, m_nPlotHeight, &m_dcPlot, 
			m_rectPlot.left+m_nShiftPixels, m_rectPlot.top+1, 
			SRCCOPY) ;

		// establish a rectangle over the right side of plot
		// which now needs to be cleaned up proir to adding the new point
		rectCleanUp = m_rectPlot ;
		rectCleanUp.left  = rectCleanUp.right - m_nShiftPixels ;

		// fill the cleanup area with the background
		m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;

		// draw the next line segement
		CString strTemp;
		CFont axisFont, *oldFont ;
		axisFont.CreateFont (14, 0, 0, 0, 300,
			FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, 
			DEFAULT_PITCH|FF_SWISS, "Arial") ;

		// grab the horizontal font
		oldFont = m_dcGrid.SelectObject(&axisFont) ;
		// x min
		m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP) ;
		strTemp.Format (_T("%d          "), m_uiXAxisLabelMin) ; 
		m_dcGrid.TextOut (m_rectPlot.left, m_rectPlot.bottom+4, strTemp) ;

		// x max
		m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP) ;
		strTemp.Format (_T("          %d"), m_uiXAxisLabelMax) ; 
		m_dcGrid.TextOut (m_rectPlot.right, m_rectPlot.bottom+4, strTemp) ;

		// restore the font
		m_dcGrid.SelectObject(oldFont) ;

		// grab the plotting pen
		oldPen = m_dcPlot.SelectObject(&m_penPlot) ;

		// move to the previous point
		prevX = m_rectPlot.right-m_nPlotShiftPixels ;
		prevY = m_rectPlot.bottom - 
			(long)((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor) ;
		m_dcPlot.MoveTo (prevX, prevY) ;

		// draw to the current point
		currX = m_rectPlot.right-m_nHalfShiftPixels ;
		currY = m_rectPlot.bottom -
			(long)((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor) ;
		m_dcPlot.LineTo (currX, currY) ;

		// restore the pen 
		m_dcPlot.SelectObject(oldPen) ;

		// if the data leaks over the upper or lower plot boundaries
		// fill the upper and lower leakage with the background
		// this will facilitate clipping on an as needed basis
		// as opposed to always calling IntersectClipRect
		if ((prevY <= m_rectPlot.top) || (currY <= m_rectPlot.top))
			m_dcPlot.FillRect(CRect(prevX, m_rectClient.top, currX+1, m_rectPlot.top+1), &m_brushBack) ;
		if ((prevY >= m_rectPlot.bottom) || (currY >= m_rectPlot.bottom))
			m_dcPlot.FillRect(CRect(prevX, m_rectPlot.bottom+1, currX+1, m_rectClient.bottom+1), &m_brushBack) ;

		// store the current point for connection to the next point
		m_dPreviousPosition = m_dCurrentPosition ;
	}
} // end DrawPoint
// 滚动条右移，图像左移
void COScopeCtrl::LeftDrawPoint(double* dSampleData, UINT uiLength, int iMathValueIdNum, UINT uiLeftDrawPointNum)
{
	// 要减去数据处理的4个数据
	UINT iSampleDataSize = uiLength - iMathValueIdNum;
	if (iSampleDataSize < uiLeftDrawPointNum)
	{
		return;
	}

		// 动态显示X轴标签
		CString strTemp ;
		CFont axisFont, *oldFont ;

		axisFont.CreateFont (14, 0, 0, 0, 300,
			FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, 
			DEFAULT_PITCH|FF_SWISS, "Arial") ;

		// grab the horizontal font
		oldFont = m_dcGrid.SelectObject(&axisFont) ;

		// x min
		m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP) ;
		m_uiXAxisLabelMin = m_uiXAxisLabelMin + uiLeftDrawPointNum;
		// 加空格是为了解决TextOut输出窗口残留上次绘制的文字问题
		strTemp.Format (_T("%d          "), m_uiXAxisLabelMin) ; 
		m_dcGrid.TextOut (m_rectPlot.left, m_rectPlot.bottom+4, strTemp) ;
		

		// x max
		m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP) ;
		m_uiXAxisLabelMax = m_uiXAxisLabelMax + uiLeftDrawPointNum;
		strTemp.Format (_T("          %d"), m_uiXAxisLabelMax) ; 
		m_dcGrid.TextOut (m_rectPlot.right, m_rectPlot.bottom+4, strTemp) ;

		// restore the font
		m_dcGrid.SelectObject(oldFont) ;


	// append a data point to the plot
	// return the previous point
	int currX, prevX, currY, prevY ;

	CPen *oldPen ;
	CRect rectCleanUp ;

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		// shift the plot by BitBlt'ing it to itself 
		// note: the m_dcPlot covers the entire client
		//       but we only shift bitmap that is the size 
		//       of the plot rectangle
		// grab the right side of the plot (exluding m_nShiftPixels on the left)
		// move this grabbed bitmap to the left by m_nShiftPixels

		m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top, 
			m_nPlotWidth, m_nPlotHeight, &m_dcPlot, 
			m_rectPlot.left+ m_nShiftPixels * uiLeftDrawPointNum, m_rectPlot.top, 
			SRCCOPY) ;

		// establish a rectangle over the right side of plot
		// which now needs to be cleaned up proir to adding the new point
		rectCleanUp = m_rectPlot ;
		rectCleanUp.left = rectCleanUp.right - m_nShiftPixels * uiLeftDrawPointNum; 
		// fill the cleanup area with the background
		m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;

		rectCleanUp = m_rectPlot;
		// 清除X轴轴线上载数据区外的点
		rectCleanUp.bottom += 1;
		rectCleanUp.right = rectCleanUp.left;
		rectCleanUp.left = rectCleanUp.right - m_nShiftPixels * uiLeftDrawPointNum;
		// fill the cleanup area with the background
		m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;

		if (iSampleDataSize+uiLeftDrawPointNum >= m_uiXAxisLabelMax)
		{
			if (iSampleDataSize < m_uiXAxisLabelMax)
			{
				int iLeftMove = 0;
				iLeftMove = iSampleDataSize - (m_uiXAxisLabelMax - uiLeftDrawPointNum);
				if (iLeftMove == 0)
				{
					return;
				}
					// draw the next line segement
				// grab the plotting pen
				oldPen = m_dcPlot.SelectObject(&m_penPlot) ;

				for (int i=iLeftMove; i>0; i--)
				{
				//	ProcessMessages();
					// move to the previous point
					prevX = m_rectPlot.right - m_nPlotShiftPixels - m_nShiftPixels * (uiLeftDrawPointNum - 1) ;
					double temp = dSampleData[iSampleDataSize-i-1];
					prevY = m_rectPlot.bottom - (long)((temp - m_dLowerLimit) * m_dVerticalFactor) ;
					m_dcPlot.MoveTo (prevX, prevY) ;

					// draw to the current point
					currX = m_rectPlot.right - m_nHalfShiftPixels - m_nShiftPixels * (uiLeftDrawPointNum - 1) ;
					temp = dSampleData[iSampleDataSize-i];
					currY = m_rectPlot.bottom -(long)((temp  - m_dLowerLimit) * m_dVerticalFactor) ;
					m_dcPlot.LineTo (currX, currY) ;

					uiLeftDrawPointNum--;
				}
				// restore the pen 
				m_dcPlot.SelectObject(oldPen) ;

			}
			else
			{
				// draw the next line segement
				// grab the plotting pen
				oldPen = m_dcPlot.SelectObject(&m_penPlot) ;

				for (int i=uiLeftDrawPointNum; i>0; i--)
				{
				//	ProcessMessages();
					// move to the previous point

					prevX = m_rectPlot.right - m_nPlotShiftPixels - m_nShiftPixels * (i - 1) ;
					double temp = dSampleData[m_uiXAxisLabelMax-i-1];
					prevY = m_rectPlot.bottom - (long)((temp - m_dLowerLimit) * m_dVerticalFactor) ;
					m_dcPlot.MoveTo (prevX, prevY) ;

					// draw to the current point
					currX = m_rectPlot.right - m_nHalfShiftPixels - m_nShiftPixels * (i - 1) ;
					temp = dSampleData[m_uiXAxisLabelMax-i];
					currY = m_rectPlot.bottom -(long)((temp  - m_dLowerLimit) * m_dVerticalFactor) ;
					m_dcPlot.LineTo (currX, currY) ;
				}
				// restore the pen 
				m_dcPlot.SelectObject(oldPen) ;
			}
		}
	}

	InvalidateRect(m_rectClient) ;

} // LeftDrawPoint

// 滚动条左移，图像右移
void COScopeCtrl::RightDrawPoint(double* dSampleData, UINT uiLength, int iMathValueIdNum, UINT uiRightDrawPointNum)
{
	// 要减去数据处理的4个数据
	UINT iSampleDataSize = uiLength - iMathValueIdNum;
	if (iSampleDataSize < uiRightDrawPointNum)
	{
		return;
	}
	if (m_uiXAxisLabelMin == 0)
	{
		return;
	}
	if (m_uiXAxisLabelMin < uiRightDrawPointNum)
	{
		uiRightDrawPointNum = m_uiXAxisLabelMin;
	}

	// 动态显示X轴标签
	CString strTemp ;
	CFont axisFont, *oldFont ;

	axisFont.CreateFont (14, 0, 0, 0, 300,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, "Arial") ;

	// grab the horizontal font
	oldFont = m_dcGrid.SelectObject(&axisFont) ;

	// x min
	m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP) ;
	m_uiXAxisLabelMin = m_uiXAxisLabelMin - uiRightDrawPointNum;
	// 加空格是为了解决TextOut输出窗口残留上次绘制的文字问题
	strTemp.Format (_T("%d          "), m_uiXAxisLabelMin) ; 
	m_dcGrid.TextOut (m_rectPlot.left, m_rectPlot.bottom+4, strTemp) ;


	// x max
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP) ;
	m_uiXAxisLabelMax = m_uiXAxisLabelMax - uiRightDrawPointNum;
	strTemp.Format (_T("          %d"), m_uiXAxisLabelMax) ; 
	m_dcGrid.TextOut (m_rectPlot.right, m_rectPlot.bottom+4, strTemp) ;

	// restore the font
	m_dcGrid.SelectObject(oldFont) ;

	int currX, prevX, currY, prevY ;

	CPen *oldPen ;
	CRect rectCleanUp ;

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		// shift the plot by BitBlt'ing it to itself 
		// note: the m_dcPlot covers the entire client
		//       but we only shift bitmap that is the size 
		//       of the plot rectangle
		// grab the right side of the plot (exluding m_nShiftPixels on the left)
		// move this grabbed bitmap to the left by m_nShiftPixels

		m_dcPlot.BitBlt(m_rectPlot.left + m_nShiftPixels * uiRightDrawPointNum, m_rectPlot.top, 
			m_nPlotWidth, m_nPlotHeight, &m_dcPlot, 
			m_rectPlot.left, m_rectPlot.top, 
			SRCCOPY) ;

		// establish a rectangle over the right side of plot
		// which now needs to be cleaned up proir to adding the new point
		rectCleanUp = m_rectPlot ;
		rectCleanUp.right  = rectCleanUp.left + m_nShiftPixels  * uiRightDrawPointNum;

		// fill the cleanup area with the background
		m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;
		
		rectCleanUp = m_rectPlot ;
		// 清除X轴轴线上载数据区外的点
		rectCleanUp.bottom += 1;
		rectCleanUp.left  = rectCleanUp.right;
		rectCleanUp.right  = rectCleanUp.right + m_nShiftPixels  * uiRightDrawPointNum;
		// fill the cleanup area with the background
		m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;

		// 由于各画图区域上载入的点个数不同，在滚动条拖动过程中为了防止载入未知数据
		if (m_uiXAxisLabelMin + uiRightDrawPointNum > iSampleDataSize)
		{
			return;
		}

		// draw the next line segement
		// grab the plotting pen
			oldPen = m_dcPlot.SelectObject(&m_penPlot) ;

			for (int i=uiRightDrawPointNum; i>0; i--)
			{
			//	ProcessMessages();
				// move to the previous point
				prevX = m_rectPlot.left+ m_nPlotShiftPixels + m_nShiftPixels * (i - 1);
				double temp = dSampleData[m_uiXAxisLabelMin + i-1];
				prevY = m_rectPlot.bottom - (long)((temp - m_dLowerLimit) * m_dVerticalFactor) ;
				m_dcPlot.MoveTo (prevX, prevY) ;

				// draw to the current point
				currX = m_rectPlot.left+ m_nHalfShiftPixels + m_nShiftPixels * (i - 1);
				if ((i == 1)&&(m_uiXAxisLabelMin == 0))
				{
					temp = 0.0;
				}
				else
				{
					temp = dSampleData[m_uiXAxisLabelMin + i-2];
				}
			
				currY = m_rectPlot.bottom -(long)((temp  - m_dLowerLimit) * m_dVerticalFactor) ;
				m_dcPlot.LineTo (currX, currY) ;
			}

			// restore the pen 
			m_dcPlot.SelectObject(oldPen) ;
	}
	InvalidateRect(m_rectClient) ;

}// RightDrawPoint

/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy) ;

	// NOTE: OnSize automatically gets called during the setup of the control

	GetClientRect(m_rectClient) ;

	// set some member variables to avoid multiple function calls
	m_nClientHeight = m_rectClient.Height() ;
	m_nClientWidth  = m_rectClient.Width() ;

	// the "left" coordinate and "width" will be modified in 
	// InvalidateCtrl to be based on the width of the y axis scaling
	m_rectPlot.left   = 20 ;  
	m_rectPlot.top    = 10 ;
	m_rectPlot.right  = m_rectClient.right-25 ;
	m_rectPlot.bottom = m_rectClient.bottom-25 ;

	// set some member variables to avoid multiple function calls
	m_nPlotHeight = m_rectPlot.Height() ;
	m_nPlotWidth  = m_rectPlot.Width() ;

	// set the scaling factor for now, this can be adjusted 
	// in the SetRange functions
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange ; 

} // OnSize


/////////////////////////////////////////////////////////////////////////////
void COScopeCtrl::Reset(bool bXMaxLableDefault)
{
	// to clear the existing data (in the form of a bitmap)
	// simply invalidate the entire control
	InvalidateCtrl(bXMaxLableDefault) ;
}
void COScopeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

 
 	if((point.x >= m_rectPlot.left) && (point.x <= m_rectPlot.right)
		&& (point.y >= m_rectPlot.top) && (point.y <= m_rectPlot.bottom))
	{
		m_iMousepointX = (point.x-m_rectPlot.left)/m_nShiftPixels + m_uiXAxisLabelMin;
		m_dMousepointY = (m_rectPlot.bottom - point.y)/m_dVerticalFactor + m_dLowerLimit;
		if(m_bLButtonDown == TRUE)
		{
			m_bMouseDrag = TRUE;
			m_pointMouseDragCur = point;
			InvalidateRect( m_rectPlot, FALSE );
		}
	}
	else
	{
		if(m_bLButtonDown == TRUE)
		{
			if(point.x > m_pointStart.x) 
			{
				m_pointMouseDragCur.x = min(point.x, m_rectPlot.right);
			}
			else
			{
				m_pointMouseDragCur.x = max(point.x, m_rectPlot.left);
			}
			if(point.y > m_pointStart.y)
			{
				m_pointMouseDragCur.y = min(point.y, m_rectPlot.bottom);
			}
			else
			{
				m_pointMouseDragCur.y = max(point.y, m_rectPlot.top);
			}
			m_bMouseDrag = TRUE;
			InvalidateRect( m_rectPlot, FALSE );
		}
	}

	CStatic* pWnd = (CStatic*)m_pParentWnd->GetDlgItem(IDC_STATIC_AXIS_X);
	CString str_x;
	str_x.Format(_T(" X = %d "), m_iMousepointX);//6位有效位数
	pWnd->SetWindowText(str_x);

	CStatic* sWnd = (CStatic*)m_pParentWnd->GetDlgItem(IDC_STATIC_AXIS_Y);
	CString str_y;
	str_y.Format(_T(" Y = %2.*lf "),m_nYDecimals, m_dMousepointY);//6位有效位数
	sWnd->SetWindowText(str_y);

	CWnd::OnMouseMove(nFlags, point);
}
void COScopeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	if((point.x >= m_rectPlot.left) && (point.x <= m_rectPlot.right)
		&& (point.y >= m_rectPlot.top) && (point.y <= m_rectPlot.bottom))
	{
		m_pointStart = point;
	}
	else
	{
		if(point.x > m_rectPlot.right) 
		{
			m_pointStart.x = m_rectPlot.right;
		}
		else if(point.x < m_rectPlot.left)
		{
			m_pointStart.x = m_rectPlot.left;
		}
		else 
		{
			m_pointStart.x = point.x;
		}
		if(point.y > m_rectPlot.bottom)
		{
			m_pointStart.y = m_rectPlot.bottom;
		}
		else if(point.y < m_rectPlot.top)
		{
			m_pointStart.y = m_rectPlot.top;
		}
		else
		{
			m_pointStart.y = point.y;
		}
	}
	m_bLButtonDown = TRUE;
	CWnd::OnLButtonDown(nFlags, point);
}

void COScopeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	if((point.x >= m_rectPlot.left) && (point.x <= m_rectPlot.right)
		&& (point.y >= m_rectPlot.top) && (point.y <= m_rectPlot.bottom))
	{
		m_pointEnd = point;
	}
	else
	{
		if(point.x > m_pointStart.x) 
		{
			m_pointEnd.x = min(point.x, m_rectPlot.right);
		}
		else 
		{
			m_pointEnd.x = max(point.x, m_rectPlot.left);
		}
		if(point.y > m_pointStart.y)
		{	
			m_pointEnd.y = min(point.y, m_rectPlot.bottom);
		}
		else
		{
			m_pointEnd.y = max(point.y, m_rectPlot.top);
		}
	}
	if (m_bLButtonDown == TRUE)
	{
		m_bLButtonDown = FALSE;
		m_pointMouseDragCur = m_pointEnd;
		// 判断鼠标有拖动
		if ((m_pointStart.x != m_pointEnd.x)&&(m_pointStart.y != m_pointEnd.y))
		{
			m_bMouseDrag = FALSE;
			// X轴坐标不能放大，大小取决于控件尺寸
// 			int iMaxLabel_X = 0;
// 			int iMinLabel_X = 0;
			int iMaxLabel_Y = 0;
			int iMinLabel_Y = 0;
// 			if(m_pointEnd.x > m_pointStart.x) 
// 			{
// 				iMinLabel_X = m_pointStart.x;
// 				iMaxLabel_X = m_pointEnd.x;
// 			}
// 			else
// 			{
// 				iMinLabel_X = m_pointEnd.x;
// 				iMaxLabel_X = m_pointStart.x;
// 			}
			if(m_pointEnd.y > m_pointStart.y)
			{
				iMinLabel_Y = m_pointStart.y;
				iMaxLabel_Y = m_pointEnd.y;
			}
			else
			{
				iMinLabel_Y = m_pointEnd.y;
				iMaxLabel_Y = m_pointStart.y;
			}

			double dMinLabel_Y = 0.0;
			double dMaxLabel_Y = 0.0;

			dMinLabel_Y=(m_rectPlot.bottom - (double)iMaxLabel_Y) * (m_dUpperLimit - m_dLowerLimit)/(m_rectPlot.bottom - m_rectPlot.top) + m_dLowerLimit;
			dMaxLabel_Y=(m_rectPlot.bottom - (double)iMinLabel_Y) * (m_dUpperLimit - m_dLowerLimit)/(m_rectPlot.bottom - m_rectPlot.top) + m_dLowerLimit;

			SetRange(dMinLabel_Y, dMaxLabel_Y, m_nYDecimals);

			InvalidateRect( m_rectPlot, FALSE );
		}
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void COScopeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	
	SetRange(-2.5, 2.5, m_nYDecimals);
	InvalidateRect( m_rectPlot, FALSE );

	CWnd::OnRButtonUp(nFlags, point);
}

// 绘制鼠标拖动放大矩形框
void COScopeCtrl::OnDrawMouseDragRect(CDC* pDC, POINT StartPoint, POINT EndPoint)
{
	CPen * OldPen;
	CPen CurPen;

	CurPen.CreatePen( PS_DOT, 1, RGB( 255,0,0 ) );
	OldPen = pDC->SelectObject( &CurPen );
	pDC->MoveTo(StartPoint.x, StartPoint.y);   
	pDC->LineTo(EndPoint.x, StartPoint.y);   
	pDC->LineTo(EndPoint.x, EndPoint.y);   
	pDC->LineTo(StartPoint.x, EndPoint.y);   
	pDC->LineTo(StartPoint.x, StartPoint.y); 

	pDC->SelectObject( OldPen );
}

// 防止程序在循环中运行无法响应消息
void COScopeCtrl::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}
