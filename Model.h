#pragma once
#include "CGameObject.h" // 你的游戏对象基类
#include "Graphic.h"     // 包含 CGraphic 和 MyImageInfo 定义
#include <vector>
#include <string>
#include <d3dx9.h>

// FBX 加载时使用的顶点结构体
// 确保这个结构与 Model.cpp 中定义的 D3DFVF_FBXVERTEX 匹配
struct FBXVertex {
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DXVECTOR2 texcoord;
};

class Model : public GameObject {
public:
    Model();
    virtual ~Model();

    bool LoadXModelFromFile(const std::string& modelFilePath);
    bool LoadFBXModelFromFile(const std::string& modelFilePath);
    bool LoadGenericModel(const std::string& modelFilePath); // 可选

    void Render(LPDIRECT3DDEVICE9 pd3dDevice);
    void ReleaseResources();

private:
    enum class ModelType {
        NONE,
        X_MODEL,
        FBX_MODEL
    };
    ModelType m_modelType;
    std::string m_filePath;

    // .X 模型数据
    LPD3DXMESH m_pMesh;

    // .FBX 模型数据
    struct SubMesh {
        LPDIRECT3DVERTEXBUFFER9 vb = nullptr;
        LPDIRECT3DINDEXBUFFER9  ib = nullptr;
        UINT numVertices = 0;
        UINT numFaces = 0;
        UINT materialIndex = 0;
    };
    std::vector<SubMesh> m_vSubMeshes;
    DWORD m_dwFBXVertexFVF;

    // 通用材质和纹理数据
    DWORD m_dwNumMaterials;
    std::vector<D3DMATERIAL9> m_vMaterials;
    // 修改 m_vTextures 以存储 CGraphic::MyImageInfo
    std::vector<MyImageInfo> m_vTextures;
};