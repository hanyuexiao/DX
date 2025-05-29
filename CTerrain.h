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
};

#define MYCUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE) //自定义顶点格式（位置，颜色）

class Terrain : public GameObject{
public:
    Terrain();
    LPD3DXMESH g_pTerrainMesh;



};


#endif //DX_CTERRAIN_H
