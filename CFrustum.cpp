//
// Created by admin on 2025/5/29.
//

#include "CFrustum.h"

void CFrustum::UpdateFrustum(D3DXMATRIX matView, D3DXMATRIX matProj) {

    D3DXMATRIX matViewProj;
    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);
    D3DXMatrixInverse(&InvViewProj, NULL, &matViewProj);
    D3DXVec3TransformCoordArray(m_vecWorldPos, sizeof(D3DXVECTOR3),m_vecClip, sizeof(D3DXVECTOR3),&InvViewProj,8);
}

