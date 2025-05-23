//
// Created by admin on 2025/5/22.
//

#ifndef DX_MODEL_H
#define DX_MODEL_H

#include "head.h"
#include "CGameObject.h"
#include "Graphic.h"

class Model : public GameObject {
public:
    // 成员变量
    std::string filePath; // 模型文件的路径

protected:
    // 受保护成员变量，供派生类或内部实现访问
    ID3DXMesh* m_pMesh;                             // D3DX网格对象
    std::vector<D3DMATERIAL9> m_vMaterials;         // 模型的材质列表
    std::vector<LPDIRECT3DTEXTURE9> m_vTextures;    // 模型的纹理列表 (注意LPDIRECT3DTEXTURE9本身就是指针类型)
    DWORD m_dwNumMaterials;                         // 材质数量，D3DXLoadMeshFromX会返回

public:
    // 构造与析构
    Model(const std::string& name = "Model");

    virtual ~Model();

    //核心功能
    // 参数 LPDIRECT3DDEVICE9 pd3dDevice: D3D设备指针，用于加载资源
    // 参数 const std::string& modelFilePath: 模型文件的路径
    // 返回值: bool, true表示加载成功, false表示失败
    virtual bool LoadModelFromFile(const std::string& modelFilePath);

    // 重写 GameObject 的虚函数
    virtual void Update(float deltaTime) override;

    // Render 函数需要 Direct3D 设备指针来进行绘制操作
    // 我们在这里给 Render 函数添加一个 LPDIRECT3DDEVICE9 参数。
    // 这意味着它不再严格“覆盖”GameObject::Render()的原始签名（如果GameObject::Render()无参）。
    // 或者，GameObject::Render() 也应该被修改为接受一个渲染上下文或设备指针。
    // 另一种选择是在 Model 类内部能够访问到全局或通过 Graphic 类获取设备指针。
    //
    // **决定：为了让 Model 能独立渲染，且考虑到实际绘制需求，我们给 Model 的 Render 添加设备参数。**
    // **如果希望保持与 GameObject::Render() 完全一致的签名，则需要在 Model::Render() 实现内部获取设备指针。**
    // **这里我们选择让 Model::Render 有参数，强调其渲染职责。**
    // 你可以根据你的 Graphic 类设计来决定最佳实践。
    //
    // 最终决定：保持和基类 GameObject::Render() 签名一致，在 cpp 文件中实现时再考虑如何获取device。
    // 前面我们讨论过，GameObject::Render() 是 virtual void Render();
    virtual void Render() override;


protected:
    // 内部辅助函数
    virtual void ReleaseResources(); // 用于释放 m_pMesh 和 m_vTextures 中的资源
};


#endif //DX_MODEL_H
