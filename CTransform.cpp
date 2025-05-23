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
    orientation = quad; // 1. 先将传入的四元数赋值给成员变量
    D3DXQuaternionNormalize(&orientation, &orientation); // 2. 然后对成员变量自身进行归一化
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

// 获取前方向向量 (局部 Z 轴正方向在世界空间中的表示)
D3DXVECTOR3 Transform::GetForward() const {
    // 局部空间中的 Z 轴正方向 (通常模型是朝向Z轴正方向建模的)
    D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation); // 从当前四元数获取旋转矩阵

    D3DXVECTOR3 worldForward;
    // D3DXVec3TransformNormal 用于变换方向向量 (忽略矩阵的平移部分)
    D3DXVec3TransformNormal(&worldForward, &localForward, &rotationMatrix);

    D3DXVec3Normalize(&worldForward, &worldForward); // 确保返回的是单位向量
    return worldForward;
}

// 获取右方向向量 (局部 X 轴正方向在世界空间中的表示)
D3DXVECTOR3 Transform::GetRight() const {
    // 局部空间中的 X 轴正方向
    D3DXVECTOR3 localRight(1.0f, 0.0f, 0.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation);

    D3DXVECTOR3 worldRight;
    D3DXVec3TransformNormal(&worldRight, &localRight, &rotationMatrix);

    D3DXVec3Normalize(&worldRight, &worldRight);
    return worldRight;
}

// 获取上方向向量 (局部 Y 轴正方向在世界空间中的表示)
D3DXVECTOR3 Transform::GetUp() const {
    // 局部空间中的 Y 轴正方向
    D3DXVECTOR3 localUp(0.0f, 1.0f, 0.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation);

    D3DXVECTOR3 worldUp;
    D3DXVec3TransformNormal(&worldUp, &localUp, &rotationMatrix);

    D3DXVec3Normalize(&worldUp, &worldUp);
    return worldUp;
}










