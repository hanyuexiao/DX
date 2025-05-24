#pragma once
#include "CGameObject.h" // �����Ϸ�������
#include "Graphic.h"     // ���� CGraphic �� MyImageInfo ����
#include <vector>
#include <string>
#include <d3dx9.h>

// FBX ����ʱʹ�õĶ���ṹ��
// ȷ������ṹ�� Model.cpp �ж���� D3DFVF_FBXVERTEX ƥ��
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
    bool LoadGenericModel(const std::string& modelFilePath); // ��ѡ

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

    // .X ģ������
    LPD3DXMESH m_pMesh;

    // .FBX ģ������
    struct SubMesh {
        LPDIRECT3DVERTEXBUFFER9 vb = nullptr;
        LPDIRECT3DINDEXBUFFER9  ib = nullptr;
        UINT numVertices = 0;
        UINT numFaces = 0;
        UINT materialIndex = 0;
    };
    std::vector<SubMesh> m_vSubMeshes;
    DWORD m_dwFBXVertexFVF;

    // ͨ�ò��ʺ���������
    DWORD m_dwNumMaterials;
    std::vector<D3DMATERIAL9> m_vMaterials;
    // �޸� m_vTextures �Դ洢 CGraphic::MyImageInfo
    std::vector<MyImageInfo> m_vTextures;
};