//
// Created by admin on 2025/5/21.
//

#include <head.h>
#include "Graphic.h" // 假设这里有 CGraphic 和 MyImageInfo 的定义
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
        case WM_DESTROY://窗口被销毁
        {
            if(g_pDogModel){
                delete g_pDogModel;
                g_pDogModel = nullptr;
            }
            PostQuitMessage(0);//退出消息
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
    //初始化窗口
    HWND hwnd=CGraphic::GetSingleObjPtr()->InitWindow(EventProc,800,600,_T("DX_3D_Dog_Model")); //
    CGraphic::GetSingleObjPtr()->InitDX(hwnd); //
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice; // 获取设备指针

    // 创建你的旧几何体 (圆柱和立方体)
//    D3DXCreateCylinder(pd3dDevice,1,1,10,10,10,&g_pMesh,nullptr); //
//    D3DXCreateBox(pd3dDevice,2,3,1,&g_pMesh2,nullptr); //

    // 创建和加载我们的 .x 模型 (Dog)
    g_pDogModel = new Model("Doggy"); // 给模型一个名字
    // ======== 非常重要：文件路径！ ========
    // 1. 将你上传的 dog.txt 重命名为 dog.x
    // 2. 在你的 .exe 文件所在的目录下，创建一个名为 "Assets" 的文件夹
    // 3. 将 dog.x 和 dog.jpg (虽然简化版渲染暂时不用jpg) 复制到这个 "Assets" 文件夹中
    // 如果路径不正确，LoadModelFromFile 会失败。
    if (g_pDogModel->LoadModelFromFile("Assets/dog.x")) {
        // 模型加载成功，设置其初始变换
        g_pDogModel->transform.SetPosition(0.0f, 0.0f, 0.0f); // 放在世界原点
        g_pDogModel->transform.SetScale(0.01f, 0.01f, 0.01f);    // .x 模型通常比较大，大幅度缩小
        // g_pDogModel->transform.SetRotationEuler(0, D3DX_PI / 2.0f, 0); // 例如，让狗旋转90度
    } else {
        MessageBox(hwnd, _T("错误：无法加载 Assets/dog.x 模型！\n请检查文件路径和文件是否存在。"), _T("模型加载失败"), MB_OK | MB_ICONERROR);
        // 考虑是否要退出程序或如何处理
    }

    // 设置光照 (只需要设置一次，除非你想改变光照)
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // 开启光照计算
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL; // 平行光
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f; // 白色漫反射
    light.Ambient.r = 0.3f; light.Ambient.g = 0.3f; light.Ambient.b = 0.3f; // 环境光分量
    light.Specular.r = 0.5f; light.Specular.g = 0.5f; light.Specular.b = 0.5f; // 高光分量
    D3DXVECTOR3 vecDir(0.5f, -0.5f, 0.5f); // 光照方向 (从斜上方照下)
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE); // 启用0号光源
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(60, 60, 60)); // 设置全局环境光，增加整体亮度
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // 如果模型有缩放，让法线自动归一化

    // 深度测试 (确保在 Graphic::InitDX 中已正确设置和启用)
    // 你的 Graphic.cpp 中 d3dpp.EnableAutoDepthStencil = true;
    // 确保 Clear 时也清除了Z缓冲 (D3DCLEAR_ZBUFFER)
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); // 显式启用Z缓冲
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL); // 深度测试函数

    // 视图和投影矩阵 (只需要在参数改变时重新设置)
    D3DXMATRIX matView; //
    D3DXMATRIX matProj; //
    D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI/4.0f, 800.0f/600.0f, 1.0f, 1000.0f); // 修正了FOV参数，使用浮点数
    pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj); //

    // 用于旧几何体的变换矩阵
//    D3DXMATRIX matWorldCylinder, matWorldBox; // 为每个物体使用独立的世界矩阵变量
//    D3DXMATRIX matMesh2Rotation, matMesh2Translation, matMeshRotation, matMeshTranslation; //
//    D3DXMatrixTranslation(&matMesh2Translation,0,5,0); // 立方体向上平移
//    D3DXMatrixTranslation(&matMeshTranslation,5,0,0);   // 圆柱体向右平移

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
            // 更新游戏逻辑
            // float deltaTime = ... (如果需要基于时间的动画或物理)
            if (g_pDogModel) {
                // 示例：让狗模型缓慢旋转
                // g_pDogModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.001f);
                g_pDogModel->Update(0.0f); // 即使Update为空，调用它也是个好习惯
            }


            // 更新视图矩阵 (因为 vEye 可能通过键盘改变了)
            D3DXMatrixLookAtLH(&matView,&vEye,&vAt,&vUp); //
            pd3dDevice->SetTransform(D3DTS_VIEW,&matView); //

            CGraphic::GetSingleObjPtr()->BeginDraw(); //

            // --- 渲染圆柱体 (你的旧代码) ---
//            if (g_pMesh) {
//                g_mesh_rotation_angle_x+=0.005f; // 减慢旋转速度
//                D3DXMatrixRotationX(&matMeshRotation,g_mesh_rotation_angle_x); //
//                D3DXMatrixMultiply(&matWorldCylinder, &matMeshRotation, &matMeshTranslation); // 使用 matWorldCylinder
//                pd3dDevice->SetTransform(D3DTS_WORLD,&matWorldCylinder); //
//                g_pMesh->DrawSubset(0); //
//            }
//
//            // --- 渲染立方体 (你的旧代码) ---
//            if (g_pMesh2) {
//                g_mesh2_rotation_angle_y+=0.005f; // 减慢旋转速度
//                D3DXMatrixRotationY(&matMesh2Rotation, g_mesh2_rotation_angle_y); //
//                D3DXMatrixMultiply(&matWorldBox, &matMesh2Rotation, &matMesh2Translation); // 使用 matWorldBox
//                pd3dDevice->SetTransform(D3DTS_WORLD,&matWorldBox); //
//                g_pMesh2->DrawSubset(0); //
//            }

            // --- 渲染我们的 .x 模型 (Dog) ---
            if (g_pDogModel) {
                g_pDogModel->Render(); // Model::Render() 内部会设置它自己的世界矩阵
                // 基于 g_pDogModel->transform 的值
            }

            CGraphic::GetSingleObjPtr()->EndDraw(); //
            // Sleep(1); // 建议移除或用更精确的帧率控制器替代
        }
    }

    // 清理资源 (WM_DESTROY 中也会做，但这里是退出循环后的最终保障)
    if (g_pDogModel) { delete g_pDogModel; g_pDogModel = nullptr; }
    if (g_pMesh) { g_pMesh->Release(); g_pMesh = nullptr; }
    if (g_pMesh2) { g_pMesh2->Release(); g_pMesh2 = nullptr; }

    // CGraphic 的 CloseDX() 应该由其自身的单例析构或专门的关闭函数处理
    // 如果 CGraphic::CloseDX() 需要显式调用，可以在这里加上

    return 0;
}
