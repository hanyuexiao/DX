//
// Created by admin on 2025/5/22.
//

#include "CCamera.h"

// ���캯��
GameObject::GameObject(const std::string& name)
        : name(name),       // ��ʼ������
          isActive(true),   // Ĭ�ϼ���
          m_pParent(nullptr) { // Ĭ��û�и�����
    // transform ��Ա���Զ�������Ĭ�Ϲ��캯�����г�ʼ��
}

// ����������
GameObject::~GameObject() {
    // ע�⣺GameObject ͨ��������ֱ��ɾ�����Ӷ���
    // �Ӷ������������Ӧ���ɴ������ǵĵط������� Scene ����ֶ����������ơ�
    // ��� GameObject �����Ϊ��ӵ�С����Ӷ�����ô������Ҫ���� m_vChildren �� delete ���ǡ�
    // ͬʱ��������и�����������Ӧ�ôӸ���������б����Ƴ��Լ�������ͨ���� SetParent(nullptr) �д���
    // Ϊ�������������������������������ӵ��Ӷ������
}

// ÿ֡����
void GameObject::Update(float deltaTime) {
    if (!isActive) { // �������δ�����ִ�и���
        return;
    }

    // �������л���Ӷ���
    // ע�⣺����һ���ݹ���µĹ��̣����������ʱ����������Ӷ������
    for (GameObject* child : m_vChildren) {
        if (child && child->IsActive()) { // ȷ���Ӷ�������Ҽ���
            child->Update(deltaTime);
        }
    }
}

// ÿ֡��Ⱦ (����ͨ���ǿյģ���ֻ��������Ӷ������Ⱦ)
void GameObject::Render() {
    if (!isActive) { // �������δ�����ִ����Ⱦ
        return;
    }

    // ��Ⱦ���л���Ӷ���
    // ����һ���ݹ���Ⱦ�Ĺ���
    for (GameObject* child : m_vChildren) {
        if (child && child->IsActive()) {
            child->Render();
        }
    }
}

// ���ö���ļ���״̬
void GameObject::SetActive(bool active) {
    isActive = active;
}

// ��ȡ����ļ���״̬
bool GameObject::IsActive() const {
    return isActive;
}

// ���ø�����
void GameObject::SetParent(GameObject* newParent) {
    // 1. �����ǰ�и������ȴӾɸ�������ӽڵ��б����Ƴ��Լ�
    if (m_pParent != nullptr) {
        m_pParent->RemoveChildInternal(this); // ʹ���ڲ�����
    }

    // 2. �����µĸ�����
    m_pParent = newParent;

    // 3. ����µĸ������� nullptr�����Լ���ӵ��¸�������ӽڵ��б���
    //    ����ȷ���µĸ��������Լ� (��ֹѭ������)
    if (m_pParent != nullptr && m_pParent != this) {
        m_pParent->AddChildInternal(this); // ʹ���ڲ�����
    }
}

// ��ȡ������
GameObject* GameObject::GetParent() const {
    return m_pParent;
}

// ����Ӷ��� (�����ӿ�)
void GameObject::AddChild(GameObject* child) {
    if (child != nullptr && child != this && child->GetParent() != this) {
        // �����Ӷ���� SetParent ����ȷ����˫���ϵ
        // SetParent �ڲ��ᴦ��Ӿɸ������Ƴ����߼�
        child->SetParent(this);
    }
}

// �Ƴ��Ӷ��� (�����ӿ�) - ����� child �� this �����б����Ƴ��������child�ĸ�ָ��
void GameObject::RemoveChild(GameObject* child) {
    if (child != nullptr && child->GetParent() == this) {
        child->SetParent(nullptr); // ��ᴥ�� child �� this �����б����Ƴ�
    }
}

// ��ȡ�Ӷ����б�ĳ�������
const std::vector<GameObject*>& GameObject::GetChildren() const {
    return m_vChildren;
}

// �ڲ���������������Ӷ��� m_vChildren (�� SetParent ����)
void GameObject::AddChildInternal(GameObject* child) {
    // �����ظ����
    if (std::find(m_vChildren.begin(), m_vChildren.end(), child) == m_vChildren.end()) {
        m_vChildren.push_back(child);
    }
}

// �ڲ������������� m_vChildren �Ƴ��Ӷ��� (�� SetParent ����)
void GameObject::RemoveChildInternal(GameObject* child) {
    // std::remove ��ָ��Ԫ���Ƶ�ĩβ��������ָ����ĩβ�ĵ�����
    // Ȼ�� erase ɾ���Ӹõ���������ʵĩβ��Ԫ��
    auto it = std::remove(m_vChildren.begin(), m_vChildren.end(), child);
    m_vChildren.erase(it, m_vChildren.end());
}