//
// Created by admin on 2025/5/29.
//

#include "CTerrain.h"

Terrain::Terrain() {
    // TODO: 初始化地形
    HRESULT hr = D3DXCreateMeshFVF(2,4,D3DXMESH_MANAGED,MYCUSTOMVERTEX,CGraphic::GetSingleObjPtr()->m_pDevice,&g_pTerrainMesh);
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
    }
}
