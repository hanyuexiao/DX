#include "Model.h"
// Graphic.h 已经在 Model.h 中包含

// Assimp 相关头文件
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string> // 用于 std::to_string, std::string
#include <vector> // 用于 std::vector
#include <locale> // for tolower

// 定义FBX顶点格式的FVF (确保与 Model.h 中的 FBXVertex 结构匹配)
#define D3DFVF_FBXVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

// 构造函数
Model::Model() :
        GameObject("Model"), // 调用基类 GameObject 的构造函数，可以提供一个默认名字
        m_modelType(ModelType::NONE),
        m_filePath(""),
        m_pMesh(nullptr),
        m_dwFBXVertexFVF(D3DFVF_FBXVERTEX),
        m_dwNumMaterials(0) {
    m_vSubMeshes.clear();
    m_vMaterials.clear();
    m_vTextures.clear(); // m_vTextures 是 std::vector<CGraphic::MyImageInfo>
}

// 析构函数
Model::~Model() {
    ReleaseResources();
}

// 释放所有已分配的资源
void Model::ReleaseResources() {

    if (m_pMesh) {
        m_pMesh->Release();
        m_pMesh = nullptr;
    }

    for (auto& subMesh : m_vSubMeshes) {
        if (subMesh.vb) {
            subMesh.vb->Release();
            subMesh.vb = nullptr;
        }
        if (subMesh.ib) {
            subMesh.ib->Release();
            subMesh.ib = nullptr;
        }
    }
    m_vSubMeshes.clear();

    // 释放 MyImageInfo 中的纹理指针
    for (auto& imageInfo : m_vTextures) {
        if (imageInfo.pTex) { // 注意这里是 pTex
            imageInfo.pTex->Release();
            imageInfo.pTex = nullptr;
        }
    }
    m_vTextures.clear();
    m_vMaterials.clear();

    m_dwNumMaterials = 0;
    m_modelType = ModelType::NONE;
    m_filePath = "";
}

// 从 .X 文件加载模型
bool Model::LoadXModelFromFile(const std::string& modelFilePath) {
    ReleaseResources();
    m_filePath = modelFilePath;
    m_modelType = ModelType::NONE;

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        OutputDebugStringA("LoadXModelFromFile Error: No D3D Device found.\n");
        return false;
    }

    LPD3DXBUFFER pD3DXMtrlBuffer = nullptr;
    HRESULT hr = D3DXLoadMeshFromX(
            modelFilePath.c_str(),
            D3DXMESH_SYSTEMMEM,
            pd3dDevice,
            NULL,
            &pD3DXMtrlBuffer,
            NULL,
            &m_dwNumMaterials,
            &m_pMesh
    );

    if (FAILED(hr)) {
        std::string errorMsg = "Failed to load .X model: " + modelFilePath + ". HRESULT: " + std::to_string(hr) + "\n";
        OutputDebugStringA(errorMsg.c_str());
        if (pD3DXMtrlBuffer) pD3DXMtrlBuffer->Release();
        return false;
    }

    if (m_dwNumMaterials == 0 && m_pMesh != nullptr) {
        m_dwNumMaterials = 1;
        D3DMATERIAL9 defaultMat;
        ZeroMemory(&defaultMat, sizeof(D3DMATERIAL9));
        defaultMat.Diffuse.r = defaultMat.Ambient.r = 0.8f;
        defaultMat.Diffuse.g = defaultMat.Ambient.g = 0.8f;
        defaultMat.Diffuse.b = defaultMat.Ambient.b = 0.8f;
        defaultMat.Diffuse.a = defaultMat.Ambient.a = 1.0f;
        m_vMaterials.push_back(defaultMat);
        m_vTextures.resize(1); // 创建一个默认的 MyImageInfo，其 pTex 将为 nullptr
    }
    else if (pD3DXMtrlBuffer != nullptr && m_dwNumMaterials > 0) {
        D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
        m_vMaterials.resize(m_dwNumMaterials);
        m_vTextures.resize(m_dwNumMaterials); // 为每个材质创建一个 MyImageInfo 对象

        std::string modelDir = "";
        size_t lastSlash = modelFilePath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            modelDir = modelFilePath.substr(0, lastSlash + 1);
        }

        for (DWORD i = 0; i < m_dwNumMaterials; i++) {
            m_vMaterials[i] = d3dxMaterials[i].MatD3D;
            m_vMaterials[i].Ambient = m_vMaterials[i].Diffuse;

            if (d3dxMaterials[i].pTextureFilename && strlen(d3dxMaterials[i].pTextureFilename) > 0) {
                std::string texturePathStr = modelDir + d3dxMaterials[i].pTextureFilename;
                // 调用 LoadTex，第二个参数是 MyImageInfo&
                if (!CGraphic::GetSingleObjPtr()->LoadTex(texturePathStr.c_str(), m_vTextures[i])) {
                    if (!CGraphic::GetSingleObjPtr()->LoadTex(d3dxMaterials[i].pTextureFilename, m_vTextures[i])) {
                        std::string texErrorMsg = "Warning: Failed to load texture for .X model: " + std::string(d3dxMaterials[i].pTextureFilename) + "\n";
                        OutputDebugStringA(texErrorMsg.c_str());
                    }
                }
            }
        }
    }

    if (pD3DXMtrlBuffer) {
        pD3DXMtrlBuffer->Release();
    }

    if (m_pMesh) {
        m_modelType = ModelType::X_MODEL;
        return true;
    }
    return false;
}

// 从 .FBX 文件加载模型
bool Model::LoadFBXModelFromFile(const std::string& modelFilePath) {
    ReleaseResources();
    m_filePath = modelFilePath;
    m_modelType = ModelType::NONE;

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        OutputDebugStringA("LoadFBXModelFromFile Error: No D3D Device found.\n");
        return false;
    }

    Assimp::Importer importer;
    unsigned int assimpFlags = aiProcess_Triangulate | aiProcess_ConvertToLeftHanded |
                               aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
                               aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure |
                               aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials |
                               aiProcess_SortByPType | aiProcess_FindInvalidData | aiProcess_FlipUVs;

    const aiScene* pScene = importer.ReadFile(modelFilePath.c_str(), assimpFlags);

    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
        std::string errorMsg = "ASSIMP Error loading FBX model: " + std::string(importer.GetErrorString()) + "\n";
        OutputDebugStringA(errorMsg.c_str());
        return false;
    }

    m_dwNumMaterials = pScene->mNumMaterials;
    if (m_dwNumMaterials > 0) {
        m_vMaterials.resize(m_dwNumMaterials);
        m_vTextures.resize(m_dwNumMaterials); // 创建 MyImageInfo 对象

        std::string modelDir = "";
        size_t lastSlash = modelFilePath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            modelDir = modelFilePath.substr(0, lastSlash + 1);
        }

        for (unsigned int i = 0; i < m_dwNumMaterials; ++i) {
            aiMaterial* pAiMaterial = pScene->mMaterials[i];
            D3DMATERIAL9 d3dMaterial;
            ZeroMemory(&d3dMaterial, sizeof(D3DMATERIAL9));
            aiColor4D color;

            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                d3dMaterial.Diffuse = { color.r, color.g, color.b, color.a };
            }
            else { d3dMaterial.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; }
            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_AMBIENT, &color) == AI_SUCCESS) {
                d3dMaterial.Ambient = { color.r, color.g, color.b, color.a };
            }
            else { d3dMaterial.Ambient = d3dMaterial.Diffuse; }
            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_SPECULAR, &color) == AI_SUCCESS) {
                d3dMaterial.Specular = { color.r, color.g, color.b, color.a };
            }
            else { d3dMaterial.Specular = { 0.5f, 0.5f, 0.5f, 1.0f }; }
            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_EMISSIVE, &color) == AI_SUCCESS) {
                d3dMaterial.Emissive = { color.r, color.g, color.b, color.a };
            }
            else { d3dMaterial.Emissive = { 0.0f, 0.0f, 0.0f, 1.0f }; }
            float shininessFactor = 0.0f; unsigned int max = 1;
            if (aiGetMaterialFloatArray(pAiMaterial, AI_MATKEY_SHININESS, &shininessFactor, &max) == AI_SUCCESS) {
                d3dMaterial.Power = shininessFactor;
            }
            else { d3dMaterial.Power = 32.0f; }
            if (d3dMaterial.Power <= 0.0f && (d3dMaterial.Specular.r > 0.0f || d3dMaterial.Specular.g > 0.0f || d3dMaterial.Specular.b > 0.0f)) {
                d3dMaterial.Power = 32.0f;
            }
            m_vMaterials[i] = d3dMaterial;

            aiString aiTexPath;
            if (pAiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS) {
                std::string textureFilename = aiTexPath.C_Str();
                if (!textureFilename.empty()) {
                    std::string fullTexturePath = modelDir + textureFilename;
                    bool isAbsolutePath = (textureFilename.length() > 1 && textureFilename[1] == ':') || (textureFilename.length() > 0 && (textureFilename[0] == '/' || textureFilename[0] == '\\'));
                    if (isAbsolutePath) {
                        if (!CGraphic::GetSingleObjPtr()->LoadTex(textureFilename.c_str(), m_vTextures[i])) {
                            std::string texError = "Warning: Failed to load absolute texture path for FBX: " + textureFilename + "\n";
                            OutputDebugStringA(texError.c_str());
                        }
                    }
                    else {
                        if (!CGraphic::GetSingleObjPtr()->LoadTex(fullTexturePath.c_str(), m_vTextures[i])) {
                            if (!CGraphic::GetSingleObjPtr()->LoadTex(textureFilename.c_str(), m_vTextures[i])) {
                                std::string texError = "Warning: Failed to load texture for FBX (tried relative and direct): " + textureFilename + "\n";
                                OutputDebugStringA(texError.c_str());
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        m_dwNumMaterials = 1;
        m_vMaterials.resize(1);
        m_vTextures.resize(1);
        ZeroMemory(&m_vMaterials[0], sizeof(D3DMATERIAL9));
        m_vMaterials[0].Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
        m_vMaterials[0].Ambient = { 0.8f, 0.8f, 0.8f, 1.0f };
        m_vMaterials[0].Specular = { 0.2f, 0.2f, 0.2f, 1.0f };
        m_vMaterials[0].Power = 32.0f;
    }

    m_vSubMeshes.reserve(pScene->mNumMeshes);
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
        aiMesh* pAiMesh = pScene->mMeshes[i];
        if (pAiMesh->mNumVertices == 0 || pAiMesh->mNumFaces == 0) continue;

        SubMesh currentSubMesh;
        currentSubMesh.numVertices = pAiMesh->mNumVertices;
        currentSubMesh.numFaces = pAiMesh->mNumFaces;
        currentSubMesh.materialIndex = pAiMesh->mMaterialIndex;

        if (pScene->mNumMaterials == 0 && m_dwNumMaterials == 1) {
            currentSubMesh.materialIndex = 0;
        }
        if (currentSubMesh.materialIndex >= m_dwNumMaterials) {
            std::string matError = "Warning: Mesh material index " + std::to_string(currentSubMesh.materialIndex) +
                                   " is out of bounds (" + std::to_string(m_dwNumMaterials) + "). Using material 0.\n";
            OutputDebugStringA(matError.c_str());
            currentSubMesh.materialIndex = (m_dwNumMaterials > 0) ? 0 : 0;
            if (m_dwNumMaterials == 0) {
                OutputDebugStringA("Error: No materials loaded, cannot assign material to submesh. Skipping submesh.\n");
                continue;
            }
        }

        std::vector<FBXVertex> vertices;
        vertices.reserve(pAiMesh->mNumVertices);
        for (unsigned int v = 0; v < pAiMesh->mNumVertices; ++v) {
            FBXVertex vertex;
            vertex.position = { pAiMesh->mVertices[v].x, pAiMesh->mVertices[v].y, pAiMesh->mVertices[v].z };
            if (pAiMesh->HasNormals()) {
                vertex.normal = { pAiMesh->mNormals[v].x, pAiMesh->mNormals[v].y, pAiMesh->mNormals[v].z };
            }
            else { vertex.normal = { 0.0f, 1.0f, 0.0f }; }
            if (pAiMesh->HasTextureCoords(0)) {
                vertex.texcoord = { pAiMesh->mTextureCoords[0][v].x, pAiMesh->mTextureCoords[0][v].y };
            }
            else { vertex.texcoord = { 0.0f, 0.0f }; }
            vertices.push_back(vertex);
        }

        HRESULT hr = pd3dDevice->CreateVertexBuffer(
                vertices.size() * sizeof(FBXVertex), D3DUSAGE_WRITEONLY,
                m_dwFBXVertexFVF, D3DPOOL_MANAGED, &currentSubMesh.vb, NULL);
        if (FAILED(hr)) {
            std::string vbError = "Failed to create Vertex Buffer for SubMesh " + std::to_string(i) + ". HRESULT: " + std::to_string(hr) + "\n";
            OutputDebugStringA(vbError.c_str());
            continue;
        }
        VOID* pVBData;
        if (SUCCEEDED(currentSubMesh.vb->Lock(0, 0, &pVBData, 0))) {
            memcpy(pVBData, vertices.data(), vertices.size() * sizeof(FBXVertex));
            currentSubMesh.vb->Unlock();
        }
        else {
            std::string lockError = "Failed to lock Vertex Buffer for SubMesh " + std::to_string(i) + "\n";
            OutputDebugStringA(lockError.c_str());
            currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr;
            continue;
        }

        std::vector<DWORD> indices;
        indices.reserve(pAiMesh->mNumFaces * 3);
        for (unsigned int f = 0; f < pAiMesh->mNumFaces; ++f) {
            const aiFace& face = pAiMesh->mFaces[f];
            if (face.mNumIndices != 3) continue;
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }
        if (indices.empty() && pAiMesh->mNumFaces > 0) {
            std::string idxError = "Warning: SubMesh " + std::to_string(i) + " has faces but no indices were processed.\n";
            OutputDebugStringA(idxError.c_str());
            if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; }
            continue;
        }
        if (indices.empty()) {
            if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; }
            continue;
        }

        hr = pd3dDevice->CreateIndexBuffer(
                indices.size() * sizeof(DWORD), D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX32, D3DPOOL_MANAGED, &currentSubMesh.ib, NULL);
        if (FAILED(hr)) {
            std::string ibError = "Failed to create Index Buffer for SubMesh " + std::to_string(i) + ". HRESULT: " + std::to_string(hr) + "\n";
            OutputDebugStringA(ibError.c_str());
            if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; }
            continue;
        }
        VOID* pIBData;
        if (SUCCEEDED(currentSubMesh.ib->Lock(0, 0, &pIBData, 0))) {
            memcpy(pIBData, indices.data(), indices.size() * sizeof(DWORD));
            currentSubMesh.ib->Unlock();
        }
        else {
            std::string lockError = "Failed to lock Index Buffer for SubMesh " + std::to_string(i) + "\n";
            OutputDebugStringA(lockError.c_str());
            if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; }
            currentSubMesh.ib->Release(); currentSubMesh.ib = nullptr;
            continue;
        }
        m_vSubMeshes.push_back(currentSubMesh);
    }

    if (m_vSubMeshes.empty() && pScene->mNumMeshes > 0) {
        OutputDebugStringA("Warning: FBX scene has meshes, but no submeshes were processed successfully.\n");
    }
    m_modelType = ModelType::FBX_MODEL;
    return true;
}

// 通用加载函数
bool Model::LoadGenericModel(const std::string& modelFilePath) {
    std::string extension = "";
    size_t dotPos = modelFilePath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 < modelFilePath.length()) {
        extension = modelFilePath.substr(dotPos + 1);
    }
    std::string lower_extension = extension;
    for (char& c : lower_extension) {
        c = static_cast<char>(tolower(c)); // 使用 tolower 需要 #include <locale> 或 <cctype>
    }

    if (lower_extension == "x") {
        return LoadXModelFromFile(modelFilePath);
    }
    else if (lower_extension == "fbx") {
        return LoadFBXModelFromFile(modelFilePath);
    }
    std::string extError = "Unsupported model format or unknown extension: " + modelFilePath + " (ext: " + extension + ")\n";
    OutputDebugStringA(extError.c_str());
    return false;
}

// 渲染函数
void Model::Render(LPDIRECT3DDEVICE9 pd3dDevice) {
    if (!pd3dDevice) { // GameObject 中的 transform 是对象实例，通常不需要检查是否为 nullptr
        OutputDebugStringA("Model::Render Error: No D3D Device.\n");
        return;
    }

    // 直接访问继承自 GameObject 的 transform 成员对象，并调用其 GetWorldMatrix 方法
    D3DXMATRIXA16 matWorld = transform.GetWorldMatrix();
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    if (m_modelType == ModelType::X_MODEL && m_pMesh) {
        for (DWORD i = 0; i < m_dwNumMaterials; i++) {
            if (i < m_vMaterials.size()) {
                pd3dDevice->SetMaterial(&m_vMaterials[i]);
            }
            else if (!m_vMaterials.empty()) {
                pd3dDevice->SetMaterial(&m_vMaterials[0]);
            }
            // 使用 MyImageInfo 中的 pTex
            if (i < m_vTextures.size() && m_vTextures[i].pTex != nullptr) {
                pd3dDevice->SetTexture(0, m_vTextures[i].pTex);
            }
            else {
                pd3dDevice->SetTexture(0, nullptr);
            }
            m_pMesh->DrawSubset(i);
        }
    }
    else if (m_modelType == ModelType::FBX_MODEL && !m_vSubMeshes.empty()) {
        for (const auto& subMesh : m_vSubMeshes) {
            if (subMesh.vb && subMesh.ib && subMesh.numFaces > 0) {
                pd3dDevice->SetStreamSource(0, subMesh.vb, 0, sizeof(FBXVertex));
                pd3dDevice->SetFVF(m_dwFBXVertexFVF);
                pd3dDevice->SetIndices(subMesh.ib);

                UINT matIdxToUse = subMesh.materialIndex;
                if (matIdxToUse >= m_dwNumMaterials) { // 确保索引在范围内
                    matIdxToUse = (m_dwNumMaterials > 0) ? 0 : 0; // 回退到第一个材质
                    if (m_dwNumMaterials == 0) {
                        pd3dDevice->SetTexture(0, nullptr); // 清除纹理
                        pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, subMesh.numVertices, 0, subMesh.numFaces);
                        continue; // 如果没有材质，则不设置材质，直接绘制
                    }
                }
                // 只有在 matIdxToUse 有效且 m_vMaterials 不为空时才设置材质
                if (matIdxToUse < m_vMaterials.size()) {
                    pd3dDevice->SetMaterial(&m_vMaterials[matIdxToUse]);
                }

                // 使用 MyImageInfo 中的 pTex
                if (matIdxToUse < m_vTextures.size() && m_vTextures[matIdxToUse].pTex != nullptr) {
                    pd3dDevice->SetTexture(0, m_vTextures[matIdxToUse].pTex);
                }
                else {
                    pd3dDevice->SetTexture(0, nullptr);
                }
                pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, subMesh.numVertices, 0, subMesh.numFaces);
            }
        }
    }
}
