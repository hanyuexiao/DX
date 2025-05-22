// ��Ҫ��Windows��DirectXͷ�ļ�
#include <windows.h>
#include <tchar.h>
#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h> // For timeGetTime (�ǵ����� winmm.lib)

// ����Զ�����ͽṹ�嶨��
#include "Graphic.h" // ���������� CGraphic �� MyImageInfo �Ķ���

// --- ȫ�ֱ����� WinMain �����ڵľ�̬���� ---
MyImageInfo g_hookImage;    // ץ����ͼ����Ϣ
MyImageInfo g_ropeImage;    // ������ͼ����Ϣ

LPD3DXLINE  g_pDebugLine = nullptr; // ���ڻ��Ƶ�������
// ------------------------------------

LRESULT CALLBACK MainWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_DESTROY: PostQuitMessage(0); break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwndMain = CGraphic::GetSingleObjPtr()->InitWindow(MainWndProc, 800, 600, _T("Gold Miner - User Matrix"), _T("GoldMinerUserMatrixClass"));
    if (!hwndMain) return 1;
    CGraphic::GetSingleObjPtr()->InitDX(hwndMain);
    LPDIRECT3DDEVICE9 pDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pDevice) {
        MessageBox(hwndMain, _T("DirectX Initialization Failed!"), _T("Error"), MB_OK | MB_ICONERROR);
        return 1;
    }

    if (FAILED(D3DXCreateLine(pDevice, &g_pDebugLine))) {
        MessageBox(hwndMain, _T("D3DXCreateLine failed! Debug lines won't be available."), _T("Warning"), MB_OK);
        g_pDebugLine = nullptr;
    }

    MyImageInfo backgroundImage1, backgroundImage2; // �ֲ���������Ϊֻ��WinMain��ʹ��
    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\first1.png"), backgroundImage1, 0xFFFF0000);
    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\first3.png"), backgroundImage2, 0xFFFFFFFF);
    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\grad.png"), g_hookImage, 0xFFFFFFFF);
    CGraphic::GetSingleObjPtr()->LoadTex(TEXT("C:\\Users\\admin\\CLionProjects\\DX\\Gold_Miner_img\\rope.png"), g_ropeImage, 0xFFFFFFFF);

    if (!g_hookImage.pTex || !g_ropeImage.pTex) {
        MessageBox(hwndMain, _T("Hook or Rope image failed to load!"), _T("Error"), MB_OK | MB_ICONERROR);
        if (g_pDebugLine) g_pDebugLine->Release();
        CGraphic::GetSingleObjPtr()->CloseDX();
        return 1;
    }

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // --- �û�����ľ�̬�������������ǵľ������ ---
    static D3DXVECTOR3 s_gradpos1_offset(0.0f, 30.0f, 0.0f);      // ��Ӧ������е� gradpos1
    static D3DXVECTOR3 s_final_translation_pos(415.0f, 70.0f, 0.0f); // ��Ӧ������е� gradpos
    static float s_angle_amplitude = (80.0f/180.0f) * D3DX_PI;   // ��Ӧ������е� angle (��Ϊ���)
    static float s_timer_v = 0.0f;                               // ��Ӧ������е� v (��ʱ��)
    // -------------------------------------------------

    // (��ѡ) �����������deltaTime���������߼������Ա���
    // DWORD lastFrameTime = timeGetTime();

    while(msg.message != WM_QUIT) {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // (��ѡ) deltaTime ����
            // DWORD currentTime = timeGetTime();
            // float deltaTime = (currentTime - lastFrameTime) * 0.001f;
            // lastFrameTime = currentTime;
            // if (deltaTime > 0.1f) deltaTime = 0.1f;

            // --- �û�ָ���ľ�������߼� ---
            s_timer_v += 0.05f; // ��ļ�ʱ������

            D3DXMATRIX matWorldByUser, matT1_user, matR_user, matT_user;

            D3DXMatrixTranslation(&matT1_user, s_gradpos1_offset.x, s_gradpos1_offset.y, s_gradpos1_offset.z);
            D3DXMatrixTranslation(&matT_user, s_final_translation_pos.x, s_final_translation_pos.y, s_final_translation_pos.z);
            D3DXMatrixRotationZ(&matR_user, s_angle_amplitude * sinf(s_timer_v)); // ʹ��sinf���float

            // ��ľ������˳��: T1 * R * T
            matWorldByUser = matT1_user * matR_user * matT_user;
            // --- �û����������� ---


            // --- ��Ⱦ ---
            CGraphic::GetSingleObjPtr()->BeginDraw();

            CGraphic::GetSingleObjPtr()->DrawTex(backgroundImage1, D3DXVECTOR3(0,0,0));
            // CGraphic::GetSingleObjPtr()->DrawTex(backgroundImage2, D3DXVECTOR3(0,0,0));


            // --- ʹ���û������ matWorldByUser ����ץ�������� ---
            // ץ�����ƣ�ʹ�����Լ��ľֲ����ĵ�
            D3DXVECTOR3 gradLocalCenter(g_hookImage.width / 2.0f, 0.0f, 0.0f);
            if (g_hookImage.pTex) {
                CGraphic::GetSingleObjPtr()->DrawTex(
                        g_hookImage,
                        gradLocalCenter,    // ��Ϊץ��ָ���ľֲ�����
                        matWorldByUser      // �������������
                );

                // --- ����ץ����׷�پ��ο� ---
                if (g_pDebugLine) {
                    // ����ץ����������Ч�任����
                    // ���� DrawTex(image, center, worldMatrix) ���ڲ��߼���: EffectiveTransform = Translate(-center) * worldMatrix
                    D3DXMATRIX matTranslateByNegGradCenter, matEffectiveTotalForGrad;
                    D3DXMatrixTranslation(&matTranslateByNegGradCenter,
                                          -gradLocalCenter.x, -gradLocalCenter.y, -gradLocalCenter.z);
                    matEffectiveTotalForGrad = matTranslateByNegGradCenter * matWorldByUser;

                    D3DXVECTOR3 local_corners[4]; // �����ͼƬ���Ͻ�(0,0)
                    local_corners[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
                    local_corners[1] = D3DXVECTOR3((float)g_hookImage.width, 0.0f, 0.0f);
                    local_corners[2] = D3DXVECTOR3((float)g_hookImage.width, (float)g_hookImage.height, 0.0f);
                    local_corners[3] = D3DXVECTOR3(0.0f, (float)g_hookImage.height, 0.0f);

                    D3DXVECTOR3 world_corners_3d[4];
                    for (int i = 0; i < 4; ++i) {
                        D3DXVec3TransformCoord(&world_corners_3d[i], &local_corners[i], &matEffectiveTotalForGrad);
                    }

                    D3DXVECTOR2 line_points[5];
                    for(int i=0; i<4; ++i) {
                        line_points[i].x = world_corners_3d[i].x;
                        line_points[i].y = world_corners_3d[i].y;
                    }
                    line_points[4] = line_points[0];

                    g_pDebugLine->SetWidth(1.0f);
                    g_pDebugLine->Begin();
                    g_pDebugLine->Draw(line_points, 5, D3DCOLOR_ARGB(255, 0, 255, 0)); // ��ɫ����
                    g_pDebugLine->End();
                }
            }

            // �������ƣ�ʹ�����Լ��ľֲ����ĵ��ͬһ�� matWorldByUser
            D3DXVECTOR3 ropeLocalCenter(g_ropeImage.width / 2.0f, 30.0f, 0.0f); // ������Ϊ����ָ���ľֲ�����
            if (g_ropeImage.pTex) {
                CGraphic::GetSingleObjPtr()->DrawTex(
                        g_ropeImage,
                        ropeLocalCenter,    // ��Ϊ����ָ���ľֲ�����
                        matWorldByUser      // �������������
                );
            }
            // --- ץ�����������ƽ��� ---

            CGraphic::GetSingleObjPtr()->EndDraw();
            pDevice->Present(nullptr, nullptr, nullptr, nullptr);
        }
    }

    // ����
    if (g_pDebugLine) {
        g_pDebugLine->Release();
        g_pDebugLine = nullptr;
    }
    CGraphic::GetSingleObjPtr()->CloseDX();

    return (int)msg.wParam;
}