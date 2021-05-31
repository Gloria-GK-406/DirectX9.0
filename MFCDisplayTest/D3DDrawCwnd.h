#pragma once
#include <afxwin.h>
#include "DirectXCreator.h"

class D3DDrawCwnd:public CWnd
{
public:
    bool InitDirectX(CreateDeviceStruct paramter);

private:
    DXWapper wapper;

protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()
};

