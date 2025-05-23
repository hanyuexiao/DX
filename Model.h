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
    // ��Ա����
    std::string filePath; // ģ���ļ���·��

protected:
    // �ܱ�����Ա����������������ڲ�ʵ�ַ���
    ID3DXMesh* m_pMesh;                             // D3DX�������
    std::vector<D3DMATERIAL9> m_vMaterials;         // ģ�͵Ĳ����б�
    std::vector<LPDIRECT3DTEXTURE9> m_vTextures;    // ģ�͵������б� (ע��LPDIRECT3DTEXTURE9�������ָ������)
    DWORD m_dwNumMaterials;                         // ����������D3DXLoadMeshFromX�᷵��

public:
    // ����������
    Model(const std::string& name = "Model");

    virtual ~Model();

    //���Ĺ���
    // ���� LPDIRECT3DDEVICE9 pd3dDevice: D3D�豸ָ�룬���ڼ�����Դ
    // ���� const std::string& modelFilePath: ģ���ļ���·��
    // ����ֵ: bool, true��ʾ���سɹ�, false��ʾʧ��
    virtual bool LoadModelFromFile(const std::string& modelFilePath);

    // ��д GameObject ���麯��
    virtual void Update(float deltaTime) override;

    // Render ������Ҫ Direct3D �豸ָ�������л��Ʋ���
    // ����������� Render �������һ�� LPDIRECT3DDEVICE9 ������
    // ����ζ���������ϸ񡰸��ǡ�GameObject::Render()��ԭʼǩ�������GameObject::Render()�޲Σ���
    // ���ߣ�GameObject::Render() ҲӦ�ñ��޸�Ϊ����һ����Ⱦ�����Ļ��豸ָ�롣
    // ��һ��ѡ������ Model ���ڲ��ܹ����ʵ�ȫ�ֻ�ͨ�� Graphic ���ȡ�豸ָ�롣
    //
    // **������Ϊ���� Model �ܶ�����Ⱦ���ҿ��ǵ�ʵ�ʻ����������Ǹ� Model �� Render ����豸������**
    // **���ϣ�������� GameObject::Render() ��ȫһ�µ�ǩ��������Ҫ�� Model::Render() ʵ���ڲ���ȡ�豸ָ�롣**
    // **��������ѡ���� Model::Render �в�����ǿ������Ⱦְ��**
    // ����Ը������ Graphic ��������������ʵ����
    //
    // ���վ��������ֺͻ��� GameObject::Render() ǩ��һ�£��� cpp �ļ���ʵ��ʱ�ٿ�����λ�ȡdevice��
    // ǰ���������۹���GameObject::Render() �� virtual void Render();
    virtual void Render() override;


protected:
    // �ڲ���������
    virtual void ReleaseResources(); // �����ͷ� m_pMesh �� m_vTextures �е���Դ
};


#endif //DX_MODEL_H
