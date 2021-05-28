#pragma once
#include <d3d9.h>


class DXWapper
{
public:
    bool CreateDevice(HWND hwnd);


private:
    IDirect3DDevice9* m_pDevice = NULL;


};


