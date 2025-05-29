//
// Created by admin on 2025/5/29.
//

#include "CFrustum.h"

void CFrustum::UpdateFrustum(D3DXMATRIX matView, D3DXMATRIX matProj) {

    D3DXMatrixMultiply(&InvViewProj, &matView, &matProj);
    D3DXMatrixInverse(&InvViewProj, NULL, &InvViewProj);


}
