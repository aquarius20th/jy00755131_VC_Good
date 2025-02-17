// ReceiveDataUDPDlg.h : 头文件
//

#pragma once
#include "Receive.h"

// CReceiveDataUDPDlg 对话框
class CReceiveDataUDPDlg : public CDialog
{
// 构造
public:
	CReceiveDataUDPDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_RECEIVEDATAUDP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CReceive m_Receive;


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReceive();
};
