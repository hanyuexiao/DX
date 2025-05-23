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
    // ���г�Ա����
    Transform transform;
    std::string name;
    bool isActive;

protected:
    // �ܱ�����Ա����
    GameObject* m_pParent;
    std::vector<GameObject*> m_vChildren;

public:
    // ����������
    GameObject(const std::string& name = "GameObject");
    virtual ~GameObject(); // ����������

    // �����麯��
    virtual void Update(float deltaTime);
    virtual void Render();

    // ״̬����
    void SetActive(bool active);
    bool IsActive() const;

    // ���ӹ�ϵ����
    void SetParent(GameObject* parent);
    GameObject* GetParent() const;
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child); // ���Ӷ����б����Ƴ�������һ��ɾ����
    const std::vector<GameObject*>& GetChildren() const;

private:
    // �ڲ���������������AddChild����ȷ����˫���ϵ
    void AddChildInternal(GameObject* child);

    void RemoveChildInternal(GameObject* child); // �ڲ�ʹ�ã����� m_vChildren �Ƴ�
};


#endif //DX_CGAMEOBJECT_H
