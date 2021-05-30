#include "DirectXCreator.h"
#include <d3dx9.h>
#include <d3d9.h>



bool DXWapper::CreateDevice(HWND hwnd)
{

    IDirect3D9* _d3d9;
    _d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    D3DCAPS9 caps;
    _d3d9->GetDeviceCaps(
        D3DADAPTER_DEFAULT,     //主显示适配器
        D3DDEVTYPE_HAL,         //硬件方式使用D3D9
        &caps                   //获取设备Caps
    );

    DWORD VertextType = GetVertexProcessType(caps);

    //Setp2 填充D3DPRESENT_PARAMETER以初始化Device
    _D3DPRESENT_PARAMETERS_ d3dpp = InitD3DParameter(hwnd);

    if (FAILED(CreateDevice(_d3d9,hwnd,d3dpp,VertextType)))
    {
        _d3d9->Release();
        ::MessageBox(0, TEXT("Device建立失败!"), 0, 0);
        return false;
    }

    _d3d9->Release();
    return true;
}

DWORD DXWapper::GetVertexProcessType(D3DCAPS9 caps)
{
    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        return D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else
        return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}

_D3DPRESENT_PARAMETERS_ DXWapper::InitD3DParameter(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom- rc.top;

    _D3DPRESENT_PARAMETERS_ d3dpp;
    memset(&d3dpp, 0, sizeof(_D3DPRESENT_PARAMETERS_));

    d3dpp.BackBufferWidth = width;                                  //后台缓存表面宽度
    d3dpp.BackBufferHeight = height;                                //高度
    d3dpp.BackBufferFormat = D3DFMT_A32B32G32R32F;                       //设备使用的色彩格式
    d3dpp.BackBufferCount = 1;                                      //后台缓存数量
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;                    //多重采样类型,用来去锯齿的
    d3dpp.MultiSampleQuality = 0;                                   //多重采样质量
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;                       //刷新方式,DISCARD为系统自动选择,具体看笔记
    d3dpp.hDeviceWindow = hwnd;                                     //目标窗口句柄
    d3dpp.Windowed = false;                                         //是否为窗口化
    d3dpp.EnableAutoDepthStencil = D3DFMT_D24S8;                    //缓冲设置
    d3dpp.Flags = 0;                                                //不知
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;     //刷新率,默认
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;     //看起来像是垂直同步

    return d3dpp;
}

HRESULT DXWapper::CreateDevice(IDirect3D9* _d3d9, HWND hwnd, _D3DPRESENT_PARAMETERS_ d3dpp,DWORD VertextType)
{
    //Setp3 创建D3DDevice
    HRESULT hr = _d3d9->CreateDevice(
        D3DADAPTER_DEFAULT,             //希望使用的显卡是哪个
        D3DDEVTYPE_HAL,                 //CPU模拟还是GPU模式,HAL是GPU模式
        hwnd,                           //目标窗口,和D3dprameter的保持一致
        VertextType,                    //向量处理模式,GPU处理还是CPU处理
        &d3dpp,
        &m_pDevice
    );

    return hr;
}
