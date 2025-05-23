//
// Created by admin on 2025/5/21.
//

#include <head.h>
#include "Graphic.h" // ���������� CGraphic �� MyImageInfo �Ķ���
#include "Model.h"
#include "CTransform.h"
#include "CGameObject.h"



LPD3DXMESH g_pMesh = nullptr;
LPD3DXMESH g_pMesh2 = nullptr;
float g_mesh2_rotation_angle_y = 0.0f;
float g_mesh_rotation_angle_x = 0.0f;

D3DXVECTOR3 vEye{0,20,-100};
D3DXVECTOR3 vAt{0,0,0};
D3DXVECTOR3 vUp{0,1,0};

Model* g_pDogModel = nullptr;

LRESULT EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY://���ڱ�����
        {
            if(g_pDogModel){
                delete g_pDogModel;
                g_pDogModel = nullptr;
            }
            PostQuitMessage(0);//�˳���Ϣ
            break;
        }
        case WM_KEYDOWN:
        {
            if(wParam == 'A'){
                vEye.x -= 0.5f;
            }
            if(wParam == 'D'){
                vEye.x += 0.5f;
            }
            if(wParam == 'W'){
                vEye.z += 0.5f;
            }
            if(wParam == 'S'){
                vEye.z -= 0.5f;
            }
            if(wParam == 'Q'){
                vEye.y += 0.5f;
            }
            if(wParam == 'E'){
                vEye.y -= 0.5f;
            }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
    //��ʼ������
    HWND hwnd=CGraphic::GetSingleObjPtr()->InitWindow(EventProc,800,600,_T("DX_3D_Dog_Model")); //
    CGraphic::GetSingleObjPtr()->InitDX(hwnd); //
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice; // ��ȡ�豸ָ��

    // ������ľɼ����� (Բ����������)
//    D3DXCreateCylinder(pd3dDevice,1,1,10,10,10,&g_pMesh,nullptr); //
//    D3DXCreateBox(pd3dDevice,2,3,1,&g_pMesh2,nullptr); //

    // �����ͼ������ǵ� .x ģ�� (Dog)
    g_pDogModel = new Model("Doggy"); // ��ģ��һ������
    // ======== �ǳ���Ҫ���ļ�·���� ========
    // 1. �����ϴ��� dog.txt ������Ϊ dog.x
    // 2. ����� .exe �ļ����ڵ�Ŀ¼�£�����һ����Ϊ "Assets" ���ļ���
    // 3. �� dog.x �� dog.jpg (��Ȼ�򻯰���Ⱦ��ʱ����jpg) ���Ƶ���� "Assets" �ļ�����
    // ���·������ȷ��LoadModelFromFile ��ʧ�ܡ�
    if (g_pDogModel->LoadModelFromFile("Assets/dog.x")) {
        // ģ�ͼ��سɹ����������ʼ�任
        g_pDogModel->transform.SetPosition(0.0f, 0.0f, 0.0f); // ��������ԭ��
        g_pDogModel->transform.SetScale(0.01f, 0.01f, 0.01f);    // .x ģ��ͨ���Ƚϴ󣬴������С
        // g_pDogModel->transform.SetRotationEuler(0, D3DX_PI / 2.0f, 0); // ���磬�ù���ת90��
    } else {
        MessageBox(hwnd, _T("�����޷����� Assets/dog.x ģ�ͣ�\n�����ļ�·�����ļ��Ƿ���ڡ�"), _T("ģ�ͼ���ʧ��"), MB_OK | MB_ICONERROR);
        // �����Ƿ�Ҫ�˳��������δ���
    }

    // ���ù��� (ֻ��Ҫ����һ�Σ���������ı����)
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // �������ռ���
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL; // ƽ�й�
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f; // ��ɫ������
    light.Ambient.r = 0.3f; light.Ambient.g = 0.3f; light.Ambient.b = 0.3f; // ���������
    light.Specular.r = 0.5f; light.Specular.g = 0.5f; light.Specular.b = 0.5f; // �߹����
    D3DXVECTOR3 vecDir(0.5f, -0.5f, 0.5f); // ���շ��� (��б�Ϸ�����)
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE); // ����0�Ź�Դ
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(60, 60, 60)); // ����ȫ�ֻ����⣬������������
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // ���ģ�������ţ��÷����Զ���һ��

    // ��Ȳ��� (ȷ���� Graphic::InitDX ������ȷ���ú�����)
    // ��� Graphic.cpp �� d3dpp.EnableAutoDepthStencil = true;
    // ȷ�� Clear ʱҲ�����Z���� (D3DCLEAR_ZBUFFER)
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); // ��ʽ����Z����
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL); // ��Ȳ��Ժ���

    // ��ͼ��ͶӰ���� (ֻ��Ҫ�ڲ����ı�ʱ��������)
    D3DXMATRIX matView; //
    D3DXMATRIX matProj; //
    D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI/4.0f, 800.0f/600.0f, 1.0f, 1000.0f); // ������FOV������ʹ�ø�����
    pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj); //

    // ���ھɼ�����ı任����
//    D3DXMATRIX matWorldCylinder, matWorldBox; // Ϊÿ������ʹ�ö���������������
//    D3DXMATRIX matMesh2Rotation, matMesh2Translation, matMeshRotation, matMeshTranslation; //
//    D3DXMatrixTranslation(&matMesh2Translation,0,5,0); // ����������ƽ��
//    D3DXMatrixTranslation(&matMeshTranslation,5,0,0);   // Բ��������ƽ��

    while (TRUE)
    {
        MSG msg; //
        ZeroMemory(&msg, sizeof(MSG));
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //
        {
            if (msg.message == WM_QUIT) //
                break;
            TranslateMessage(&msg); //
            DispatchMessage(&msg);  //
        }
        else
        {
            // ������Ϸ�߼�
            // float deltaTime = ... (�����Ҫ����ʱ��Ķ���������)
            if (g_pDogModel) {
                // ʾ�����ù�ģ�ͻ�����ת
                // g_pDogModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.001f);
                g_pDogModel->Update(0.0f); // ��ʹUpdateΪ�գ�������Ҳ�Ǹ���ϰ��
            }


            // ������ͼ���� (��Ϊ vEye ����ͨ�����̸ı���)
            D3DXMatrixLookAtLH(&matView,&vEye,&vAt,&vUp); //
            pd3dDevice->SetTransform(D3DTS_VIEW,&matView); //

            CGraphic::GetSingleObjPtr()->BeginDraw(); //

            // --- ��ȾԲ���� (��ľɴ���) ---
//            if (g_pMesh) {
//                g_mesh_rotation_angle_x+=0.005f; // ������ת�ٶ�
//                D3DXMatrixRotationX(&matMeshRotation,g_mesh_rotation_angle_x); //
//                D3DXMatrixMultiply(&matWorldCylinder, &matMeshRotation, &matMeshTranslation); // ʹ�� matWorldCylinder
//                pd3dDevice->SetTransform(D3DTS_WORLD,&matWorldCylinder); //
//                g_pMesh->DrawSubset(0); //
//            }
//
//            // --- ��Ⱦ������ (��ľɴ���) ---
//            if (g_pMesh2) {
//                g_mesh2_rotation_angle_y+=0.005f; // ������ת�ٶ�
//                D3DXMatrixRotationY(&matMesh2Rotation, g_mesh2_rotation_angle_y); //
//                D3DXMatrixMultiply(&matWorldBox, &matMesh2Rotation, &matMesh2Translation); // ʹ�� matWorldBox
//                pd3dDevice->SetTransform(D3DTS_WORLD,&matWorldBox); //
//                g_pMesh2->DrawSubset(0); //
//            }

            // --- ��Ⱦ���ǵ� .x ģ�� (Dog) ---
            if (g_pDogModel) {
                g_pDogModel->Render(); // Model::Render() �ڲ����������Լ����������
                // ���� g_pDogModel->transform ��ֵ
            }

            CGraphic::GetSingleObjPtr()->EndDraw(); //
            // Sleep(1); // �����Ƴ����ø���ȷ��֡�ʿ��������
        }
    }

    // ������Դ (WM_DESTROY ��Ҳ���������������˳�ѭ��������ձ���)
    if (g_pDogModel) { delete g_pDogModel; g_pDogModel = nullptr; }
    if (g_pMesh) { g_pMesh->Release(); g_pMesh = nullptr; }
    if (g_pMesh2) { g_pMesh2->Release(); g_pMesh2 = nullptr; }

    // CGraphic �� CloseDX() Ӧ����������ĵ���������ר�ŵĹرպ�������
    // ��� CGraphic::CloseDX() ��Ҫ��ʽ���ã��������������

    return 0;
}
