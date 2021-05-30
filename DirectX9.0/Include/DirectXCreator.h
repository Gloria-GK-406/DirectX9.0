#pragma once
#include <d3d9.h>


class DXWapper
{
public:
    bool CreateDevice(HWND hwnd);


private:
    IDirect3DDevice9* m_pDevice = NULL;
    

private:
    DWORD GetVertexProcessType(D3DCAPS9 caps);
    _D3DPRESENT_PARAMETERS_ InitD3DParameter(HWND hwnd);
    HRESULT CreateDevice(IDirect3D9* _d3d9, HWND hwnd, _D3DPRESENT_PARAMETERS_ d3dpp, DWORD VertextType);

};


