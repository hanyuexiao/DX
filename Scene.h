//
// Created by admin on 2025/5/22.
//

#ifndef DX_SCENE_H
#define DX_SCENE_H

#include "head.h"

// ǰ������������ѭ������
class GameObject;
class Camera; // ����֪�� Camera �̳��� GameObject

// ��� LPDIRECT3DDEVICE9 ������ȫ�ֻ���ͷ�ļ��ж��壬������Ҫ���� <d3d9.h>
#include <d3d9.h>

class Scene {
protected:
    std::vector<GameObject*> m_vRootGameObjects; // ֻ�洢�����еĸ�����
    Camera* m_pMainCamera;                     // �������������

public:
    // ����������
    Scene();
    virtual ~Scene(); // ��Ҫ�������� m_vRootGameObjects �еĶ��������������Scene������ӵ�еģ�

    // GameObject ����
    // ע�⣺AddGameObject Ӧ��ֻ��Ӹ������Ӷ�������ͨ�� GameObject::AddChild ��ɡ�
    void AddRootGameObject(GameObject* pObject);
    void RemoveRootGameObject(GameObject* pObject); // ͬ����ֻ�Ƴ�������

    // ���������
    void SetMainCamera(Camera* pCamera);
    Camera* GetMainCamera() const;

    // ����GameObject (��ѡ������������)
    GameObject* FindGameObjectByName(const std::string& name) const; // �ݹ����

    // ������������
    virtual void Update(float deltaTime);
    // Render������ҪD3D�豸ָ����������ͼ/ͶӰ���󲢵��ö����Render
    virtual void Render(LPDIRECT3DDEVICE9 pd3dDevice);

protected:
    // �ڲ��������������ڵݹ����
    GameObject* FindGameObjectByNameRecursive(GameObject* currentObject, const std::string& name) const;
};


#endif //DX_SCENE_H
