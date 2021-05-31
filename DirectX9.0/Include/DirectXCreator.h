#pragma once
#include <d3d9.h>

struct CreateDeviceStruct
{
    HWND hwndParent;
    HWND hwndVideo;
    HINSTANCE hInstance;
    UINT WindowsWidth;
    UINT WindowsHeight;
};

class DXWapper
{
public:
    bool CreateDevice(CreateDeviceStruct paramter);


private:
    IDirect3DDevice9* m_pDevice = NULL;
    HWND m_pVideoHwnd = NULL;
    

private:
    DWORD GetVertexProcessType(D3DCAPS9 caps);
    _D3DPRESENT_PARAMETERS_ InitD3DParameter(CreateDeviceStruct paramter);
    HRESULT CreateDevice(IDirect3D9* _d3d9, _D3DPRESENT_PARAMETERS_ d3dpp, DWORD VertextType);
    BOOL CreateVideoWnd(CreateDeviceStruct paramter);

};


