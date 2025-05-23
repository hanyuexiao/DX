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
    this->filePath = modelFilePath; // �����ļ�·��
    ReleaseResources(); // ���ͷž���Դ�������������֮ǰ�����������

    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice) {
        // OutputDebugString("Model::LoadModelFromFile - D3D Device is null.\n");
        return false;
    }

    ID3DXBuffer* pAdjacencyBuffer = nullptr;
    ID3DXBuffer* pMaterialBuffer = nullptr; // ������ȡ���ʺ�������Ϣ

    HRESULT hr = D3DXLoadMeshFromX(
            modelFilePath.c_str(),
            D3DXMESH_MANAGED,
            pd3dDevice,
            &pAdjacencyBuffer,
            &pMaterialBuffer,  // ��������Ҫ����� buffer �ˣ�
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

    // === ��ʼ������ʺ����� ===
    if (pMaterialBuffer != nullptr && m_dwNumMaterials > 0) {
        // D3DXMATERIAL �ṹ������ D3DMATERIAL9 ��һ��ָ�������ļ�����ָ��
        D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();

        m_vMaterials.resize(m_dwNumMaterials);
        m_vTextures.resize(m_dwNumMaterials); // ÿ�����ʶ�Ӧһ������ۣ�����Ϊ��

        for (DWORD i = 0; i < m_dwNumMaterials; ++i) {
            // 1. ���Ʋ������� (D3DMATERIAL9)
            m_vMaterials[i] = d3dxMaterials[i].MatD3D;

            // 2. ���û����ⷴ��
            //    .x �ļ��еĻ�����ͨ��δ���û�ܰ�������������ɫ����Ǹ���ϰ��
            m_vMaterials[i].Ambient = m_vMaterials[i].Diffuse;

            // 3. �������� (�������ָ���������ļ���)
            m_vTextures[i] = nullptr; // ���ÿ�
            if (d3dxMaterials[i].pTextureFilename != nullptr &&
                strlen(d3dxMaterials[i].pTextureFilename) > 0) {

                std::string textureFilenameStr = d3dxMaterials[i].pTextureFilename;
                std::string texturePath = "";

                // �������������·�� (���������ģ����ͬһĿ¼�����·��)
                // ���� modelFilePath �����һ��·���ָ���
                size_t lastSlashPos = modelFilePath.find_last_of("/\\");
                if (lastSlashPos != std::string::npos) {
                    // ����ҵ��ˣ���ȡĿ¼���ֲ�ƴ�������ļ���
                    texturePath = modelFilePath.substr(0, lastSlashPos + 1) + textureFilenameStr;
                } else {
                    // ��� modelFilePath û��·���ָ��� (���� "dog.x")��
                    // ����Ϊ�����ļ���ģ���ļ����ڵ�ǰ����Ŀ¼��
                    texturePath = textureFilenameStr;
                }

                // ʹ�� CGraphic �� LoadTex ������������
                MyImageInfo tempImageInfo;
                ZeroMemory(&tempImageInfo, sizeof(MyImageInfo));

                // ��� LoadTex ���� LPCTSTR����������Ŀ��UNICODE (TCHAR �� wchar_t)��
                // �� texturePath �� std::string��������Ҫת����
                // ������� TString ������ std::string ���ݻ���ĿΪ�� UNICODE��
                if (SUCCEEDED(CGraphic::GetSingleObjPtr()->LoadTex(texturePath.c_str(), tempImageInfo))) {
                    m_vTextures[i] = tempImageInfo.pTex; // pTex �� LPDIRECT3DTEXTURE9
                } else {
                    // TCHAR errorMsg[512];
                    // _stprintf_s(errorMsg, _T("Model::LoadModelFromFile - Failed to load texture '%s' for mesh '%s'.\n"), texturePath.c_str(), modelFilePath.c_str());
                    // OutputDebugString(errorMsg);
                    m_vTextures[i] = nullptr;
                }
            }
        }
    } else if (m_pMesh != nullptr && m_dwNumMaterials == 0) {
        // ������سɹ�������û�в�����Ϣ
        // OutputDebugString(_T("Model::LoadModelFromFile - Mesh loaded but no material info in .x file.\n"));
    }

    // �ͷ���ʱ������
    if (pAdjacencyBuffer) pAdjacencyBuffer->Release();
    if (pMaterialBuffer) pMaterialBuffer->Release(); // ���ʻ�����ʹ����Ϻ�����ͷ�

    return true;
}


void Model::Render() {
    LPDIRECT3DDEVICE9 pd3dDevice = CGraphic::GetSingleObjPtr()->m_pDevice;
    if (!pd3dDevice || !m_pMesh) {
        return;
    }
// 1. ��ȡ�����������ã����� GetWorldMatrix ���� D3DXMATRIX&��
    D3DXMATRIX worldMatrix = transform.GetWorldMatrix();

// 2. ���� D3DXMatrixIdentity ��ʼ���þ���
    D3DXMatrixIdentity(&worldMatrix); // ��ȷ���������ǳ־ö���

// 3. ���ݳ�ʼ����ľ����ַ�� SetTransform
    pd3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix);

    // === ��ʼӦ��ÿ���Ӽ��Ĳ��ʺ����� ===
    if (m_dwNumMaterials > 0) {
        for (DWORD i = 0; i < m_dwNumMaterials; ++i) {
            // ���õ�ǰ�Ӽ��Ĳ���
            // ȷ�� m_vMaterials ������ i+1 ��Ԫ��
            if (i < m_vMaterials.size()) {
                pd3dDevice->SetMaterial(&m_vMaterials[i]);
            } else {
                // ��� m_vMaterials ���㣬������Ҫ����һ��Ĭ�ϲ��ʻ򷢳�����
                // �������� m_vMaterials.size() Ӧ�õ��� m_dwNumMaterials��
                D3DMATERIAL9 defaultMat; // ����Ĭ�ϲ���
                ZeroMemory(&defaultMat, sizeof(D3DMATERIAL9));
                defaultMat.Diffuse.r = defaultMat.Ambient.r = 0.7f;
                defaultMat.Diffuse.g = defaultMat.Ambient.g = 0.7f;
                defaultMat.Diffuse.b = defaultMat.Ambient.b = 0.7f;
                defaultMat.Diffuse.a = defaultMat.Ambient.a = 1.0f;
                pd3dDevice->SetMaterial(&defaultMat);
            }

            // ���õ�ǰ�Ӽ������� (�������)
            // ȷ�� m_vTextures ������ i+1 ��Ԫ��
            if (i < m_vTextures.size()) {
                pd3dDevice->SetTexture(0, m_vTextures[i]); // ��� m_vTextures[i] Ϊ nullptr�����������
            } else {
                pd3dDevice->SetTexture(0, nullptr); // ȷ����������
            }

            m_pMesh->DrawSubset(i); // ��������ĵ�ǰ�Ӽ�
        }
    } else if (m_pMesh) {
        // ���û�в�����Ϣ (m_dwNumMaterials == 0) ��������ڣ�
        // ���Խ�����������Ϊ�����Ӽ�(0)���ơ�
        // ��ʱ��Ҫȷ������һ��Ĭ�ϲ��ʺͽ�������
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


