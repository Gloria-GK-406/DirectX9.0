
// MFCDisplayTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCDisplayTest.h"
#include "MFCDisplayTestDlg.h"
#include "afxdialogex.h"
#include "DirectXCreator.h"
#include "d3dUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCDisplayTestDlg 对话框


CMFCDisplayTestDlg::CMFCDisplayTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MFCDISPLAYTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDisplayTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_box);
}

BOOL CMFCDisplayTestDlg::InitD3DDrawWnd()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.bottom -= 50;
	CreateDeviceStruct deviceStruct;
	deviceStruct.hInstance = AfxGetInstanceHandle();
	deviceStruct.hwndParent = m_hWnd;
	deviceStruct.WindowsHeight = rcClient.Height();
	deviceStruct.WindowsWidth = rcClient.Width();

	if (!m_D3DDrawCwnd.InitDirectX(deviceStruct))
		return FALSE;
	InitComboBox();
	m_D3DDrawCwnd.PrepareData();

	return TRUE;
}

void CMFCDisplayTestDlg::InitComboBox()
{
	m_box.AddString(L"Triangle");
	m_box.AddString(L"Cube");
	m_box.AddString(L"Custom");
	m_box.AddString(L"ColoredTriangle");
	m_box.AddString(L"Texture");
}

BEGIN_MESSAGE_MAP(CMFCDisplayTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMFCDisplayTestDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()

// CMFCDisplayTestDlg 消息处理程序

BOOL CMFCDisplayTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	if (!InitD3DDrawWnd())
		return FALSE;
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCDisplayTestDlg::OnPaint()
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
HCURSOR CMFCDisplayTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCDisplayTestDlg::OnCbnSelchangeCombo1()
{
	int sel =  m_box.GetCurSel();
	DRAWTYPE type = (DRAWTYPE)(sel);
	m_D3DDrawCwnd.SetDrawType(type);
}
