//
// Created by admin on 2025/5/29.
//

#ifndef DX_CTERRAIN_H
#define DX_CTERRAIN_H

#include "CGameObject.h" // 包含基类 GameObject 的定义
#include "Graphic.h"     // 为了 MyImageInfo 和 ModelType (假设定义在这里或 common.h)


struct MYCUSTOMVERTEX
{
    D3DXVECTOR3 position;  //位置
    DWORD color; //颜色
    D3DXVECTOR2 texCoord; //纹理坐标
};

#define MYCUSTOMVERTEX_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1) //自定义顶点格式（位置，颜色）

class Terrain : public GameObject{
public:
    Terrain();
    LPD3DXMESH g_pTerrainMesh;
    void Render();
    
private:
    MYCUSTOMVERTEX* pVertices;
    D3DXMATRIX matTerrain;
    WORD* pIndex;
    LPDIRECT3DTEXTURE9 g_pTexture;
    void Init();



};


#endif //DX_CTERRAIN_H
