#pragma once
#include <afxwin.h>
#include "DirectXCreator.h"

enum class DRAWTYPE
{
    Triangle,
    Cube,
    Custom,
    ColoredTriangle,
    Texture
};

class D3DDrawCwnd:public CWnd
{
public:
    bool InitDirectX(CreateDeviceStruct paramter);
    void PrepareData();
    void SetDrawType(DRAWTYPE type);

private:
    DXWapper wapper;
    DRAWTYPE curType = DRAWTYPE::Triangle;

    float startTime;

protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()
};

