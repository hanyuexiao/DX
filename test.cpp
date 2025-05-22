////
//// Created by admin on 2025/5/7.
////
//#include "head.h"
//#include "Graphic.h" // �����
//#include <mmsystem.h>
//// --- ȫ�ֻ� WinMain ��̬����������ץ���ĵײ���Ⱦ ---
//LPDIRECT3DVERTEXBUFFER9 g_pHookVB = nullptr;      // ץ���Ķ��㻺��
//MyImageInfo g_hookImageInfo_grad;                 // ץ����ͼ����Ϣ (֮ǰ�Ǿֲ��� grad)
//
//// ץ��������λ����صı��� (��Щ���Ը��������Ϸ�߼����ڸ����ʵĵط�)
//float g_hook_currentAngle = 0.0f;                 // ץ����ǰ��ת�Ƕ� (����)
//D3DXVECTOR3 g_hook_localPivotOffset;              // ץ���ֲ���ת����ƫ��
//D3DXVECTOR3 g_hook_worldPivotPosition(400.0f, 100.0f, 0.0f); // ץ���������е����ҵ�
//
//// �������� (���Ը�����Ҫ����)
//const float HOOK_ANGLE_SPEED = D3DXToRadian(60.0f); // ÿ��ڶ�60��
//const float HOOK_MIN_ANGLE = D3DXToRadian(-45.0f);
//const float HOOK_MAX_ANGLE = D3DXToRadian(45.0f);
//int   g_hook_swingDirection = 1;
//// ----------------------------------------------------
//
//// ����ṹ (ȷ����������������Ŀ�п���)
//struct CUSTOMVERTEX {
//    FLOAT x, y, z; // λ��
//    FLOAT tu, tv;  // ��������
//};
//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)
//
//// ������Ϊͼ�񴴽�һ���ı��ζ��㻺���� (��֮ǰ�ṩ��һ��)
//HRESULT CreateQuadVertexBufferForImage(LPDIRECT3DDEVICE9 pDevice,
//                                       const MyImageInfo& imageInfo,
//                                       LPDIRECT3DVERTEXBUFFER9* ppVB)
//{
//    if (!pDevice || !ppVB) return E_POINTER;
//    if (imageInfo.width == 0 || imageInfo.height == 0) return E_INVALIDARG;
//
//    CUSTOMVERTEX vertices[] = {
//            {0.0f,                  0.0f,                 0.0f, 0.0f, 0.0f}, // V0: ����
//            {(float)imageInfo.width, 0.0f,                 0.0f, 1.0f, 0.0f}, // V1: ����
//            {0.0f,                  (float)imageInfo.height,0.0f, 0.0f, 1.0f}, // V2: ����
//            {(float)imageInfo.width, (float)imageInfo.height,0.0f, 1.0f, 1.0f}  // V3: ����
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
//    // ... (��� WndProc ���뱣�ֲ���) ...
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
//                                 float currentHookAngle_rad, // ��ǰץ������ת�Ƕ� (����)
//                                 const D3DXVECTOR3& localPivotOffset, // ץ���ֲ���ת���������ģ�����Ͻǵ�ƫ��
//                                 const D3DXVECTOR3& worldPivotPosition) // ץ����ת���������������е�����λ��
//{
//    if (!pDevice || !pHookVB || !hookImageInfo.pTex) {
//        return;
//    }
//
//    D3DXMATRIX matTranslatePivotToOrigin, matRotation, matTranslateToWorld, matFinalWorld;
//
//    // ================================================================================
//    // ==                                                                            ==
//    // ==    �������ʼ��ʵ�֡�ƽ�� - ��ת - ƽ�Ƶ����硱�ľ���任�߼�         ==
//    // ==                                                                            ==
//    // ================================================================================
//
//    // ���� 1: ����ƽ�ƾ��� (matTranslatePivotToOrigin)
//    //         Ŀ��: ��ץ���ľֲ���ת���� (localPivotOffset) �ƶ����ֲ�����ϵ��ԭ�� (0,0,0)��
//    //         ��ʾ: ʹ�� D3DXMatrixTranslation��ƽ�Ƶ���Ӧ���� localPivotOffset �ĸ�ֵ��
//    //         ����: D3DXMatrixTranslation(&matTranslatePivotToOrigin, -localPivotOffset.x, ...);
//    // --------------------------------------------------------------------------------
//    // ������ ���·���д��Ĵ��� ������
//
//    D3DXMatrixTranslation(&matTranslatePivotToOrigin,
//                          -localPivotOffset.x,
//                          -localPivotOffset.y,
//                          -localPivotOffset.z); // �Ѱ�����д�������Ƿ����������
//
//    // ������ ��Ĵ������ ������
//    // --------------------------------------------------------------------------------
//
//
//    // ���� 2: ������ת���� (matRotation)
//    //         Ŀ��: Χ��Z����ת currentHookAngle_rad ���ȡ�
//    //         ��ʾ: ʹ�� D3DXMatrixRotationZ��
//    // --------------------------------------------------------------------------------
//    // ������ ���·���д��Ĵ��� ������
//
//    D3DXMatrixRotationZ(&matRotation, currentHookAngle_rad); // �Ѱ�����д������
//
//    // ������ ��Ĵ������ ������
//    // --------------------------------------------------------------------------------
//
//
//    // ���� 3: ����ƽ�ƾ��� (matTranslateToWorld)
//    //         Ŀ��: ����ת���ץ��������ת��������תʱλ��ԭ�㣩���Ǹ���ת���ģ�
//    //               �ƶ������������е���������λ�� (worldPivotPosition)��
//    //         ��ʾ: ʹ�� D3DXMatrixTranslation��
//    // --------------------------------------------------------------------------------
//    // ������ ���·���д��Ĵ��� ������
//
//    D3DXMatrixTranslation(&matTranslateToWorld,
//                          worldPivotPosition.x,
//                          worldPivotPosition.y,
//                          worldPivotPosition.z); // �Ѱ�����д������
//
//    // ������ ��Ĵ������ ������
//    // --------------------------------------------------------------------------------
//
//
//    // ���� 4: �������������õ����յ�����任���� (matFinalWorld)
//    //         ��Ҫ: ����˷���˳������˱任��˳��
//    //               ��ȷ��˳���ǣ��Ƚ��ֲ���ת�����Ƶ�ԭ�㣬Ȼ����ת�������ת���Ķ�λ������λ�á�
//    //               ���������� v' = v * M1 * M2 * M3��M1�����á�
//    //         ��ʾ: matFinalWorld = matTranslatePivotToOrigin * matRotation * matTranslateToWorld;
//    // --------------------------------------------------------------------------------
//    // ������ ���·���д��Ĵ��� ������
//
//    matFinalWorld = matTranslatePivotToOrigin * matRotation * matTranslateToWorld; // �Ѱ�����д��������ؼ������˳��
//
//    // ������ ��Ĵ������ ������
//    // --------------------------------------------------------------------------------
//
//    // ================================================================================
//    // ==                                                                            ==
//    // ==    ����������                                                              ==
//    // ==                                                                            ==
//    // ================================================================================
//
//
//    // --- ������DirectX���Ʋ��� (�����ṩ) ---
//    pDevice->SetTransform(D3DTS_WORLD, &matFinalWorld);
//
//    pDevice->SetTexture(0, hookImageInfo.pTex);
//    pDevice->SetStreamSource(0, pHookVB, 0, sizeof(CUSTOMVERTEX));
//    pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
//
//    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//    // (��ѡ) Alpha Blending ����
//    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//    // pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); // D3DTOP_SELECTARG1 for just texture alpha
//
//    pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
//
//    // (��ѡ) �ָ���Ⱦ״̬
//    // pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//}
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
//    HWND myHand = CGraphic::GetSingleObjPtr()->InitWindow(MainWndProc,800, 600, _T("C++ Image"),_T("C++ Image") );
//    CGraphic::GetSingleObjPtr()->InitDX(myHand);
//
//    // --- ��������ͼ�� ---
//    MyImageInfo background_image1;
//    MyImageInfo background_image2;
//    // MyImageInfo grad; // ����ʹ�� g_hookImageInfo_grad
//    MyImageInfo rope;
//
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\first1.png"),background_image1,0xFFFF0000);
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\first3.png"),background_image2,0xFFFFFFFF);
//    // Ϊץ����������ȫ�ֱ���
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\grad.png"),g_hookImageInfo_grad,0xFFFFFFFF);
//    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\rope.png"),rope,0xFFFFFFFF);
//
//    // --- ��ʼ��ץ���Ķ��㻺�� (�� LoadTex ֮����Ϊ��Ҫ width/height) ---
//    if (FAILED(CreateQuadVertexBufferForImage(CGraphic::GetSingleObjPtr()->m_pDevice, g_hookImageInfo_grad, &g_pHookVB))) {
//        MessageBox(myHand, TEXT("Failed to create hook vertex buffer!"), TEXT("Error"), MB_OK | MB_ICONERROR);
//        // ����DX��Դ��... (�������Ҫ��CGraphic�����CloseDX�ĵ���)
//        if (g_pHookVB) g_pHookVB->Release();
//        CGraphic::GetSingleObjPtr()->CloseDX(); // �����������������
//        return 1;
//    }
//
//    // --- ��ʼ��ץ���ľֲ���ת���� ---
//    // ���磬�������ͼƬ������ת:
//    g_hook_localPivotOffset = D3DXVECTOR3(g_hookImageInfo_grad.width / 2.0f, g_hookImageInfo_grad.height / 2.0f, 0.0f);
//    // ���ߣ�������ƶ����е� (����(0,0)��VB�ж�������Ͻ�):
//    // g_hook_localPivotOffset = D3DXVECTOR3(g_hookImageInfo_grad.width / 2.0f, 0.0f, 0.0f);
//
//
//    MSG msg;
//    ZeroMemory(&msg, sizeof(MSG)); // �Ƽ���ʼ��MSG�ṹ��
//
//    // ��ȡ��ʼʱ�����ڼ��� deltaTime (����ʹ�ù̶� deltaTime)
//    DWORD lastTime = timeGetTime();
//
//    while(msg.message != WM_QUIT){ // �޸�ѭ����������ȷ�˳�
//        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }else{
//            // --- ��Ϸ�߼����� ---
//            DWORD currentTime = timeGetTime();
//            float deltaTime = (currentTime - lastTime) * 0.001f; // ת��Ϊ��
//            lastTime = currentTime;
//
//            // ����ץ���Ƕ� (ʾ��)
//            g_hook_currentAngle += HOOK_ANGLE_SPEED * deltaTime * g_hook_swingDirection;
//            if (g_hook_currentAngle >= HOOK_MAX_ANGLE) {
//                g_hook_currentAngle = HOOK_MAX_ANGLE;
//                g_hook_swingDirection = -1;
//            } else if (g_hook_currentAngle <= HOOK_MIN_ANGLE) {
//                g_hook_currentAngle = HOOK_MIN_ANGLE;
//                g_hook_swingDirection = 1;
//            }
//
//            // (����ܻ���Ҫ������Ϸ״̬���� g_hook_worldPivotPosition)
//
//
//            // --- ��Ⱦ ---
//            CGraphic::GetSingleObjPtr()->BeginDraw();
//
//            // ���Ʊ����� (ʹ����� CGraphic ��)
//            CGraphic::GetSingleObjPtr()->DrawTex(background_image1,D3DXVECTOR3(0,0,0));
//            CGraphic::GetSingleObjPtr()->DrawTex(background_image2,D3DXVECTOR3(0,0,0));
//
//            // ����Ҫ��ʹ�� RenderRotatingHook_LowLevel ������ת��ץ��
//            // �滻��: CGraphic::GetSingleObjPtr()->DrawTex(grad,D3DXVECTOR3(400,100,0));
//            if (g_pHookVB) {
//                RenderRotatingHook_LowLevel(
//                        CGraphic::GetSingleObjPtr()->m_pDevice,
//                        g_hookImageInfo_grad,         // ץ���� MyImageInfo
//                        g_pHookVB,                    // ץ���Ķ��㻺��
//                        g_hook_currentAngle,          // ����ա������ĵ�ǰ��ת�Ƕ�
//                        g_hook_localPivotOffset,      // ����ա��㶨��ľֲ���ת����ƫ��
//                        g_hook_worldPivotPosition     // ����ա��㶨��������е���ת����λ��
//                );
//            }
//
//            // �������� (�������Ҫ������������ʼ/����λ�������ӵ�ץ���� g_hook_worldPivotPosition)
//            CGraphic::GetSingleObjPtr()->DrawTex(rope,D3DXVECTOR3(g_hook_worldPivotPosition.x - (rope.width/2) + (g_hookImageInfo_grad.width/2 - g_hook_localPivotOffset.x) , g_hook_worldPivotPosition.y - rope.height,0)); // �򻯵�����λ�õ���ʾ��
//
//
//            CGraphic::GetSingleObjPtr()->EndDraw();
//            CGraphic::GetSingleObjPtr()->m_pDevice->Present(NULL,NULL,NULL,NULL); // ȷ������Present
//        }
//    }
//
//    // --- ������Դ ---
//    if (g_pHookVB) {
//        g_pHookVB->Release();
//        g_pHookVB = nullptr;
//    }
//    CGraphic::GetSingleObjPtr()->CloseDX(); // ȷ����DX����������
//
//    return (int)msg.wParam;
//}
//
