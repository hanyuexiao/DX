//
// Created by admin on 2025/5/21.
//

#include <head.h>
#include "Graphic.h" // 假设这里有 CGraphic 和 MyImageInfo 的定义

LPD3DXMESH g_pMesh = nullptr;
LPD3DXMESH g_pMesh2 = nullptr;
float g_mesh2_rotation_angle_y = 0.0f;
float g_mesh_rotation_angle_x = 0.0f;

D3DXVECTOR3 vEye{0,0,-25};
D3DXVECTOR3 vAt{0,0,0};
D3DXVECTOR3 vUp{0,1,0};

LRESULT EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY://窗口被销毁
        {
            PostQuitMessage(0);//退出消息
            break;
        }
        case WM_KEYDOWN:
        {
            if(wParam == 'A'){
                vEye.x -= 0.1f;
            }
            if(wParam == 'D'){
                vEye.x += 0.1f;
            }
            if(wParam == 'W'){
                vEye.z += 0.1f;
            }
            if(wParam == 'S'){
                vEye.z -= 0.1f;
            }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{

    //初始化窗口
    HWND hwnd=CGraphic::GetSingleObjPtr()->InitWindow(EventProc,800,600,_T("DX_3D"));
    CGraphic::GetSingleObjPtr()->InitDX(hwnd);

    D3DXCreateCylinder(CGraphic::GetSingleObjPtr()->m_pDevice,1,1,10,10,10,&g_pMesh,nullptr);
    D3DXCreateBox(CGraphic::GetSingleObjPtr()->m_pDevice,2,3,1,&g_pMesh2,nullptr);



    D3DXMATRIX matWorld;
    D3DXMatrixTranslation(&matWorld,0,0,0);
    CGraphic::GetSingleObjPtr()->m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);


    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView,&vEye,&vAt,&vUp);
    CGraphic::GetSingleObjPtr()->m_pDevice->SetTransform(D3DTS_VIEW,&matView);


    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI/4,800.0f/600.0f,1.0f,100.0f);
    CGraphic::GetSingleObjPtr()->m_pDevice->SetTransform(D3DTS_PROJECTION,&matProj);

    D3DVIEWPORT9 viewPort;
    viewPort.X = 0;
    viewPort.Y = 0;
    viewPort.Width = 800;
    viewPort.Height = 600;
    viewPort.MinZ = 0.0f;
    viewPort.MaxZ = 1.0f;
    CGraphic::GetSingleObjPtr()->m_pDevice->GetViewport(&viewPort);


    D3DXMATRIX matMesh2Rotation;
    D3DXMATRIX matMesh2Translation;
    D3DXMatrixTranslation(&matMesh2Translation,0,5,0);
    D3DXMATRIX matMeshRotation;
    D3DXMATRIX matMeshTranslation;
    D3DXMatrixTranslation(&matMeshTranslation,5,0,0);

    while (TRUE)
    {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);

            DispatchMessage(&msg);
        }
        else
        {
            D3DXMatrixLookAtLH(&matView,&vEye,&vAt,&vUp);
            CGraphic::GetSingleObjPtr()->m_pDevice->SetTransform(D3DTS_VIEW,&matView);

            CGraphic::GetSingleObjPtr()->BeginDraw();

            D3DXMatrixTranslation(&matWorld,0,0,0);
            g_mesh_rotation_angle_x+=0.01f;
            D3DXMatrixRotationX(&matMeshRotation,g_mesh_rotation_angle_x);
            D3DXMatrixMultiply(&matWorld, &matMeshRotation, &matMeshTranslation);
            CGraphic::GetSingleObjPtr()->m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);
            g_pMesh->DrawSubset(0);



            D3DXMatrixTranslation(&matWorld,0,0,0);
            g_mesh2_rotation_angle_y+=0.01f;
            D3DXMatrixRotationY(&matMesh2Rotation, g_mesh2_rotation_angle_y);
            D3DXMatrixMultiply(&matWorld, &matMesh2Rotation, &matMesh2Translation);
            CGraphic::GetSingleObjPtr()->m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);
            g_pMesh2->DrawSubset(0);

            CGraphic::GetSingleObjPtr()->EndDraw();
            Sleep(1);
        }

        if (msg.message == WM_QUIT)
            break;
    }
    return 0;
}