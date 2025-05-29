
#include "head.h"      // 应包含windows.h, d3dx9.h, tchar.h等头文件
#include "Graphic.h"
#include "CPlayer.h"   // 玩家类
#include "CCamera.h"   // 相机类
#include "CFrustum.h"  // 视锥体类
#include <iostream>    // 用于调试输出
#include <vector>      // 用于存储顶点数据

// 全局变量
Player* g_pPlayer = nullptr;
Camera* g_pMainCamera = nullptr;
HWND    g_hWnd = nullptr;

// 视锥体绘制相关变量
CFrustum* g_pFrozenFrustum = nullptr;     // 用于存储冻结状态的视锥体对象
D3DXMATRIX  g_matFrozenView;              // 冻结时的视图矩阵
D3DXMATRIX  g_matFrozenProj;              // 冻结时的投影矩阵
LPDIRECT3DVERTEXBUFFER9 g_pFrustumVB;
bool     g_bDrawFrozenFrustum;
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
    if (deltaTime > 0.1f) deltaTime = 0.1f; // 防止时间跳跃过大
    return deltaTime;
}

LRESULT CALLBACK EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            if (g_pPlayer) { delete g_pPlayer; g_pPlayer = nullptr; }
            if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; }
            if (g_pFrozenFrustum) { delete g_pFrozenFrustum; g_pFrozenFrustum = nullptr; }
            if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            // 相机和玩家控制
            // (假设如果相机未绑定到玩家，则使用方向键/WASD控制相机)
            if (g_pMainCamera) {
                float camMoveSpeed = 50.0f * GetDeltaTime(); // 根据需要调整
                float camRotateSpeed = 1.0f * GetDeltaTime(); // 根据需要调整

                if (GetAsyncKeyState('W') & 0x8000) g_pMainCamera->RotatePitch(camRotateSpeed);
                if (GetAsyncKeyState('S') & 0x8000) g_pMainCamera->RotatePitch(-camRotateSpeed);
                if (GetAsyncKeyState('A') & 0x8000) g_pMainCamera->RotateYaw(-camRotateSpeed);
                if (GetAsyncKeyState('D') & 0x8000) g_pMainCamera->RotateYaw(camRotateSpeed);
                if(GetAsyncKeyState('Q') & 0x8000) g_pMainCamera->RotatePitch(camRotateSpeed);
                if(GetAsyncKeyState('E') & 0x8000) g_pMainCamera->RotateYaw(-camRotateSpeed);
            }
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            return 0;

        case WM_MBUTTONDOWN: // 中键按下
            if (g_pMainCamera) {
                g_bDrawFrozenFrustum = true; // 启用绘制
                g_matFrozenView = g_pMainCamera->GetViewMatrix();
                g_matFrozenProj = g_pMainCamera->GetProjectionMatrix();

                if (!g_pFrozenFrustum) {
                    g_pFrozenFrustum = new CFrustum();
                }
                // 根据捕获的矩阵更新冻结视锥体的内部表示
                g_pFrozenFrustum->UpdateFrustum(g_matFrozenView, g_matFrozenProj);

                // 为冻结的视锥体重新创建或更新顶点缓冲区
                g_pFrozenFrustum->SetupFrustumBuffers(CGraphic::GetSingleObjPtr()->m_pDevice,g_pFrozenFrustum,g_pFrustumVB);
                std::cout << "中键按下。冻结的视锥体已更新。" << std::endl;
            }
            return 0;

        case WM_MOUSEWHEEL:
            if (g_pMainCamera) {
                int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                float zoomAmount = static_cast<float>(zDelta) / WHEEL_DELTA * 20.0f; // 增加缩放速度
                g_pMainCamera->MoveForward(zoomAmount);
            }
            return 0;

        case WM_RBUTTONDOWN:
            if (g_pPlayer) {
                POINTS pts = MAKEPOINTS(lParam);
                g_pPlayer->StartMouseRotation(pts.x, pts.y);
                SetCapture(hwnd);
                ShowCursor(FALSE);
            }
            return 0;

        case WM_RBUTTONUP:
            if (g_pPlayer) {
                g_pPlayer->StopMouseRotation();
                ReleaseCapture();
                ShowCursor(TRUE);
            }
            return 0;

        case WM_MOUSEMOVE:
            if (g_pPlayer && g_pPlayer->m_isMouseRotating) {
                POINTS ptsCurrent = MAKEPOINTS(lParam);
                int deltaX = ptsCurrent.x - g_pPlayer->m_lastMousePos.x;
                int deltaY = ptsCurrent.y - g_pPlayer->m_lastMousePos.y;
                g_pPlayer->ProcessMouseMovement(deltaX, deltaY);
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                POINT centerPoint = { (clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2 };
                ClientToScreen(hwnd, &centerPoint);
                SetCursorPos(centerPoint.x, centerPoint.y);
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
    g_hWnd = CGraphic::GetSingleObjPtr()->InitWindow(EventProc, 1600, 1200, _T("DX"));
    if (!g_hWnd) {
        MessageBox(NULL, _T("窗口初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR);
        return -1;
    }

    // 2. 初始化Direct3D
    if (!CGraphic::GetSingleObjPtr()->InitDX(g_hWnd)) {
        MessageBox(NULL, _T("Direct3D初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR);
        return -1;
    }
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;

    InitTimer();

    // 3. 创建并初始化相机
    g_pMainCamera = new Camera("MainCamera");
    g_pMainCamera->transform.SetPosition(0.0f, 10.0f, -30.0f); // 初始位置稍远
    // 相机投影设置
    float windowWidth = 1600.0f;
    float windowHeight = 1200.0f;
    RECT rc; GetClientRect(g_hWnd, &rc); windowWidth = (float)(rc.right - rc.left); windowHeight = (float)(rc.bottom - rc.top);
    g_pMainCamera->fieldOfViewY = D3DX_PI / 4.0f;
    g_pMainCamera->aspectRatio = windowWidth / (windowHeight == 0 ? 1.0f : windowHeight);
    g_pMainCamera->nearPlane = 1.0f;
    g_pMainCamera->farPlane = 3000.0f; // 增加远平面
    g_pMainCamera->UpdateProjectionMatrix();


    // 4. 创建并加载玩家模型
    g_pPlayer = new Player("HeroPlayer");
    g_pPlayer->transform.SetPosition(0.0f, 0.0f, 0.0f);
    const char* fbxModelPath = "C:\\Users\\admin\\CLionProjects\\DX\\cmake-build-debug\\Assets\\shigeju.fbx"; // 使用实际路径
    if (!g_pPlayer->LoadFBXModelFromFile(fbxModelPath)) {
        MessageBox(NULL, _T("加载FBX模型失败!"), _T("错误"), MB_OK | MB_ICONERROR);
    }

    // 5. 设置光照
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f;
    light.Ambient.r = 0.5f; light.Ambient.g = 0.5f; light.Ambient.b = 0.5f;
    D3DXVECTOR3 vecDir(0.5f, -0.8f, 0.75f);
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

    // 6. 设置渲染状态
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // 非透明物体的默认剔除

    // 7. 主循环
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            float deltaTime = GetDeltaTime();

            // 更新逻辑
            if (g_pPlayer) {
                g_pPlayer->Update(deltaTime);
            }
            if (g_pMainCamera) {
                // 如果相机独立或跟随玩家，在此更新其变换
                // 本例假设相机由按键/鼠标滚轮独立控制
                g_pMainCamera->UpdateViewMatrix(); // 确保移动后更新视图矩阵
            }

            // 为场景设置视图和投影矩阵
            if (g_pMainCamera) {
                pd3dDevice->SetTransform(D3DTS_VIEW, &g_pMainCamera->GetViewMatrix());
                pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_pMainCamera->GetProjectionMatrix());
            }

            // 渲染
            CGraphic::GetSingleObjPtr()->BeginDraw();

            // 先渲染玩家(或其他非透明物体)
            if (g_pPlayer) {
                g_pPlayer->Render(pd3dDevice);
            }

            // 渲染冻结的视锥体(透明，可能在非透明物体之后渲染)
            g_pFrozenFrustum->DrawFrozenFrustumFaces(pd3dDevice, g_bDrawFrozenFrustum,g_pFrustumVB);

            CGraphic::GetSingleObjPtr()->EndDraw();
        }
    }

    // 8. 清理资源
    if (g_pPlayer) { delete g_pPlayer; g_pPlayer = nullptr; }
    if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; }
    if (g_pFrozenFrustum) { delete g_pFrozenFrustum; g_pFrozenFrustum = nullptr; }
    if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }

    return (int)msg.wParam;
}