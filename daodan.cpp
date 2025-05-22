//
//#include <windows.h>
//#include <tchar.h>
//#include <vector>
//#include <cmath>
//#include <random>
//#include <ctime>
//#include <debugapi.h> // **** 用于 OutputDebugString ****
//#include <stdio.h>    // **** 用于 _stprintf_s ****
//
//#include "head.h"    // 假设您包含了必要的定义
//#include "Graphic.h" // 包含您的图形类
//
//// --- 全局变量 ---
//CGraphic* g_pGraphics = nullptr;
//MyImageInfo g_missileImageInfo;
//HWND g_hWnd = nullptr;
//
//// 导弹结构体
//struct Missile {
//    D3DXVECTOR3 position; // 当前位置
//    D3DXVECTOR3 velocity; // 速度向量 (方向 * 速度)
//    float       rotation; // 旋转角度 (弧度)
//    bool        active;   // 是否活动
//};
//
//std::vector<Missile> g_missiles; // 存储所有导弹的容器
//const float MISSILE_SPEED = 300.0f; // 导弹速度 (像素/秒)
//const int WINDOW_WIDTH = 800;
//const int WINDOW_HEIGHT = 600;
//
//// 用于计算帧间隔时间 (Delta Time)
//LARGE_INTEGER g_timerFrequency;
//LARGE_INTEGER g_lastTime;
//
//// 随机数生成器 (C++11)
//std::mt19937 g_randomEngine(static_cast<unsigned int>(time(nullptr)));
//std::uniform_real_distribution<float> g_randomAngleDist(0.0f, 2.0f * D3DX_PI);
//
//// --- 函数声明 ---
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//void LaunchMissile();
//void UpdateMissiles(float deltaTime);
//void RenderMissiles();
//void InitTimer();
//float GetDeltaTime();
//
//// --- WinMain ---
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//    // 获取图形实例 (使用您代码中提供的函数名)
//    g_pGraphics = CGraphic::GetSingleObjPtr(); // !! 再次确认这个函数有效 !!
//    if (!g_pGraphics) {
//        MessageBox(NULL, _T("获取 CGraphic 实例失败!"), _T("错误"), MB_OK | MB_ICONERROR);
//        return -1;
//    }
//
//    // 初始化窗口 (使用您代码中提供的函数名)
//    g_hWnd = g_pGraphics->InitWindow(MainWndProc, WINDOW_WIDTH, WINDOW_HEIGHT, _T("发射导弹测试"), _T("MissileTestClass")); // !! 确保 InitWindow 已取消注释 !!
//    if (!g_hWnd) {
//        MessageBox(NULL, _T("初始化窗口失败!"), _T("错误"), MB_OK | MB_ICONERROR);
//        return -1;
//    }
//
//    // 初始化 DirectX (使用您代码中提供的函数名)
//    g_pGraphics->InitDX(g_hWnd); // !! 确保 InitDX 已取消注释且有效 !!
//    if (!g_pGraphics->m_pDevice) // 假设 m_pDevice 是可访问的用于检查
//    {
//        MessageBox(NULL, _T("DirectX 初始化失败 (m_pDevice is NULL)!"), _T("错误"), MB_OK | MB_ICONERROR);
//        DestroyWindow(g_hWnd);
//        return -1;
//    }
//
//    // 加载导弹纹理
//    // **** 务必修改为您的导弹图片的正确相对路径 ****
//    TCHAR imagePath[] = _T("C:\\Users\\admin\\CLionProjects\\DX\\daoDan.png");
//    OutputDebugString(_T("正在尝试加载纹理: "));
//    OutputDebugString(imagePath);
//    OutputDebugString(_T("\n"));
//    if (FAILED(g_pGraphics->LoadTex(imagePath, g_missileImageInfo, 0xFFFFFFFF))) // !! 确保 LoadTex 已取消注释且有效 !!
//    {
//        TCHAR errorMsg[MAX_PATH + 100];
//        _stprintf_s(errorMsg, _T("加载纹理失败: %s\n请确保文件路径正确且文件存在于输出目录或指定位置！"), imagePath);
//        MessageBox(g_hWnd, errorMsg, _T("错误"), MB_OK | MB_ICONERROR);
//        OutputDebugString(_T("纹理加载失败!\n"));
//        // return -1; // 可以选择退出或继续运行
//    }
//    else
//    {
//        OutputDebugString(_T("纹理加载成功!\n"));
//        if (g_missileImageInfo.pTex) {
//            TCHAR texInfoMsg[100];
//            _stprintf_s(texInfoMsg, _T("纹理尺寸: %d x %d\n"), g_missileImageInfo.width, g_missileImageInfo.height);
//            OutputDebugString(texInfoMsg);
//        } else {
//            OutputDebugString(_T("警告: LoadTex 返回成功，但 pTex 为 NULL!\n"));
//        }
//    }
//
//
//    InitTimer(); // 初始化计时器
//
//    OutputDebugString(_T("进入主消息循环...\n"));
//
//    // 主消息循环
//    MSG msg = {0};
//    while (msg.message != WM_QUIT)
//    {
//        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//        else
//        {
//            float deltaTime = GetDeltaTime();
//            UpdateMissiles(deltaTime);
//
//            g_pGraphics->BeginDraw(); // !! 确保 BeginDraw 已取消注释且有效 !!
//            RenderMissiles();
//            g_pGraphics->EndDraw();   // !! 确保 EndDraw 已取消注释且有效 !!
//        }
//    }
//    OutputDebugString(_T("退出主消息循环。\n"));
//
//    // 清理资源
//    if (g_missileImageInfo.pTex) {
//        g_missileImageInfo.pTex->Release();
//        g_missileImageInfo.pTex = nullptr;
//        OutputDebugString(_T("导弹纹理已释放。\n"));
//    }
//    // g_pGraphics->CloseDX(); // 如果有清理函数
//
//    return (int)msg.wParam;
//}
//
//// --- 窗口过程函数 ---
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    switch (msg)
//    {
//        case WM_KEYDOWN:
//        {
//            TCHAR debugMsg[100];
//            _stprintf_s(debugMsg, _T("WM_KEYDOWN received! wParam = 0x%X (%d)\n"), wParam, wParam);
//            OutputDebugString(debugMsg); // 输出收到的按键消息
//
//            if (wParam == VK_SPACE) { // VK_SPACE 的值是 32 (0x20)
//                OutputDebugString(_T(">>> VK_SPACE detected! Calling LaunchMissile...\n")); // 确认空格键被检测到
//                LaunchMissile(); // 调用发射函数
//            }
//        }
//            break;
//        case WM_DESTROY:
//            OutputDebugString(_T("WM_DESTROY received. Posting Quit Message.\n"));
//            PostQuitMessage(0);
//            break;
//        default:
//            return DefWindowProc(hwnd, msg, wParam, lParam);
//    }
//    return 0;
//}
//
//// --- 功能函数 ---
//
///**
// * @brief 初始化高精度计时器。
// */
//void InitTimer()
//{
//    QueryPerformanceFrequency(&g_timerFrequency);
//    QueryPerformanceCounter(&g_lastTime);
//}
//
///**
// * @brief 获取自上次调用 GetDeltaTime() 以来经过的时间（增量时间）。
// * @return float 返回的时间差，以秒为单位。
// */
//float GetDeltaTime()
//{
//    LARGE_INTEGER currentTime;
//    QueryPerformanceCounter(&currentTime);
//    float deltaTime = static_cast<float>(currentTime.QuadPart - g_lastTime.QuadPart)
//                      / static_cast<float>(g_timerFrequency.QuadPart);
//    g_lastTime = currentTime;
//    if (deltaTime > 0.1f) { deltaTime = 0.1f; }
//    if (deltaTime < 0.0f) { deltaTime = 0.0f; }
//    return deltaTime;
//}
//
///**
// * @brief 发射一枚导弹。
// */
//void LaunchMissile() {
//    OutputDebugString(_T("--- LaunchMissile called.\n"));
//
//    Missile newMissile;
//    newMissile.position = D3DXVECTOR3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f);
//    float angle = g_randomAngleDist(g_randomEngine);
//    D3DXVECTOR3 direction = D3DXVECTOR3(cosf(angle), sinf(angle), 0.0f);
//    D3DXVec3Normalize(&direction, &direction); // 确保是单位向量
//    newMissile.velocity = direction * MISSILE_SPEED;
//    newMissile.rotation = atan2f(direction.y, direction.x);
//    newMissile.active = true;
//
//    g_missiles.push_back(newMissile);
//
//    TCHAR debugMsg[200];
//    _stprintf_s(debugMsg, _T("--- Missile added. Pos=(%.1f, %.1f), Vel=(%.1f, %.1f), Rot=%.2f, Total missiles: %zu\n"),
//                newMissile.position.x, newMissile.position.y,
//                newMissile.velocity.x, newMissile.velocity.y,
//                newMissile.rotation,
//                g_missiles.size());
//    OutputDebugString(debugMsg);
//}
//
///**
// * @brief 更新所有活动导弹的位置和状态。
// */
//void UpdateMissiles(float deltaTime) {
//    // 如果没有导弹，直接返回
//    if (g_missiles.empty()) return;
//
//    // 输出更新信息（可选，可能会刷屏）
//    // TCHAR updateMsg[50];
//    // _stprintf_s(updateMsg, _T("UpdateMissiles called. DeltaTime: %.4f\n"), deltaTime);
//    // OutputDebugString(updateMsg);
//
//    for (auto it = g_missiles.begin(); it != g_missiles.end(); /* iterator controlled inside */) {
//        if (it->active) {
//            it->position += it->velocity * deltaTime;
//            // 检查边界 (使用 g_missileImageInfo 尺寸做一点缓冲)
//            float marginX = g_missileImageInfo.width > 0 ? g_missileImageInfo.width : 50.0f; // 如果图片未加载，用默认值
//            float marginY = g_missileImageInfo.height > 0 ? g_missileImageInfo.height : 50.0f;
//            if (it->position.x < -marginX || it->position.x > WINDOW_WIDTH + marginX ||
//                it->position.y < -marginY || it->position.y > WINDOW_HEIGHT + marginY) {
//                it->active = false; // 标记为非活动
//                OutputDebugString(_T("Missile deactivated (out of bounds).\n"));
//            }
//            ++it; // 移动到下一个
//        } else {
//            // 移除非活动导弹
//            it = g_missiles.erase(it);
//            OutputDebugString(_T("Inactive missile removed from vector.\n"));
//        }
//    }
//}
//
///**
// * @brief 渲染所有活动的导弹。
// */
//void RenderMissiles() {
//    if (!g_missileImageInfo.pTex) {
//        // 已经在 WinMain 加载时检查过了，这里可以省略调试输出，或者保留
//        // OutputDebugString(_T("RenderMissiles skipped: Texture not loaded.\n"));
//        return;
//    }
//
//    // 检查图片信息是否有效
//    if (g_missileImageInfo.width == 0 || g_missileImageInfo.height == 0) {
//        OutputDebugString(_T("RenderMissiles Warning: Missile texture info width or height is zero.\n"));
//        // 可能无法正确计算中心点，可以尝试给个默认值或直接返回
//        // return;
//    }
//
//
//    D3DXVECTOR3 center = D3DXVECTOR3(g_missileImageInfo.width / 2.0f, g_missileImageInfo.height / 2.0f, 0.0f);
//    bool drewAny = false;
//
//    for (const auto& missile : g_missiles) {
//        if (missile.active) {
//            drewAny = true;
//            // 输出绘制信息 (可选)
//            // TCHAR drawMsg[150];
//            // _stprintf_s(drawMsg, _T("Rendering missile at (%.1f, %.1f)\n"), missile.position.x, missile.position.y);
//            // OutputDebugString(drawMsg);
//
//            g_pGraphics->DrawTex( // !! 确保 DrawTex 已取消注释且有效 !!
//                    g_missileImageInfo,
//                    missile.position,
//                    center,
//                    D3DXVECTOR3(0, 0, missile.rotation),
//                    D3DXVECTOR3(1, 1, 1)
//            );
//        }
//    }
//
//    // 如果容器不为空但没有绘制任何东西，可能是所有导弹都 inactive 了
//    if (!drewAny && !g_missiles.empty()) {
//        OutputDebugString(_T("RenderMissiles: Found missiles in vector, but none were active to draw.\n"));
//    }
//}
//
//
