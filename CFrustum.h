//
// Created by admin on 2025/5/29.
//

#ifndef DX_CFRUSTUM_H
#define DX_CFRUSTUM_H

#include "CGameObject.h" // 包含基类 GameObject 的定义
#include "Graphic.h"     // 为了 MyImageInfo 和 ModelType (假设定义在这里或 common.h)

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
};


#endif //DX_CFRUSTUM_H
