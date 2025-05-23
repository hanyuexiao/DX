//
// Created by admin on 2025/5/22.
//

#ifndef DX_CCAMERA_H
#define DX_CCAMERA_H

#include "head.h"
#include "CGameObject.h"
#include "Graphic.h"

class Camera : public GameObject {
public:
    // 投影参数
    float fieldOfViewY;
    float aspectRatio;
    float nearPlane;
    float farPlane;

private:
    // 私有成员变量
    D3DXMATRIX m_matView;
    D3DXMATRIX m_matProjection;

public:
    // 构造函数
    Camera(const std::string& name = "Camera");

    // 矩阵更新函数
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    // 获取器
    const D3DXMATRIX& GetViewMatrix() const;
    const D3DXMATRIX& GetProjectionMatrix() const;

    // 摄像机控制函数 (这些会修改继承来的 transform)
    void MoveForward(float distance);
    void Strafe(float distance);
    void Fly(float distance); // 通常指沿自身Y轴或世界Y轴移动
    void RotateYaw(float angle);   // 围绕世界Y轴或自身Y轴旋转
    void RotatePitch(float angle); // 围绕自身X轴旋转

    // 重写 GameObject 的虚函数
    virtual void Update(float deltaTime) override;
    // virtual void Render() override; // Camera 通常不直接渲染几何体
};

#endif //DX_CCAMERA_H
