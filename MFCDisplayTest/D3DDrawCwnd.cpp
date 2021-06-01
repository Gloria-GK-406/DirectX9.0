#include "pch.h"
#include <timeapi.h>
#include "D3DDrawCwnd.h"

BEGIN_MESSAGE_MAP(D3DDrawCwnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

#define TIMER_DRAW 10110

bool D3DDrawCwnd::InitDirectX(CreateDeviceStruct paramter)
{
	if (!Create(NULL, TEXT(""), CS_HREDRAW | CS_VREDRAW, CRect(0, 35, paramter.WindowsWidth, paramter.WindowsHeight), CWnd::FromHandle(paramter.hwndParent), 10086))
		return FALSE;

	paramter.hwndVideo = m_hWnd;

	return wapper.CreateDevice(paramter);
}

void D3DDrawCwnd::PrepareData()
{
	startTime = (float)timeGetTime();
	SetTimer(TIMER_DRAW, 200,NULL);
}

void D3DDrawCwnd::SetDrawType(DRAWTYPE type)
{
	curType = type;
}

BOOL D3DDrawCwnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void D3DDrawCwnd::OnTimer(UINT_PTR nIDEvent)
{
	float curTime = (float)timeGetTime();
	float timeDelta = (curTime - startTime) * 0.00001f;

	if (nIDEvent == TIMER_DRAW)
	{
		switch (curType)
		{
		case DRAWTYPE::Triangle:
			wapper.DrawTriangle();
			break;
		case DRAWTYPE::Cube:
			wapper.DrawCube(timeDelta);
			break;
		case DRAWTYPE::Custom:
			wapper.DrawCustom();
			break;
		case DRAWTYPE::ColoredTriangle:
			wapper.DrawColoredTriangle();
			break;
		case DRAWTYPE::Texture:
			wapper.DrawTexture();
			break;
		}
	}
}

