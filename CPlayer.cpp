//
// Created by admin on 2025/5/27.
//

#include "CPlayer.h"

// Assimp 相关头文件
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string> // 用于 std::to_string, std::string
#include <vector> // 用于 std::vector
#include <locale> // for tolower
#include <iostream> // 添加 iostream 用于 std::cout, std::cerr, std::endl, std::hex, std::dec

// 定义FBX顶点格式的FVF (确保与 Model.h 中的 FBXVertex 结构匹配)
#define D3DFVF_FBXVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

// 构造函数
Player:: Player(const std::string& name) :
        GameObject("Player"),
        m_modelType(ModelType::NONE),
        m_filePath(""),
        m_pMesh(nullptr),
        m_dwFBXVertexFVF(D3DFVF_FBXVERTEX),
        m_dwNumMaterials(0),
        m_moveSpeed(0.0f),
        m_rotationSpeed(0.0f),
        isActive(true)
        {
    m_vSubMeshes.clear();
    m_vMaterials.clear();
    m_vTextures.clear();
    // std::cout << "DEBUG Model Constructor: Model '" << name << "' created." << std::endl; // 精简日志
}

// 析构函数
Player::~ Player() {
    // std::cout << "DEBUG Model Destructor: Releasing resources for model '" << this->name << "'..." << std::endl; // 精简日志
    ReleaseResources();
}

// 释放所有已分配的资源
void  Player::ReleaseResources() {
    if (m_pMesh) {
        m_pMesh->Release();
        m_pMesh = nullptr;
    }
    for (auto & m_vSubMeshe : m_vSubMeshes) {
        if (m_vSubMeshe.vb) {
            m_vSubMeshe.vb->Release();
            m_vSubMeshe.vb = nullptr;
        }
        if (m_vSubMeshe.ib) {
            m_vSubMeshe.ib->Release();
            m_vSubMeshe.ib = nullptr;
        }
    }
    m_vSubMeshes.clear();
    for (auto & m_vTexture : m_vTextures) {
        if (m_vTexture.pTex) {
            m_vTexture.pTex->Release();
            m_vTexture.pTex = nullptr;
        }
    }
    m_vTextures.clear();
    m_vMaterials.clear();
    m_dwNumMaterials = 0;
    m_modelType = ModelType::NONE;
    m_filePath = "";
    // std::cout << "DEBUG ReleaseResources: All resources cleared." << std::endl; // 精简日志
}

// 从 .X 文件加载模型 (保留了较详细的日志，因为这不是当前出问题的路径)
bool  Player::LoadXModelFromFile(const std::string& modelFilePath) {
    std::cout << "DEBUG: --- ENTERING LoadXModelFromFile --- Path: " << modelFilePath << std::endl;
    ReleaseResources();
    m_filePath = modelFilePath;
    m_modelType = ModelType::NONE;

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        std::cerr << "ERROR LoadXModelFromFile: No D3D Device found for path: " << modelFilePath << std::endl;
        return false;
    }

    LPD3DXBUFFER pD3DXMtrlBuffer = nullptr;
    HRESULT hr = D3DXLoadMeshFromX(
            modelFilePath.c_str(), D3DXMESH_SYSTEMMEM, pd3dDevice,
            NULL, &pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, &m_pMesh);

    if (FAILED(hr)) {
        std::cerr << "ERROR LoadXModelFromFile: Failed to load .X model: " << modelFilePath << ". HRESULT: 0x" << std::hex << hr << std::dec << std::endl;
        if (pD3DXMtrlBuffer) pD3DXMtrlBuffer->Release();
        return false;
    }
    std::cout << "DEBUG LoadXModelFromFile: D3DXLoadMeshFromX successful. NumMaterials from X file: " << m_dwNumMaterials << std::endl;

    if (m_dwNumMaterials == 0 && m_pMesh != nullptr) {
        m_dwNumMaterials = 1;
        D3DMATERIAL9 defaultMat; ZeroMemory(&defaultMat, sizeof(D3DMATERIAL9));
        defaultMat.Diffuse.r = defaultMat.Ambient.r = 0.8f; defaultMat.Diffuse.g = defaultMat.Ambient.g = 0.8f;
        defaultMat.Diffuse.b = defaultMat.Ambient.b = 0.8f; defaultMat.Diffuse.a = defaultMat.Ambient.a = 1.0f;
        m_vMaterials.push_back(defaultMat); m_vTextures.resize(1);
    } else if (pD3DXMtrlBuffer != nullptr && m_dwNumMaterials > 0) {
        D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
        m_vMaterials.resize(m_dwNumMaterials); m_vTextures.resize(m_dwNumMaterials);
        std::string modelDir = "";
        size_t lastSlash = modelFilePath.find_last_of("/\\");
        if (lastSlash != std::string::npos) modelDir = modelFilePath.substr(0, lastSlash + 1);

        for (DWORD i = 0; i < m_dwNumMaterials; i++) {
            m_vMaterials[i] = d3dxMaterials[i].MatD3D; m_vMaterials[i].Ambient = m_vMaterials[i].Diffuse;
            if (d3dxMaterials[i].pTextureFilename && strlen(d3dxMaterials[i].pTextureFilename) > 0) {
                std::string textureFilenameStr = d3dxMaterials[i].pTextureFilename;
                std::string texturePathStr = modelDir + textureFilenameStr;
                HRESULT hr_tex_load = CGraphic::GetSingleObjPtr()->LoadTex(texturePathStr.c_str(), m_vTextures[i], 0);
                if (FAILED(hr_tex_load)) {
                    hr_tex_load = CGraphic::GetSingleObjPtr()->LoadTex(textureFilenameStr.c_str(), m_vTextures[i], 0);
                }
                if (SUCCEEDED(hr_tex_load)) {
                    std::cout << "  LoadX: MatIdx " << i << " SUCCESS loading texture: " << (m_vTextures[i].pTex ? textureFilenameStr : "nullptr") << " -> Ptr: " << m_vTextures[i].pTex << std::endl;
                } else {
                    std::cerr << "  LoadX: MatIdx " << i << " FAILED loading texture: " << textureFilenameStr << " (HRESULT: 0x" << std::hex << hr_tex_load << std::dec << ")" << std::endl;
                }
            }
        }
    }
    if (pD3DXMtrlBuffer) pD3DXMtrlBuffer->Release();
    if (m_pMesh) { m_modelType = ModelType::X_MODEL; return true; }
    return false;
}

// 从 .FBX 文件加载模型
bool  Player::LoadFBXModelFromFile(const std::string& modelFilePath) {
    std::cout << "DEBUG: --- ENTERING LoadFBXModelFromFile --- Path: " << modelFilePath << std::endl;
    ReleaseResources(); // 清理旧资源
    m_filePath = modelFilePath;
    m_modelType = ModelType::NONE;

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        std::cerr << "ERROR LoadFBXModelFromFile: No D3D Device found for path: " << modelFilePath << std::endl;
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
        std::cerr << "ASSIMP Error loading FBX model: " << importer.GetErrorString() << std::endl;
        return false;
    }
    std::cout << "DEBUG LoadFBXModelFromFile: Assimp importer.ReadFile successful. Scene has "
              << pScene->mNumMeshes << " meshes and " << pScene->mNumMaterials << " materials." << std::endl;

    m_dwNumMaterials = pScene->mNumMaterials;
    std::string modelDir = "";
    size_t lastSlash = modelFilePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        modelDir = modelFilePath.substr(0, lastSlash + 1);
    }

    if (m_dwNumMaterials > 0) {
        m_vMaterials.resize(m_dwNumMaterials);
        m_vTextures.resize(m_dwNumMaterials);

        for (unsigned int i = 0; i < m_dwNumMaterials; ++i) {
            aiMaterial* pAiMaterial = pScene->mMaterials[i];
            D3DMATERIAL9 d3dMaterial;
            ZeroMemory(&d3dMaterial, sizeof(D3DMATERIAL9));
            aiColor4D color;

            // 为了避免潜在的由打印非ASCII名字引起的堆栈问题，这里使用占位符
            // std::cout << "DEBUG LoadFBXModelFromFile: Processing MaterialIndex " << i << " (Name from FBX: " << pAiMaterial->GetName().C_Str() << ")" << std::endl;
            std::cout << "DEBUG LoadFBXModelFromFile: Processing MaterialIndex " << i << " (Name from FBX: [MaterialNamePlaceholder])" << std::endl;


            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                d3dMaterial.Diffuse = { color.r, color.g, color.b, color.a };
            } else { d3dMaterial.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; }
            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_AMBIENT, &color) == AI_SUCCESS) {
                d3dMaterial.Ambient = { color.r, color.g, color.b, color.a };
            } else { d3dMaterial.Ambient = d3dMaterial.Diffuse; }
            // 其他材质属性可以类似加载，这里简化以减少日志
            m_vMaterials[i] = d3dMaterial;

            aiString aiTexPath;
            if (pAiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS) {
                std::string textureFilename = aiTexPath.C_Str();
                if (!textureFilename.empty()) {
                    std::string fullTexturePath;
                    bool isAbsolutePath = (textureFilename.length() > 1 && textureFilename[1] == ':') ||
                                          (textureFilename.length() > 0 && (textureFilename[0] == '/' || textureFilename[0] == '\\'));
                    HRESULT hr_loadtex;
                    bool textureLoadedSuccessfully = false;

                    if (isAbsolutePath) {
                        fullTexturePath = textureFilename;
                        hr_loadtex = CGraphic::GetSingleObjPtr()->LoadTex(fullTexturePath.c_str(), m_vTextures[i], 0);
                        if (SUCCEEDED(hr_loadtex)) textureLoadedSuccessfully = true;
                    } else {
                        fullTexturePath = modelDir + textureFilename;
                        hr_loadtex = CGraphic::GetSingleObjPtr()->LoadTex(fullTexturePath.c_str(), m_vTextures[i], 0);
                        if (SUCCEEDED(hr_loadtex)) {
                            textureLoadedSuccessfully = true;
                        } else { // 相对路径失败，尝试直接文件名
                            hr_loadtex = CGraphic::GetSingleObjPtr()->LoadTex(textureFilename.c_str(), m_vTextures[i], 0);
                            if (SUCCEEDED(hr_loadtex)) textureLoadedSuccessfully = true;
                        }
                    }
                    if (textureLoadedSuccessfully) {
                        std::cout << "  MaterialIndex " << i << " - SUCCESS loading texture: " << (isAbsolutePath ? textureFilename : fullTexturePath) << " -> Ptr: " << m_vTextures[i].pTex << std::endl;
                    } else {
                        std::cerr << "  MaterialIndex " << i << " - FAILED loading texture: " << textureFilename << " (Tried: " << (isAbsolutePath ? "" : modelDir) << textureFilename << ") HRESULT: 0x" << std::hex << hr_loadtex << std::dec << std::endl;
                        m_vTextures[i].pTex = nullptr;
                    }
                } else { m_vTextures[i].pTex = nullptr; }
            } else { m_vTextures[i].pTex = nullptr; }
        }
    } else {
        std::cout << "DEBUG LoadFBXModelFromFile: Scene has no materials. Creating one default material." << std::endl;
        m_dwNumMaterials = 1; m_vMaterials.resize(1); m_vTextures.resize(1);
        ZeroMemory(&m_vMaterials[0], sizeof(D3DMATERIAL9));
        m_vMaterials[0].Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f }; m_vMaterials[0].Ambient = m_vMaterials[0].Diffuse;
    }

    m_vSubMeshes.reserve(pScene->mNumMeshes);
    for (unsigned int mesh_scene_idx = 0; mesh_scene_idx < pScene->mNumMeshes; ++mesh_scene_idx) {
        aiMesh* pAiMesh = pScene->mMeshes[mesh_scene_idx];
        if (pAiMesh->mNumVertices == 0 || pAiMesh->mNumFaces == 0) continue;

        // 为了避免潜在的由打印非ASCII名字引起的堆栈问题，这里使用占位符
        // std::cout << "DEBUG LoadFBXModelFromFile: Processing aiMesh " << mesh_scene_idx << " (Name: " << pAiMesh->mName.C_Str() << ") ..."; // 精简日志
        std::cout << "DEBUG LoadFBXModelFromFile: Processing aiMesh " << mesh_scene_idx << " (Name: [MeshNamePlaceholder])"
                  << ", Verts: " << pAiMesh->mNumVertices << ", Faces: " << pAiMesh->mNumFaces
                  << ", MatIdx: " << pAiMesh->mMaterialIndex; // 保持这部分关键信息

        SubMesh currentSubMesh;
        currentSubMesh.numVertices = pAiMesh->mNumVertices;
        currentSubMesh.numFaces = pAiMesh->mNumFaces;
        currentSubMesh.materialIndex = pAiMesh->mMaterialIndex;

        if (currentSubMesh.materialIndex >= m_dwNumMaterials) {
            std::cerr << "  WARNING: aiMesh " << mesh_scene_idx << " MatIdx " << currentSubMesh.materialIndex << " OOB. Correcting to 0.";
            currentSubMesh.materialIndex = (m_dwNumMaterials > 0) ? 0 : 0;
            if (m_dwNumMaterials == 0) { std::cerr << " No materials loaded, skipping submesh."; std::cout << std::endl; continue;}
        }
        std::cout << " -> UsingMatIdx: " << currentSubMesh.materialIndex;


        bool hasUVs = pAiMesh->HasTextureCoords(0);
        if (!hasUVs) {
            std::cerr << " | WARNING: This aiMesh has NO texture coordinates (UVs)!"; // 保留这个重要警告
        }
        std::cout << std::endl; // 换行，让每个aiMesh的处理日志清晰

        std::vector<FBXVertex> vertices; vertices.reserve(pAiMesh->mNumVertices);
        for (unsigned int v = 0; v < pAiMesh->mNumVertices; ++v) {
            FBXVertex vertex;
            vertex.position = { pAiMesh->mVertices[v].x, pAiMesh->mVertices[v].y, pAiMesh->mVertices[v].z };
            if (pAiMesh->HasNormals()) vertex.normal = { pAiMesh->mNormals[v].x, pAiMesh->mNormals[v].y, pAiMesh->mNormals[v].z };
            else vertex.normal = { 0.0f, 1.0f, 0.0f };
            if (hasUVs) vertex.texcoord = { pAiMesh->mTextureCoords[0][v].x, pAiMesh->mTextureCoords[0][v].y };
            else vertex.texcoord = { 0.0f, 0.0f };
            vertices.push_back(vertex);
        }

        HRESULT hr_vb = pd3dDevice->CreateVertexBuffer(
                vertices.size() * sizeof(FBXVertex), D3DUSAGE_WRITEONLY,
                m_dwFBXVertexFVF, D3DPOOL_MANAGED, &currentSubMesh.vb, NULL);
        if (FAILED(hr_vb)) { std::cerr << "  ERROR: Failed to create VB for aiMesh " << mesh_scene_idx << ". HRESULT: 0x" << std::hex << hr_vb << std::dec << std::endl; continue; }
        VOID* pVBData;
        if (SUCCEEDED(currentSubMesh.vb->Lock(0, 0, &pVBData, 0))) {
            memcpy(pVBData, vertices.data(), vertices.size() * sizeof(FBXVertex));
            currentSubMesh.vb->Unlock();
        } else { std::cerr << "  ERROR: Failed to lock VB for aiMesh " << mesh_scene_idx << std::endl; currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; continue; }

        std::vector<DWORD> indices; indices.reserve(pAiMesh->mNumFaces * 3);
        for (unsigned int f = 0; f < pAiMesh->mNumFaces; ++f) {
            const aiFace& face = pAiMesh->mFaces[f];
            if (face.mNumIndices != 3) continue;
            indices.push_back(face.mIndices[0]); indices.push_back(face.mIndices[1]); indices.push_back(face.mIndices[2]);
        }
        if (indices.empty() && pAiMesh->mNumFaces > 0) { if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; } continue; }
        if (indices.empty()) { if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; } continue; }

        HRESULT hr_ib = pd3dDevice->CreateIndexBuffer(
                indices.size() * sizeof(DWORD), D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX32, D3DPOOL_MANAGED, &currentSubMesh.ib, NULL);
        if (FAILED(hr_ib)) { std::cerr << "  ERROR: Failed to create IB for aiMesh " << mesh_scene_idx << ". HRESULT: 0x" << std::hex << hr_ib << std::dec << std::endl; if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; } continue; }
        VOID* pIBData;
        if (SUCCEEDED(currentSubMesh.ib->Lock(0, 0, &pIBData, 0))) {
            memcpy(pIBData, indices.data(), indices.size() * sizeof(DWORD));
            currentSubMesh.ib->Unlock();
        } else { std::cerr << "  ERROR: Failed to lock IB for aiMesh " << mesh_scene_idx << std::endl; if (currentSubMesh.vb) { currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr; } currentSubMesh.ib->Release(); currentSubMesh.ib = nullptr; continue; }

        m_vSubMeshes.push_back(currentSubMesh);
        std::cout << "  DEBUG: Successfully processed aiMesh " << mesh_scene_idx << " into a SubMesh." << std::endl; // 精简日志
    }

    if (m_vSubMeshes.empty() && pScene->mNumMeshes > 0) {
        std::cerr << "WARNING LoadFBXModelFromFile: FBX scene has meshes, but no submeshes were processed successfully." << std::endl;
    } else if (!m_vSubMeshes.empty()){
        std::cout << "DEBUG LoadFBXModelFromFile: Finished processing all aiMeshes. Total SubMeshes created: " << m_vSubMeshes.size() << std::endl;
    }

    m_modelType = ModelType::FBX_MODEL;
    std::cout << "DEBUG LoadFBXModelFromFile: Set model type to FBX_MODEL and returning true." << std::endl;
    return true;
}

// 通用加载函数
bool  Player::LoadGenericModel(const std::string& modelFilePath) {
    // std::cout << "DEBUG: --- ENTERING LoadGenericModel --- Path: " << modelFilePath << std::endl; // 精简日志
    std::string extension;
    size_t dotPos = modelFilePath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 < modelFilePath.length()) {
        extension = modelFilePath.substr(dotPos + 1);
    }
    std::string lower_extension = extension;
    for (char& c : lower_extension) { c = static_cast<char>(tolower(c)); }

    if (lower_extension == "x") return LoadXModelFromFile(modelFilePath);
    else if (lower_extension == "fbx") return LoadFBXModelFromFile(modelFilePath);

    std::cerr << "ERROR LoadGenericModel: Unsupported model format or unknown extension: " << modelFilePath << " (ext: " << extension << ")" << std::endl;
    return false;
}

// 渲染函数
void  Player::Render(LPDIRECT3DDEVICE9 pd3dDevice) {
    if (!pd3dDevice) {
        std::cerr << "ERROR Model::Render: No D3D Device." << std::endl;
        return;
    }

    D3DXMATRIXA16 matWorld = transform.GetWorldMatrix();
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    if (m_modelType == ModelType::X_MODEL && m_pMesh) {
        for (DWORD i = 0; i < m_dwNumMaterials; i++) {
            if (i < m_vMaterials.size()) pd3dDevice->SetMaterial(&m_vMaterials[i]);
            else if (!m_vMaterials.empty()) pd3dDevice->SetMaterial(&m_vMaterials[0]);
            if (i < m_vTextures.size() && m_vTextures[i].pTex != nullptr) pd3dDevice->SetTexture(0, m_vTextures[i].pTex);
            else pd3dDevice->SetTexture(0, nullptr);
            m_pMesh->DrawSubset(i);
        }
    } else if (m_modelType == ModelType::FBX_MODEL && !m_vSubMeshes.empty()) {
        for (const auto & subMesh : m_vSubMeshes) {
            if (subMesh.vb && subMesh.ib && subMesh.numFaces > 0) {
                pd3dDevice->SetStreamSource(0, subMesh.vb, 0, sizeof(FBXVertex));
                pd3dDevice->SetFVF(m_dwFBXVertexFVF);
                pd3dDevice->SetIndices(subMesh.ib);
                UINT matIdxToUse = subMesh.materialIndex;
                if (matIdxToUse >= m_dwNumMaterials) {
                    matIdxToUse = (m_dwNumMaterials > 0) ? 0 : 0;
                    if (m_dwNumMaterials == 0) {
                        pd3dDevice->SetTexture(0, nullptr);
                        pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, subMesh.numVertices, 0, subMesh.numFaces);
                        continue;
                    }
                }
                if (matIdxToUse < m_vMaterials.size()) pd3dDevice->SetMaterial(&m_vMaterials[matIdxToUse]);
                if (matIdxToUse < m_vTextures.size() && m_vTextures[matIdxToUse].pTex != nullptr) pd3dDevice->SetTexture(0, m_vTextures[matIdxToUse].pTex);
                else pd3dDevice->SetTexture(0, nullptr);
                pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, subMesh.numVertices, 0, subMesh.numFaces);
            }
        }
    }
}

void Player::MoveForward(float deltaTime) {
    D3DXVECTOR3 forward = transform.GetForward();
    transform.position += forward * deltaTime * m_moveSpeed;
    isActive = true;
}

void Player::MoveBackward(float deltaTime) {
    D3DXVECTOR3 forward = transform.GetForward();
    transform.position -= forward * deltaTime * m_moveSpeed;
    isActive = true;
}

void Player::StrafeLeft(float deltaTime) {
    D3DXVECTOR3 right = transform.GetRight();
    transform.position -= right * deltaTime * m_moveSpeed;
    isActive = true;
}

void Player::StrafeRight(float deltaTime) {
    D3DXVECTOR3 right = transform.GetRight();
    transform.position += right * deltaTime * m_moveSpeed;
    isActive = true;
}

void Player::RotateLeft(float deltaTime) {
    D3DXVECTOR3 up = transform.GetUp();

    float angle = deltaTime * m_rotationSpeed;

    transform.RotateAxis(up, angle);
    isActive = true;
}

void Player::RotateRight(float deltaTime) {
    D3DXVECTOR3 up = transform.GetUp();

    float angle = deltaTime * m_rotationSpeed;

    transform.RotateAxis(up, -angle);
    isActive = true;
}

int Player::GetHealth() const {
    return m_health;
}

void Player::HandleInput(float deltaTime) {
    // 使用 GetAsyncKeyState (Windows特定) 或其他跨平台的输入库
    if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
        MoveForward(deltaTime);
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
        MoveBackward(deltaTime);
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) { // 通常A/D用于平移
        StrafeLeft(deltaTime); // 或者 RotateLeft(deltaTime) 如果你希望A/D是旋转
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) { // 通常A/D用于平移
        StrafeRight(deltaTime); // 或者 RotateRight(deltaTime)
    }
    // 示例：使用Q/E进行旋转
    if (GetAsyncKeyState('Q') & 0x8000) {
        RotateLeft(deltaTime);
    }
    if (GetAsyncKeyState('E') & 0x8000) {
        RotateRight(deltaTime);
    }
    // ... 其他按键处理，如跳跃、攻击等 ...
}

void Player::Update(float deltaTime)  {
    if(!isActive) {
        return;
    }
    HandleInput(deltaTime);
}
