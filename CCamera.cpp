//
// Created by admin on 2025/5/22.
//

#include "CCamera.h"

// 构造函数
GameObject::GameObject(const std::string& name)
        : name(name),       // 初始化名称
          isActive(true),   // 默认激活
          m_pParent(nullptr) { // 默认没有父对象
    // transform 成员会自动调用其默认构造函数进行初始化
}

// 虚析构函数
GameObject::~GameObject() {
    // 注意：GameObject 通常不负责直接删除其子对象。
    // 子对象的生命周期应该由创建它们的地方（例如 Scene 类或手动管理）来控制。
    // 如果 GameObject 被设计为“拥有”其子对象，那么这里需要遍历 m_vChildren 并 delete 它们。
    // 同时，如果它有父对象，理论上应该从父对象的子列表中移除自己，但这通常在 SetParent(nullptr) 中处理。
    // 为简单起见，我们这里的析构函数不做复杂的子对象管理。
}

// 每帧更新
void GameObject::Update(float deltaTime) {
    if (!isActive) { // 如果对象未激活，则不执行更新
        return;
    }

    // 更新所有活动的子对象
    // 注意：这是一个递归更新的过程，父对象更新时会带动所有子对象更新
    for (GameObject* child : m_vChildren) {
        if (child && child->IsActive()) { // 确保子对象存在且激活
            child->Update(deltaTime);
        }
    }
}

// 每帧渲染 (基类通常是空的，或只负责调用子对象的渲染)
void GameObject::Render() {
    if (!isActive) { // 如果对象未激活，则不执行渲染
        return;
    }

    // 渲染所有活动的子对象
    // 这是一个递归渲染的过程
    for (GameObject* child : m_vChildren) {
        if (child && child->IsActive()) {
            child->Render();
        }
    }
}

// 设置对象的激活状态
void GameObject::SetActive(bool active) {
    isActive = active;
}

// 获取对象的激活状态
bool GameObject::IsActive() const {
    return isActive;
}

// 设置父对象
void GameObject::SetParent(GameObject* newParent) {
    // 1. 如果当前有父对象，先从旧父对象的子节点列表中移除自己
    if (m_pParent != nullptr) {
        m_pParent->RemoveChildInternal(this); // 使用内部方法
    }

    // 2. 设置新的父对象
    m_pParent = newParent;

    // 3. 如果新的父对象不是 nullptr，将自己添加到新父对象的子节点列表中
    //    并且确保新的父对象不是自己 (防止循环引用)
    if (m_pParent != nullptr && m_pParent != this) {
        m_pParent->AddChildInternal(this); // 使用内部方法
    }
}

// 获取父对象
GameObject* GameObject::GetParent() const {
    return m_pParent;
}

// 添加子对象 (公开接口)
void GameObject::AddChild(GameObject* child) {
    if (child != nullptr && child != this && child->GetParent() != this) {
        // 调用子对象的 SetParent 来正确建立双向关系
        // SetParent 内部会处理从旧父对象移除的逻辑
        child->SetParent(this);
    }
}

// 移除子对象 (公开接口) - 这仅将 child 从 this 的子列表中移除，并清空child的父指针
void GameObject::RemoveChild(GameObject* child) {
    if (child != nullptr && child->GetParent() == this) {
        child->SetParent(nullptr); // 这会触发 child 从 this 的子列表中移除
    }
}

// 获取子对象列表的常量引用
const std::vector<GameObject*>& GameObject::GetChildren() const {
    return m_vChildren;
}

// 内部辅助函数：添加子对象到 m_vChildren (由 SetParent 调用)
void GameObject::AddChildInternal(GameObject* child) {
    // 避免重复添加
    if (std::find(m_vChildren.begin(), m_vChildren.end(), child) == m_vChildren.end()) {
        m_vChildren.push_back(child);
    }
}

// 内部辅助函数：从 m_vChildren 移除子对象 (由 SetParent 调用)
void GameObject::RemoveChildInternal(GameObject* child) {
    // std::remove 将指定元素移到末尾，并返回指向新末尾的迭代器
    // 然后 erase 删除从该迭代器到真实末尾的元素
    auto it = std::remove(m_vChildren.begin(), m_vChildren.end(), child);
    m_vChildren.erase(it, m_vChildren.end());
}