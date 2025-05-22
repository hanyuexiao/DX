////
//// Created by admin on 2025/5/7.
////
//#include "head.h"
//#include "Graphic.h" // 你的类
//#include <mmsystem.h>
//// --- 全局或 WinMain 静态变量，用于抓钩的底层渲染 ---
//LPDIRECT3DVERTEXBUFFER9 g_pHookVB = nullptr;      // 抓钩的顶点缓冲
//MyImageInfo g_hookImageInfo_grad;                 // 抓钩的图像信息 (之前是局部的 grad)
//
//// 抓钩动画和位置相关的变量 (这些可以根据你的游戏逻辑放在更合适的地方)
//float g_hook_currentAngle = 0.0f;                 // 抓钩当前旋转角度 (弧度)
//D3DXVECTOR3 g_hook_localPivotOffset;              // 抓钩局部旋转中心偏移
//D3DXVECTOR3 g_hook_worldPivotPosition(400.0f, 100.0f, 0.0f); // 抓钩在世界中的悬挂点
//
//// 动画参数 (可以根据需要调整)
//const float HOOK_ANGLE_SPEED = D3DXToRadian(60.0f); // 每秒摆动60度
//const float HOOK_MIN_ANGLE = D3DXToRadian(-45.0f);
//const float HOOK_MAX_ANGLE = D3DXToRadian(45.0f);
//int   g_hook_swingDirection = 1;
//// ----------------------------------------------------
//
//// 顶点结构 (确保这个定义在你的项目中可用)
//struct CUSTOMVERTEX {
//    FLOAT x, y, z; // 位置
//    FLOAT tu, tv;  // 纹理坐标
//};
//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)
//
//// 函数：为图像创建一个四边形顶点缓冲区 (与之前提供的一致)
//HRESULT CreateQuadVertexBufferForImage(LPDIRECT3DDEVICE9 pDevice,
//                                       const MyImageInfo& imageInfo,
//                                       LPDIRECT3DVERTEXBUFFER9* ppVB)
//{
//    if (!pDevice || !ppVB) return E_POINTER;
//    if (imageInfo.width == 0 || imageInfo.height == 0) return E_INVALIDARG;
//
//    CUSTOMVERTEX vertices[] = {
//            {0.0f,                  0.0f,                 0.0f, 0.0f, 0.0f}, // V0: 左上
//            {(float)imageInfo.width, 0.0f,                 0.0f, 1.0f, 0.0f}, // V1: 右上
//            {0.0f,                  (float)imageInfo.height,0.0f, 0.0f, 1.0f}, // V2: 左下
//            {(float)imageInfo.width, (float)imageInfo.height,0.0f, 1.0f, 1.0f}  // V3: 右下
//    };
//
//    HRESULT hr = pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
//                                             D3DUSAGE_WRITEONLY,
//                                             D3DFVF_CUSTOMVERTEX,
//                                             D3DPOOL_MANAGED,
//                                             ppVB, nullptr);
//    if (FAILED(hr)) return hr;
//
//    VOID* pLockedVertices;
//    if (FAILED((*ppVB)->Lock(0, sizeof(vertices), (void**)&pLockedVertices, 0))) {
//        (*ppVB)->Release(); *ppVB = nullptr; return hr;
//    }
//    memcpy(pLockedVertices, vertices, sizeof(vertices));
//    (*ppVB)->Unlock();
//    return S_OK;
//}
//
//LRESULT MainWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    // ... (你的 WndProc 代码保持不变) ...
//    switch (msg) {
//        case WM_DESTROY:
//        {
//            PostQuitMessage(0);
//            break;
//        }
//    }
//    return DefWindowProc(hwnd, msg, wParam, lParam);
//}
//
//
//void RenderRotatingHook_LowLevel(LPDIRECT3DDEVICE9 pDevice,
//                                 MyImageInfo& hookImageInfo,
//                                 LPDIRECT3DVERTEXBUFFER9 pHookVB,
//                                 float currentHookAngle_rad, // 当前抓钩的旋转角度 (弧度)
//                                 const D3DXVECTOR3& localPivotOffset, // 抓钩局部旋转中心相对其模型左上角的偏移
//                                 const D3DXVECTOR3& worldPivotPosition) // 抓钩旋转中心在世界坐标中的最终位置
//{
//    if (!pDevice || !pHookVB || !hookImageInfo.pTex) {
//        return;
//    }
//
//    D3DXMATRIX matTranslatePivotToOrigin, matRotation, matTranslateToWorld, matFinalWorld;
//
//    // ================================================================================
//    // ==                                                                            ==
//    // ==    你的任务开始：实现“平移 - 旋转 - 平移到世界”的矩阵变换逻辑         ==
//    // ==                                                                            ==
//    // ================================================================================
//
//    // 任务 1: 创建平移矩阵 (matTranslatePivotToOrigin)
//    //         目标: 将抓钩的局部旋转中心 (localPivotOffset) 移动到局部坐标系的原点 (0,0,0)。
//    //         提示: 使用 D3DXMatrixTranslation。平移的量应该是 localPivotOffset 的负值。
//    //         例如: D3DXMatrixTranslation(&matTranslatePivotToOrigin, -localPivotOffset.x, ...);
//    // --------------------------------------------------------------------------------
//    // ▼▼▼ 在下方填写你的代码 ▼▼▼
//
//    D3DXMatrixTranslation(&matTranslatePivotToOrigin,
//                          -localPivotOffset.x,
//                          -localPivotOffset.y,
//                          -localPivotOffset.z); // 已帮你填写，请检查是否符合你的理解
//
//    // ▲▲▲ 你的代码结束 ▲▲▲
//    // --------------------------------------------------------------------------------
//
//
//    // 任务 2: 创建旋转矩阵 (matRotation)
//    //         目标: 围绕Z轴旋转 currentHookAngle_rad 弧度。
//    //         提示: 使用 D3DXMatrixRotationZ。
//    // --------------------------------------------------------------------------------
//    // ▼▼▼ 在下方填写你的代码 ▼▼▼
//
//    D3DXMatrixRotationZ(&matRotation, currentHookAngle_rad); // 已帮你填写，请检查
//
//    // ▲▲▲ 你的代码结束 ▲▲▲
//    // --------------------------------------------------------------------------------
//
//
//    // 任务 3: 创建平移矩阵 (matTranslateToWorld)
//    //         目标: 将旋转后的抓钩（其旋转中心在旋转时位于原点）的那个旋转中心，
//    //               移动到它在世界中的最终悬挂位置 (worldPivotPosition)。
//    //         提示: 使用 D3DXMatrixTranslation。
//    // --------------------------------------------------------------------------------
//    // ▼▼▼ 在下方填写你的代码 ▼▼▼
//
//    D3DXMatrixTranslation(&matTranslateToWorld,
//                          worldPivotPosition.x,
//                          worldPivotPosition.y,
//                          worldPivotPosition.z); // 已帮你填写，请检查
//
//    // ▲▲▲ 你的代码结束 ▲▲▲
//    // --------------------------------------------------------------------------------
//
//
//    // 任务 4: 组合这三个矩阵得到最终的世界变换矩阵 (matFinalWorld)
//    //         重要: 矩阵乘法的顺序决定了变换的顺序。
//    //               正确的顺序是：先将局部旋转中心移到原点，然后旋转，最后将旋转中心定位到世界位置。
//    //               对于行向量 v' = v * M1 * M2 * M3，M1先作用。
//    //         提示: matFinalWorld = matTranslatePivotToOrigin * matRotation * matTranslateToWorld;
//    // --------------------------------------------------------------------------------
//    // ▼▼▼ 在下方填写你的代码 ▼▼▼
//
//    matFinalWorld = matTranslatePivotToOrigin * matRotation * matTranslateToWorld; // 已帮你填写，这是最关键的组合顺序
//
//    // ▲▲▲ 你的代码结束 ▲▲▲
//    // --------------------------------------------------------------------------------
//
//    // ================================================================================
//    // ==                                                                            ==
//    // ==    你的任务结束                                                              ==
//    // ==                                                                            ==
//    // ================================================================================
//
//
//    // --- 后续的DirectX绘制步骤 (由我提供) ---
//    pDevice->SetTransform(D3DTS_WORLD, &matFinalWorld);
//
//    pDevice->SetTexture(0, hookImageInfo.pTex);
//    pDevice->SetStreamSource(0, pHookVB, 0, sizeof(CUSTOMVERTEX));
//    pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
//
//    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//    // (可选) Alpha Blending 设置
//    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//    // pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); // D3DTOP_SELECTARG1 for just texture alpha
//
//    pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
//
//    // (可选) 恢复渲染状态
//    // pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//}
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
//    HWND myHand = CGraphic::GetSingleObjPtr()->InitWindow(MainWndProc,800, 600, _T("C++ Image"),_T("C++ Image") );
//    CGraphic::GetSingleObjPtr()->InitDX(myHand);
//
//    // --- 加载所有图像 ---
//    MyImageInfo background_image1;
//    MyImageInfo background_image2;
//    // MyImageInfo grad; // 现在使用 g_hookImageInfo_grad
//    MyImageInfo rope;
//
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\first1.png"),background_image1,0xFFFF0000);
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\first3.png"),background_image2,0xFFFFFFFF);
//    // 为抓钩加载纹理到全局变量
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\grad.png"),g_hookImageInfo_grad,0xFFFFFFFF);
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\rope.png"),rope,0xFFFFFFFF);
//
//    // --- 初始化抓钩的顶点缓冲 (在 LoadTex 之后，因为需要 width/height) ---
//    if (FAILED(CreateQuadVertexBufferForImage(CGraphic::GetSingleObjPtr()->m_pDevice, g_hookImageInfo_grad, &g_pHookVB))) {
//        MessageBox(myHand, TEXT("Failed to create hook vertex buffer!"), TEXT("Error"), MB_OK | MB_ICONERROR);
//        // 清理DX资源等... (你可能需要在CGraphic中添加CloseDX的调用)
//        if (g_pHookVB) g_pHookVB->Release();
//        CGraphic::GetSingleObjPtr()->CloseDX(); // 假设你有这个清理函数
//        return 1;
//    }
//
//    // --- 初始化抓钩的局部旋转中心 ---
//    // 例如，如果想绕图片中心旋转:
//    g_hook_localPivotOffset = D3DXVECTOR3(g_hookImageInfo_grad.width / 2.0f, g_hookImageInfo_grad.height / 2.0f, 0.0f);
//    // 或者，如果想绕顶部中点 (假设(0,0)是VB中定义的左上角):
//    // g_hook_localPivotOffset = D3DXVECTOR3(g_hookImageInfo_grad.width / 2.0f, 0.0f, 0.0f);
//
//
//    MSG msg;
//    ZeroMemory(&msg, sizeof(MSG)); // 推荐初始化MSG结构体
//
//    // 获取初始时间用于计算 deltaTime (或者使用固定 deltaTime)
//    DWORD lastTime = timeGetTime();
//
//    while(msg.message != WM_QUIT){ // 修改循环条件以正确退出
//        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }else{
//            // --- 游戏逻辑更新 ---
//            DWORD currentTime = timeGetTime();
//            float deltaTime = (currentTime - lastTime) * 0.001f; // 转换为秒
//            lastTime = currentTime;
//
//            // 更新抓钩角度 (示例)
//            g_hook_currentAngle += HOOK_ANGLE_SPEED * deltaTime * g_hook_swingDirection;
//            if (g_hook_currentAngle >= HOOK_MAX_ANGLE) {
//                g_hook_currentAngle = HOOK_MAX_ANGLE;
//                g_hook_swingDirection = -1;
//            } else if (g_hook_currentAngle <= HOOK_MIN_ANGLE) {
//                g_hook_currentAngle = HOOK_MIN_ANGLE;
//                g_hook_swingDirection = 1;
//            }
//
//            // (你可能还需要根据游戏状态更新 g_hook_worldPivotPosition)
//
//
//            // --- 渲染 ---
//            CGraphic::GetSingleObjPtr()->BeginDraw();
//
//            // 绘制背景等 (使用你的 CGraphic 类)
//            CGraphic::GetSingleObjPtr()->DrawTex(background_image1,D3DXVECTOR3(0,0,0));
//            CGraphic::GetSingleObjPtr()->DrawTex(background_image2,D3DXVECTOR3(0,0,0));
//
//            // 【重要】使用 RenderRotatingHook_LowLevel 绘制旋转的抓钩
//            // 替换掉: CGraphic::GetSingleObjPtr()->DrawTex(grad,D3DXVECTOR3(400,100,0));
//            if (g_pHookVB) {
//                RenderRotatingHook_LowLevel(
//                        CGraphic::GetSingleObjPtr()->m_pDevice,
//                        g_hookImageInfo_grad,         // 抓钩的 MyImageInfo
//                        g_pHookVB,                    // 抓钩的顶点缓冲
//                        g_hook_currentAngle,          // 【填空】你计算的当前旋转角度
//                        g_hook_localPivotOffset,      // 【填空】你定义的局部旋转中心偏移
//                        g_hook_worldPivotPosition     // 【填空】你定义的世界中的旋转中心位置
//                );
//            }
//
//            // 绘制绳索 (你可能需要调整绳索的起始/结束位置以连接到抓钩的 g_hook_worldPivotPosition)
//            CGraphic::GetSingleObjPtr()->DrawTex(rope,D3DXVECTOR3(g_hook_worldPivotPosition.x - (rope.width/2) + (g_hookImageInfo_grad.width/2 - g_hook_localPivotOffset.x) , g_hook_worldPivotPosition.y - rope.height,0)); // 简化的绳索位置调整示例
//
//
//            CGraphic::GetSingleObjPtr()->EndDraw();
//            CGraphic::GetSingleObjPtr()->m_pDevice->Present(NULL,NULL,NULL,NULL); // 确保调用Present
//        }
//    }
//
//    // --- 清理资源 ---
//    if (g_pHookVB) {
//        g_pHookVB->Release();
//        g_pHookVB = nullptr;
//    }
//    CGraphic::GetSingleObjPtr()->CloseDX(); // 确保有DX的完整清理
//
//    return (int)msg.wParam;
//}
//
