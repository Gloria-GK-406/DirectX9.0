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
        D3DADAPTER_DEFAULT,     //����ʾ������
        D3DDEVTYPE_HAL,         //Ӳ����ʽʹ��D3D9
        &caps2                   //��ȡ�豸Caps
    );

    D3DDISPLAYMODE d3ddm;
    _d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm);

    UINT adepterCount =  _d3d9->GetAdapterCount();

    DWORD VertextType = GetVertexProcessType(caps2);

    if (!CreateVideoWnd(paramter))
        return FALSE;

    //Setp2 ���D3DPRESENT_PARAMETER�Գ�ʼ��Device
    _D3DPRESENT_PARAMETERS_ d3dpp = InitD3DParameter(paramter);

    if (FAILED(CreateDevice(_d3d9,d3dpp,VertextType)))
    {
        Release(_d3d9);
        ::MessageBox(0, TEXT("Device����ʧ��!"), 0, 0);
        return false;
    }

    Release(_d3d9);
    return true;
}

void DXWapper::DrawTriangle()
{
    PrepareTriangleBuffer();
    SetTriangleMatrix();

    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);            //�ò�͸����ɫ��䱳��
    m_pDevice->BeginScene();                                                                    //����ǰ����

    m_pDevice->SetFVF(D3DFVF_XYZC);                                                             //�������Ķ�������
    m_pDevice->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex));                            //���ö�����

    // Draw one triangle.
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);                                         //����һ�������Σ��Ӷ���0��ʼ��Ҳ������0��1��2����������һ��������

    m_pDevice->EndScene();                                                                      //���ƽ���ʱ����
    m_pDevice->Present(0, 0, 0, 0);                                                             //�����ˢ�µ���Ļ��ˢ��ʱ����device��ʼ��ʱ�����й�
}

void DXWapper::PrepareTriangleBuffer()
{
    if (Triangle != 0)
        return;

    m_pDevice->CreateVertexBuffer(
        3 * sizeof(ColorVertex),                       //buffer��С����������3������
        D3DUSAGE_WRITEONLY,                     //ֻд�����ɶ�
        D3DFVF_XYZC,                             //�����ʽΪxyz
        D3DPOOL_MANAGED,                        //�洢���ڴ�λ�ã����Դ棬�����
        &Triangle,                              //ָ��洢λ�õ�ָ��
        0                                       //SharedHandle,��ʹ�ã�����Ϊ0
    );

    ColorVertex* vertices;
    Triangle->Lock(0, 0, (void**)&vertices, 0); //1��Offset��2��Size��3��������ָ�룬4����־λ��������google����2Ϊ0ʱ����ʾȫ������

    vertices[0] = ColorVertex(-1.0f, 0.0f, 2.0f);        //���������εĶ���
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

    m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);               //����Ŵ�ģʽ
    m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);               //������Сģʽ
    m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);                //������Զ�������ͬ��С����ʱ�õ�ģʽ

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);                        //Alphaͨ�����
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);                   //�������ǻ��ģʽ
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);                               //�رչ���
    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);                       //���ģʽ
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
    SetTexture(CusTexture1);                    //��������
    m_pDevice->BeginScene();

    m_pDevice->SetFVF(D3DFVF_TEXTURE);
    m_pDevice->SetStreamSource(0, CustomBuffer, 0, sizeof(NormalTexVertex));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 3, 1);


    m_pDevice->EndScene();

    SetTexture(CusTexture2);                    //��������
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
    D3DXMatrixTranslation(&WorldMatrix, -1.25f, 0.0f, 0.0f);                           // ��������-1.25������
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);                         //���ģʽΪȫ������׸������ɫ
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    D3DXMatrixTranslation(&WorldMatrix, 1.25f, 0.0f, 0.0f);                             //���Ҳ�1.25������
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);                       //���ģʽΪ�Զ������ɫ����
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
        &proj,                                                                                  //���
        D3DX_PI * 0.5f,                                                                         // 90 - degree���ӽ�90��
        (float)m_Parameter.WindowsWidth / (float)m_Parameter.WindowsHeight,
        1.0f,                                                                                   //�������1
        1000.0f);                                                                               //Զ�����1000
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    //
    // Set wireframe mode render state.
    //

    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);                               //�����ģʽ��ֻ���߲������ɫ                       
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);                                    //Alpha��Ͻ���
    m_pDevice->SetTexture(0, 0);                                                                //����ȡ��

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);                                         //�����������굽ԭ��


    D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);                                                     //�۲���λ��
    D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);                                                       //�۲�ĵ�
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);                                                           //����ͷ����λ�ã������������ô��������ͷ����ġ��������ָ��������ͷ�Ϸ���ָ
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &V);                                                    //���ù۲�������
}

void DXWapper::SetColoredTriangleMatrix()
{
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,                                                                                  //���
        D3DX_PI * 0.5f,                                                                         // 90 - degree���ӽ�90��
        (float)m_Parameter.WindowsWidth / (float)m_Parameter.WindowsHeight,
        1.0f,                                                                                   //�������1
        1000.0f);                                                                               //Զ�����1000
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);                                           //�رչ��գ��ڿ����������Ǻ�ɫ�ģ�Ӧ����û���ù�Դ��ԭ���
    m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);                                   //���������ó�D3DFILL_WIREFRAME�����ߵ�ģʽ���м䲿�ֲ��ᱻ���
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);                                   //Alpha��Ϲر�
    m_pDevice->SetTexture(0, 0);                                                                //ȡ�������زģ�����еĻ���

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);                                         //�����������굽ԭ��

    D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &V);                                                    //���ù۲�������
}

void DXWapper::SetTextureMatrix()
{
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,                                                                                  //���
        D3DX_PI * 0.5f,                                                                         // 90 - degree���ӽ�90��
        (float)m_Parameter.WindowsWidth / (float)m_Parameter.WindowsHeight,
        1.0f,                                                                                   //�������1
        1000.0f);                                                                               //Զ�����1000
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    D3DXMATRIX WorldMatrix;
    D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 0.0f);
    m_pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);                                         //����

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

    d3dpp.BackBufferWidth = paramter.WindowsWidth;                                  //��̨���������
    d3dpp.BackBufferHeight = paramter.WindowsHeight;                                //�߶�
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;                       //�豸ʹ�õ�ɫ�ʸ�ʽ
    d3dpp.BackBufferCount = 1;                                      //��̨��������
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;                    //���ز�������,����ȥ��ݵ�
    d3dpp.MultiSampleQuality = 0;                                   //���ز�������
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;                       //ˢ�·�ʽ,DISCARDΪϵͳ�Զ�ѡ��,���忴�ʼ�
    d3dpp.hDeviceWindow = m_pVideoHwnd;                                     //Ŀ�괰�ھ��
    d3dpp.Windowed = TRUE;                                         //�Ƿ�Ϊ���ڻ�
    d3dpp.EnableAutoDepthStencil = TRUE;                             
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;                    //��������
    d3dpp.Flags = 0;                                                //��֪
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;     //ˢ����,Ĭ��
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;     //���������Ǵ�ֱͬ��
    d3dpp.EnableAutoDepthStencil = TRUE;

    return d3dpp;
}

HRESULT DXWapper::CreateDevice(IDirect3D9* _d3d9, _D3DPRESENT_PARAMETERS_ d3dpp,DWORD VertextType)
{
    //Setp3 ����D3DDevice
    HRESULT hr = _d3d9->CreateDevice(
        D3DADAPTER_DEFAULT,             //ϣ��ʹ�õ��Կ����ĸ�
        D3DDEVTYPE_HAL,                 //CPUģ�⻹��GPUģʽ,HAL��GPUģʽ
        m_pVideoHwnd,                   //Ŀ�괰��,��D3dprameter�ı���һ��
        VertextType,                    //��������ģʽ,GPU������CPU����
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
