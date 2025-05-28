//
// Created by admin on 2025/5/27.
//

#ifndef DX_CPLAYER_H
#define DX_CPLAYER_H

#include "CGameObject.h" // 你的游戏对象基类
#include "Graphic.h"     // 包含 CGraphic 和 MyImageInfo 定义
#include <vector>
#include <string>
#include <d3dx9.h>

class Player:public GameObject {
public:
    Player(const std::string& name);
    virtual ~ Player();

    bool LoadXModelFromFile(const std::string& modelFilePath);
    bool LoadFBXModelFromFile(const std::string& modelFilePath);
    bool LoadGenericModel(const std::string& modelFilePath); // 可选

    void Render(LPDIRECT3DDEVICE9 pd3dDevice);
    void ReleaseResources();

private:
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

// Player

#endif //DX_CPLAYER_H
