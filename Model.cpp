//
// Created by admin on 2025/5/22.
//

#include "Model.h"

Model::Model(const string &name) :
                GameObject{name},
                m_pMesh{nullptr},
                m_dwNumMaterials{0}
                {


}

Model::~Model() {
    ReleaseResources();
}

void Model::ReleaseResources() {
    for (auto &textures : m_vTextures) {
        if(textures){
            textures->Release();
            textures = nullptr;
        }
    }
    m_vTextures.clear();

    m_vMaterials.clear();

    if(m_pMesh){
        m_pMesh->Release();
        m_pMesh = nullptr;
    }
    m_dwNumMaterials = 0;
}

bool Model::LoadModelFromFile(const std::string& modelFilePath) {
    this->filePath = modelFilePath; // 保存文件路径
    ReleaseResources(); // 先释放旧资源，这个函数我们之前是完整定义的

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        // OutputDebugString("Model::LoadModelFromFile - D3D Device is null.\n");
        return false;
    }

    ID3DXBuffer* pAdjacencyBuffer = nullptr;
    ID3DXBuffer* pMaterialBuffer = nullptr; // 用来获取材质和纹理信息

    HRESULT hr = D3DXLoadMeshFromX(
            modelFilePath.c_str(),
            D3DXMESH_MANAGED,
            pd3dDevice,
            &pAdjacencyBuffer,
            &pMaterialBuffer,  // 我们现在要用这个 buffer 了！
            nullptr,
            &m_dwNumMaterials,
            &m_pMesh
    );

    if (FAILED(hr)) {
        // TCHAR errorMsg[256];
        // _stprintf_s(errorMsg, _T("Model::LoadModelFromFile - Failed to load mesh '%s'. HRESULT: 0x%08X\n"), modelFilePath.c_str(), hr);
        // OutputDebugString(errorMsg);
        if (pAdjacencyBuffer) pAdjacencyBuffer->Release();
        if (pMaterialBuffer) pMaterialBuffer->Release();
        m_pMesh = nullptr;
        m_dwNumMaterials = 0;
        return false;
    }

    // === 开始处理材质和纹理 ===
    if (pMaterialBuffer != nullptr && m_dwNumMaterials > 0) {
        // D3DXMATERIAL 结构包含了 D3DMATERIAL9 和一个指向纹理文件名的指针
        D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();

        m_vMaterials.resize(m_dwNumMaterials);
        m_vTextures.resize(m_dwNumMaterials); // 每个材质对应一个纹理槽，可能为空

        for (DWORD i = 0; i < m_dwNumMaterials; ++i) {
            // 1. 复制材质属性 (D3DMATERIAL9)
            m_vMaterials[i] = d3dxMaterials[i].MatD3D;

            // 2. 设置环境光反射
            //    .x 文件中的环境光通常未设置或很暗，用漫反射颜色填充是个好习惯
            m_vMaterials[i].Ambient = m_vMaterials[i].Diffuse;

            // 3. 加载纹理 (如果材质指定了纹理文件名)
            m_vTextures[i] = nullptr; // 先置空
            if (d3dxMaterials[i].pTextureFilename != nullptr &&
                strlen(d3dxMaterials[i].pTextureFilename) > 0) {

                std::string textureFilenameStr = d3dxMaterials[i].pTextureFilename;
                std::string texturePath = "";

                // 构造纹理的完整路径 (假设纹理和模型在同一目录或相对路径)
                // 查找 modelFilePath 中最后一个路径分隔符
                size_t lastSlashPos = modelFilePath.find_last_of("/\\");
                if (lastSlashPos != std::string::npos) {
                    // 如果找到了，提取目录部分并拼接纹理文件名
                    texturePath = modelFilePath.substr(0, lastSlashPos + 1) + textureFilenameStr;
                } else {
                    // 如果 modelFilePath 没有路径分隔符 (例如 "dog.x")，
                    // 则认为纹理文件和模型文件都在当前工作目录下
                    texturePath = textureFilenameStr;
                }

                // 使用 CGraphic 的 LoadTex 方法加载纹理
                MyImageInfo tempImageInfo;
                ZeroMemory(&tempImageInfo, sizeof(MyImageInfo));

                // 你的 LoadTex 接受 LPCTSTR。如果你的项目是UNICODE (TCHAR 是 wchar_t)，
                // 而 texturePath 是 std::string，这里需要转换。
                // 假设你的 TString 定义与 std::string 兼容或项目为非 UNICODE。
                if (SUCCEEDED(CGraphic::GetSingleObjPtr()->LoadTex(texturePath.c_str(), tempImageInfo))) {
                    m_vTextures[i] = tempImageInfo.pTex; // pTex 是 LPDIRECT3DTEXTURE9
                } else {
                    // TCHAR errorMsg[512];
                    // _stprintf_s(errorMsg, _T("Model::LoadModelFromFile - Failed to load texture '%s' for mesh '%s'.\n"), texturePath.c_str(), modelFilePath.c_str());
                    // OutputDebugString(errorMsg);
                    m_vTextures[i] = nullptr;
                }
            }
        }
    } else if (m_pMesh != nullptr && m_dwNumMaterials == 0) {
        // 网格加载成功，但是没有材质信息
        // OutputDebugString(_T("Model::LoadModelFromFile - Mesh loaded but no material info in .x file.\n"));
    }

    // 释放临时缓冲区
    if (pAdjacencyBuffer) pAdjacencyBuffer->Release();
    if (pMaterialBuffer) pMaterialBuffer->Release(); // 材质缓冲区使用完毕后必须释放

    return true;
}


void Model::Render() {
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice || !m_pMesh) {
        return;
    }
// 1. 获取世界矩阵的引用（假设 GetWorldMatrix 返回 D3DXMATRIX&）
    D3DXMATRIX worldMatrix = transform.GetWorldMatrix();

// 2. 调用 D3DXMatrixIdentity 初始化该矩阵
    D3DXMatrixIdentity(&worldMatrix); // 正确：操作的是持久对象

// 3. 传递初始化后的矩阵地址给 SetTransform
    pd3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix);

    // === 开始应用每个子集的材质和纹理 ===
    if (m_dwNumMaterials > 0) {
        for (DWORD i = 0; i < m_dwNumMaterials; ++i) {
            // 设置当前子集的材质
            // 确保 m_vMaterials 至少有 i+1 个元素
            if (i < m_vMaterials.size()) {
                pd3dDevice->SetMaterial(&m_vMaterials[i]);
            } else {
                // 如果 m_vMaterials 不足，可能需要设置一个默认材质或发出警告
                // （理论上 m_vMaterials.size() 应该等于 m_dwNumMaterials）
                D3DMATERIAL9 defaultMat; // 备用默认材质
                ZeroMemory(&defaultMat, sizeof(D3DMATERIAL9));
                defaultMat.Diffuse.r = defaultMat.Ambient.r = 0.7f;
                defaultMat.Diffuse.g = defaultMat.Ambient.g = 0.7f;
                defaultMat.Diffuse.b = defaultMat.Ambient.b = 0.7f;
                defaultMat.Diffuse.a = defaultMat.Ambient.a = 1.0f;
                pd3dDevice->SetMaterial(&defaultMat);
            }

            // 设置当前子集的纹理 (如果存在)
            // 确保 m_vTextures 至少有 i+1 个元素
            if (i < m_vTextures.size()) {
                pd3dDevice->SetTexture(0, m_vTextures[i]); // 如果 m_vTextures[i] 为 nullptr，则禁用纹理
            } else {
                pd3dDevice->SetTexture(0, nullptr); // 确保禁用纹理
            }

            m_pMesh->DrawSubset(i); // 绘制网格的当前子集
        }
    } else if (m_pMesh) {
        // 如果没有材质信息 (m_dwNumMaterials == 0) 但网格存在，
        // 尝试将整个网格作为单个子集(0)绘制。
        // 此时需要确保设置一个默认材质和禁用纹理。
        D3DMATERIAL9 defaultMat;
        ZeroMemory(&defaultMat, sizeof(D3DMATERIAL9));
        defaultMat.Diffuse.r = defaultMat.Ambient.r = 0.7f;
        defaultMat.Diffuse.g = defaultMat.Ambient.g = 0.7f;
        defaultMat.Diffuse.b = defaultMat.Ambient.b = 0.7f;
        defaultMat.Diffuse.a = defaultMat.Ambient.a = 1.0f;
        pd3dDevice->SetMaterial(&defaultMat);
        pd3dDevice->SetTexture(0, nullptr);
        m_pMesh->DrawSubset(0);
    }
}


void Model::Update(float deltaTime) {
    GameObject::Update(deltaTime);
}


