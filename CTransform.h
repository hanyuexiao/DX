//
// Created by admin on 2025/5/22.
//

#ifndef DX_CTRANSFORM_H
#define DX_CTRANSFORM_H

#include <head.h>
#include "Graphic.h" // 假设这里有 CGraphic 和 MyImageInfo 的定义


class Transform {
public:
    // 成员变量
    D3DXVECTOR3 position;
    D3DXQUATERNION orientation; // 使用四元数表示旋转
    D3DXVECTOR3 scale;

public:
    // 构造函数
    Transform();

    // 获取世界矩阵
    D3DXMATRIX GetWorldMatrix() const;

    // 设置器 (Setters)
    void SetPosition(float x, float y, float z);
    void SetPosition(const D3DXVECTOR3& vec);
    void SetRotationEuler(float yaw, float pitch, float roll); // 欧拉角 (弧度)
    void SetRotation(const D3DXQUATERNION& quad);
    void SetScale(float x, float y, float z);
    void SetScale(const D3DXVECTOR3& vec);

    // 操作函数 (Modifiers/Operators)
    void Translate(const D3DXVECTOR3& offset);
    void Rotate(const D3DXQUATERNION& rot);
    void RotateAxis(const D3DXVECTOR3& axis, float angle); // angle 为弧度

    // 获取器 (Getters) - 获取局部坐标轴在世界空间中的方向
    D3DXVECTOR3 GetForward() const;
    D3DXVECTOR3 GetRight() const;
    D3DXVECTOR3 GetUp() const;
};

#endif //DX_CTRANSFORM_H
