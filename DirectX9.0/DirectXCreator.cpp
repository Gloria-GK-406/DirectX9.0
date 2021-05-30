#include "DirectXCreator.h"
#include <d3dx9.h>
#include <d3d9.h>



bool DXWapper::CreateDevice(HWND hwnd)
{

    IDirect3D9* _d3d9;
    _d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    D3DCAPS9 caps;
    _d3d9->GetDeviceCaps(
        D3DADAPTER_DEFAULT,     //����ʾ������
        D3DDEVTYPE_HAL,         //Ӳ����ʽʹ��D3D9
        &caps                   //��ȡ�豸Caps
    );

    DWORD VertextType = GetVertexProcessType(caps);

    //Setp2 ���D3DPRESENT_PARAMETER�Գ�ʼ��Device
    _D3DPRESENT_PARAMETERS_ d3dpp = InitD3DParameter(hwnd);

    if (FAILED(CreateDevice(_d3d9,hwnd,d3dpp,VertextType)))
    {
        _d3d9->Release();
        ::MessageBox(0, TEXT("Device����ʧ��!"), 0, 0);
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

    d3dpp.BackBufferWidth = width;                                  //��̨���������
    d3dpp.BackBufferHeight = height;                                //�߶�
    d3dpp.BackBufferFormat = D3DFMT_A32B32G32R32F;                       //�豸ʹ�õ�ɫ�ʸ�ʽ
    d3dpp.BackBufferCount = 1;                                      //��̨��������
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;                    //���ز�������,����ȥ��ݵ�
    d3dpp.MultiSampleQuality = 0;                                   //���ز�������
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;                       //ˢ�·�ʽ,DISCARDΪϵͳ�Զ�ѡ��,���忴�ʼ�
    d3dpp.hDeviceWindow = hwnd;                                     //Ŀ�괰�ھ��
    d3dpp.Windowed = false;                                         //�Ƿ�Ϊ���ڻ�
    d3dpp.EnableAutoDepthStencil = D3DFMT_D24S8;                    //��������
    d3dpp.Flags = 0;                                                //��֪
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;     //ˢ����,Ĭ��
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;     //���������Ǵ�ֱͬ��

    return d3dpp;
}

HRESULT DXWapper::CreateDevice(IDirect3D9* _d3d9, HWND hwnd, _D3DPRESENT_PARAMETERS_ d3dpp,DWORD VertextType)
{
    //Setp3 ����D3DDevice
    HRESULT hr = _d3d9->CreateDevice(
        D3DADAPTER_DEFAULT,             //ϣ��ʹ�õ��Կ����ĸ�
        D3DDEVTYPE_HAL,                 //CPUģ�⻹��GPUģʽ,HAL��GPUģʽ
        hwnd,                           //Ŀ�괰��,��D3dprameter�ı���һ��
        VertextType,                    //��������ģʽ,GPU������CPU����
        &d3dpp,
        &m_pDevice
    );

    return hr;
}
