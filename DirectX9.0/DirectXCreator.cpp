#include "DirectXCreator.h"
#include <d3dx9.h>
#include <d3d9.h>
#include "d3dUtility.h"

using namespace d3d;
#define D3DFVF_XYZC D3DFVF_XYZ|D3DFVF_DIFFUSE
#define D3DFVF_TEXTURE D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1

bool DXWapper::CreateDevice(CreateDeviceStruct paramter)
{
    m_Parameter = paramter;

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

void DXWapper::DrawTriangle()
{
    PrepareTriangleBuffer();
    SetTriangleMatrix();

    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);            //用不透明白色填充背景
    m_pDevice->BeginScene();                                                                    //绘制前调用

    m_pDevice->SetFVF(D3DFVF_XYZC);                                                             //顶点流的顶点属性
    m_pDevice->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex));                            //设置顶点流

    // Draw one triangle.
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);                                         //绘制一个三角形，从顶点0开始。也就是用0，1，2顶点来绘制一个三角形

    m_pDevice->EndScene();                                                                      //绘制结束时调用
    m_pDevice->Present(0, 0, 0, 0);                                                             //将结果刷新到屏幕，刷新时机和device初始化时设置有关
}

void DXWapper::PrepareTriangleBuffer()
{
    if (Triangle != 0)
        return;

    m_pDevice->CreateVertexBuffer(
        3 * sizeof(ColorVertex),                       //buffer大小，三角形是3个顶点
        D3DUSAGE_WRITEONLY,                     //只写，不可读
        D3DFVF_XYZC,                             //顶点格式为xyz
        D3DPOOL_MANAGED,                        //存储的内存位置，有显存，主存等
        &Triangle,                              //指向存储位置的指针
        0                                       //SharedHandle,不使用，设置为0
    );

    ColorVertex* vertices;
    Triangle->Lock(0, 0, (void**)&vertices, 0); //1，Offset，2，Size，3，操作的指针，4，标志位。详情找google，当2为0时，表示全部锁定

    vertices[0] = ColorVertex(-1.0f, 0.0f, 2.0f);        //设置三角形的顶点
    vertices[1] = ColorVertex(0.0f, 1.0f, 2.0f);
    vertices[2] = ColorVertex(1.0f, 0.0f, 2.0f);

    Triangle->Unlock();

}

void DXWapper::PrepareColoredTriangleBuffer()
{
    if (ColoredTriangle != 0)
        return;

    m_pDevice->CreateVertexBuffer(
        3 * sizeof(ColorVertex),
        D3DUSAGE_WRITEONLY,
        D3DFVF_XYZC,
        D3DPOOL_MANAGED,
        &ColoredTriangle,
        0
    );

    ColorVertex* v;
    ColoredTriangle->Lock(0, 0, (void**)&v, 0);

    v[0] = ColorVertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
    v[1] = ColorVertex(0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
    v[2] = ColorVertex(1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0,255));

    ColoredTriangle->Unlock();
}

void DXWapper::SetTexture(IDirect3DTexture9* texture)
{
    m_pDevice->SetTexture(0, texture);

    m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);               //纹理放大模式
    m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);               //纹理缩小模式
    m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);                //这个是自动创建不同大小纹理时用的模式

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);                        //Alpha通道混合
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);                   //这两个是混合模式
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);                               //关闭光照
    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);                       //填充模式
}

IDirect3DTexture9* DXWapper::CreateTexture(LPCWSTR filePath)
{
    IDirect3DTexture9* texture = 0;
    D3DXCreateTextureFromFile(m_pDevice, filePath, &texture);

    return texture;
}

void DXWapper::PrepareTexture()
{
    if (Texture == 0)
        D3DXCreateTextureFromFile(m_pDevice, TEXT("D:\\VisualStudioProject\\DirectX9.0learn\\DirectX9.0\\Resource\\boli2.jpg"), &Texture);

    m_pDevice->SetTexture(0, Texture);

    m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void DXWapper::PrepareDrawTextureBuffer()
{
    if (TextureBuffer != 0)
        return;

    m_pDevice->CreateVertexBuffer(
        6 * sizeof(NormalTexVertex),
        D3DUSAGE_WRITEONLY,
        D3DFVF_TEXTURE,
        D3DPOOL_MANAGED,
        &TextureBuffer,
        0);

    NormalTexVertex* v;
    TextureBuffer->Lock(0, 0, (void**)&v, 0);

    // quad built from two triangles, note texture coordinates:
    v[0] = NormalTexVertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[1] = NormalTexVertex(-1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[2] = NormalTexVertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    v[3] = NormalTexVertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[4] = NormalTexVertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[5] = NormalTexVertex(1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    TextureBuffer->Unlock();

    m_pDevice->SetRenderState(D3DRS_LIGHTING, false);

}

void DXWapper::PrepareCubeBuffer()
{
    if (CubeBuffer != 0 && CubeIndex != 0)
        return;

    m_pDevice->CreateVertexBuffer(
        8 * sizeof(Vertex),
        D3DUSAGE_WRITEONLY,
        D3DFVF_XYZ,
        D3DPOOL_MANAGED,
        &CubeBuffer,
        0
    );

    //
// Fill the buffers with the cube data.
//

// define unique vertices:
    Vertex* vertices;
    CubeBuffer->Lock(0, 0, (void**)&vertices, 0);

    // vertices of a unit cube
    vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
    vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
    vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
    vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
    vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
    vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
    vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
    vertices[7] = Vertex(1.0f, -1.0f, 1.0f);

    CubeBuffer->Unlock();


    m_pDevice->CreateIndexBuffer(
        36 * sizeof(WORD),
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &CubeIndex,
        0);

    // define the triangles of the cube:
    WORD* indices = 0;
    CubeIndex->Lock(0, 0, (void**)&indices, 0);

    // front side
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    // back side
    indices[6] = 4; indices[7] = 6; indices[8] = 5;
    indices[9] = 4; indices[10] = 7; indices[11] = 6;

    // left side
    indices[12] = 4; indices[13] = 5; indices[14] = 1;
    indices[15] = 4; indices[16] = 1; indices[17] = 0;

    // right side
    indices[18] = 3; indices[19] = 2; indices[20] = 6;
    indices[21] = 3; indices[22] = 6; indices[23] = 7;

    // top
    indices[24] = 1; indices[25] = 5; indices[26] = 6;
    indices[27] = 1; indices[28] = 6; indices[29] = 2;

    // bottom
    indices[30] = 4; indices[31] = 0; indices[32] = 3;
    indices[33] = 4; indices[34] = 3; indices[35] = 7;

    CubeIndex->Unlock();
}

void DXWapper::DrawCube(float timeDelta)
{
    PrepareCubeBuffer();
    SetCubeMatrix();
    //
// spin the cube:
//
    D3DXMATRIX Rx, Ry;

    // rotate 45 degrees on x-axis
    D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);

    // incremement y-rotation angle each frame
    static float y = 0.0f;
    D3DXMatrixRotationY(&Ry, y);
    y += timeDelta;

    // reset angle to zero when angle reaches 2*PI
    if (y >= 6.28f)
        y = 0.0f;

    // combine x- and y-axis rotation transformations.
    D3DXMATRIX p = Rx * Ry;

    m_pDevice->SetTransform(D3DTS_WORLD, &p);

    //
    // draw the scene:
    //
    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
    m_pDevice->BeginScene();

    m_pDevice->SetFVF(D3DFVF_XYZ);
    m_pDevice->SetStreamSource(0, CubeBuffer, 0, sizeof(Vertex));
    m_pDevice->SetIndices(CubeIndex);


    // Draw cube.
    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

    m_pDevice->EndScene();
    m_pDevice->Present(0, 0, 0, 0);
}

void DXWapper::PrepareCustomBuffer()
{
    if (CustomBuffer != 0)
        return;

    m_pDevice->CreateVertexBuffer(
        12 * sizeof(NormalTexVertex),
        D3DUSAGE_WRITEONLY,
        D3DFVF_TEXTURE,
        D3DPOOL_MANAGED,
        &CustomBuffer,
        0
    );
    NormalTexVertex* vertices;
    CustomBuffer->Lock(0, 0, (void**)&vertices, 0);

    vertices[0] = NormalTexVertex(0.0f, 2.0f, 2.0f, 0, 0, 0, 0, 0);
    vertices[1] = NormalTexVertex(2.82f, 2.0f, 4.82f, 0, 0, 0, 1, 0);
    vertices[2] = NormalTexVertex(2.82f, -2.0f, 4.82f, 0, 0, 0, 1, 1);
    vertices[3] = NormalTexVertex(0.0f, 2.0f, 2.0f, 0, 0, 0, 0, 0);
    vertices[4] = NormalTexVertex(2.82f, -2.0f, 4.82f, 0, 0, 0, 1, 1);
    vertices[5] = NormalTexVertex(0.0f, -2.0f, 2.0f, 0, 0, 0, 0, 1);
    vertices[6] = NormalTexVertex(-2.82f, 2.0f, 4.82f, 0, 0, 0, 0, 0);
    vertices[7] = NormalTexVertex(0.0f, 2.0f, 2.0f, 0, 0, 0, 1, 0);
    vertices[8] = NormalTexVertex(0.0f, -2.0f, 2.0f, 0, 0, 0, 1, 1);
    vertices[9] = NormalTexVertex(-2.82f, 2.0f, 4.82f, 0, 0, 0, 0, 0);
    vertices[10] = NormalTexVertex(0.0f, -2.0f, 2.0f, 0, 0, 0, 1, 1);
    vertices[11] = NormalTexVertex(-2.82f, -2.0f, 4.82f, 0, 0, 0, 0, 1);

    CustomBuffer->Unlock();
}

void DXWapper::DrawCustom()
{
    PrepareCustomBuffer();
    SetCustomMatrix();

    if (CusTexture1 == 0)
        CusTexture1 = CreateTexture(L"D:\\VisualStudioProject\\DirectX9.0learn\\DirectX9.0\\Resource\\boliTexture.dds");

    if (CusTexture2 == 0)
        CusTexture2 = CreateTexture(L"D:\\VisualStudioProject\\DirectX9.0learn\\DirectX9.0\\Resource\\boli2.jpg");

//
// draw the scene:
//
    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
    SetTexture(CusTexture1);                    //设置纹理
    m_pDevice->BeginScene();

    m_pDevice->SetFVF(D3DFVF_TEXTURE);
    m_pDevice->SetStreamSource(0, CustomBuffer, 0, sizeof(NormalTexVertex));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 3, 1);


    m_pDevice->EndScene();

    SetTexture(CusTexture2);                    //设置纹理
    m_pDevice->BeginScene();

    m_pDevice->SetFVF(D3DFVF_TEXTURE);
    m_pDevice->SetStreamSource(0, CustomBuffer, 0, sizeof(NormalTexVertex));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 1);
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 9, 1);

    m_pDevice->EndScene();

    m_pDevice->Present(0, 0, 0, 0);
}

void DXWapper::DrawColoredTriangle()
{
    PrepareColoredTriangleBuffer();
    SetColoredTriangleMatrix();

    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
    m_pDevice->BeginScene();

    m_pDevice->SetFVF(D3DFVF_XYZC);
    m_pDevice->SetStreamSource(0, ColoredTriangle, 0, sizeof(ColorVertex));

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, -1.25f, 0.0f, 0.0f);                           // 在左侧绘制-1.25处绘制
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);                         //填充模式为全部填充首个点的颜色
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    D3DXMatrixTranslation(&WorldMatrix, 1.25f, 0.0f, 0.0f);                             //在右侧1.25处绘制
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);                       //填充模式为以顶点间颜色渐变
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1); 

    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

    m_pDevice->EndScene();
    m_pDevice->Present(0, 0, 0, 0);
}

void DXWapper::DrawTexture()
{
    PrepareTexture();
    PrepareDrawTextureBuffer();
    SetTextureMatrix();

    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF00ffff, 1.0f, 0);
    m_pDevice->BeginScene();

    m_pDevice->SetFVF(D3DFVF_TEXTURE);
    m_pDevice->SetStreamSource(0, TextureBuffer, 0, sizeof(NormalTexVertex));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    m_pDevice->EndScene();
    m_pDevice->Present(0, 0, 0, 0);
}

void DXWapper::SetCustomMatrix()
{
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,                        // result
        D3DX_PI * 0.5f,               // 90 - degrees
        (float)m_Parameter.WindowsHeight / (float)m_Parameter.WindowsHeight, // aspect ratio
        1.0f,                         // near plane
        1000.0f);                     // far plane
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    //
    // Set wireframe mode render state.
    //
    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pDevice->SetTexture(0, 0);

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

    //
// Position and aim the camera.
//
    D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &V);
}

void DXWapper::SetCubeMatrix()
{
    //
// Set the projection matrix.
//
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,
        D3DX_PI * 0.3f, // 90 - degree
        (float)m_Parameter.WindowsHeight / (float)m_Parameter.WindowsHeight,
        1.0f,
        1000.0f);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    //
    // Switch to wireframe mode.
    //

    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pDevice->SetTexture(0, 0);


//
// Position and aim the camera.
//
    D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);

    m_pDevice->SetTransform(D3DTS_VIEW, &V);
}

void DXWapper::SetTriangleMatrix()
{
    //
// Set the projection matrix.
//
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,                                                                                  //结果
        D3DX_PI * 0.5f,                                                                         // 90 - degree，视角90°
        (float)m_Parameter.WindowsWidth / (float)m_Parameter.WindowsHeight,
        1.0f,                                                                                   //近面距离1
        1000.0f);                                                                               //远面距离1000
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    //
    // Set wireframe mode render state.
    //

    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);                               //线填充模式，只画线不填充颜色                       
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);                                    //Alpha混合禁用
    m_pDevice->SetTexture(0, 0);                                                                //纹理取消

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);                                         //设置世界坐标到原点


    D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);                                                     //观察者位置
    D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);                                                       //观察的点
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);                                                           //摄像头的上位置，可以想成是怎么拿着摄像头拍摄的。这个向量指的是摄像头上方所指
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &V);                                                    //设置观察者坐标
}

void DXWapper::SetColoredTriangleMatrix()
{
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,                                                                                  //结果
        D3DX_PI * 0.5f,                                                                         // 90 - degree，视角90°
        (float)m_Parameter.WindowsWidth / (float)m_Parameter.WindowsHeight,
        1.0f,                                                                                   //近面距离1
        1000.0f);                                                                               //远面距离1000
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);                                           //关闭光照，在开启光照下是黑色的，应该是没设置光源的原因吧
    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);                                   //如果这个设置成D3DFILL_WIREFRAME，或者点模式，中间部分不会被填充
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);                                   //Alpha混合关闭
    m_pDevice->SetTexture(0, 0);                                                                //取消纹理素材（如果有的话）

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);                                         //设置世界坐标到原点

    D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &V);                                                    //设置观察者坐标
}

void DXWapper::SetTextureMatrix()
{
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,                                                                                  //结果
        D3DX_PI * 0.5f,                                                                         // 90 - degree，视角90°
        (float)m_Parameter.WindowsWidth / (float)m_Parameter.WindowsHeight,
        1.0f,                                                                                   //近面距离1
        1000.0f);                                                                               //远面距离1000
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);                                         //设置

    D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &V);
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
        m_pVideoHwnd,                   //目标窗口,和D3dprameter的保持一致
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
