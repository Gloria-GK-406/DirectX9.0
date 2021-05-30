#include "pch.h"
#include "D3DDrawCwnd.h"


BEGIN_MESSAGE_MAP(D3DDrawCwnd, CWnd)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

bool D3DDrawCwnd::InitDirectX()
{
	return wapper.CreateDevice(m_hWnd);
}

BOOL D3DDrawCwnd::OnEraseBkgnd(CDC* pDC)
{
	BOOL res = CWnd::OnEraseBkgnd(pDC);
	CRect clientRc;
	GetClientRect(&clientRc);

	pDC->FillSolidRect(clientRc, RGB(255, 0, 0));
	return res;
}
