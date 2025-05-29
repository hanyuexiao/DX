//
// Created by admin on 2025/5/29.
//

#ifndef DX_CFRUSTUM_H
#define DX_CFRUSTUM_H

#include "CGameObject.h" // 包含基类 GameObject 的定义
#include "Graphic.h"     // 为了 MyImageInfo 和 ModelType (假设定义在这里或 common.h)

// 自定义视锥体顶点格式(位置+颜色)
struct FVF_FrustumVertex {
    D3DXVECTOR3 position;
    D3DCOLOR    color;
};

#define D3DFVF_FRUSTUMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class CFrustum {
private:

    D3DXVECTOR3 m_vecClip[8] ={
            D3DXVECTOR3 (-1.0f,1.0f,0.0f),
            D3DXVECTOR3 (1.0f,1.0f,0.0f),
            D3DXVECTOR3 (1.0f,-1.0f,0.0f),
            D3DXVECTOR3 (-1.0f,-1.0f,0.0f),
            D3DXVECTOR3 (-1.0f,1.0f,1.0f),
            D3DXVECTOR3 (1.0f,1.0f,1.0f),
            D3DXVECTOR3 (1.0f,-1.0f,1.0f),
            D3DXVECTOR3 (-1.0f,-1.0f,1.0f),
    };
    D3DXVECTOR3 m_vecWorldPos[8] = {};
    D3DXMATRIX InvViewProj;

public:
    void UpdateFrustum(D3DXMATRIX matView, D3DXMATRIX matProj);
    const D3DXVECTOR3* GetWorldCorners() const {return m_vecWorldPos;}

    void SetupFrustumBuffers(LPDIRECT3DDEVICE9 pd3dDevice, CFrustum* pFrustum,LPDIRECT3DVERTEXBUFFER9 g_pFrustumVB);

    void DrawFrozenFrustumFaces(LPDIRECT3DDEVICE9 pd3dDevice,bool  g_bDrawFrozenFrustum,LPDIRECT3DVERTEXBUFFER9 g_pFrustumVB);
};


#endif //DX_CFRUSTUM_H
