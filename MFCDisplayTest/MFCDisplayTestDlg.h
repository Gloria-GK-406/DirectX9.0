
// MFCDisplayTestDlg.h: 头文件
//

#pragma once
#include "D3DDrawCwnd.h"


// CMFCDisplayTestDlg 对话框
class CMFCDisplayTestDlg : public CDialog
{
// 构造
public:
	CMFCDisplayTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCDISPLAYTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	D3DDrawCwnd m_D3DDrawCwnd;
	CComboBox m_box;
	

private:
	BOOL InitD3DDrawWnd();
	void InitComboBox();


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombo1();
};
