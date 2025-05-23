//
// Created by admin on 2025/5/22.
//

#include "CTransform.h"

Transform::Transform():position{0,0,0},scale{1,1,1} {
    D3DXQuaternionIdentity(&orientation);
}

D3DXMATRIX Transform::GetWorldMatrix() const {
    D3DXMATRIX matScaling,matRotation,matTranslation,world;
    D3DXMatrixRotationQuaternion(&matRotation, &orientation);
    D3DXMatrixScaling(&matScaling, scale.x, scale.y, scale.z);
    D3DXMatrixTranslation(&matTranslation, position.x, position.y, position.z);
    world = matScaling * matRotation * matTranslation;
    return world;
}

void Transform::SetPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

void Transform::SetPosition(const D3DXVECTOR3 &vec) {
    position = vec;
}

void Transform::SetScale(float x, float y, float z) {
    scale.x = x;
    scale.y = y;
    scale.z = z;
}

void Transform::SetScale(const D3DXVECTOR3 &vec) {
    scale = vec;
}

void Transform::SetRotationEuler(float yaw, float pitch, float roll) {
    D3DXQuaternionRotationYawPitchRoll(&orientation, yaw, pitch, roll);
}

void Transform::SetRotation(const D3DXQUATERNION &quad) {
    orientation = quad; // 1. �Ƚ��������Ԫ����ֵ����Ա����
    D3DXQuaternionNormalize(&orientation, &orientation); // 2. Ȼ��Գ�Ա����������й�һ��
}

void Transform::Translate(const D3DXVECTOR3 &offset) {
    position += offset;
}

void Transform::Rotate(const D3DXQUATERNION &rot) {
    D3DXQuaternionMultiply(&orientation, &orientation, &rot);
    D3DXQuaternionNormalize(&orientation, &orientation);
}

void Transform::RotateAxis(const D3DXVECTOR3 &axis, float angle) {
    D3DXQUATERNION tempRotQuat;
    D3DXQuaternionRotationAxis(&tempRotQuat, &axis, angle);
    D3DXQuaternionMultiply(&orientation, &orientation, &tempRotQuat);
    D3DXQuaternionNormalize(&orientation, &orientation);
}

// ��ȡǰ�������� (�ֲ� Z ��������������ռ��еı�ʾ)
D3DXVECTOR3 Transform::GetForward() const {
    // �ֲ��ռ��е� Z �������� (ͨ��ģ���ǳ���Z��������ģ��)
    D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation); // �ӵ�ǰ��Ԫ����ȡ��ת����

    D3DXVECTOR3 worldForward;
    // D3DXVec3TransformNormal ���ڱ任�������� (���Ծ����ƽ�Ʋ���)
    D3DXVec3TransformNormal(&worldForward, &localForward, &rotationMatrix);

    D3DXVec3Normalize(&worldForward, &worldForward); // ȷ�����ص��ǵ�λ����
    return worldForward;
}

// ��ȡ�ҷ������� (�ֲ� X ��������������ռ��еı�ʾ)
D3DXVECTOR3 Transform::GetRight() const {
    // �ֲ��ռ��е� X ��������
    D3DXVECTOR3 localRight(1.0f, 0.0f, 0.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation);

    D3DXVECTOR3 worldRight;
    D3DXVec3TransformNormal(&worldRight, &localRight, &rotationMatrix);

    D3DXVec3Normalize(&worldRight, &worldRight);
    return worldRight;
}

// ��ȡ�Ϸ������� (�ֲ� Y ��������������ռ��еı�ʾ)
D3DXVECTOR3 Transform::GetUp() const {
    // �ֲ��ռ��е� Y ��������
    D3DXVECTOR3 localUp(0.0f, 1.0f, 0.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation);

    D3DXVECTOR3 worldUp;
    D3DXVec3TransformNormal(&worldUp, &localUp, &rotationMatrix);

    D3DXVec3Normalize(&worldUp, &worldUp);
    return worldUp;
}










