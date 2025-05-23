//
// Created by admin on 2025/5/22.
//

#ifndef DX_CGAMEOBJECT_H
#define DX_CGAMEOBJECT_H

#include "head.h"
#include "Graphic.h"
#include "CTransform.h"

class GameObject {
public:
    // 公有成员变量
    Transform transform;
    std::string name;
    bool isActive;

protected:
    // 受保护成员变量
    GameObject* m_pParent;
    std::vector<GameObject*> m_vChildren;

public:
    // 构造与析构
    GameObject(const std::string& name = "GameObject");
    virtual ~GameObject(); // 虚析构函数

    // 核心虚函数
    virtual void Update(float deltaTime);
    virtual void Render();

    // 状态管理
    void SetActive(bool active);
    bool IsActive() const;

    // 父子关系管理
    void SetParent(GameObject* parent);
    GameObject* GetParent() const;
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child); // 从子对象列表中移除，但不一定删除它
    const std::vector<GameObject*>& GetChildren() const;

private:
    // 内部辅助函数，用于AddChild中正确设置双向关系
    void AddChildInternal(GameObject* child);

    void RemoveChildInternal(GameObject* child); // 内部使用，仅从 m_vChildren 移除
};


#endif //DX_CGAMEOBJECT_H
