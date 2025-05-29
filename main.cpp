// main.cpp
#include "head.h"      // 应该包含 windows.h, d3dx9.h, tchar.h 等
#include "Graphic.h"
#include "CPlayer.h"   // 包含Player类
#include "CCamera.h"   // 包含Camera类
#include <iostream>    // 用于调试输出

// 全局变量
Player* g_pPlayer = nullptr; // 将 g_pFbxModel 重命名为 g_pPlayer 更清晰
Camera* g_pMainCamera = nullptr;
HWND    g_hWnd = nullptr; // 保存窗口句柄，方便 SetCapture/ReleaseCapture

// 计时器相关
LARGE_INTEGER g_ticksPerSecond;
LARGE_INTEGER g_lastFrameTime;

void InitTimer() {
    QueryPerformanceFrequency(&g_ticksPerSecond);
    QueryPerformanceCounter(&g_lastFrameTime);
}

float GetDeltaTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    float deltaTime = static_cast<float>(currentTime.QuadPart - g_lastFrameTime.QuadPart) / g_ticksPerSecond.QuadPart;
    g_lastFrameTime = currentTime;
    if (deltaTime > 0.1f) deltaTime = 0.1f; // 防止deltaTime过大导致跳跃
    return deltaTime;
}


// 窗口事件处理函数
LRESULT CALLBACK EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            if (g_pPlayer) { delete g_pPlayer; g_pPlayer = nullptr; }
            if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; }
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            // 相机控制 (如果需要独立的相机控制，或者让相机跟随玩家)
            if (g_pMainCamera) {
                // 示例：用箭头键控制相机（如果相机不跟随玩家）
                // float camMoveSpeed = 20.0f * GetDeltaTime(); // 假设GetDeltaTime可用
                // float camRotateSpeed = 1.0f * GetDeltaTime();
                // if (wParam == VK_LEFT)  g_pMainCamera->RotateYaw(-camRotateSpeed);
                // if (wParam == VK_RIGHT) g_pMainCamera->RotateYaw(camRotateSpeed);
                // if (wParam == VK_UP)    g_pMainCamera->RotatePitch(-camRotateSpeed);
                // if (wParam == VK_DOWN)  g_pMainCamera->RotatePitch(camRotateSpeed);
            }
            if (wParam == VK_ESCAPE) { // 按ESC退出
                PostQuitMessage(0);
            }
            return 0;

        case WM_MOUSEWHEEL:
            if (g_pMainCamera) { // 示例：用滚轮控制相机远近
                int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                float zoomAmount = static_cast<float>(zDelta) / WHEEL_DELTA * 5.0f; // 调整缩放速度
                g_pMainCamera->MoveForward(zoomAmount); // Camera类需要有MoveForward方法
            }
            return 0;

            // 玩家鼠标旋转控制
        case WM_RBUTTONDOWN: // 鼠标右键按下，开始旋转玩家
            if (g_pPlayer) {
                POINTS pts = MAKEPOINTS(lParam);
                g_pPlayer->StartMouseRotation(pts.x, pts.y);
                SetCapture(hwnd);    // 捕获鼠标输入到当前窗口
                ShowCursor(FALSE);   // 隐藏鼠标光标
            }
            return 0;

        case WM_RBUTTONUP:   // 鼠标右键抬起，停止旋转玩家
            if (g_pPlayer) {
                g_pPlayer->StopMouseRotation();
                ReleaseCapture();    // 释放鼠标捕获
                ShowCursor(TRUE);    // 显示鼠标光标
            }
            return 0;

        case WM_MOUSEMOVE:
            if (g_pPlayer && g_pPlayer->m_isMouseRotating) {
                POINTS ptsCurrent = MAKEPOINTS(lParam);
                int deltaX = ptsCurrent.x - g_pPlayer->m_lastMousePos.x;
                int deltaY = ptsCurrent.y - g_pPlayer->m_lastMousePos.y;

                g_pPlayer->ProcessMouseMovement(deltaX, deltaY);

                // 为了实现"无限"旋转，需要将鼠标重置回屏幕或窗口中心
                // 如果不重置，鼠标碰到屏幕边缘就无法继续旋转了
                // 这里简单地更新 lastMousePos，如果需要无限旋转，则需要 SetCursorPos
                // g_pPlayer->m_lastMousePos.x = ptsCurrent.x;
                // g_pPlayer->m_lastMousePos.y = ptsCurrent.y;
                // 替换为：
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                POINT centerPoint = { (clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2 };
                ClientToScreen(hwnd, &centerPoint); // 转换到屏幕坐标
                SetCursorPos(centerPoint.x, centerPoint.y);
                // 更新 m_lastMousePos 为重置后的屏幕中心点（转换回客户区坐标）
                ScreenToClient(hwnd, &centerPoint);
                g_pPlayer->m_lastMousePos = centerPoint;

            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 主函数
INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd) {
    // 1. 初始化窗口
    g_hWnd = CGraphic::GetSingleObjPtr()->InitWindow(EventProc, 1600, 1200, _T("DX Player Mouse Look"));
    if (!g_hWnd) {
        MessageBox(NULL, _T("窗口初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR);
        return -1;
    }

    // 2. 初始化Direct3D
    if (!CGraphic::GetSingleObjPtr()->InitDX(g_hWnd)) {
        MessageBox(NULL, _T("Direct3D 初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR);
        return -1;
    }
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;

    // 初始化计时器
    InitTimer();

    // 3. 创建和初始化相机
    g_pMainCamera = new Camera("MainCamera");
    g_pMainCamera->transform.SetPosition(0.0f, 10.0f, -20.0f); // 初始相机位置
    // 让相机看向玩家可能在的位置 (0,0,0) 或玩家初始位置
    // D3DXVECTOR3 playerInitialPos(0.0f, 0.0f, 0.0f); // 假设玩家初始在原点
    // g_pMainCamera->LookAt(playerInitialPos); // Camera类需要有LookAt方法

    // 设置相机投影参数
    float windowWidth = 1600.0f;  // 应从窗口实际尺寸获取
    float windowHeight = 1200.0f; // 应从窗口实际尺寸获取
    RECT rc; GetClientRect(g_hWnd, &rc); windowWidth = (float)(rc.right - rc.left); windowHeight = (float)(rc.bottom - rc.top);

    g_pMainCamera->fieldOfViewY = D3DX_PI / 4.0f; // 45度FOV
    g_pMainCamera->aspectRatio = windowWidth / (windowHeight == 0 ? 1.0f : windowHeight);
    g_pMainCamera->nearPlane = 1.0f;
    g_pMainCamera->farPlane = 2000.0f;
    g_pMainCamera->UpdateProjectionMatrix();


    // 4. 创建和加载玩家模型
    g_pPlayer = new Player("HeroPlayer");
    // 设置玩家初始位置和旋转 (如果需要)
    g_pPlayer->transform.SetPosition(0.0f, 0.0f, 0.0f);
    // 玩家的初始旋转应该已经在Player构造函数中通过m_currentYaw/Pitch设置了
    // g_pPlayer->transform.SetRotationEuler(0.0f, D3DXToRadian(0.0f), 0.0f);


    // 加载模型文件 - 使用你的实际路径
    const char* fbxModelPath = "C:\\Users\\admin\\CLionProjects\\DX\\cmake-build-debug\\Assets\\shigeju.fbx";
    if (!g_pPlayer->LoadFBXModelFromFile(fbxModelPath)) {
        MessageBox(NULL, _T("加载FBX模型失败!"), _T("错误"), MB_OK | MB_ICONERROR);
        // 决定是否继续运行，或者直接退出
    }


    // 5. 设置光照 (与之前类似)
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f;
    light.Ambient.r = 0.5f; light.Ambient.g = 0.5f; light.Ambient.b = 0.5f; // 稍微调亮环境光
    // light.Specular.r = 0.7f; light.Specular.g = 0.7f; light.Specular.b = 0.7f; // 高光可选
    D3DXVECTOR3 vecDir(0.5f, -0.8f, 0.75f); // 光源方向
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100)); // 全局环境光
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // 法线规格化

    // 6. 设置渲染状态
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // 或 D3DCULL_CW，取决于模型

    // 7. 进入主循环
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            float deltaTime = GetDeltaTime(); // 获取帧间隔时间

            // 更新逻辑
            if (g_pPlayer) {
                g_pPlayer->Update(deltaTime);
            }
            if (g_pMainCamera) {
//                 让相机跟随玩家 (简单的第三人称跟随)
//                 D3DXVECTOR3 playerPos = g_pPlayer->transform.position;
//                 D3DXVECTOR3 playerForward = g_pPlayer->transform.GetForward();
//                 D3DXVECTOR3 desiredCamPos = playerPos - playerForward * 15.0f + D3DXVECTOR3(0, 5.0f, 0); // 在玩家后上方
//                 g_pMainCamera->transform.SetPosition(desiredCamPos.x, desiredCamPos.y, desiredCamPos.z);
//                 g_pMainCamera->LookAt(playerPos + playerForward * 2.0f); // 看向玩家前方一点

                // 或者，如果玩家是第一人称，相机就是玩家的眼睛
                // g_pMainCamera->transform.position = g_pPlayer->transform.position + D3DXVECTOR3(0, 1.7f, 0); // 眼睛高度
                // g_pMainCamera->transform.orientation = g_pPlayer->transform.orientation; // 相机朝向与玩家一致
                // g_pMainCamera->UpdateViewMatrix(); // 更新视图矩阵

                // 当前代码是独立相机，如果想让相机跟随玩家，需要取消注释并调整上面的逻辑
                g_pMainCamera->Update(deltaTime); // 如果相机也有自己的更新逻辑 (如键盘控制)
            }

            // 设置视图和投影矩阵
            if (g_pMainCamera) {
                pd3dDevice->SetTransform(D3DTS_VIEW, &g_pMainCamera->GetViewMatrix());
                pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_pMainCamera->GetProjectionMatrix());
            }

            // 渲染
            CGraphic::GetSingleObjPtr()->BeginDraw();
            if (g_pPlayer) {
                g_pPlayer->Render(pd3dDevice);
            }
            // 可以在这里渲染其他对象，如场景、敌人等
            CGraphic::GetSingleObjPtr()->EndDraw();
        }
    }

    // 8. 清理资源
    if (g_pPlayer) { delete g_pPlayer; g_pPlayer = nullptr; }
    if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; }
    return (int)msg.wParam;
}
