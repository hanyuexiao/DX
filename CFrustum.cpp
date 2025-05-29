//
// Created by admin on 2025/5/29.
//

#include "CFrustum.h"

void CFrustum::UpdateFrustum(D3DXMATRIX matView, D3DXMATRIX matProj) {

    D3DXMATRIX matViewProj;
    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);
    D3DXMatrixInverse(&InvViewProj, NULL, &matViewProj);

    for (int i = 0; i < 8; ++i) {
        D3DXVec3TransformCoord(&m_vecWorldPos[i], &m_vecClip[i], &InvViewProj);
    }
}

