//
// Created by admin on 2025/5/22.
//

#ifndef DX_CCAMERA_H
#define DX_CCAMERA_H
// 包含头文件
#include "head.h" // 假设这里包含了一些基础定义，例如 D3DXMATRIX
#include "CGameObject.h" // 包含 GameObject 基类的定义
#include "Graphic.h" // 假设这里包含图形相关的定义

// Camera 类继承自 GameObject
class Camera : public GameObject {
public:
    // 投影参数 (这些似乎是公开的，但通常建议通过 getter/setter 访问或在构造时设置)
    float fieldOfViewY;  // 视野 (Y 轴方向)
    float aspectRatio;   // 宽高比
    float nearPlane;     // 近裁剪面距离
    float farPlane;      // 远裁剪面距离

private:
    // 私有成员变量
    D3DXMATRIX m_matView;         // 视图矩阵
    D3DXMATRIX m_matProjection;   // 投影矩阵
    bool m_bViewMatrixDirty;      // 视图矩阵是否需要更新的标志

    // 这些私有成员变量似乎是用于存储投影参数的内部副本，
    // 与上面公开的投影参数 (fieldOfViewY, aspectRatio, nearPlane, farPlane) 有些重复。
    // 考虑是否可以统一。
    float m_fCurrentYaw;         // 当前偏航角 (Y 轴旋转)
    float m_fCurrentPitch;       // 当前俯仰角 (X 轴旋转)

    // 以下参数用于构建投影矩阵，与公开的参数作用相同。
    // 建议只保留一组，并通过方法更新。
    float m_fFOV;                // 视野 (Field of View)
    float m_fAspect;             // 宽高比 (Aspect Ratio)
    float m_fNearPlane;          // 近裁剪面
    float m_fFarPlane;           // 远裁剪面

public:
    // 构造函数
    // explicit 关键字防止隐式类型转换
    // @param name 相机对象的名称，默认为 "Camera"
    explicit Camera(const std::string& name = "Camera");

    void LookAt(const D3DXVECTOR3& vTarget, const D3DXVECTOR3& vWorldUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    // 析构函数
    // override 关键字确保正确覆盖基类的虚析构函数
    ~Camera() override;

    // 矩阵更新函数
    void UpdateViewMatrix();       // 更新视图矩阵
    void UpdateProjectionMatrix(); // 更新投影矩阵

    // 获取器 (Getter)
    // 返回视图矩阵的常量引用
    const D3DXMATRIX& GetViewMatrix() ;
    // 返回投影矩阵的常量引用
    // const 成员函数，表示该函数不会修改对象的状态
    const D3DXMATRIX& GetProjectionMatrix() const;

    // 摄像机控制函数 (这些会修改继承来的 transform 组件，例如位置和旋转)
    // @param distance 移动的距离
    void MoveForward(float distance); // 向前移动 (沿相机自身 Z 轴负方向)
    // @param distance 移动的距离
    void Strafe(float distance);      // 平移 (沿相机自身 X 轴方向)
    // @param distance 移动的距离
    void Fly(float distance);         // 飞行 (通常指沿自身 Y 轴或世界 Y 轴移动)

    // @param angle 旋转的角度 (通常为弧度)
    void RotateYaw(float angle);      // 偏航旋转 (围绕世界 Y 轴或自身 Y 轴旋转)
    // @param angle 旋转的角度 (通常为弧度)
    void RotatePitch(float angle);    // 俯仰旋转 (围绕自身 X 轴旋转)

    // 重写 GameObject 的虚函数
    // Camera 通常不直接渲染几何体，所以这个函数可能为空或者不被调用。
    // virtual void Render() override;
};

#endif //DX_CCAMERA_H
