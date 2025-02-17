#include "StdAfx.h"
#include "ButtonItem.h"
#include "MainFrm.h"

CButtonItem::CButtonItem (LPCTSTR lpszText, UINT id) :
	CBCGPGridItem (lpszText)
{
	m_id = id;
}
//*****************************************************************************************
void CButtonItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);
	CRect rectBtn;
	rectBtn = rect;
// 	rectBtn.left = rect.left + rect.Width()/2 - rect.Height()/2;
// 	rectBtn.right = rect.left + rect.Width()/2 + rect.Height()/2;
	OnFillBackground (pDC, rectBtn);

	rectBtn.DeflateRect (1, 1);

	pDC->FillRect (rectBtn, &globalData.brBarFace);

	pDC->Draw3dRect (rectBtn, globalData.clrBarLight, globalData.clrBarDkShadow);
	rectBtn.DeflateRect (1, 1);
	pDC->Draw3dRect (rectBtn, globalData.clrBarHilite, globalData.clrBarShadow);

	COLORREF clrText = pDC->SetTextColor (globalData.clrBarText);

	CString strText = (LPCTSTR)(_bstr_t) m_varValue;

	pDC->DrawText (strText, rectBtn, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	pDC->SetTextColor (clrText);
}
//*****************************************************************************************
BOOL CButtonItem::OnClickValue (UINT uiMsg, CPoint point)
{
	if (uiMsg != WM_LBUTTONDOWN)
	{
		return CBCGPGridItem::OnClickValue (uiMsg, point);
	}
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	ASSERT_VALID (pFrm);
	pFrm->m_wndActiveSource.SendMessage(WM_COMMAND,
		MAKEWPARAM (m_id, BN_CLICKED));
	return TRUE;
}
