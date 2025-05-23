//
// Created by admin on 2025/5/22.
//

#ifndef DX_SCENE_H
#define DX_SCENE_H

#include "head.h"

// 前向声明，避免循环依赖
class GameObject;
class Camera; // 我们知道 Camera 继承自 GameObject

// 如果 LPDIRECT3DDEVICE9 不是在全局或常用头文件中定义，可能需要包含 <d3d9.h>
#include <d3d9.h>

class Scene {
protected:
    std::vector<GameObject*> m_vRootGameObjects; // 只存储场景中的根对象
    Camera* m_pMainCamera;                     // 场景的主摄像机

public:
    // 构造与析构
    Scene();
    virtual ~Scene(); // 需要负责清理 m_vRootGameObjects 中的对象（如果它们是由Scene创建和拥有的）

    // GameObject 管理
    // 注意：AddGameObject 应该只添加根对象。子对象的添加通过 GameObject::AddChild 完成。
    void AddRootGameObject(GameObject* pObject);
    void RemoveRootGameObject(GameObject* pObject); // 同样，只移除根对象

    // 摄像机管理
    void SetMainCamera(Camera* pCamera);
    Camera* GetMainCamera() const;

    // 查找GameObject (可选，但可能有用)
    GameObject* FindGameObjectByName(const std::string& name) const; // 递归查找

    // 场景生命周期
    virtual void Update(float deltaTime);
    // Render函数需要D3D设备指针来设置视图/投影矩阵并调用对象的Render
    virtual void Render(LPDIRECT3DDEVICE9 pd3dDevice);

protected:
    // 内部辅助函数，用于递归查找
    GameObject* FindGameObjectByNameRecursive(GameObject* currentObject, const std::string& name) const;
};


#endif //DX_SCENE_H
