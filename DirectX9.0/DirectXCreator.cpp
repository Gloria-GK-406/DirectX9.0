#include "DirectXCreator.h"
#include <d3dx9.h>
#include <d3d9.h>
#include "d3dUtility.h"

using namespace d3d;

bool DXWapper::CreateDevice(CreateDeviceStruct paramter)
{
    IDirect3D9* _d3d9;
    _d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    D3DCAPS9 caps2;
    _d3d9->GetDeviceCaps(
        D3DADAPTER_DEFAULT,     //主显示适配器
        D3DDEVTYPE_HAL,         //硬件方式使用D3D9
        &caps2                   //获取设备Caps
    );

    D3DDISPLAYMODE d3ddm;
    _d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm);

    UINT adepterCount =  _d3d9->GetAdapterCount();

    DWORD VertextType = GetVertexProcessType(caps2);

    if (!CreateVideoWnd(paramter))
        return FALSE;

    //Setp2 填充D3DPRESENT_PARAMETER以初始化Device
    _D3DPRESENT_PARAMETERS_ d3dpp = InitD3DParameter(paramter);

    if (FAILED(CreateDevice(_d3d9,d3dpp,VertextType)))
    {
        Release(_d3d9);
        ::MessageBox(0, TEXT("Device建立失败!"), 0, 0);
        return false;
    }

    Release(_d3d9);
    return true;
}

bool DXWapper::DrawShape()
{
    IDirect3DVertexBuffer9* vb;
    m_pDevice->CreateVertexBuffer(
        8 * sizeof(Vertex),
        0,
        D3DFVF_XYZ,
        D3DPOOL_DEFAULT,
        &vb,
        0
    );

    

    return false;
}

DWORD DXWapper::GetVertexProcessType(D3DCAPS9 caps)
{
    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        return D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else
        return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}

_D3DPRESENT_PARAMETERS_ DXWapper::InitD3DParameter(CreateDeviceStruct paramter)
{
    _D3DPRESENT_PARAMETERS_ d3dpp;
    memset(&d3dpp, 0, sizeof(_D3DPRESENT_PARAMETERS_));

    d3dpp.BackBufferWidth = paramter.WindowsWidth;                                  //后台缓存表面宽度
    d3dpp.BackBufferHeight = paramter.WindowsHeight;                                //高度
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;                       //设备使用的色彩格式
    d3dpp.BackBufferCount = 1;                                      //后台缓存数量
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;                    //多重采样类型,用来去锯齿的
    d3dpp.MultiSampleQuality = 0;                                   //多重采样质量
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;                       //刷新方式,DISCARD为系统自动选择,具体看笔记
    d3dpp.hDeviceWindow = m_pVideoHwnd;                                     //目标窗口句柄
    d3dpp.Windowed = TRUE;                                         //是否为窗口化
    d3dpp.EnableAutoDepthStencil = TRUE;                             
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;                    //缓冲设置
    d3dpp.Flags = 0;                                                //不知
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;     //刷新率,默认
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;     //看起来像是垂直同步
    d3dpp.EnableAutoDepthStencil = TRUE;

    return d3dpp;
}

HRESULT DXWapper::CreateDevice(IDirect3D9* _d3d9, _D3DPRESENT_PARAMETERS_ d3dpp,DWORD VertextType)
{
    //Setp3 创建D3DDevice
    HRESULT hr = _d3d9->CreateDevice(
        D3DADAPTER_DEFAULT,             //希望使用的显卡是哪个
        D3DDEVTYPE_HAL,                 //CPU模拟还是GPU模式,HAL是GPU模式
        m_pVideoHwnd,                           //目标窗口,和D3dprameter的保持一致
        VertextType,                    //向量处理模式,GPU处理还是CPU处理
        &d3dpp,
        &m_pDevice
    );

    return hr;
}

BOOL DXWapper::CreateVideoWnd(CreateDeviceStruct paramter)
{
    if (paramter.hwndVideo)
    {
        m_pVideoHwnd = paramter.hwndVideo;
        ::ShowWindow(m_pVideoHwnd, SW_SHOW);
        ::UpdateWindow(m_pVideoHwnd);
        return TRUE;
    }

    WNDCLASS wc;
    
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)d3d::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = paramter.hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = TEXT("Direct3D9App");

    if (!RegisterClass(&wc))
    {
        ::MessageBox(0, TEXT("RegisterClass() - FAILED"), 0, 0);
        return FALSE;
    }

    m_pVideoHwnd = ::CreateWindow(wc.lpszClassName, wc.lpszClassName,
        WS_EX_TOPMOST,
        0, 0, paramter.WindowsWidth, paramter.WindowsHeight,
        paramter.hwndParent /*parent hwnd*/, 0 /* menu */, paramter.hInstance, 0 /*extra*/);

    if (!m_pVideoHwnd)
    {
        ::MessageBox(0, TEXT("CreateWindow() - FAILED"), 0, 0);
        return FALSE;
    }



    return TRUE;
}
