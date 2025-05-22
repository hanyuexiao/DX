//
//#include <windows.h>
//#include <tchar.h>
//#include <vector>
//#include <cmath>
//#include <random>
//#include <ctime>
//#include <debugapi.h> // **** ���� OutputDebugString ****
//#include <stdio.h>    // **** ���� _stprintf_s ****
//
//#include "head.h"    // �����������˱�Ҫ�Ķ���
//#include "Graphic.h" // ��������ͼ����
//
//// --- ȫ�ֱ��� ---
//CGraphic* g_pGraphics = nullptr;
//MyImageInfo g_missileImageInfo;
//HWND g_hWnd = nullptr;
//
//// �����ṹ��
//struct Missile {
//    D3DXVECTOR3 position; // ��ǰλ��
//    D3DXVECTOR3 velocity; // �ٶ����� (���� * �ٶ�)
//    float       rotation; // ��ת�Ƕ� (����)
//    bool        active;   // �Ƿ�
//};
//
//std::vector<Missile> g_missiles; // �洢���е���������
//const float MISSILE_SPEED = 300.0f; // �����ٶ� (����/��)
//const int WINDOW_WIDTH = 800;
//const int WINDOW_HEIGHT = 600;
//
//// ���ڼ���֡���ʱ�� (Delta Time)
//LARGE_INTEGER g_timerFrequency;
//LARGE_INTEGER g_lastTime;
//
//// ����������� (C++11)
//std::mt19937 g_randomEngine(static_cast<unsigned int>(time(nullptr)));
//std::uniform_real_distribution<float> g_randomAngleDist(0.0f, 2.0f * D3DX_PI);
//
//// --- �������� ---
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
//    // ��ȡͼ��ʵ�� (ʹ�����������ṩ�ĺ�����)
//    g_pGraphics = CGraphic::GetSingleObjPtr(); // !! �ٴ�ȷ�����������Ч !!
//    if (!g_pGraphics) {
//        MessageBox(NULL, _T("��ȡ CGraphic ʵ��ʧ��!"), _T("����"), MB_OK | MB_ICONERROR);
//        return -1;
//    }
//
//    // ��ʼ������ (ʹ�����������ṩ�ĺ�����)
//    g_hWnd = g_pGraphics->InitWindow(MainWndProc, WINDOW_WIDTH, WINDOW_HEIGHT, _T("���䵼������"), _T("MissileTestClass")); // !! ȷ�� InitWindow ��ȡ��ע�� !!
//    if (!g_hWnd) {
//        MessageBox(NULL, _T("��ʼ������ʧ��!"), _T("����"), MB_OK | MB_ICONERROR);
//        return -1;
//    }
//
//    // ��ʼ�� DirectX (ʹ�����������ṩ�ĺ�����)
//    g_pGraphics->InitDX(g_hWnd); // !! ȷ�� InitDX ��ȡ��ע������Ч !!
//    if (!g_pGraphics->m_pDevice) // ���� m_pDevice �ǿɷ��ʵ����ڼ��
//    {
//        MessageBox(NULL, _T("DirectX ��ʼ��ʧ�� (m_pDevice is NULL)!"), _T("����"), MB_OK | MB_ICONERROR);
//        DestroyWindow(g_hWnd);
//        return -1;
//    }
//
//    // ���ص�������
//    // **** ����޸�Ϊ���ĵ���ͼƬ����ȷ���·�� ****
//    TCHAR imagePath[] = _T("C:\\Users\\admin\\CLionProjects\\DX\\daoDan.png");
//    OutputDebugString(_T("���ڳ��Լ�������: "));
//    OutputDebugString(imagePath);
//    OutputDebugString(_T("\n"));
//    if (FAILED(g_pGraphics->LoadTex(imagePath, g_missileImageInfo, 0xFFFFFFFF))) // !! ȷ�� LoadTex ��ȡ��ע������Ч !!
//    {
//        TCHAR errorMsg[MAX_PATH + 100];
//        _stprintf_s(errorMsg, _T("��������ʧ��: %s\n��ȷ���ļ�·����ȷ���ļ����������Ŀ¼��ָ��λ�ã�"), imagePath);
//        MessageBox(g_hWnd, errorMsg, _T("����"), MB_OK | MB_ICONERROR);
//        OutputDebugString(_T("�������ʧ��!\n"));
//        // return -1; // ����ѡ���˳����������
//    }
//    else
//    {
//        OutputDebugString(_T("������سɹ�!\n"));
//        if (g_missileImageInfo.pTex) {
//            TCHAR texInfoMsg[100];
//            _stprintf_s(texInfoMsg, _T("����ߴ�: %d x %d\n"), g_missileImageInfo.width, g_missileImageInfo.height);
//            OutputDebugString(texInfoMsg);
//        } else {
//            OutputDebugString(_T("����: LoadTex ���سɹ����� pTex Ϊ NULL!\n"));
//        }
//    }
//
//
//    InitTimer(); // ��ʼ����ʱ��
//
//    OutputDebugString(_T("��������Ϣѭ��...\n"));
//
//    // ����Ϣѭ��
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
//            g_pGraphics->BeginDraw(); // !! ȷ�� BeginDraw ��ȡ��ע������Ч !!
//            RenderMissiles();
//            g_pGraphics->EndDraw();   // !! ȷ�� EndDraw ��ȡ��ע������Ч !!
//        }
//    }
//    OutputDebugString(_T("�˳�����Ϣѭ����\n"));
//
//    // ������Դ
//    if (g_missileImageInfo.pTex) {
//        g_missileImageInfo.pTex->Release();
//        g_missileImageInfo.pTex = nullptr;
//        OutputDebugString(_T("�����������ͷš�\n"));
//    }
//    // g_pGraphics->CloseDX(); // �����������
//
//    return (int)msg.wParam;
//}
//
//// --- ���ڹ��̺��� ---
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    switch (msg)
//    {
//        case WM_KEYDOWN:
//        {
//            TCHAR debugMsg[100];
//            _stprintf_s(debugMsg, _T("WM_KEYDOWN received! wParam = 0x%X (%d)\n"), wParam, wParam);
//            OutputDebugString(debugMsg); // ����յ��İ�����Ϣ
//
//            if (wParam == VK_SPACE) { // VK_SPACE ��ֵ�� 32 (0x20)
//                OutputDebugString(_T(">>> VK_SPACE detected! Calling LaunchMissile...\n")); // ȷ�Ͽո������⵽
//                LaunchMissile(); // ���÷��亯��
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
//// --- ���ܺ��� ---
//
///**
// * @brief ��ʼ���߾��ȼ�ʱ����
// */
//void InitTimer()
//{
//    QueryPerformanceFrequency(&g_timerFrequency);
//    QueryPerformanceCounter(&g_lastTime);
//}
//
///**
// * @brief ��ȡ���ϴε��� GetDeltaTime() ����������ʱ�䣨����ʱ�䣩��
// * @return float ���ص�ʱ������Ϊ��λ��
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
// * @brief ����һö������
// */
//void LaunchMissile() {
//    OutputDebugString(_T("--- LaunchMissile called.\n"));
//
//    Missile newMissile;
//    newMissile.position = D3DXVECTOR3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f);
//    float angle = g_randomAngleDist(g_randomEngine);
//    D3DXVECTOR3 direction = D3DXVECTOR3(cosf(angle), sinf(angle), 0.0f);
//    D3DXVec3Normalize(&direction, &direction); // ȷ���ǵ�λ����
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
// * @brief �������л������λ�ú�״̬��
// */
//void UpdateMissiles(float deltaTime) {
//    // ���û�е�����ֱ�ӷ���
//    if (g_missiles.empty()) return;
//
//    // ���������Ϣ����ѡ�����ܻ�ˢ����
//    // TCHAR updateMsg[50];
//    // _stprintf_s(updateMsg, _T("UpdateMissiles called. DeltaTime: %.4f\n"), deltaTime);
//    // OutputDebugString(updateMsg);
//
//    for (auto it = g_missiles.begin(); it != g_missiles.end(); /* iterator controlled inside */) {
//        if (it->active) {
//            it->position += it->velocity * deltaTime;
//            // ���߽� (ʹ�� g_missileImageInfo �ߴ���һ�㻺��)
//            float marginX = g_missileImageInfo.width > 0 ? g_missileImageInfo.width : 50.0f; // ���ͼƬδ���أ���Ĭ��ֵ
//            float marginY = g_missileImageInfo.height > 0 ? g_missileImageInfo.height : 50.0f;
//            if (it->position.x < -marginX || it->position.x > WINDOW_WIDTH + marginX ||
//                it->position.y < -marginY || it->position.y > WINDOW_HEIGHT + marginY) {
//                it->active = false; // ���Ϊ�ǻ
//                OutputDebugString(_T("Missile deactivated (out of bounds).\n"));
//            }
//            ++it; // �ƶ�����һ��
//        } else {
//            // �Ƴ��ǻ����
//            it = g_missiles.erase(it);
//            OutputDebugString(_T("Inactive missile removed from vector.\n"));
//        }
//    }
//}
//
///**
// * @brief ��Ⱦ���л�ĵ�����
// */
//void RenderMissiles() {
//    if (!g_missileImageInfo.pTex) {
//        // �Ѿ��� WinMain ����ʱ�����ˣ��������ʡ�Ե�����������߱���
//        // OutputDebugString(_T("RenderMissiles skipped: Texture not loaded.\n"));
//        return;
//    }
//
//    // ���ͼƬ��Ϣ�Ƿ���Ч
//    if (g_missileImageInfo.width == 0 || g_missileImageInfo.height == 0) {
//        OutputDebugString(_T("RenderMissiles Warning: Missile texture info width or height is zero.\n"));
//        // �����޷���ȷ�������ĵ㣬���Գ��Ը���Ĭ��ֵ��ֱ�ӷ���
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
//            // ���������Ϣ (��ѡ)
//            // TCHAR drawMsg[150];
//            // _stprintf_s(drawMsg, _T("Rendering missile at (%.1f, %.1f)\n"), missile.position.x, missile.position.y);
//            // OutputDebugString(drawMsg);
//
//            g_pGraphics->DrawTex( // !! ȷ�� DrawTex ��ȡ��ע������Ч !!
//                    g_missileImageInfo,
//                    missile.position,
//                    center,
//                    D3DXVECTOR3(0, 0, missile.rotation),
//                    D3DXVECTOR3(1, 1, 1)
//            );
//        }
//    }
//
//    // ���������Ϊ�յ�û�л����κζ��������������е����� inactive ��
//    if (!drewAny && !g_missiles.empty()) {
//        OutputDebugString(_T("RenderMissiles: Found missiles in vector, but none were active to draw.\n"));
//    }
//}
//
//
