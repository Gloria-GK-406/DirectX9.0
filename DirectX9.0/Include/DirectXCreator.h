#pragma once
#include <d3d9.h>

struct Vertex
{
     Vertex(float x, float y, float z)
    {
        _x = x;
        _y = y;
        _z = z;
    };

    float _x, _y, _z;
};

struct ColorVertex
{
    ColorVertex(float x, float y, float z)
    {
        _x = x;
        _y = y;
        _z = z;
        _color = 0xff000000;;
    }

    ColorVertex(float x, float y, float z, DWORD color)
    {
        _x = x;
        _y = y;
        _z = z;
        _color = color;
    }

    float _x, _y, _z;
    D3DCOLOR _color;
};

struct NormalTexVertex
{
    NormalTexVertex(
        float x, float y, float z,
        float nx, float ny, float nz,
        float u, float v)
    {
        _x = x;  _y = y;  _z = z;
        _nx = nx; _ny = ny; _nz = nz;
        _u = u;  _v = v;
    }

    float _x, _y, _z;           //点坐标
    float _nx, _ny, _nz;        //没用到不清楚
    float _u, _v;               //纹理的坐标
};

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

public:
    void DrawTriangle();
    void DrawCube(float timeDelta =0);
    void DrawCustom();
    void DrawColoredTriangle();
    void DrawTexture();

private:
    void SetCustomMatrix();
    void SetCubeMatrix();
    void SetTriangleMatrix();
    void SetColoredTriangleMatrix();
    void PrepareCustomBuffer();
    void PrepareCubeBuffer();
    void PrepareTriangleBuffer();
    void PrepareColoredTriangleBuffer();

private:
    void SetTextureMatrix();
    void PrepareTexture();
    void PrepareDrawTextureBuffer();

private:
    void SetTexture(IDirect3DTexture9* texture);
    IDirect3DTexture9* CreateTexture(LPCWSTR filePath);

private:
    IDirect3DDevice9* m_pDevice = NULL;
    HWND m_pVideoHwnd = NULL;
    CreateDeviceStruct m_Parameter;
    
private:
    IDirect3DVertexBuffer9* Triangle = 0;

    IDirect3DVertexBuffer9* CubeBuffer = 0;
    IDirect3DIndexBuffer9* CubeIndex = 0;

    IDirect3DVertexBuffer9* CustomBuffer = 0;
    IDirect3DIndexBuffer9* CustomIndex = 0;

    IDirect3DVertexBuffer9* ColoredTriangle = 0;

    IDirect3DVertexBuffer9* TextureBuffer = 0;
    IDirect3DTexture9* Texture =0;



    IDirect3DTexture9* CusTexture1 = 0;
    IDirect3DTexture9* CusTexture2 = 0;
private:
    DWORD GetVertexProcessType(D3DCAPS9 caps);
    _D3DPRESENT_PARAMETERS_ InitD3DParameter(CreateDeviceStruct paramter);
    HRESULT CreateDevice(IDirect3D9* _d3d9, _D3DPRESENT_PARAMETERS_ d3dpp, DWORD VertextType);
    BOOL CreateVideoWnd(CreateDeviceStruct paramter);

};


