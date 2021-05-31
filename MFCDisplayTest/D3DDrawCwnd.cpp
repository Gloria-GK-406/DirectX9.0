#include "pch.h"
#include "D3DDrawCwnd.h"

BEGIN_MESSAGE_MAP(D3DDrawCwnd, CWnd)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

bool D3DDrawCwnd::InitDirectX(CreateDeviceStruct paramter)
{
	if (!Create(NULL, TEXT(""), CS_HREDRAW | CS_VREDRAW, CRect(0, 0, paramter.WindowsWidth, paramter.WindowsHeight), CWnd::FromHandle(paramter.hwndParent), 10086))
		return FALSE;

	paramter.hwndVideo = m_hWnd;

	return wapper.CreateDevice(paramter);
}

BOOL D3DDrawCwnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	pDC->FillSolidRect(rcClient, RGB(0, 255, 255));

	return TRUE;
}

