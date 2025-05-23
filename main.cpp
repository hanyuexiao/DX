// Created by admin on 2025/5/21.
//

#include "head.h"      // 低議�酊人�喘遊猟周
#include "Graphic.h"   // 低議夕侘窃
#include "Model.h"     // 低議庁侏窃
#include "CGameObject.h" // 低議嗄老斤�鷸�窃 (鳩隠 Model.h 嶄 Model 写覚徭 GameObject)
#include "CTransform.h"  // 低議延算窃

// 畠蕉延楚
// LPD3DXMESH g_pMesh = nullptr; // 症議D3DX利鯉��辛參隠藻賜卞茅
// LPD3DXMESH g_pMesh2 = nullptr; // 症議D3DX利鯉��辛參隠藻賜卞茅

D3DXVECTOR3 vEye{ 0.0f, 20.0f, -100.0f }; // 父�饂�了崔
D3DXVECTOR3 vAt{ 0.0f, 0.0f, 0.0f };    // 父�饂�鉱賀朕炎
D3DXVECTOR3 vUp{ 0.0f, 1.0f, 0.0f };     // 父�饂��鯢狼掴鯀�

// Model* g_pDogModel = nullptr; // 喘噐紗墮 .x 庁侏議峺寞 (辛參隠藻喘噐斤曳賜卞茅)
Model* g_pFbxModel = nullptr;   // 仟奐�采池攫嘖� .fbx 庁侏議峺寞

// 完笥並周侃尖痕方
LRESULT EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY: // 完笥瓜��支
        {
            // if (g_pDogModel) { // 賠尖症議 .x 庁侏
            //     delete g_pDogModel;
            //     g_pDogModel = nullptr;
            // }
            if (g_pFbxModel) { // 賠尖仟議 .fbx 庁侏
                delete g_pFbxModel;
                g_pFbxModel = nullptr;
            }
            PostQuitMessage(0); // 窟僕曜竃��連
            break;
        }
        case WM_KEYDOWN: // 囚徒梓和並周
        {
            // 酒汽議父�饂�陣崙
            if (wParam == 'A') { vEye.x -= 1.5f; }
            if (wParam == 'D') { vEye.x += 1.5f; }
            if (wParam == 'W') { vEye.z += 1.5f; }
            if (wParam == 'S') { vEye.z -= 1.5f; }
            if (wParam == 'Q') { vEye.y += 1.5f; }
            if (wParam == 'E') { vEye.y -= 1.5f; }
            break; // 耶紗 break
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam); // 潮範完笥侃尖
}

// 麼痕方
INT _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
    // 1. 兜兵晒完笥
    HWND hwnd = CGraphic::GetSingleObjPtr()->InitWindow(EventProc, 800, 600, _T("DX_3D_FBX_Model_Loader"));
    if (!hwnd) {
        MessageBox(NULL, _T("完笥兜兵晒払移!"), _T("危列"), MB_OK | MB_ICONERROR);
        return -1;
    }

    // 2. 兜兵晒Direct3D
    if (   !CGraphic::GetSingleObjPtr()->InitDX(hwnd)) {
        MessageBox(hwnd, _T("Direct3D 兜兵晒払移!"), _T("危列"), MB_OK | MB_ICONERROR);
        return -1;
    }
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;

    // 3. 幹秀才紗墮庁侏
    // --- 廣瞥渠症議 .x 庁侏紗墮 ---
    /*
    g_pDogModel = new Model("Doggy_X_Model"); // 公庁侏匯倖兆忖
    // 鳩隠 "Assets/dog.x" 猟周贋壓噐屎鳩議了崔 (宥械頁辛峇佩猟周朕村和議 Assets 猟周斜)
    if (g_pDogModel->LoadXModelFromFile("Assets/dog.x")) { // 賜宀 LoadGenericModel
        g_pDogModel->transform.SetPosition(50.0f, 0.0f, 0.0f); // 慧壓嘔円匯泣參宴曝蛍
        g_pDogModel->transform.SetScale(0.02f, 0.02f, 0.02f); // .x 庁侏宥械熟寄
    } else {
        MessageBox(hwnd, _T("危列�採涎┝嘖� Assets/dog.x 庁侏��"), _T("庁侏紗墮払移"), MB_OK | MB_ICONERROR);
    }
    */

    // --- 仟奐�瑳嘖� .fbx 庁侏 ---
    g_pFbxModel = new Model("MyFBX_Model"); // 公FBX庁侏匯倖兆忖
    // ======== 俐個葎低糞縞議FBX猟周揃抄 ========
    // 秀咏繍庁侏猟周慧壓辛峇佩猟周朕村和議 "Assets" 猟周斜嶄
    // 箭泌: "Assets/your_character.fbx" 賜 "Assets/scene.fbx"
    const char* fbxModelPath = "Assets/miku_v2.fbx"; // <--- 壓宸戦俐個低議FBX猟周揃抄!
    // 低匆辛參聞喘低貧勧議 "つかさ塀まちたん - 険云.txt" 猟周��徽俶勣繍凪嶷凋兆葎 .fbx 朔弸
    // const char* fbxModelPath = "Assets/tsukasa_machitan.fbx";

    if (g_pFbxModel->LoadFBXModelFromFile(fbxModelPath)) { // 賜宀聞喘 LoadGenericModel
        // FBX 庁侏紗墮撹孔��譜崔凪兜兵延算
        g_pFbxModel->transform.SetPosition(0.0f, -20.0f, 0.0f); // 距屁Y已��聞凪久何俊除圻泣
        g_pFbxModel->transform.SetScale(0.1f, 0.1f, 0.1f);    // FBX庁侏議寄弌光音�猴���低辛嬬俶勣距屁宸倖抹慧峙
        // g_pFbxModel->transform.SetRotationEuler(0, D3DXToRadian(180.0f), 0); // 箭泌��傴廬180業聞凪中�鯢穗饂�
    }
    else {
        std::wstring errorMsg = L"危列�採涎┝嘖� FBX 庁侏��\n揃抄: ";
        // 繍 char* 廬算葎 wchar_t* 參喘噐 MessageBoxW (賜 _T崎)
        // 廣吭�砕瞞嶐�算斤噐掲ASCII忖憲辛嬬音頼胆��徽斤噐酒汽揃抄宥械辛佩
        // 厚宗彝議圭隈頁聞喘 MultiByteToWideChar
        int bufferSize = MultiByteToWideChar(CP_ACP, 0, fbxModelPath, -1, NULL, 0);
        if (bufferSize > 0) {
            std::vector<wchar_t> widePath(bufferSize);
            MultiByteToWideChar(CP_ACP, 0, fbxModelPath, -1, &widePath[0], bufferSize);
            errorMsg += &widePath[0];
        }
        else {
            errorMsg += L"[揃抄廬算払移]";
        }
        errorMsg += L"\n萩殊臥猟周揃抄才猟周頁倦贋壓��參式Assimp垂頁倦屎鳩塘崔。";
        MessageBox(hwnd, errorMsg.c_str(), _T("庁侏紗墮払移"), MB_OK | MB_ICONERROR);
    }


    // 4. 譜崔高孚
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f;
    light.Ambient.r = 0.4f; light.Ambient.g = 0.4f; light.Ambient.b = 0.4f; // 不裏奐膿桟廠高
    light.Specular.r = 0.7f; light.Specular.g = 0.7f; light.Specular.b = 0.7f;
    D3DXVECTOR3 vecDir(0.5f, -0.8f, 0.75f); // 距屁高孚圭��
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(80, 80, 80)); // 不裏奐膿畠蕉桟廠高
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // 斤噐嗤抹慧議庁侏��隈�濆蚋算�載嶷勣

    // 5. 譜崔籌半彜蓑 (侮業霞編吉)
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // 宥械譜崔葎 D3DCULL_CCW 賜 D3DCULL_CW

    // 6. 譜崔誘唹裳專
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 800.0f / 600.0f, 1.0f, 2000.0f); // 奐紗垓加儒中
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    // 7. 序秘麼儉桟
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (msg.message != WM_QUIT) // 俐屎儉桟訳周
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 厚仟貸辞
            // float deltaTime = ... ; // 柴麻屐寂侯扮寂
            // if (g_pDogModel) {
            //     g_pDogModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.0005f); // 幣箭傴廬
            //     g_pDogModel->Update(0.0f); // 邪譜deltaTime葎0��泌惚音喘
            // }
            if (g_pFbxModel) {
                // g_pFbxModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.0005f); // 幣箭�哉�FBX庁侏匆傴廬
                g_pFbxModel->Update(0.0f); // 距喘Update
            }

            // 譜崔篇夕裳專 (耽肝儉桟脅厚仟��咀葎vEye辛嬬延晒)
            D3DXMATRIX matView;
            D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
            pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

            // 籌半
            CGraphic::GetSingleObjPtr()->BeginDraw();

            // if (g_pDogModel) {
            //     g_pDogModel->Render(pd3dDevice); // 勧弓譜姥峺寞公Render
            // }
            if (g_pFbxModel) {
                g_pFbxModel->Render(pd3dDevice); // 勧弓譜姥峺寞公Render
            }

            CGraphic::GetSingleObjPtr()->EndDraw();
        }
    }

    // 8. 賠尖彿坿 (WM_DESTROY 嶄匆氏恂��宸戦頁曜竃儉桟朔議恷嶮隠嬾)
    // if (g_pDogModel) { delete g_pDogModel; g_pDogModel = nullptr; }
    if (g_pFbxModel) { delete g_pFbxModel; g_pFbxModel = nullptr; }

    // CGraphic 汽箭議賠尖哘乎喇凪徭附砿尖賜壓殻會頼畠曜竃念距喘匯倖廨壇議賠尖痕方
    // 箭泌 CGraphic::GetSingleObjPtr()->ShutdownDX(); (泌惚贋壓宸劔議痕方)

    return (int)msg.wParam;
}
