// main.cpp

#include "head.h"
#include "Graphic.h"
#include "Model.h"
#include "CPlayer.h"
#include "CGameObject.h" //
#include "CTransform.h"  //
#include "CCamera.h"     // <<-- 新增：包含相机头文件

// 全局变量
Player* g_pFbxModel = nullptr;
Camera* g_pMainCamera = nullptr;

// 窗口事件处理函数
LRESULT EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY: {
            if (g_pFbxModel) {
                delete g_pFbxModel;
                g_pFbxModel = nullptr;
            }
            if (g_pMainCamera) {
                delete g_pMainCamera;
                g_pMainCamera = nullptr;
            }
            PostQuitMessage(0);
            break;
        }
        case WM_KEYDOWN: {
            if (g_pMainCamera) {
                float moveSpeed = 1.5f;
                float rotateSpeed = 0.03f; // 旋转速度 (弧度)

//                if (wParam == 'A') { g_pMainCamera->Strafe(-moveSpeed); }
//                if (wParam == 'D') { g_pMainCamera->Strafe(moveSpeed); }
//                if (wParam == 'W') { g_pMainCamera->MoveForward(moveSpeed); }
//                if (wParam == 'S') { g_pMainCamera->MoveForward(-moveSpeed); }
//                if (wParam == 'Q') { g_pMainCamera->Fly(moveSpeed); }
//                if (wParam == 'E') { g_pMainCamera->Fly(-moveSpeed); }

                // <<-- 新增：使用箭头键控制相机旋转 (简单示例)
                if (wParam == VK_LEFT) { g_pMainCamera->RotateYaw(-rotateSpeed); }
                if (wParam == VK_RIGHT) { g_pMainCamera->RotateYaw(rotateSpeed); }
                if (wParam == VK_UP) { g_pMainCamera->RotatePitch(-rotateSpeed); } // 向上看，俯仰角减小
                if (wParam == VK_DOWN) { g_pMainCamera->RotatePitch(rotateSpeed); } // 向下看，俯仰角增大
            }
        }
        case WM_MOUSEWHEEL: {
            if (g_pMainCamera) {
                int zDelta = GET_WHEEL_DELTA_WPARAM(wParam); // 获取滚轮滚动的距离和方向
                float zoomSpeedFactor = 0.05f; // 调整滚轮缩放的灵敏度

                if (zDelta > 0) {
                    // 滚轮向前滚动 (通常是放大/拉近)
                    g_pMainCamera->MoveForward(static_cast<float>(zDelta) / WHEEL_DELTA * zoomSpeedFactor * 10.0f);
                } else if (zDelta < 0) {
                        // 滚轮向后滚动 (通常是缩小/拉远)
                        g_pMainCamera->MoveForward(static_cast<float>(zDelta) / WHEEL_DELTA * zoomSpeedFactor * 10.0f);
                    }
                }
                return 0;
            }

    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 主函数
INT _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd) {
    // 1. 初始化窗口
    HWND hwnd = CGraphic::GetSingleObjPtr()->InitWindow(EventProc, 1600, 1200, _T("DX_3D_FBX_Model_Loader_With_Camera")); //
    if (!hwnd) { /* ...错误处理... */ return -1; }

    // 2. 初始化Direct3D
    if (!CGraphic::GetSingleObjPtr()->InitDX(hwnd)) { /* ...错误处理... */ return -1; } //
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;

    // <<-- 新增：创建和初始化相机 -->>
    g_pMainCamera = new Camera("MainCamera");
    // 设置相机初始位置
    g_pMainCamera->transform.SetPosition(0.0f, 20.0f, -100.0f); //

    // 设置相机初始朝向，使其看向原点 (0,0,0)
    D3DXVECTOR3 targetLookAt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 camPos = g_pMainCamera->transform.position;
    D3DXVECTOR3 direction = targetLookAt - camPos;
    D3DXVec3Normalize(&direction, &direction);

    float initialYaw = atan2(direction.x, direction.z);
    float initialPitch = asin(-direction.y);
    // CTransform.h 中有 SetRotationEuler
    g_pMainCamera->transform.SetRotationEuler(initialYaw, initialPitch, 0.0f);


    // 设置相机投影参数 (来自 CCamera.h 的 public 成员)
    g_pMainCamera->fieldOfViewY = D3DX_PI / 4.0f; // 45度视场角
    // 宽高比应该从窗口获取，如果 CGraphic 提供了窗口尺寸的话
    float windowWidth = 1600.0f;
    float windowHeight = 1200.0f;
    g_pMainCamera->aspectRatio = windowWidth / windowHeight;
    g_pMainCamera->nearPlane = 1.0f;
    g_pMainCamera->farPlane = 2000.0f; //
    g_pMainCamera->UpdateProjectionMatrix(); // 计算并存储投影矩阵

    // 3. 创建和加载模型
    g_pFbxModel = new  Player("MyFBX_Model"); //
    const char* fbxModelPath = "C:/Users/admin/CLionProjects/DX/cmake-build-release/Assets/shigeju.fbx"; //
    if (g_pFbxModel->LoadFBXModelFromFile(fbxModelPath)) { //
        g_pFbxModel->transform.SetPosition(0.0f, -20.0f, 0.0f); //
        g_pFbxModel->transform.SetScale(1.f, 1.f, 1.f);    //
        g_pFbxModel->transform.SetRotationEuler(0, D3DXToRadian(90.0f), 0); //
    } else { /* ...错误处理... */ }

    // 4. 设置光照
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f;
    light.Ambient.r = 0.4f; light.Ambient.g = 0.4f; light.Ambient.b = 0.4f;
    light.Specular.r = 0.7f; light.Specular.g = 0.7f; light.Specular.b = 0.7f;
    D3DXVECTOR3 vecDir(0.5f, -0.8f, 0.75f);
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(80, 80, 80));
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

    // 5. 设置渲染状态 (与之前相同)
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // 7. 进入主循环
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    // float deltaTime = 0.016f; // 固定的时间步长，用于Update，或者你可以实现一个计时器

    while (msg.message != WM_QUIT) { //
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // 更新逻辑
            // float currentFrameTime = ... ; // 获取当前时间
            // float deltaTime = currentFrameTime - lastFrameTime;
            // lastFrameTime = currentFrameTime;
            float deltaTime = 0.016f; // 暂时使用固定值

            if (g_pMainCamera) {
                g_pMainCamera->Update(deltaTime);
            }

            if (g_pFbxModel) {
                // g_pFbxModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.0005f); // 示例旋转
                g_pFbxModel->Update(deltaTime); // 调用Update
            }

            // 设置视图矩阵 (<<-- 修改：从相机获取 -->>)
            if (g_pMainCamera) {
                pd3dDevice->SetTransform(D3DTS_VIEW, &g_pMainCamera->GetViewMatrix());
            }

            // 设置投影矩阵 (<<-- 新增：从相机获取, 确保在相机初始化和窗口大小改变时也更新了 -->>)
            if (g_pMainCamera) {
                // 如果窗口大小可能改变，这里可能需要重新计算宽高比并更新投影矩阵
                // g_pMainCamera->aspectRatio = new_width / new_height;
                // g_pMainCamera->UpdateProjectionMatrix();
                pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_pMainCamera->GetProjectionMatrix());
            }


            // 渲染
            CGraphic::GetSingleObjPtr()->BeginDraw(); //
            if (g_pFbxModel) {
                g_pFbxModel->Render(pd3dDevice); //
            }
            CGraphic::GetSingleObjPtr()->EndDraw(); //
        }
    }

    // 8. 清理资源
    if (g_pFbxModel) { delete g_pFbxModel; g_pFbxModel = nullptr; } //
    if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; } // <<-- 新增：清理相机

    return (int)msg.wParam;
}