//
// Created by admin on 2025/5/29.
//

#include "CTerrain.h"

Terrain::Terrain() {
    Init();
}

bool Terrain::Init() {
    // TODO: 初始化地形
    HRESULT hr = D3DXCreateMeshFVF(2,4,D3DXMESH_MANAGED,MYCUSTOMVERTEX_FVF,CGraphic::GetSingleObjPtr()->m_pDevice,&g_pTerrainMesh); //4个顶点，2个三角形的空间分配出来
    if(SUCCEEDED(hr))
    {
//        MYCUSTOMVERTEX* pVertices;
//        WORD* pIndices;
//        g_pTerrainMesh->LockVertexBuffer(0,(void**)&pVertices,D3DLOCK_DISCARD);
//        g_pTerrainMesh->LockIndexBuffer(0,(void**)&pIndices,D3DLOCK_DISCARD);
//        pVertices[0].pos = D3DXVECTOR3(-100.0f,0.0f,-100.0f);
//        pVertices[1].pos = D3DXVECTOR3(100.0f,0.0f,-100.0f);
//        pVertices[2].pos = D3DXVECTOR3
    }
    else{
        MessageBox(NULL,"创建地形失败","错误",MB_OK);
        return false;
    }
    g_pTerrainMesh->LockVertexBuffer(0,(void**)&pVertices); //获取顶点缓冲区

    //TODO: 填充顶点数据
    pVertices[0].position = D3DXVECTOR3(-5.0f, 5.0f, 0.0f); pVertices[0].color = D3DCOLOR_XRGB(255, 0, 0);
    pVertices[1].position = D3DXVECTOR3(5.0f, 5.0f, 0.0f); pVertices[1].color = D3DCOLOR_XRGB(0, 255, 0);
    pVertices[2].position = D3DXVECTOR3(-5.0f, -5.0f, 0.0f); pVertices[2].color = D3DCOLOR_XRGB(0, 0, 255);
    pVertices[3].position = D3DXVECTOR3(5.0f, -5.0f, 0.0f); pVertices[3].color = D3DCOLOR_XRGB(255, 255, 0);

    g_pTerrainMesh->UnlockVertexBuffer();

    g_pTerrainMesh->LockIndexBuffer(0,(void**)&pIndex); //获取索引缓冲区

    //TODO: 填充索引数据
    pIndex[0] = 0; pIndex[1] = 1; pIndex[2] = 2;
    pIndex[3] = 1; pIndex[4] = 3; pIndex[5] = 2;

    //TODO: 填充贴图
    D3DXCreateTextureFromFile(CGraphic::GetSingleObjPtr()->m_pDevice, "C:\\Users\\admin\\CLionProjects\\DX\\cmake-build-debug\\Assets\\Grass2.jpg", &g_pTexture);
    return true;
}

void Terrain::Render() {
    D3DXMatrixIdentity(&matTerrain);
    CGraphic::GetSingleObjPtr()->m_pDevice->SetTransform(D3DTS_WORLD,&matTerrain);
    CGraphic::GetSingleObjPtr()->m_pDevice->SetTexture(0,nullptr);
    g_pTerrainMesh->DrawSubset(0);
}
