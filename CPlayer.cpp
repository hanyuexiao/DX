// CPlayer.cpp
#include "CPlayer.h"
#include "Graphic.h" // For CGraphic::GetSingleObjPtr() and MyImageInfo
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream> // For std::cout, std::cerr
#include <locale>   // For tolower

// 构造函数
Player::Player(const std::string& name) :
        GameObject(name), // 调用基类构造函数
        m_moveSpeed(50.0f),      // 默认移动速度
        m_rotationSpeed(1.5f),   // 默认键盘旋转速度 (弧度/秒)
        isActive(true),
        m_mouseSensitivity(0.005f),// 默认鼠标灵敏度
        m_isMouseRotating(false),
        m_modelType(ModelType::NONE),
        m_filePath(""),
        m_pMesh(nullptr),
        m_dwFBXVertexFVF(D3DFVF_FBXVERTEX),
        m_dwNumMaterials(0),
        m_currentYaw(0.0f),
        m_currentPitch(0.0f)
// m_health(100), // 示例
// m_score(0)     // 示例
{
    m_vSubMeshes.clear();
    m_vMaterials.clear();
    m_vTextures.clear();
    m_lastMousePos = {0, 0};

    // 尝试从 transform 获取初始旋转并设置 m_currentYaw 和 m_currentPitch
    // 这需要 CTransform::GetEulerAngles() 返回正确的欧拉角 (pitch, yaw, roll)
    // 假设 GetEulerAngles() 返回 D3DXVECTOR3(pitch, yaw, roll)
    // 如果你的模型初始就有旋转，这一步很重要，避免鼠标第一次动时视角跳变
    D3DXVECTOR3 initialEuler = transform.GetEulerAngles(); // 假设这个方法存在且有效
    m_currentPitch = initialEuler.x; // Pitch 对应 X 轴旋转
    m_currentYaw = initialEuler.y;   // Yaw 对应 Y 轴旋转
    // Roll (Z轴) 通常不由鼠标直接控制，保持 transform 中的值或设为0
    transform.SetRotationEuler(m_currentYaw, m_currentPitch, initialEuler.z);


    std::cout << "Player '" << this->name << "' created. Initial Yaw: " << m_currentYaw << ", Pitch: " << m_currentPitch << std::endl;
}

// 析构函数
Player::~Player() {
    ReleaseResources();
    std::cout << "Player '" << this->name << "' destroyed." << std::endl;
}

// 释放所有已分配的资源
void Player::ReleaseResources() {
    if (m_pMesh) {
        m_pMesh->Release();
        m_pMesh = nullptr;
    }
    for (auto & subMesh : m_vSubMeshes) {
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
    for (auto & texInfo : m_vTextures) {
        if (texInfo.pTex) {
            texInfo.pTex->Release();
            texInfo.pTex = nullptr;
        }
    }
    m_vTextures.clear();
    m_vMaterials.clear();
    m_dwNumMaterials = 0;
    m_modelType = ModelType::NONE;
    m_filePath = "";
}

// 从 .X 文件加载模型
bool Player::LoadXModelFromFile(const std::string& modelFilePath) {
    std::cout << "Player: Loading X Model from: " << modelFilePath << std::endl;
    ReleaseResources();
    m_filePath = modelFilePath;
    m_modelType = ModelType::NONE;

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        std::cerr << "Player::LoadXModelFromFile: No D3D Device found!" << std::endl;
        return false;
    }

    LPD3DXBUFFER pD3DXMtrlBuffer = nullptr;
    HRESULT hr = D3DXLoadMeshFromX(
            modelFilePath.c_str(), D3DXMESH_SYSTEMMEM, pd3dDevice,
            NULL, &pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, &m_pMesh);

    if (FAILED(hr)) {
        std::cerr << "Player::LoadXModelFromFile: Failed to load .X model. HRESULT: 0x" << std::hex << hr << std::dec << std::endl;
        if (pD3DXMtrlBuffer) pD3DXMtrlBuffer->Release();
        return false;
    }

    if (m_dwNumMaterials == 0 && m_pMesh != nullptr) { // 如果没有材质但有网格，创建一个默认材质
        m_dwNumMaterials = 1;
        D3DMATERIAL9 defaultMat;
        ZeroMemory(&defaultMat, sizeof(D3DMATERIAL9));
        defaultMat.Diffuse.r = defaultMat.Ambient.r = 0.8f;
        defaultMat.Diffuse.g = defaultMat.Ambient.g = 0.8f;
        defaultMat.Diffuse.b = defaultMat.Ambient.b = 0.8f;
        defaultMat.Diffuse.a = defaultMat.Ambient.a = 1.0f;
        m_vMaterials.push_back(defaultMat);
        m_vTextures.resize(1); // 对应一个空的纹理信息
        m_vTextures[0].pTex = nullptr;
    } else if (pD3DXMtrlBuffer != nullptr && m_dwNumMaterials > 0) {
        D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
        m_vMaterials.resize(m_dwNumMaterials);
        m_vTextures.resize(m_dwNumMaterials);

        std::string modelDir = "";
        size_t lastSlash = modelFilePath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            modelDir = modelFilePath.substr(0, lastSlash + 1);
        }

        for (DWORD i = 0; i < m_dwNumMaterials; i++) {
            m_vMaterials[i] = d3dxMaterials[i].MatD3D;
            m_vMaterials[i].Ambient = m_vMaterials[i].Diffuse; // 通常环境光用漫反射光
            m_vTextures[i].pTex = nullptr; // 初始化为nullptr

            if (d3dxMaterials[i].pTextureFilename && strlen(d3dxMaterials[i].pTextureFilename) > 0) {
                std::string textureFilenameStr = d3dxMaterials[i].pTextureFilename;
                std::string texturePathStr = modelDir + textureFilenameStr;
                // 尝试加载纹理
                HRESULT hr_tex_load = CGraphic::GetSingleObjPtr()->LoadTex(texturePathStr.c_str(), m_vTextures[i], 0);
                if (FAILED(hr_tex_load)) { // 如果带路径失败，尝试不带路径（可能纹理在工作目录）
                    hr_tex_load = CGraphic::GetSingleObjPtr()->LoadTex(textureFilenameStr.c_str(), m_vTextures[i], 0);
                }
                if (SUCCEEDED(hr_tex_load)) {
                    std::cout << "  Loaded texture for X model: " << textureFilenameStr << std::endl;
                } else {
                    std::cerr << "  Failed to load texture for X model: " << textureFilenameStr << std::endl;
                }
            }
        }
    }

    if (pD3DXMtrlBuffer) {
        pD3DXMtrlBuffer->Release();
    }

    if (m_pMesh) {
        m_modelType = ModelType::X_MODEL;
        std::cout << "Player: X Model loaded successfully." << std::endl;
        return true;
    }
    return false;
}

// 从 .FBX 文件加载模型
bool Player::LoadFBXModelFromFile(const std::string& modelFilePath) {
    std::cout << "Player: Loading FBX Model from: " << modelFilePath << std::endl;
    ReleaseResources();
    m_filePath = modelFilePath;
    m_modelType = ModelType::NONE;

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        std::cerr << "Player::LoadFBXModelFromFile: No D3D Device found!" << std::endl;
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
        std::cerr << "Player::LoadFBXModelFromFile: ASSIMP Error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    m_dwNumMaterials = pScene->mNumMaterials;
    std::string modelDir = "";
    size_t lastSlash = modelFilePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        modelDir = modelFilePath.substr(0, lastSlash + 1);
    }

    // 加载材质和纹理
    if (m_dwNumMaterials > 0) {
        m_vMaterials.resize(m_dwNumMaterials);
        m_vTextures.resize(m_dwNumMaterials);
        for (unsigned int i = 0; i < m_dwNumMaterials; ++i) {
            aiMaterial* pAiMaterial = pScene->mMaterials[i];
            D3DMATERIAL9 d3dMaterial;
            ZeroMemory(&d3dMaterial, sizeof(D3DMATERIAL9));
            aiColor4D color;

            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                d3dMaterial.Diffuse = { color.r, color.g, color.b, color.a };
            } else { d3dMaterial.Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f }; } // 默认漫反射
            if (aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_AMBIENT, &color) == AI_SUCCESS) {
                d3dMaterial.Ambient = { color.r, color.g, color.b, color.a };
            } else { d3dMaterial.Ambient = d3dMaterial.Diffuse; } // 默认环境光

            // 可以加载更多材质属性，如高光等
            m_vMaterials[i] = d3dMaterial;
            m_vTextures[i].pTex = nullptr; // 初始化

            aiString aiTexPath;
            if (pAiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS) {
                std::string textureFilename = aiTexPath.C_Str();
                if (!textureFilename.empty()) {
                    std::string fullTexturePath;
                    // 检查是否是绝对路径 (简单判断)
                    bool isAbsolutePath = (textureFilename.length() > 1 && textureFilename[1] == ':') ||
                                          (textureFilename.length() > 0 && (textureFilename[0] == '/' || textureFilename[0] == '\\'));

                    HRESULT hr_loadtex = E_FAIL;
                    if (isAbsolutePath) {
                        fullTexturePath = textureFilename;
                        hr_loadtex = CGraphic::GetSingleObjPtr()->LoadTex(fullTexturePath.c_str(), m_vTextures[i], 0);
                    } else {
                        fullTexturePath = modelDir + textureFilename;
                        hr_loadtex = CGraphic::GetSingleObjPtr()->LoadTex(fullTexturePath.c_str(), m_vTextures[i], 0);
                        if (FAILED(hr_loadtex)) { // 尝试直接用文件名（可能在工作目录）
                            hr_loadtex = CGraphic::GetSingleObjPtr()->LoadTex(textureFilename.c_str(), m_vTextures[i], 0);
                        }
                    }

                    if (SUCCEEDED(hr_loadtex)) {
                        std::cout << "  Loaded texture for FBX (Mat " << i << "): " << textureFilename << std::endl;
                    } else {
                        std::cerr << "  Failed to load texture for FBX (Mat " << i << "): " << textureFilename << " (Path tried: " << fullTexturePath << ")" << std::endl;
                    }
                }
            }
        }
    } else { // 如果没有材质，创建一个默认的
        m_dwNumMaterials = 1;
        m_vMaterials.resize(1);
        m_vTextures.resize(1);
        ZeroMemory(&m_vMaterials[0], sizeof(D3DMATERIAL9));
        m_vMaterials[0].Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
        m_vMaterials[0].Ambient = m_vMaterials[0].Diffuse;
        m_vTextures[0].pTex = nullptr;
    }

    // 加载网格数据
    m_vSubMeshes.reserve(pScene->mNumMeshes);
    for (unsigned int meshIdx = 0; meshIdx < pScene->mNumMeshes; ++meshIdx) {
        aiMesh* pAiMesh = pScene->mMeshes[meshIdx];
        if (pAiMesh->mNumVertices == 0 || pAiMesh->mNumFaces == 0) continue;

        SubMesh currentSubMesh;
        currentSubMesh.numVertices = pAiMesh->mNumVertices;
        currentSubMesh.numFaces = pAiMesh->mNumFaces;
        currentSubMesh.materialIndex = pAiMesh->mMaterialIndex;

        if (currentSubMesh.materialIndex >= m_dwNumMaterials) {
            std::cerr << "  Warning: FBX Mesh " << meshIdx << " has material index " << currentSubMesh.materialIndex
                      << " but only " << m_dwNumMaterials << " materials loaded. Using material 0." << std::endl;
            currentSubMesh.materialIndex = (m_dwNumMaterials > 0) ? 0 : 0; // 安全处理
            if (m_dwNumMaterials == 0) continue; // 如果真的一个材质都没有，跳过这个submesh
        }


        std::vector<FBXVertex> vertices;
        vertices.reserve(pAiMesh->mNumVertices);
        bool hasUVs = pAiMesh->HasTextureCoords(0);

        for (unsigned int v = 0; v < pAiMesh->mNumVertices; ++v) {
            FBXVertex vertex;
            vertex.position = { pAiMesh->mVertices[v].x, pAiMesh->mVertices[v].y, pAiMesh->mVertices[v].z };
            if (pAiMesh->HasNormals()) {
                vertex.normal = { pAiMesh->mNormals[v].x, pAiMesh->mNormals[v].y, pAiMesh->mNormals[v].z };
            } else {
                vertex.normal = { 0.0f, 1.0f, 0.0f }; // 默认法线
            }
            if (hasUVs) {
                vertex.texcoord = { pAiMesh->mTextureCoords[0][v].x, pAiMesh->mTextureCoords[0][v].y };
            } else {
                vertex.texcoord = { 0.0f, 0.0f }; // 默认UV
            }
            vertices.push_back(vertex);
        }

        // 创建顶点缓冲
        HRESULT hr_vb = pd3dDevice->CreateVertexBuffer(
                vertices.size() * sizeof(FBXVertex), D3DUSAGE_WRITEONLY,
                m_dwFBXVertexFVF, D3DPOOL_MANAGED, &currentSubMesh.vb, NULL);
        if (FAILED(hr_vb)) {
            std::cerr << "  ERROR: Failed to create Vertex Buffer for FBX submesh " << meshIdx << ". HRESULT: 0x" << std::hex << hr_vb << std::dec << std::endl;
            continue;
        }
        VOID* pVBData;
        if (SUCCEEDED(currentSubMesh.vb->Lock(0, 0, &pVBData, 0))) {
            memcpy(pVBData, vertices.data(), vertices.size() * sizeof(FBXVertex));
            currentSubMesh.vb->Unlock();
        } else {
            std::cerr << "  ERROR: Failed to lock Vertex Buffer for FBX submesh " << meshIdx << std::endl;
            currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr;
            continue;
        }


        std::vector<DWORD> indices;
        indices.reserve(pAiMesh->mNumFaces * 3);
        for (unsigned int f = 0; f < pAiMesh->mNumFaces; ++f) {
            const aiFace& face = pAiMesh->mFaces[f];
            if (face.mNumIndices != 3) continue; // 只处理三角形
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }
        if (indices.empty() && pAiMesh->mNumFaces > 0) { // 如果有面但没有有效的索引
            if(currentSubMesh.vb) {currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr;}
            continue;
        }
        if (indices.empty()) { // 如果根本没有索引（例如点云或线）
            if(currentSubMesh.vb) {currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr;}
            continue;
        }


        // 创建索引缓冲
        HRESULT hr_ib = pd3dDevice->CreateIndexBuffer(
                indices.size() * sizeof(DWORD), D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX32, D3DPOOL_MANAGED, &currentSubMesh.ib, NULL);
        if (FAILED(hr_ib)) {
            std::cerr << "  ERROR: Failed to create Index Buffer for FBX submesh " << meshIdx << ". HRESULT: 0x" << std::hex << hr_ib << std::dec << std::endl;
            if(currentSubMesh.vb) {currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr;}
            continue;
        }
        VOID* pIBData;
        if (SUCCEEDED(currentSubMesh.ib->Lock(0, 0, &pIBData, 0))) {
            memcpy(pIBData, indices.data(), indices.size() * sizeof(DWORD));
            currentSubMesh.ib->Unlock();
        } else {
            std::cerr << "  ERROR: Failed to lock Index Buffer for FBX submesh " << meshIdx << std::endl;
            if(currentSubMesh.vb) {currentSubMesh.vb->Release(); currentSubMesh.vb = nullptr;}
            currentSubMesh.ib->Release(); currentSubMesh.ib = nullptr;
            continue;
        }
        m_vSubMeshes.push_back(currentSubMesh);
    }

    if (m_vSubMeshes.empty() && pScene->mNumMeshes > 0) {
        std::cerr << "Player::LoadFBXModelFromFile: FBX scene has meshes, but no submeshes were successfully processed." << std::endl;
        return false;
    }

    m_modelType = ModelType::FBX_MODEL;
    std::cout << "Player: FBX Model loaded successfully. SubMeshes: " << m_vSubMeshes.size() << std::endl;
    return true;
}

// 通用加载函数
bool Player::LoadGenericModel(const std::string& modelFilePath) {
    std::string extension;
    size_t dotPos = modelFilePath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 < modelFilePath.length()) {
        extension = modelFilePath.substr(dotPos + 1);
    }
    std::string lower_extension = extension;
    for (char& c : lower_extension) { c = static_cast<char>(tolower(c)); }

    if (lower_extension == "x") return LoadXModelFromFile(modelFilePath);
    else if (lower_extension == "fbx") return LoadFBXModelFromFile(modelFilePath);
    // 可以添加对其他格式的支持，如 .obj
    // else if (lower_extension == "obj") return LoadObjModelFromFile(modelFilePath);

    std::cerr << "Player::LoadGenericModel: Unsupported model format or unknown extension: " << modelFilePath << std::endl;
    return false;
}


// 渲染函数
void Player::Render(LPDIRECT3DDEVICE9 pd3dDevice) {
    if (!pd3dDevice || !isActive) {
        return;
    }

    D3DXMATRIXA16 matWorld = transform.GetWorldMatrix();
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    if (m_modelType == ModelType::X_MODEL && m_pMesh) {
        for (DWORD i = 0; i < m_dwNumMaterials; i++) {
            if (i < m_vMaterials.size()) {
                pd3dDevice->SetMaterial(&m_vMaterials[i]);
            } else if (!m_vMaterials.empty()) { // 安全回退到第一个材质
                pd3dDevice->SetMaterial(&m_vMaterials[0]);
            }

            if (i < m_vTextures.size() && m_vTextures[i].pTex != nullptr) {
                pd3dDevice->SetTexture(0, m_vTextures[i].pTex);
            } else {
                pd3dDevice->SetTexture(0, nullptr);
            }
            m_pMesh->DrawSubset(i);
        }
    } else if (m_modelType == ModelType::FBX_MODEL && !m_vSubMeshes.empty()) {
        for (const auto & subMesh : m_vSubMeshes) {
            if (subMesh.vb && subMesh.ib && subMesh.numFaces > 0) {
                pd3dDevice->SetStreamSource(0, subMesh.vb, 0, sizeof(FBXVertex));
                pd3dDevice->SetFVF(m_dwFBXVertexFVF);
                pd3dDevice->SetIndices(subMesh.ib);

                UINT matIdxToUse = subMesh.materialIndex;
                // 安全检查材质索引
                if (matIdxToUse >= m_dwNumMaterials) {
                    matIdxToUse = (m_dwNumMaterials > 0) ? 0 : 0; // 回退到第一个材质
                    if (m_dwNumMaterials == 0) { // 如果完全没有材质，则不设置材质和纹理
                        pd3dDevice->SetTexture(0, nullptr);
                        // 仍然尝试绘制，可能只是纯色
                        pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, subMesh.numVertices, 0, subMesh.numFaces);
                        continue;
                    }
                }

                if (matIdxToUse < m_vMaterials.size()) {
                    pd3dDevice->SetMaterial(&m_vMaterials[matIdxToUse]);
                }
                if (matIdxToUse < m_vTextures.size() && m_vTextures[matIdxToUse].pTex != nullptr) {
                    pd3dDevice->SetTexture(0, m_vTextures[matIdxToUse].pTex);
                } else {
                    pd3dDevice->SetTexture(0, nullptr);
                }
                pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, subMesh.numVertices, 0, subMesh.numFaces);
            }
        }
    }
}

// 鼠标控制方法实现
void Player::StartMouseRotation(int x, int y) {
    m_isMouseRotating = true;
    m_lastMousePos.x = x;
    m_lastMousePos.y = y;
    // 可以在这里添加 ShowCursor(FALSE); 和 SetCapture(hwnd);
    // 但 hwnd 需要从外部传入或通过某种方式获取
}

void Player::StopMouseRotation() {
    m_isMouseRotating = false;
    // 可以在这里添加 ShowCursor(TRUE); 和 ReleaseCapture();
}

void Player::ProcessMouseMovement(int deltaX, int deltaY) {
    if (!m_isMouseRotating) {
        return;
    }

    // 更新偏航角 (绕Y轴旋转) - 水平鼠标移动
    m_currentYaw += static_cast<float>(deltaX) * m_mouseSensitivity;

    // 更新俯仰角 (绕X轴旋转) - 垂直鼠标移动
    m_currentPitch += static_cast<float>(deltaY) * m_mouseSensitivity;

    // 限制俯仰角范围
    const float pitchLimit = D3DX_PI / 2.0f - 0.01f; // 略小于90度 (约1.56弧度)
    if (m_currentPitch > pitchLimit) {
        m_currentPitch = pitchLimit;
    }
    if (m_currentPitch < -pitchLimit) {
        m_currentPitch = -pitchLimit;
    }

    // 更新GameObject的旋转
    // 假设 CTransform::SetRotationEuler 的参数顺序是 (yaw, pitch, roll)
    // 并且 GetEulerAngles 返回的是 D3DXVECTOR3(pitch, yaw, roll)
    // 如果你的 CTransform 实现不同，需要相应调整
    transform.SetRotationEuler(m_currentYaw, m_currentPitch, 0.0f); // 保留当前的Z轴旋转 (roll)
}


// 移动方法
void Player::MoveForward(float deltaTime) {
    D3DXVECTOR3 forward = transform.GetForward(); // 获取当前的前方向量
    transform.position += forward * m_moveSpeed * deltaTime;
}

void Player::MoveBackward(float deltaTime) {
    D3DXVECTOR3 forward = transform.GetForward();
    transform.position -= forward * m_moveSpeed * deltaTime;
}

void Player::StrafeLeft(float deltaTime) {
    D3DXVECTOR3 right = transform.GetRight(); // 获取当前的右方向量
    transform.position -= right * m_moveSpeed * deltaTime;
}

void Player::StrafeRight(float deltaTime) {
    D3DXVECTOR3 right = transform.GetRight();
    transform.position += right * m_moveSpeed * deltaTime;
}

// 键盘控制的旋转
void Player::RotateLeft(float deltaTime) { // 绕Y轴向左旋转 (偏航角减小)
    m_currentYaw -= m_rotationSpeed * deltaTime;
    transform.SetRotationEuler(m_currentYaw, m_currentPitch, 0.0f);
}

void Player::RotateRight(float deltaTime) { // 绕Y轴向右旋转 (偏航角增大)
    m_currentYaw += m_rotationSpeed * deltaTime;
    transform.SetRotationEuler(m_currentYaw, m_currentPitch, 0.0f);
}


void Player::HandleInput(float deltaTime) {
    if (!isActive) return;

    // 键盘移动
    if (GetAsyncKeyState('W') & 0x8000) MoveForward(deltaTime);
    if (GetAsyncKeyState('S') & 0x8000) MoveBackward(deltaTime);
    if (GetAsyncKeyState('A') & 0x8000) StrafeLeft(deltaTime);
    if (GetAsyncKeyState('D') & 0x8000) StrafeRight(deltaTime);

    // 键盘旋转 (如果不想和鼠标旋转同时生效，可以在 m_isMouseRotating 为 false 时才执行)
    if (!m_isMouseRotating) {
        if (GetAsyncKeyState('Q') & 0x8000) RotateLeft(deltaTime);
        if (GetAsyncKeyState('E') & 0x8000) RotateRight(deltaTime);
    }
    // 向上/下移动 (如果需要，例如飞行)
    // if (GetAsyncKeyState(VK_SPACE) & 0x8000) transform.position.y += m_moveSpeed * deltaTime;
    // if (GetAsyncKeyState(VK_CONTROL) & 0x8000) transform.position.y -= m_moveSpeed * deltaTime;
}

void Player::Update(float deltaTime) {
    GameObject::Update(deltaTime); // 调用基类的Update (如果它有逻辑)
    if (!isActive) {
        return;
    }
    HandleInput(deltaTime); // 处理键盘输入
    // 鼠标旋转的实际调用在 EventProc 中的 WM_MOUSEMOVE 消息处理中
}
