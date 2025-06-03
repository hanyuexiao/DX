
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
bool        g_bDrawFrozenFrustum = false;  // 是否绘制冻结的视锥体
CFrustum* g_pFrozenFrustum = nullptr;     // 用于存储冻结状态的视锥体对象
D3DXMATRIX  g_matFrozenView;              // 冻结时的视图矩阵
D3DXMATRIX  g_matFrozenProj;              // 冻结时的投影矩阵
LPDIRECT3DVERTEXBUFFER9 g_pFrustumVB = nullptr; // 视锥体面的顶点缓冲区



// 计时器相关(假设这些定义在其他地方或如你之前的代码)
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

// 前置声明
void SetupFrustumBuffers(LPDIRECT3DDEVICE9 pd3dDevice);
void DrawFrozenFrustumFaces(LPDIRECT3DDEVICE9 pd3dDevice);


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
                if (GetAsyncKeyState('A') & 0x8000) g_pMainCamera->Strafe(-camMoveSpeed);
                if (GetAsyncKeyState('D') & 0x8000) g_pMainCamera->Strafe(camMoveSpeed);
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
                SetupFrustumBuffers(CGraphic::GetSingleObjPtr()->m_pDevice);
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


void SetupFrustumBuffers(LPDIRECT3DDEVICE9 pd3dDevice) {
    if (!pd3dDevice || !g_pFrozenFrustum) return;

    // 释放旧的缓冲区(如果存在)
    if (g_pFrustumVB) {
        g_pFrustumVB->Release();
        g_pFrustumVB = nullptr;
    }

    const D3DXVECTOR3* corners = g_pFrozenFrustum->GetWorldCorners();
    if (!corners) return;

    // 定义颜色(半透明)
    D3DCOLOR blueColor = D3DCOLOR_ARGB(128, 0, 0, 255);   // Alpha, R, G, B
    D3DCOLOR greenColor = D3DCOLOR_ARGB(128, 0, 255, 0);
    D3DCOLOR redColor = D3DCOLOR_ARGB(128, 255, 0, 0);
    D3DCOLOR yellowColor = D3DCOLOR_ARGB(128, 255, 255, 0);
    D3DCOLOR magentaColor = D3DCOLOR_ARGB(128, 255, 0, 255);
    D3DCOLOR cyanColor = D3DCOLOR_ARGB(128, 0, 255, 255);

    std::vector<FVF_FrustumVertex> vertices;
    vertices.reserve(36); // 6个面 * 每个面2个三角形 * 每个三角形3个顶点

    // 视锥体角点(假设CFrustum::GetWorldCorners()按此顺序返回)
    // N = 近, F = 远, T = 上, B = 下, L = 左, R = 右
    // m_vecClip[0] = 近左下, m_vecClip[1] = 近右下, m_vecClip[2] = 近右上, m_vecClip[3] = 近左上
    // m_vecClip[4] = 远左下, m_vecClip[5] = 远右下, m_vecClip[6] = 远右上, m_vecClip[7] = 远左上

    // 近面(蓝色) - 三角形: (0,1,2), (0,2,3)
    vertices.push_back({ corners[0], blueColor }); vertices.push_back({ corners[1], blueColor }); vertices.push_back({ corners[2], blueColor });
    vertices.push_back({ corners[0], blueColor }); vertices.push_back({ corners[2], blueColor }); vertices.push_back({ corners[3], blueColor });

    // 远面(蓝色) - 三角形: (4,7,6), (4,6,5) (外表面朝向)
    vertices.push_back({ corners[4], blueColor }); vertices.push_back({ corners[7], blueColor }); vertices.push_back({ corners[6], blueColor });
    vertices.push_back({ corners[4], blueColor }); vertices.push_back({ corners[6], blueColor }); vertices.push_back({ corners[5], blueColor });

    // 左面(绿色) - 三角形: (4,0,3), (4,3,7)
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[0], cyanColor }); vertices.push_back({ corners[3], greenColor });
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[3], cyanColor }); vertices.push_back({ corners[7], greenColor });

    // 右面(绿色) - 三角形: (1,5,6), (1,6,2)
    vertices.push_back({ corners[1], greenColor }); vertices.push_back({ corners[5],  yellowColor }); vertices.push_back({ corners[6], greenColor });
    vertices.push_back({ corners[1], greenColor }); vertices.push_back({ corners[6],  yellowColor }); vertices.push_back({ corners[2], greenColor });

    // 上面(绿色) - 三角形: (3,2,6), (3,6,7)
    vertices.push_back({ corners[3], greenColor }); vertices.push_back({ corners[2], redColor }); vertices.push_back({ corners[6], greenColor });
    vertices.push_back({ corners[3], greenColor }); vertices.push_back({ corners[6], redColor }); vertices.push_back({ corners[7], greenColor });

    // 下面(绿色) - 三角形: (4,5,1), (4,1,0)
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[5], greenColor }); vertices.push_back({ corners[1], greenColor });
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[1], greenColor }); vertices.push_back({ corners[0], greenColor });


    // 创建顶点缓冲区
    if (FAILED(pd3dDevice->CreateVertexBuffer(vertices.size() * sizeof(FVF_FrustumVertex),
                                              D3DUSAGE_WRITEONLY, D3DFVF_FRUSTUMVERTEX,
                                              D3DPOOL_MANAGED, &g_pFrustumVB, NULL))) { // D3DPOOL_MANAGED简化管理
        std::cerr << "创建视锥体顶点缓冲区失败!" << std::endl;
        return;
    }

    // 填充顶点缓冲区
    VOID* pVertices;
    if (FAILED(g_pFrustumVB->Lock(0, 0, (void**)&pVertices, 0))) {
        std::cerr << "锁定视锥体顶点缓冲区失败!" << std::endl;
        return;
    }
    memcpy(pVertices, vertices.data(), vertices.size() * sizeof(FVF_FrustumVertex));
    g_pFrustumVB->Unlock();
    std::cout << "视锥体顶点缓冲区创建/更新，包含 " << vertices.size() << " 个顶点。" << std::endl;
}

void DrawFrozenFrustumFaces(LPDIRECT3DDEVICE9 pd3dDevice) {
    // 如果不需要绘制或顶点缓冲无效，则直接返回
    if (!pd3dDevice || !g_bDrawFrozenFrustum || !g_pFrustumVB) {
        return;
    }

    // --- 1. 保存当前渲染状态 ---
    DWORD oldAlphaBlendEnable, oldSrcBlend, oldDestBlend;
    DWORD oldCullMode, oldLighting, oldZWriteEnable, oldZEnable;
    pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlendEnable);
    pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &oldSrcBlend);
    pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend);
    pd3dDevice->GetRenderState(D3DRS_CULLMODE, &oldCullMode);
    pd3dDevice->GetRenderState(D3DRS_LIGHTING, &oldLighting);
    pd3dDevice->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
    pd3dDevice->GetRenderState(D3DRS_ZENABLE, &oldZEnable); // 也保存Z缓冲启用状态

    // 保存纹理阶段状态 (以纹理阶段0为例，通常模型渲染会用到)
    // 如果你的模型可能使用更多纹理阶段，你也需要保存和恢复它们
    DWORD oldTSS_ColorOp_0, oldTSS_ColorArg1_0, oldTSS_ColorArg2_0;
    DWORD oldTSS_AlphaOp_0, oldTSS_AlphaArg1_0, oldTSS_AlphaArg2_0;
    DWORD oldTSS_ResultArg_0; // 有些驱动或旧代码可能会用到 D3DTSS_RESULTARG

    pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &oldTSS_ColorOp_0);
    pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &oldTSS_ColorArg1_0);
    pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &oldTSS_ColorArg2_0);
    pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &oldTSS_AlphaOp_0);
    pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &oldTSS_AlphaArg1_0);
    pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &oldTSS_AlphaArg2_0);
    // pd3dDevice->GetTextureStageState(0, D3DTSS_RESULTARG, &oldTSS_ResultArg_0); // 如果需要

    // 保存当前FVF，因为我们要改变它
    DWORD oldFVF;
    pd3dDevice->GetFVF(&oldFVF);


    // --- 2. 设置用于绘制视锥体的渲染状态 ---
    // 启用Alpha混合以实现半透明
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);   // 源混合因子: 源Alpha
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // 目标混合因子: 1 - 源Alpha

    // 关闭光照，因为视锥体颜色由顶点颜色定义
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    // 剔除模式：通常对于透明物体，会设置为D3DCULL_NONE来绘制双面，
    // 或者确保顶点顺序正确后使用D3DCULL_CCW（逆时针为正面）或D3DCULL_CW（顺时针为正面）。
    // 如果顶点顺序定义了明确的内外，可以只绘制正面。
    // 为了简单起见和确保能看到，先设置为D3DCULL_NONE。
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Z缓冲写入：对于透明物体，通常在绘制完所有不透明物体后，
    // 并且在绘制透明物体时禁用Z写入（设置为FALSE），以避免透明物体遮挡其后的其他透明物体。
    // 但如果只是一个孤立的视锥体，或者不关心复杂的透明排序，可以保持TRUE或设置为FALSE。
    // 设置为FALSE可以避免它完全写入深度缓冲，可能有助于解决一些透明物体排序问题。
    pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    // 确保Z测试仍然开启，以便视锥体能被场景中的不透明物体正确遮挡
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);


    // 设置纹理阶段状态，以使用顶点的漫反射颜色 (D3DTA_DIFFUSE)
    // 视锥体不使用纹理，所以颜色直接来自顶点
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    // 对于Alpha也一样
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    // 禁用后续可能被模型使用的纹理阶段，防止干扰
    // 如果你的模型只用到了纹理阶段0，那么禁用阶段1就足够了
    pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    // 可以根据需要禁用更多阶段


    // --- 3. 设置变换、FVF和顶点源，并绘制 ---
    // 设置世界变换矩阵为单位矩阵，因为顶点已经是世界坐标
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // 设置顶点格式 (FVF)
    pd3dDevice->SetFVF(D3DFVF_FRUSTUMVERTEX);

    // 设置顶点流来源
    pd3dDevice->SetStreamSource(0, g_pFrustumVB, 0, sizeof(FVF_FrustumVertex));

    // 绘制视锥体的面 (假设有12个三角形，即6个四边形面)
    // 每个四边形面由2个三角形组成，共 6 * 2 = 12 个三角形
    // 每个三角形有3个顶点，所以总共需要 12 * 3 = 36 个顶点在VB中
    pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12); // 12个三角形


    // --- 4. 恢复之前保存的渲染状态 ---
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlendEnable);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND, oldSrcBlend);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND, oldDestBlend);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, oldCullMode);
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, oldLighting);
    pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, oldZEnable); // 恢复Z缓冲启用状态

    // 恢复纹理阶段状态
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, oldTSS_ColorOp_0);
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, oldTSS_ColorArg1_0);
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, oldTSS_ColorArg2_0);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, oldTSS_AlphaOp_0);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, oldTSS_AlphaArg1_0);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, oldTSS_AlphaArg2_0);
    // pd3dDevice->SetTextureStageState(0, D3DTSS_RESULTARG, oldTSS_ResultArg_0); // 如果之前保存了

    // 如果之前禁用了纹理阶段1，并且你知道模型渲染时阶段1应该是什么状态，则恢复它
    // 通常，如果模型的渲染函数会自己设置所有需要的纹理阶段，这里可能不需要额外恢复阶段1。
    // 但为了更安全，如果知道模型默认会使用纹理阶段1（例如进行光照贴图），则应恢复其默认状态，
    // 否则，保持禁用或恢复到一个已知的“安全”状态。
    // 假设模型渲染会自己处理后续阶段，这里就不再恢复阶段1的状态，因为我们之前是强制禁用了它。
    // 或者，如果模型渲染依赖于阶段1被设置为某个特定值（如D3DTOP_DISABLE），那么恢复到那个值。

    // 恢复FVF
    pd3dDevice->SetFVF(oldFVF);
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
            DrawFrozenFrustumFaces(pd3dDevice);

            CGraphic::GetSingleObjPtr()->EndDraw();
        }
    }

    // 8. 清理资源
    if (g_pPlayer) { delete g_pPlayer; g_pPlayer = nullptr; }
    if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; }
    if (g_pFrozenFrustum) { delete g_pFrozenFrustum; g_pFrozenFrustum = nullptr; }
    if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }

    // CGraphic单例的清理应适当处理
    // CGraphic::GetSingleObjPtr()->Shutdown(); // 或类似方法

    return (int)msg.wParam;
}