//
// Created by admin on 2025/5/29.
//

#include "CFrustum.h"

CFrustum* g_Frustum;

void CFrustum::UpdateFrustum(D3DXMATRIX matView, D3DXMATRIX matProj) {

    D3DXMATRIX matViewProj;
    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);
    D3DXMatrixInverse(&InvViewProj, NULL, &matViewProj);

    for (int i = 0; i < 8; ++i) {
        D3DXVec3TransformCoord(&m_vecWorldPos[i], &m_vecClip[i], &InvViewProj);
    }
}

void CFrustum::DrawFrozenFrustumFaces(LPDIRECT3DDEVICE9 pd3dDevice,bool  g_bDrawFrozenFrustum,LPDIRECT3DVERTEXBUFFER9 g_pFrustumVB) {
    if (!pd3dDevice || !g_bDrawFrozenFrustum || !g_pFrustumVB) {
        return;
    }
    DWORD oldAlphaBlendEnable, oldSrcBlend, oldDestBlend, oldCullMode, oldLighting, oldZWriteEnable;
    pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlendEnable);
    pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &oldSrcBlend);
    pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend);
    pd3dDevice->GetRenderState(D3DRS_CULLMODE, &oldCullMode);
    pd3dDevice->GetRenderState(D3DRS_LIGHTING, &oldLighting);
    pd3dDevice->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);

    // 设置透明、无光照的视锥体渲染状态
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);


    // 设置世界矩阵为单位矩阵，因为顶点已经在世界空间
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // 设置FVF和流源
    pd3dDevice->SetFVF(D3DFVF_FRUSTUMVERTEX);
    pd3dDevice->SetStreamSource(0, g_pFrustumVB, 0, sizeof(FVF_FrustumVertex));

    // 绘制视锥体面(12个三角形)
    pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12); // 6个面 * 每个面2个三角形 = 12个三角形

    // 恢复旧的渲染状态
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlendEnable);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND, oldSrcBlend);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND, oldDestBlend);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE, oldCullMode);
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, oldLighting);
    pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);
}


void CFrustum::SetupFrustumBuffers(LPDIRECT3DDEVICE9 pd3dDevice, CFrustum* pFrustum,LPDIRECT3DVERTEXBUFFER9 g_pFrustumVB) {
    if (!pd3dDevice || !pFrustum) return;

    // 释放旧的缓冲区(如果存在)

    if (g_pFrustumVB) {
        g_pFrustumVB->Release();
        g_pFrustumVB = nullptr;
    }

    const D3DXVECTOR3* corners = pFrustum->GetWorldCorners();
    if (!corners) return;

    // 定义颜色(半透明)
    D3DCOLOR blueColor = D3DCOLOR_ARGB(128, 0, 0, 255);   // Alpha, R, G, B
    D3DCOLOR greenColor = D3DCOLOR_ARGB(128, 0, 255, 0);

    std::vector<FVF_FrustumVertex> vertices;
    vertices.reserve(36); // 6个面 * 每个面2个三角形 * 每个三角形3个顶点

    // 视锥体角点(假设CFrustum::GetWorldCorners()按此顺序返回)
    // N = 近, F = 远, T = 上, B = 下, L = 左, R = 右
    // m_vecClip[0] = 近左下, m_vecClip[1] = 近右下, m_vecClip[2] = 近右上, m_vecClip[3] = 近左上
    // m_vecClip[4] = 远左下, m_vecClip[5] = 远右下, m_vecClip[6] = 远右上, m_vecClip[7] = 远左上

    // 近面(蓝色) - 三角形: (0,1,2), (0,2,3)
    vertices.push_back({ corners[0], blueColor }); vertices.push_back({ corners[1], blueColor }); vertices.push_back({ corners[2], blueColor });
    vertices.push_back({ corners[0], blueColor }); vertices.push_back({ corners[2], blueColor }); vertices.push_back({ corners[3], blueColor });

    // 远面(蓝色) - 三角形: (4,7,6), (4,6,5) (外表面朝向)
    vertices.push_back({ corners[4], blueColor }); vertices.push_back({ corners[7], blueColor }); vertices.push_back({ corners[6], blueColor });
    vertices.push_back({ corners[4], blueColor }); vertices.push_back({ corners[6], blueColor }); vertices.push_back({ corners[5], blueColor });

    // 左面(绿色) - 三角形: (4,0,3), (4,3,7)
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[0], greenColor }); vertices.push_back({ corners[3], greenColor });
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[3], greenColor }); vertices.push_back({ corners[7], greenColor });

    // 右面(绿色) - 三角形: (1,5,6), (1,6,2)
    vertices.push_back({ corners[1], greenColor }); vertices.push_back({ corners[5], greenColor }); vertices.push_back({ corners[6], greenColor });
    vertices.push_back({ corners[1], greenColor }); vertices.push_back({ corners[6], greenColor }); vertices.push_back({ corners[2], greenColor });

    // 上面(绿色) - 三角形: (3,2,6), (3,6,7)
    vertices.push_back({ corners[3], greenColor }); vertices.push_back({ corners[2], greenColor }); vertices.push_back({ corners[6], greenColor });
    vertices.push_back({ corners[3], greenColor }); vertices.push_back({ corners[6], greenColor }); vertices.push_back({ corners[7], greenColor });

    // 下面(绿色) - 三角形: (4,5,1), (4,1,0)
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[5], greenColor }); vertices.push_back({ corners[1], greenColor });
    vertices.push_back({ corners[4], greenColor }); vertices.push_back({ corners[1], greenColor }); vertices.push_back({ corners[0], greenColor });


    // 创建顶点缓冲区
    if (FAILED(pd3dDevice->CreateVertexBuffer(vertices.size() * sizeof(FVF_FrustumVertex),
                                              D3DUSAGE_WRITEONLY, D3DFVF_FRUSTUMVERTEX,
                                              D3DPOOL_MANAGED, &g_pFrustumVB, NULL))) { // D3DPOOL_MANAGED简化管理
        std::cerr << "创建视锥体顶点缓冲区失败!" << std::endl;
        return;
    }

    // 填充顶点缓冲区
    VOID* pVertices;
    if (FAILED(g_pFrustumVB->Lock(0, 0, (void**)&pVertices, 0))) {
        std::cerr << "锁定视锥体顶点缓冲区失败!" << std::endl;
        return;
    }
    memcpy(pVertices, vertices.data(), vertices.size() * sizeof(FVF_FrustumVertex));
    g_pFrustumVB->Unlock();
    std::cout << "视锥体顶点缓冲区创建/更新，包含 " << vertices.size() << " 个顶点。" << std::endl;
}