// RecSocketShotPro.cpp : 实现文件
//

#include "stdafx.h"
#include "BLASTER_SHOTPRO.h"
#include "RecSocketShotPro.h"


// CRecSocketShotPro

CRecSocketShotPro::CRecSocketShotPro()
{
}

CRecSocketShotPro::~CRecSocketShotPro()
{
}


// CRecSocketShotPro 成员函数

void CRecSocketShotPro::OnReceive(int nErrorCode)
{
	// TODO: ÔÚ´ËÌí¼Ó×¨ÓÃ´úÂëºÍ/»òµ÷ÓÃ»ùÀà
	int ret=0;
	CString str;
	CString strshow;
	CWnd* pwnd = AfxGetMainWnd();
	ret=Receive(m_ucRecBufShotPro,128);
	if(ret==ERROR)
	{
		TRACE("ERROR!");
	}
	else if(ret != 0xffffffff) 
	{
		str = &m_ucRecBufShotPro[32];
		int iPos = str.Find(_T("\r\n"), 0);
		strshow = str.Mid(0,iPos);
		pwnd->GetDlgItem(IDC_EDIT_RECSHOW_SHOTPRO)->SetWindowText(strshow);
		pwnd->SetTimer(1, 3000, NULL);
	}

	CSocket::OnReceive(nErrorCode);
}
