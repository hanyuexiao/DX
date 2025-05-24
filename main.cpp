// Created by admin on 2025/5/21.
//

#include "head.h"      // �����Ŀͨ��ͷ�ļ�
#include "Graphic.h"   // ���ͼ����
#include "Model.h"     // ���ģ����
#include "CGameObject.h" // �����Ϸ������� (ȷ�� Model.h �� Model �̳��� GameObject)
#include "CTransform.h"  // ��ı任��

// ȫ�ֱ���
// LPD3DXMESH g_pMesh = nullptr; // �ɵ�D3DX���񣬿��Ա������Ƴ�
// LPD3DXMESH g_pMesh2 = nullptr; // �ɵ�D3DX���񣬿��Ա������Ƴ�

D3DXVECTOR3 vEye{ 0.0f, 20.0f, -100.0f }; // �����λ��
D3DXVECTOR3 vAt{ 0.0f, 0.0f, 0.0f };    // ������۲�Ŀ��
D3DXVECTOR3 vUp{ 0.0f, 1.0f, 0.0f };     // ��������ϵ�����

// Model* g_pDogModel = nullptr; // ���ڼ��� .x ģ�͵�ָ�� (���Ա������ڶԱȻ��Ƴ�)
Model* g_pFbxModel = nullptr;   // ���������ڼ��� .fbx ģ�͵�ָ��

// �����¼�������
LRESULT EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY: // ���ڱ�����
        {
            // if (g_pDogModel) { // ����ɵ� .x ģ��
            //     delete g_pDogModel;
            //     g_pDogModel = nullptr;
            // }
            if (g_pFbxModel) { // �����µ� .fbx ģ��
                delete g_pFbxModel;
                g_pFbxModel = nullptr;
            }
            PostQuitMessage(0); // �����˳���Ϣ
            break;
        }
        case WM_KEYDOWN: // ���̰����¼�
        {
            // �򵥵����������
            if (wParam == 'A') { vEye.x -= 1.5f; }
            if (wParam == 'D') { vEye.x += 1.5f; }
            if (wParam == 'W') { vEye.z += 1.5f; }
            if (wParam == 'S') { vEye.z -= 1.5f; }
            if (wParam == 'Q') { vEye.y += 1.5f; }
            if (wParam == 'E') { vEye.y -= 1.5f; }
            break; // ��� break
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam); // Ĭ�ϴ��ڴ���
}

// ������
INT _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
    // 1. ��ʼ������
    HWND hwnd = CGraphic::GetSingleObjPtr()->InitWindow(EventProc, 800, 600, _T("DX_3D_FBX_Model_Loader"));
    if (!hwnd) {
        MessageBox(NULL, _T("���ڳ�ʼ��ʧ��!"), _T("����"), MB_OK | MB_ICONERROR);
        return -1;
    }

    // 2. ��ʼ��Direct3D
    if (   !CGraphic::GetSingleObjPtr()->InitDX(hwnd)) {
        MessageBox(hwnd, _T("Direct3D ��ʼ��ʧ��!"), _T("����"), MB_OK | MB_ICONERROR);
        return -1;
    }
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;

    // 3. �����ͼ���ģ��
    // --- ע�͵��ɵ� .x ģ�ͼ��� ---
    /*
    g_pDogModel = new Model("Doggy_X_Model"); // ��ģ��һ������
    // ȷ�� "Assets/dog.x" �ļ���������ȷ��λ�� (ͨ���ǿ�ִ���ļ�Ŀ¼�µ� Assets �ļ���)
    if (g_pDogModel->LoadXModelFromFile("Assets/dog.x")) { // ���� LoadGenericModel
        g_pDogModel->transform.SetPosition(50.0f, 0.0f, 0.0f); // �����ұ�һ���Ա�����
        g_pDogModel->transform.SetScale(0.02f, 0.02f, 0.02f); // .x ģ��ͨ���ϴ�
    } else {
        MessageBox(hwnd, _T("�����޷����� Assets/dog.x ģ�ͣ�"), _T("ģ�ͼ���ʧ��"), MB_OK | MB_ICONERROR);
    }
    */

    // --- ���������� .fbx ģ�� ---
    g_pFbxModel = new Model("MyFBX_Model"); // ��FBXģ��һ������
    // ======== �޸�Ϊ��ʵ�ʵ�FBX�ļ�·�� ========
    // ���齫ģ���ļ����ڿ�ִ���ļ�Ŀ¼�µ� "Assets" �ļ�����
    // ����: "Assets/your_character.fbx" �� "Assets/scene.fbx"
    const char* fbxModelPath = "Assets/miku_v2.fbx"; // <--- �������޸����FBX�ļ�·��!
    // ��Ҳ����ʹ�����ϴ��� "�Ĥ���ʽ�ޤ����� - ����.txt" �ļ�������Ҫ����������Ϊ .fbx ��׺
    // const char* fbxModelPath = "Assets/tsukasa_machitan.fbx";

    if (g_pFbxModel->LoadFBXModelFromFile(fbxModelPath)) { // ����ʹ�� LoadGenericModel
        // FBX ģ�ͼ��سɹ����������ʼ�任
        g_pFbxModel->transform.SetPosition(0.0f, -20.0f, 0.0f); // ����Y�ᣬʹ��ײ��ӽ�ԭ��
        g_pFbxModel->transform.SetScale(0.1f, 0.1f, 0.1f);    // FBXģ�͵Ĵ�С������ͬ���������Ҫ�����������ֵ
        // g_pFbxModel->transform.SetRotationEuler(0, D3DXToRadian(180.0f), 0); // ���磬��ת180��ʹ�����������
    }
    else {
        std::wstring errorMsg = L"�����޷����� FBX ģ�ͣ�\n·��: ";
        // �� char* ת��Ϊ wchar_t* ������ MessageBoxW (�� _T��)
        // ע�⣺����ת�����ڷ�ASCII�ַ����ܲ������������ڼ�·��ͨ������
        // ����׳�ķ�����ʹ�� MultiByteToWideChar
        int bufferSize = MultiByteToWideChar(CP_ACP, 0, fbxModelPath, -1, NULL, 0);
        if (bufferSize > 0) {
            std::vector<wchar_t> widePath(bufferSize);
            MultiByteToWideChar(CP_ACP, 0, fbxModelPath, -1, &widePath[0], bufferSize);
            errorMsg += &widePath[0];
        }
        else {
            errorMsg += L"[·��ת��ʧ��]";
        }
        errorMsg += L"\n�����ļ�·�����ļ��Ƿ���ڣ��Լ�Assimp���Ƿ���ȷ���á�";
        MessageBox(hwnd, errorMsg.c_str(), _T("ģ�ͼ���ʧ��"), MB_OK | MB_ICONERROR);
    }


    // 4. ���ù���
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f;
    light.Ambient.r = 0.4f; light.Ambient.g = 0.4f; light.Ambient.b = 0.4f; // ��΢��ǿ������
    light.Specular.r = 0.7f; light.Specular.g = 0.7f; light.Specular.b = 0.7f;
    D3DXVECTOR3 vecDir(0.5f, -0.8f, 0.75f); // �������շ���
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(80, 80, 80)); // ��΢��ǿȫ�ֻ�����
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // ���������ŵ�ģ�ͣ����߹�һ������Ҫ

    // 5. ������Ⱦ״̬ (��Ȳ��Ե�)
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // ͨ������Ϊ D3DCULL_CCW �� D3DCULL_CW

    // 6. ����ͶӰ����
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 800.0f / 600.0f, 1.0f, 2000.0f); // ����Զ�ü���
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    // 7. ������ѭ��
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (msg.message != WM_QUIT) // ����ѭ������
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // �����߼�
            // float deltaTime = ... ; // ����֡���ʱ��
            // if (g_pDogModel) {
            //     g_pDogModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.0005f); // ʾ����ת
            //     g_pDogModel->Update(0.0f); // ����deltaTimeΪ0���������
            // }
            if (g_pFbxModel) {
                // g_pFbxModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.0005f); // ʾ������FBXģ��Ҳ��ת
                g_pFbxModel->Update(0.0f); // ����Update
            }

            // ������ͼ���� (ÿ��ѭ�������£���ΪvEye���ܱ仯)
            D3DXMATRIX matView;
            D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
            pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

            // ��Ⱦ
            CGraphic::GetSingleObjPtr()->BeginDraw();

            // if (g_pDogModel) {
            //     g_pDogModel->Render(pd3dDevice); // �����豸ָ���Render
            // }
            if (g_pFbxModel) {
                g_pFbxModel->Render(pd3dDevice); // �����豸ָ���Render
            }

            CGraphic::GetSingleObjPtr()->EndDraw();
        }
    }

    // 8. ������Դ (WM_DESTROY ��Ҳ�������������˳�ѭ��������ձ���)
    // if (g_pDogModel) { delete g_pDogModel; g_pDogModel = nullptr; }
    if (g_pFbxModel) { delete g_pFbxModel; g_pFbxModel = nullptr; }

    // CGraphic ����������Ӧ���������������ڳ�����ȫ�˳�ǰ����һ��ר�ŵ�������
    // ���� CGraphic::GetSingleObjPtr()->ShutdownDX(); (������������ĺ���)

    return (int)msg.wParam;
}
