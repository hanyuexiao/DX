#include "head.h"      // 应包含windows.h, d3dx9.h, tchar.h等头文件
#include "Graphic.h"
#include "CPlayer.h"   // 玩家类
#include "CCamera.h"   // 相机类
#include "CFrustum.h"  // 视锥体类
#include "CTerrain.h"  // 引入地形类头文件
#include <iostream>    // 用于调试输出
#include <vector>      // 用于存储顶点数据
#include <algorithm>   // For std::min/max (used for clamping)

// 全局变量
Player* g_pPlayer = nullptr;
Camera* g_pMainCamera = nullptr;
Terrain* g_pTerrain = nullptr;
HWND    g_hWnd = nullptr;

// 视锥体绘制相关变量
bool        g_bDrawFrozenFrustum = false;
CFrustum* g_pFrozenFrustum = nullptr;
D3DXMATRIX  g_matFrozenView;
D3DXMATRIX  g_matFrozenProj;
LPDIRECT3DVERTEXBUFFER9 g_pFrustumVB = nullptr;
LPDIRECT3DVERTEXBUFFER9 g_pFrustumLinesVB = nullptr;

// 轨道相机参数
float g_cameraOrbitYaw = D3DX_PI;        // 初始水平角度 (从玩家后方开始)
float g_cameraOrbitPitch = D3DX_PI / 6.0f; // 初始垂直角度 (稍微俯视)
float g_cameraDistance = 25.0f;          // 初始与玩家的距离 (增加初始距离)
const float MIN_CAMERA_PITCH = -D3DX_PI / 2.2f;
const float MAX_CAMERA_PITCH = D3DX_PI / 2.2f;
const float MIN_CAMERA_DISTANCE = 5.0f;   // 最小距离 (调整)
const float MAX_CAMERA_DISTANCE = 150.0f;  // 最大距离 (大幅增加)

const float CAMERA_KEY_ROTATE_SPEED = 1.5f; // 键盘控制的轨道相机旋转速度 (弧度/秒)
const float CAMERA_KEY_ZOOM_SPEED = 30.0f;   // 键盘控制的相机缩放速度 (单位/秒)


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
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    if (deltaTime < 0.0f) deltaTime = 0.00001f;
    return deltaTime;
}

// 前置声明
void SetupFrustumBuffers(LPDIRECT3DDEVICE9 pd3dDevice);
void DrawFrozenFrustumFaces(LPDIRECT3DDEVICE9 pd3dDevice);
void HandleOrbitCameraKeyboardInput(float deltaTime); // 新增：处理轨道相机键盘输入


LRESULT CALLBACK EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            if (g_pPlayer) { delete g_pPlayer; g_pPlayer = nullptr; }
            if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; }
            if (g_pTerrain) { delete g_pTerrain; g_pTerrain = nullptr; }
            if (g_pFrozenFrustum) { delete g_pFrozenFrustum; g_pFrozenFrustum = nullptr; }
            if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }
            if (g_pFrustumLinesVB) { g_pFrustumLinesVB->Release(); g_pFrustumLinesVB = nullptr; }
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            // 玩家的按键移动应在其 Update 方法中通过 GetAsyncKeyState 处理
            // 轨道相机的键盘控制将在主循环的 HandleOrbitCameraKeyboardInput 中处理
            return 0;

        case WM_MBUTTONDOWN:
            if (g_pMainCamera) {
                if (!g_bDrawFrozenFrustum) {
                    g_bDrawFrozenFrustum = true;
                    g_matFrozenView = g_pMainCamera->GetViewMatrix();
                    g_matFrozenProj = g_pMainCamera->GetProjectionMatrix();
                    if (!g_pFrozenFrustum) { g_pFrozenFrustum = new CFrustum(); }
                    g_pFrozenFrustum->UpdateFrustum(g_matFrozenView, g_matFrozenProj);
                    SetupFrustumBuffers(CGraphic::GetSingleObjPtr()->m_pDevice);
                } else {
                    g_bDrawFrozenFrustum = false;
                    if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }
                    if (g_pFrustumLinesVB) { g_pFrustumLinesVB->Release(); g_pFrustumLinesVB = nullptr; }
                }
            }
            return 0;

            // 移除了 WM_MOUSEWHEEL, WM_RBUTTONDOWN, WM_RBUTTONUP, WM_MOUSEMOVE 中的相机控制逻辑
            // Player 原有的鼠标控制逻辑（如果由不同按键触发）可以保留，但需注意协调
        case WM_RBUTTONDOWN: // 如果玩家仍然需要右键，保留此部分，否则可移除
            if (g_pPlayer) {
                // 假设这是玩家自身的鼠标控制，而不是之前相机的
                // g_pPlayer->StartMouseRotation(pts.x, pts.y);
                // SetCapture(hwnd);
                // ShowCursor(FALSE);
            }
            return 0;
        case WM_RBUTTONUP:
            if (g_pPlayer) {
                // g_pPlayer->StopMouseRotation();
                // ReleaseCapture();
                // ShowCursor(TRUE);
            }
            return 0;
        case WM_MOUSEMOVE:
            if (g_pPlayer && g_pPlayer->m_isMouseRotating) { // 假设这是玩家自身的鼠标控制
                POINTS ptsCurrent = MAKEPOINTS(lParam);
                int playerDeltaX = ptsCurrent.x - g_pPlayer->m_lastMousePos.x;
                int playerDeltaY = ptsCurrent.y - g_pPlayer->m_lastMousePos.y;
                g_pPlayer->ProcessMouseMovement(playerDeltaX, playerDeltaY);

                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                POINT centerPoint = { (clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2 };
                ClientToScreen(hwnd, &centerPoint);
                ScreenToClient(hwnd, &centerPoint);
                g_pPlayer->m_lastMousePos = centerPoint;
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 新增函数：处理轨道相机键盘输入
void HandleOrbitCameraKeyboardInput(float deltaTime) {
    if (!g_pMainCamera) return;

    float rotateAmount = CAMERA_KEY_ROTATE_SPEED * deltaTime;
    float zoomAmount = CAMERA_KEY_ZOOM_SPEED * deltaTime;

    // 水平轨道 (Yaw)
    if (GetAsyncKeyState('A') & 0x8000) {
        g_cameraOrbitYaw -= rotateAmount;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        g_cameraOrbitYaw += rotateAmount;
    }

    // 垂直轨道 (Pitch)
    if (GetAsyncKeyState('W') & 0x8000) {
        g_cameraOrbitPitch += rotateAmount;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        g_cameraOrbitPitch -= rotateAmount;
    }
    g_cameraOrbitPitch = (std::max)(MIN_CAMERA_PITCH, (std::min)(g_cameraOrbitPitch, MAX_CAMERA_PITCH));

    // 缩放 (Distance)
    if (GetAsyncKeyState('Q') & 0x8000) {
        g_cameraDistance -= zoomAmount;
    }
    if (GetAsyncKeyState('E') & 0x8000) {
        g_cameraDistance += zoomAmount;
    }
    g_cameraDistance = (std::max)(MIN_CAMERA_DISTANCE, (std::min)(g_cameraDistance, MAX_CAMERA_DISTANCE));
}


void SetupFrustumBuffers(LPDIRECT3DDEVICE9 pd3dDevice) {
    if (!pd3dDevice || !g_pFrozenFrustum) return;
    if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }
    if (g_pFrustumLinesVB) { g_pFrustumLinesVB->Release(); g_pFrustumLinesVB = nullptr; }
    const D3DXVECTOR3* corners = g_pFrozenFrustum->GetWorldCorners();
    if (!corners) return;
    D3DCOLOR blueColor = D3DCOLOR_ARGB(100, 0, 0, 255);
    D3DCOLOR greenColor = D3DCOLOR_ARGB(100, 0, 255, 0);
    D3DCOLOR redColor = D3DCOLOR_ARGB(100, 255, 0, 0);
    D3DCOLOR cyanColor = D3DCOLOR_ARGB(100, 0, 255, 255);
    D3DCOLOR yellowColor = D3DCOLOR_ARGB(100, 255, 255, 0);
    std::vector<FVF_FrustumVertex> faceVertices;
    faceVertices.reserve(36);
    faceVertices.push_back({ corners[0], blueColor }); faceVertices.push_back({ corners[1], blueColor }); faceVertices.push_back({ corners[2], blueColor });
    faceVertices.push_back({ corners[0], blueColor }); faceVertices.push_back({ corners[2], blueColor }); faceVertices.push_back({ corners[3], blueColor });
    faceVertices.push_back({ corners[4], blueColor }); faceVertices.push_back({ corners[7], blueColor }); faceVertices.push_back({ corners[6], blueColor });
    faceVertices.push_back({ corners[4], blueColor }); faceVertices.push_back({ corners[6], blueColor }); faceVertices.push_back({ corners[5], blueColor });
    faceVertices.push_back({ corners[4], greenColor }); faceVertices.push_back({ corners[0], cyanColor }); faceVertices.push_back({ corners[3], greenColor });
    faceVertices.push_back({ corners[4], greenColor }); faceVertices.push_back({ corners[3], cyanColor }); faceVertices.push_back({ corners[7], greenColor });
    faceVertices.push_back({ corners[1], greenColor }); faceVertices.push_back({ corners[5],  yellowColor }); faceVertices.push_back({ corners[6], greenColor });
    faceVertices.push_back({ corners[1], greenColor }); faceVertices.push_back({ corners[6],  yellowColor }); faceVertices.push_back({ corners[2], greenColor });
    faceVertices.push_back({ corners[3], redColor }); faceVertices.push_back({ corners[2], redColor }); faceVertices.push_back({ corners[6], redColor });
    faceVertices.push_back({ corners[3], redColor }); faceVertices.push_back({ corners[6], redColor }); faceVertices.push_back({ corners[7], redColor });
    faceVertices.push_back({ corners[4], greenColor }); faceVertices.push_back({ corners[5], greenColor }); faceVertices.push_back({ corners[1], greenColor });
    faceVertices.push_back({ corners[4], greenColor }); faceVertices.push_back({ corners[1], greenColor }); faceVertices.push_back({ corners[0], greenColor });
    if (FAILED(pd3dDevice->CreateVertexBuffer(faceVertices.size() * sizeof(FVF_FrustumVertex),
                                              D3DUSAGE_WRITEONLY, D3DFVF_FRUSTUMVERTEX,
                                              D3DPOOL_MANAGED, &g_pFrustumVB, NULL))) {
        std::cerr << "创建视锥体面顶点缓冲区失败!" << std::endl; return;
    }
    VOID* pFaceVerticesData;
    if (FAILED(g_pFrustumVB->Lock(0, 0, (void**)&pFaceVerticesData, 0))) {
        std::cerr << "锁定视锥体面顶点缓冲区失败!" << std::endl; return;
    }
    memcpy(pFaceVerticesData, faceVertices.data(), faceVertices.size() * sizeof(FVF_FrustumVertex));
    g_pFrustumVB->Unlock();

    D3DCOLOR lineColor = D3DCOLOR_XRGB(255, 255, 255);
    std::vector<FVF_FrustumVertex> lineVertices;
    lineVertices.reserve(24);
    lineVertices.push_back({ corners[0], lineColor }); lineVertices.push_back({ corners[1], lineColor });
    lineVertices.push_back({ corners[1], lineColor }); lineVertices.push_back({ corners[2], lineColor });
    lineVertices.push_back({ corners[2], lineColor }); lineVertices.push_back({ corners[3], lineColor });
    lineVertices.push_back({ corners[3], lineColor }); lineVertices.push_back({ corners[0], lineColor });
    lineVertices.push_back({ corners[4], lineColor }); lineVertices.push_back({ corners[5], lineColor });
    lineVertices.push_back({ corners[5], lineColor }); lineVertices.push_back({ corners[6], lineColor });
    lineVertices.push_back({ corners[6], lineColor }); lineVertices.push_back({ corners[7], lineColor });
    lineVertices.push_back({ corners[7], lineColor }); lineVertices.push_back({ corners[4], lineColor });
    lineVertices.push_back({ corners[0], lineColor }); lineVertices.push_back({ corners[4], lineColor });
    lineVertices.push_back({ corners[1], lineColor }); lineVertices.push_back({ corners[5], lineColor });
    lineVertices.push_back({ corners[2], lineColor }); lineVertices.push_back({ corners[6], lineColor });
    lineVertices.push_back({ corners[3], lineColor }); lineVertices.push_back({ corners[7], lineColor });
    if (FAILED(pd3dDevice->CreateVertexBuffer(lineVertices.size() * sizeof(FVF_FrustumVertex),
                                              D3DUSAGE_WRITEONLY, D3DFVF_FRUSTUMVERTEX,
                                              D3DPOOL_MANAGED, &g_pFrustumLinesVB, NULL))) {
        std::cerr << "创建视锥体线顶点缓冲区失败!" << std::endl;
        if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }
        return;
    }
    VOID* pLineVerticesData;
    if (FAILED(g_pFrustumLinesVB->Lock(0, 0, (void**)&pLineVerticesData, 0))) {
        std::cerr << "锁定视锥体线顶点缓冲区失败!" << std::endl; return;
    }
    memcpy(pLineVerticesData, lineVertices.data(), lineVertices.size() * sizeof(FVF_FrustumVertex));
    g_pFrustumLinesVB->Unlock();
}

void DrawFrozenFrustumFaces(LPDIRECT3DDEVICE9 pd3dDevice) {
    if (!pd3dDevice || !g_bDrawFrozenFrustum) { return; }
    DWORD oldCullMode, oldLighting, oldZEnable, oldFVF;
    pd3dDevice->GetRenderState(D3DRS_CULLMODE, &oldCullMode);
    pd3dDevice->GetRenderState(D3DRS_LIGHTING, &oldLighting);
    pd3dDevice->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
    pd3dDevice->GetFVF(&oldFVF);
    D3DXMATRIX matWorld; D3DXMatrixIdentity(&matWorld);
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
    pd3dDevice->SetFVF(D3DFVF_FRUSTUMVERTEX);
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

    if (g_pFrustumVB) {
        DWORD oldAlphaBlendEnable, oldSrcBlend, oldDestBlend, oldZWriteEnable;
        DWORD oldTSS_ColorOp_0, oldTSS_ColorArg1_0, oldTSS_ColorArg2_0, oldTSS_AlphaOp_0, oldTSS_AlphaArg1_0, oldTSS_AlphaArg2_0;
        pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlendEnable); pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &oldSrcBlend);
        pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend); pd3dDevice->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
        pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &oldTSS_ColorOp_0); pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &oldTSS_ColorArg1_0);
        pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &oldTSS_ColorArg2_0); pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &oldTSS_AlphaOp_0);
        pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &oldTSS_AlphaArg1_0); pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &oldTSS_AlphaArg2_0);
        pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        pd3dDevice->SetStreamSource(0, g_pFrustumVB, 0, sizeof(FVF_FrustumVertex));
        pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
        pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlendEnable); pd3dDevice->SetRenderState(D3DRS_SRCBLEND, oldSrcBlend);
        pd3dDevice->SetRenderState(D3DRS_DESTBLEND, oldDestBlend); pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);
        pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, oldTSS_ColorOp_0); pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, oldTSS_ColorArg1_0);
        pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, oldTSS_ColorArg2_0); pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, oldTSS_AlphaOp_0);
        pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, oldTSS_AlphaArg1_0); pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, oldTSS_AlphaArg2_0);
    }
    if (g_pFrustumLinesVB) {
        DWORD oldAlphaBlendEnableLines, oldZWriteEnableLines;
        DWORD oldTSS_ColorOp_0_L, oldTSS_ColorArg1_0_L, oldTSS_AlphaOp_0_L, oldTSS_AlphaArg1_0_L;
        pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlendEnableLines); pd3dDevice->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnableLines);
        pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &oldTSS_ColorOp_0_L); pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &oldTSS_ColorArg1_0_L);
        pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &oldTSS_AlphaOp_0_L); pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &oldTSS_AlphaArg1_0_L);
        pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        pd3dDevice->SetStreamSource(0, g_pFrustumLinesVB, 0, sizeof(FVF_FrustumVertex));
        pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 12);
        pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlendEnableLines); pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnableLines);
        pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, oldTSS_ColorOp_0_L); pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, oldTSS_ColorArg1_0_L);
        pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, oldTSS_AlphaOp_0_L); pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, oldTSS_AlphaArg1_0_L);
    }
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, oldCullMode);
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, oldLighting);
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, oldZEnable);
    pd3dDevice->SetFVF(oldFVF);
}


INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd) {
    g_hWnd = CGraphic::GetSingleObjPtr()->InitWindow(EventProc, 1600, 1200, _T("DX"));
    if (!g_hWnd) { MessageBox(NULL, _T("窗口初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR); return -1; }

    if (!CGraphic::GetSingleObjPtr()->InitDX(g_hWnd)) { MessageBox(NULL, _T("Direct3D初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR); return -1; }
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;

    InitTimer();

    g_pMainCamera = new Camera("MainCamera");

    float windowWidth = 1600.0f; float windowHeight = 1200.0f;
    RECT rc; GetClientRect(g_hWnd, &rc); windowWidth = (float)(rc.right - rc.left); windowHeight = (float)(rc.bottom - rc.top);
    g_pMainCamera->fieldOfViewY = D3DX_PI / 4.0f;
    g_pMainCamera->aspectRatio = windowWidth / (windowHeight == 0 ? 1.0f : windowHeight);
    g_pMainCamera->nearPlane = 1.0f; g_pMainCamera->farPlane = 5000.0f;
    g_pMainCamera->UpdateProjectionMatrix();

    g_pPlayer = new Player("HeroPlayer");
    g_pPlayer->transform.SetPosition(0.0f, 0.0f, 0.0f);
    const char* fbxModelPath = "C:\\Users\\admin\\CLionProjects\\DX\\cmake-build-debug\\Assets\\shigeju.fbx";
    if (!g_pPlayer->LoadFBXModelFromFile(fbxModelPath)) { MessageBox(NULL, _T("加载FBX模型失败!"), _T("错误"), MB_OK | MB_ICONERROR); }

    g_pTerrain = new Terrain();
    if (g_pTerrain) {
        if (!g_pTerrain->Init()) {
            MessageBox(g_hWnd, _T("地形初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR);
            delete g_pTerrain; g_pTerrain = nullptr;
        }
    }

    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    D3DLIGHT9 light; ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f;
    light.Ambient.r = 0.4f; light.Ambient.g = 0.4f; light.Ambient.b = 0.4f;
    D3DXVECTOR3 vecDir(0.5f, -0.8f, 0.5f);
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light); pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    MSG msg; ZeroMemory(&msg, sizeof(MSG));
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            float deltaTime = GetDeltaTime();

            // 处理轨道相机键盘输入
            HandleOrbitCameraKeyboardInput(deltaTime);

            if (g_pPlayer) {
                g_pPlayer->Update(deltaTime);
            }

            if (g_pPlayer && g_pMainCamera) {
                D3DXVECTOR3 playerPos = g_pPlayer->transform.GetPosition();
                D3DXVECTOR3 targetFocusPoint = playerPos;
                // targetFocusPoint.y += 1.0f; // 可选: 调整焦点高度

                float camX = targetFocusPoint.x - g_cameraDistance * cosf(g_cameraOrbitPitch) * sinf(g_cameraOrbitYaw);
                float camY = targetFocusPoint.y + g_cameraDistance * sinf(g_cameraOrbitPitch);
                float camZ = targetFocusPoint.z - g_cameraDistance * cosf(g_cameraOrbitPitch) * cosf(g_cameraOrbitYaw);

                g_pMainCamera->transform.SetPosition(camX, camY, camZ);

                g_pMainCamera->LookAt(targetFocusPoint);
            }

            if (g_pMainCamera) {
                g_pMainCamera->UpdateViewMatrix();
            }

            if (g_pMainCamera) {
                pd3dDevice->SetTransform(D3DTS_VIEW, &g_pMainCamera->GetViewMatrix());
                pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_pMainCamera->GetProjectionMatrix());
            }

            CGraphic::GetSingleObjPtr()->BeginDraw();
            if (g_pTerrain) { g_pTerrain->Render(); }
            if (g_pPlayer) { g_pPlayer->Render(pd3dDevice); }
            DrawFrozenFrustumFaces(pd3dDevice);
            CGraphic::GetSingleObjPtr()->EndDraw();
        }
    }

    if (g_pTerrain) { delete g_pTerrain; g_pTerrain = nullptr; }
    if (g_pPlayer) { delete g_pPlayer; g_pPlayer = nullptr; }
    if (g_pMainCamera) { delete g_pMainCamera; g_pMainCamera = nullptr; }
    if (g_pFrozenFrustum) { delete g_pFrozenFrustum; g_pFrozenFrustum = nullptr; }
    if (g_pFrustumVB) { g_pFrustumVB->Release(); g_pFrustumVB = nullptr; }
    if (g_pFrustumLinesVB) { g_pFrustumLinesVB->Release(); g_pFrustumLinesVB = nullptr; }

    return (int)msg.wParam;
}
