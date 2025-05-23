//
// Created by admin on 2025/5/22.
//

#ifndef DX_CTRANSFORM_H
#define DX_CTRANSFORM_H

#include <head.h>
#include "Graphic.h" // ���������� CGraphic �� MyImageInfo �Ķ���


class Transform {
public:
    // ��Ա����
    D3DXVECTOR3 position;
    D3DXQUATERNION orientation; // ʹ����Ԫ����ʾ��ת
    D3DXVECTOR3 scale;

public:
    // ���캯��
    Transform();

    // ��ȡ�������
    D3DXMATRIX GetWorldMatrix() const;

    // ������ (Setters)
    void SetPosition(float x, float y, float z);
    void SetPosition(const D3DXVECTOR3& vec);
    void SetRotationEuler(float yaw, float pitch, float roll); // ŷ���� (����)
    void SetRotation(const D3DXQUATERNION& quad);
    void SetScale(float x, float y, float z);
    void SetScale(const D3DXVECTOR3& vec);

    // �������� (Modifiers/Operators)
    void Translate(const D3DXVECTOR3& offset);
    void Rotate(const D3DXQUATERNION& rot);
    void RotateAxis(const D3DXVECTOR3& axis, float angle); // angle Ϊ����

    // ��ȡ�� (Getters) - ��ȡ�ֲ�������������ռ��еķ���
    D3DXVECTOR3 GetForward() const;
    D3DXVECTOR3 GetRight() const;
    D3DXVECTOR3 GetUp() const;
};

#endif //DX_CTRANSFORM_H
