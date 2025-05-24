// Created by admin on 2025/5/21.
//

#include "head.h"      // 你的项目通用头文件
#include "Graphic.h"   // 你的图形类
#include "Model.h"     // 你的模型类
#include "CGameObject.h" // 你的游戏对象基类 (确保 Model.h 中 Model 继承自 GameObject)
#include "CTransform.h"  // 你的变换类

// 全局变量
// LPD3DXMESH g_pMesh = nullptr; // 旧的D3DX网格，可以保留或移除
// LPD3DXMESH g_pMesh2 = nullptr; // 旧的D3DX网格，可以保留或移除

D3DXVECTOR3 vEye{ 0.0f, 20.0f, -100.0f }; // 摄像机位置
D3DXVECTOR3 vAt{ 0.0f, 0.0f, 0.0f };    // 摄像机观察目标
D3DXVECTOR3 vUp{ 0.0f, 1.0f, 0.0f };     // 摄像机向上的向量

// Model* g_pDogModel = nullptr; // 用于加载 .x 模型的指针 (可以保留用于对比或移除)
Model* g_pFbxModel = nullptr;   // 新增：用于加载 .fbx 模型的指针

// 窗口事件处理函数
LRESULT EventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY: // 窗口被销毁
        {
            // if (g_pDogModel) { // 清理旧的 .x 模型
            //     delete g_pDogModel;
            //     g_pDogModel = nullptr;
            // }
            if (g_pFbxModel) { // 清理新的 .fbx 模型
                delete g_pFbxModel;
                g_pFbxModel = nullptr;
            }
            PostQuitMessage(0); // 发送退出消息
            break;
        }
        case WM_KEYDOWN: // 键盘按下事件
        {
            // 简单的摄像机控制
            if (wParam == 'A') { vEye.x -= 1.5f; }
            if (wParam == 'D') { vEye.x += 1.5f; }
            if (wParam == 'W') { vEye.z += 1.5f; }
            if (wParam == 'S') { vEye.z -= 1.5f; }
            if (wParam == 'Q') { vEye.y += 1.5f; }
            if (wParam == 'E') { vEye.y -= 1.5f; }
            break; // 添加 break
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam); // 默认窗口处理
}

// 主函数
INT _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
    // 1. 初始化窗口
    HWND hwnd = CGraphic::GetSingleObjPtr()->InitWindow(EventProc, 800, 600, _T("DX_3D_FBX_Model_Loader"));
    if (!hwnd) {
        MessageBox(NULL, _T("窗口初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR);
        return -1;
    }

    // 2. 初始化Direct3D
    if (   !CGraphic::GetSingleObjPtr()->InitDX(hwnd)) {
        MessageBox(hwnd, _T("Direct3D 初始化失败!"), _T("错误"), MB_OK | MB_ICONERROR);
        return -1;
    }
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;

    // 3. 创建和加载模型
    // --- 注释掉旧的 .x 模型加载 ---
    /*
    g_pDogModel = new Model("Doggy_X_Model"); // 给模型一个名字
    // 确保 "Assets/dog.x" 文件存在于正确的位置 (通常是可执行文件目录下的 Assets 文件夹)
    if (g_pDogModel->LoadXModelFromFile("Assets/dog.x")) { // 或者 LoadGenericModel
        g_pDogModel->transform.SetPosition(50.0f, 0.0f, 0.0f); // 放在右边一点以便区分
        g_pDogModel->transform.SetScale(0.02f, 0.02f, 0.02f); // .x 模型通常较大
    } else {
        MessageBox(hwnd, _T("错误：无法加载 Assets/dog.x 模型！"), _T("模型加载失败"), MB_OK | MB_ICONERROR);
    }
    */

    // --- 新增：加载 .fbx 模型 ---
    g_pFbxModel = new Model("MyFBX_Model"); // 给FBX模型一个名字
    // ======== 修改为你实际的FBX文件路径 ========
    // 建议将模型文件放在可执行文件目录下的 "Assets" 文件夹中
    // 例如: "Assets/your_character.fbx" 或 "Assets/scene.fbx"
    const char* fbxModelPath = "Assets/miku_v2.fbx"; // <--- 在这里修改你的FBX文件路径!
    // 你也可以使用你上传的 "つかさ式まちたん - 副本.txt" 文件，但需要将其重命名为 .fbx 后缀
    // const char* fbxModelPath = "Assets/tsukasa_machitan.fbx";

    if (g_pFbxModel->LoadFBXModelFromFile(fbxModelPath)) { // 或者使用 LoadGenericModel
        // FBX 模型加载成功，设置其初始变换
        g_pFbxModel->transform.SetPosition(0.0f, -20.0f, 0.0f); // 调整Y轴，使其底部接近原点
        g_pFbxModel->transform.SetScale(0.1f, 0.1f, 0.1f);    // FBX模型的大小各不相同，你可能需要调整这个缩放值
        // g_pFbxModel->transform.SetRotationEuler(0, D3DXToRadian(180.0f), 0); // 例如，旋转180度使其面向摄像机
    }
    else {
        std::wstring errorMsg = L"错误：无法加载 FBX 模型！\n路径: ";
        // 将 char* 转换为 wchar_t* 以用于 MessageBoxW (或 _T宏)
        // 注意：这种转换对于非ASCII字符可能不完美，但对于简单路径通常可行
        // 更健壮的方法是使用 MultiByteToWideChar
        int bufferSize = MultiByteToWideChar(CP_ACP, 0, fbxModelPath, -1, NULL, 0);
        if (bufferSize > 0) {
            std::vector<wchar_t> widePath(bufferSize);
            MultiByteToWideChar(CP_ACP, 0, fbxModelPath, -1, &widePath[0], bufferSize);
            errorMsg += &widePath[0];
        }
        else {
            errorMsg += L"[路径转换失败]";
        }
        errorMsg += L"\n请检查文件路径和文件是否存在，以及Assimp库是否正确配置。";
        MessageBox(hwnd, errorMsg.c_str(), _T("模型加载失败"), MB_OK | MB_ICONERROR);
    }


    // 4. 设置光照
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f; light.Diffuse.g = 1.0f; light.Diffuse.b = 1.0f;
    light.Ambient.r = 0.4f; light.Ambient.g = 0.4f; light.Ambient.b = 0.4f; // 稍微增强环境光
    light.Specular.r = 0.7f; light.Specular.g = 0.7f; light.Specular.b = 0.7f;
    D3DXVECTOR3 vecDir(0.5f, -0.8f, 0.75f); // 调整光照方向
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(80, 80, 80)); // 稍微增强全局环境光
    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // 对于有缩放的模型，法线归一化很重要

    // 5. 设置渲染状态 (深度测试等)
    pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // 通常设置为 D3DCULL_CCW 或 D3DCULL_CW

    // 6. 设置投影矩阵
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 800.0f / 600.0f, 1.0f, 2000.0f); // 增加远裁剪面
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    // 7. 进入主循环
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (msg.message != WM_QUIT) // 修正循环条件
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 更新逻辑
            // float deltaTime = ... ; // 计算帧间隔时间
            // if (g_pDogModel) {
            //     g_pDogModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.0005f); // 示例旋转
            //     g_pDogModel->Update(0.0f); // 假设deltaTime为0，如果不用
            // }
            if (g_pFbxModel) {
                // g_pFbxModel->transform.RotateAxis(D3DXVECTOR3(0,1,0), 0.0005f); // 示例：让FBX模型也旋转
                g_pFbxModel->Update(0.0f); // 调用Update
            }

            // 设置视图矩阵 (每次循环都更新，因为vEye可能变化)
            D3DXMATRIX matView;
            D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
            pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

            // 渲染
            CGraphic::GetSingleObjPtr()->BeginDraw();

            // if (g_pDogModel) {
            //     g_pDogModel->Render(pd3dDevice); // 传递设备指针给Render
            // }
            if (g_pFbxModel) {
                g_pFbxModel->Render(pd3dDevice); // 传递设备指针给Render
            }

            CGraphic::GetSingleObjPtr()->EndDraw();
        }
    }

    // 8. 清理资源 (WM_DESTROY 中也会做，这里是退出循环后的最终保障)
    // if (g_pDogModel) { delete g_pDogModel; g_pDogModel = nullptr; }
    if (g_pFbxModel) { delete g_pFbxModel; g_pFbxModel = nullptr; }

    // CGraphic 单例的清理应该由其自身管理或在程序完全退出前调用一个专门的清理函数
    // 例如 CGraphic::GetSingleObjPtr()->ShutdownDX(); (如果存在这样的函数)

    return (int)msg.wParam;
}
