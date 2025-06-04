//
// Created by admin on 2025/5/22.
//

#include "CCamera.h" // 包含 Camera 类的头文件定义
#include <string> // 确保 string 类型被正确包含 (虽然 Camera.h 可能已经间接包含了)

// 使用 std 命名空间中的 string，避免每次都写 std::string
using std::string;

// Camera 类的构造函数
// @param name: 相机对象的名称，传递给 GameObject 基类的构造函数
Camera::Camera(const string &name) : GameObject(name) {
    // 初始化投影矩阵的参数
    m_fFOV = D3DX_PI / 4.0f;       // 设置视野 (Field of View) 为 45 度 (PI/4 弧度)
    m_fAspect = 800.0f / 600.0f; // 设置宽高比 (Aspect Ratio)，例如 4:3
    m_fNearPlane = 1.0f;         // 设置近裁剪面距离
    m_fFarPlane = 1000.0f;       // 设置远裁剪面距离

    // 初始化旋转角度
    m_fCurrentYaw = 0.0f;        // 初始化当前偏航角为 0
    m_fCurrentPitch = 0.0f;      // 初始化当前俯仰角为 0
    m_bViewMatrixDirty = true;   // 标记视图矩阵需要初始更新

    // 设置相机的初始位置 (继承自 GameObject 的 transform 组件)
    // 将相机放在 Z 轴负方向 5 个单位处，看向原点
    transform.SetPosition(0.0f, 0.0f, -5.0f);

    // 更新投影矩阵 (通常在参数设置后立即更新一次)
    UpdateProjectionMatrix();

    // 设置相机的初始旋转 (基于初始的偏航角和俯仰角)
    // 注意：这里可能应该在 UpdateViewMatrix() 之前或作为其一部分来处理初始方向
    // 或者确保 transform 的旋转与 m_fCurrentYaw/m_fCurrentPitch 同步
    transform.SetRotationEuler(m_fCurrentYaw, m_fCurrentPitch, 0.0f);
    // 视图矩阵在第一次 GetViewMatrix() 调用时或显式调用 UpdateViewMatrix() 时更新
}

// 向前或向后移动相机
// @param distance: 移动的距离 (正值向前，负值向后)
void Camera::MoveForward(float distance) {
    // 获取相机当前的 "前向" 向量 (通常是局部 Z 轴的负方向)
    D3DXVECTOR3 forward = transform.GetForward();
    // 更新相机位置：当前位置 + 前向向量 * 距离
    transform.position += forward * distance;
    // 标记视图矩阵已更改，需要重新计算
    m_bViewMatrixDirty = true;
}

// 向左或向右平移相机
// @param distance: 移动的距离 (正值向右，负值向左)
void Camera::Strafe(float distance) {
    // 获取相机当前的 "右向" 向量 (通常是局部 X 轴的正方向)
    D3DXVECTOR3 right = transform.GetRight();
    // 更新相机位置：当前位置 + 右向向量 * 距离
    transform.position += right * distance;
    // 标记视图矩阵已更改，需要重新计算
    m_bViewMatrixDirty = true;
}

// 向上或向下飞行相机
// @param distance: 移动的距离 (正值向上，负值向下)
void Camera::Fly(float distance) {
    // 获取相机当前的 "上向" 向量 (通常是局部 Y 轴的正方向)
    D3DXVECTOR3 up = transform.GetUp();
    // 更新相机位置：当前位置 + 上向向量 * 距离
    transform.position += up * distance;
    // 标记视图矩阵已更改，需要重新计算
    m_bViewMatrixDirty = true;
}

// 围绕 Y 轴旋转相机 (偏航)
// @param angle: 旋转的角度 (通常为弧度)
void Camera::RotateYaw(float angle) {
    // 更新当前偏航角
    m_fCurrentYaw += angle;
    // 根据新的偏航角和当前的俯仰角更新相机的旋转 (GameObject 的 transform 组件)
    // 假设 SetRotationEuler 使用欧拉角 (Yaw, Pitch, Roll)
    transform.SetRotationEuler(m_fCurrentYaw, m_fCurrentPitch, 0.0f); // Roll 通常为 0
    // 标记视图矩阵已更改，需要重新计算
    m_bViewMatrixDirty = true;
}

// 围绕 X 轴旋转相机 (俯仰)
// @param angle: 旋转的角度 (通常为弧度)
void Camera::RotatePitch(float angle) {
    // 更新当前俯仰角
    m_fCurrentPitch += angle;

    // 限制俯仰角的范围，防止相机上下翻转 "过头"
    // 定义一个略小于 90 度 (PI/2 弧度) 的极限值
    float pitchLimit = (D3DX_PI / 2.0f) - 0.01f;
    if (m_fCurrentPitch > pitchLimit) {
        m_fCurrentPitch = pitchLimit; // 限制最大俯仰角
    } else if (m_fCurrentPitch < -pitchLimit) {
        m_fCurrentPitch = -pitchLimit; // 限制最小俯仰角
    }

    // 根据当前的偏航角和新的俯仰角更新相机的旋转
    transform.SetRotationEuler(m_fCurrentYaw, m_fCurrentPitch, 0.0f);
    // 标记视图矩阵已更改，需要重新计算
    m_bViewMatrixDirty = true;
}

// 更新投影矩阵
// 当视野、宽高比、近裁剪面或远裁剪面发生变化时调用
void Camera::UpdateProjectionMatrix() {
    // 使用 D3DX 库函数创建一个左手透视投影矩阵
    // 参数:
    // &m_matProjection: 指向要填充的投影矩阵的指针
    // m_fFOV: Y 轴方向的视野角度 (弧度)
    // m_fAspect: 宽高比 (宽度 / 高度)
    // m_fNearPlane: 近裁剪面到视点的距离
    // m_fFarPlane: 远裁剪面到视点的距离
    D3DXMatrixPerspectiveFovLH(&m_matProjection, m_fFOV, m_fAspect, m_fNearPlane, m_fFarPlane);
}

// 更新视图矩阵
// 当相机位置或朝向发生变化时调用
void Camera::UpdateViewMatrix() {
    // 仅当视图矩阵被标记为 "dirty" (需要更新) 时才执行计算
    if (m_bViewMatrixDirty) {
        // 获取相机当前的位置 (眼睛/视点位置)
        D3DXVECTOR3 eyePosition = transform.position;
        // 定义观察目标点
        D3DXVECTOR3 lookAt;
        // 定义上方向向量
        D3DXVECTOR3 upDirection;

        // 计算观察目标点：相机位置 + 相机前向向量
        // 这意味着相机总是看向其正前方
        D3DXVECTOR3 forward = transform.GetForward();
        lookAt = eyePosition + forward;

        // 获取相机的上方向向量
        upDirection = transform.GetUp();

        // 使用 D3DX 库函数创建一个左手视图矩阵
        // 参数:
        // &m_matView: 指向要填充的视图矩阵的指针
        // &eyePosition: 视点的位置
        // &lookAt: 观察目标点的位置
        // &upDirection: 世界的上方向向量 (通常是 (0,1,0)，但这里用的是相机的局部上方向)
        D3DXMatrixLookAtLH(&m_matView, &eyePosition, &lookAt, &upDirection);

        // 视图矩阵已更新，清除 "dirty" 标志
        m_bViewMatrixDirty = false;
    }
}

// 获取视图矩阵
// 如果视图矩阵是 "dirty" 的，则先更新它
// @return: 指向视图矩阵的常量引用
const D3DXMATRIX &Camera::GetViewMatrix() {
    // 惰性更新：仅在需要时更新视图矩阵
    if (m_bViewMatrixDirty) {
        UpdateViewMatrix();
    }
    return m_matView;
}

// 获取投影矩阵
// @return: 指向投影矩阵的常量引用
const D3DXMATRIX &Camera::GetProjectionMatrix() const {
    // 投影矩阵通常在参数更改时更新，这里直接返回已计算好的矩阵
    return m_matProjection;
}

// LookAt 方法实现
// LookAt 方法实现
void Camera::LookAt(const D3DXVECTOR3& vTarget, const D3DXVECTOR3& vWorldUp) {
    D3DXVECTOR3 vPosition = this->transform.GetPosition();

    // D3DXMatrixLookAtLH 计算的是视图矩阵。我们需要的是一个能代表“朝向”的世界变换矩阵的旋转部分。
    // 然后从这个旋转矩阵中提取四元数。

    // 1. 计算新的前方向 (从相机指向目标)
    D3DXVECTOR3 newForward;
    D3DXVec3Subtract(&newForward, &vTarget, &vPosition);
    D3DXVec3Normalize(&newForward, &newForward);

    // 2. 计算新的右方向
    D3DXVECTOR3 newRight;
    D3DXVec3Cross(&newRight, &vWorldUp, &newForward); // worldUp X forward
    D3DXVec3Normalize(&newRight, &newRight);
    // 处理特殊情况：如果forward和worldUp平行 (例如，直视上方或下方)
    if (D3DXVec3LengthSq(&newRight) < 0.0001f) {
        // 目标在正上方或正下方，需要选择一个不同的临时Up向量来计算Right
        // 例如，如果目标在正上方，forward 接近 (0,1,0)，worldUp 是 (0,1,0)
        // 叉乘结果会是0。此时可以尝试用世界X轴
        D3DXVECTOR3 worldXAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f); // 创建一个具名变量
        D3DXVec3Cross(&newRight, &worldXAxis, &newForward);
        D3DXVec3Normalize(&newRight, &newRight);
    }


    // 3. 计算新的上方向
    D3DXVECTOR3 newUp;
    D3DXVec3Cross(&newUp, &newForward, &newRight); // forward X right
    D3DXVec3Normalize(&newUp, &newUp);

    // 4. 构建旋转矩阵
    // D3DX标准：矩阵的行是基向量 (Right, Up, Forward)
    // 这是从物体局部空间到世界空间的旋转矩阵
    D3DXMATRIX matRot;
    D3DXMatrixIdentity(&matRot);
    // Row 1 (X-axis of camera's new orientation in world space) = newRight
    matRot._11 = newRight.x; matRot._12 = newRight.y; matRot._13 = newRight.z;
    // Row 2 (Y-axis of camera's new orientation in world space) = newUp
    matRot._21 = newUp.x;    matRot._22 = newUp.y;    matRot._23 = newUp.z;
    // Row 3 (Z-axis of camera's new orientation in world space) = newForward
    matRot._31 = newForward.x; matRot._32 = newForward.y; matRot._33 = newForward.z;
    // 注意：D3DXQuaternionRotationMatrix期望的矩阵是这样的：
    //   m._11, m._12, m._13,
    //   m._21, m._22, m._23,
    //   m._31, m._32, m._33
    // 实际上，更标准的做法是列向量是新的基向量在旧坐标系（世界）中的表示。
    // 或者说，这个矩阵的行是旧基向量在新坐标系中的表示。
    // D3DXMatrixLookAtLH 本身会构建一个视图矩阵，其逆是相机的世界变换。
    // 我们这里尝试直接从轴构建旋转。
    // 一个更可靠的方法是使用 D3DXMatrixLookAtLH 获得视图矩阵，然后取其逆，再从逆矩阵中提取四元数。
    // 但我们也可以直接从正交基构建旋转矩阵。
    // 矩阵的列是新的局部坐标轴在世界坐标系中的表示。
    // matRot._11 = newRight.x;    matRot._21 = newRight.y;    matRot._31 = newRight.z;
    // matRot._12 = newUp.x;       matRot._22 = newUp.y;       matRot._32 = newUp.z;
    // matRot._13 = newForward.x;  matRot._23 = newForward.y;  matRot._33 = newForward.z;
    // 这样，当乘以一个局部向量 (1,0,0) 时，会得到 newRight。

    // 5. 从旋转矩阵创建四元数
    D3DXQUATERNION newOrientation;
    D3DXQuaternionRotationMatrix(&newOrientation, &matRot); // matRot应该是从局部到世界的旋转
    D3DXQuaternionNormalize(&newOrientation, &newOrientation); // 确保是单位四元数

    // 6. 设置Transform的旋转
    this->transform.SetRotation(newOrientation);

    m_bViewMatrixDirty = true;
}

// Camera 类的析构函数
// = default 表示使用编译器生成的默认析构函数
// 因为 Camera 类没有直接管理需要手动释放的资源 (例如裸指针)，
// 并且基类 GameObject 的析构函数是虚函数，所以默认析构函数通常是足够的。
D3DXVECTOR3 Transform::GetEulerAngles() const {
    D3DXMATRIX matRotation;
    D3DXMatrixRotationQuaternion(&matRotation, &orientation);

    float yaw, pitch, roll;

    // 提取俯仰角 (Pitch) - 绕X轴
    // asin的值域是 [-PI/2, PI/2]，对应俯仰角的典型范围
    pitch = asinf(-matRotation._32); // _32 是 sin(pitch) (某些矩阵约定下是 -sin(pitch))

    // 根据俯仰角是否接近 +/- 90度 (万向节死锁情况) 来分别处理偏航角和翻滚角
    float cosPitch = cosf(pitch);
    if (fabs(cosPitch) > 0.0001f) { // 避免除以零
        // 偏航角 (Yaw) - 绕Y轴
        yaw = atan2f(matRotation._31 / cosPitch, matRotation._33 / cosPitch);
        // 翻滚角 (Roll) - 绕Z轴
        roll = atan2f(matRotation._12 / cosPitch, matRotation._22 / cosPitch);
    } else {
        // 万向节死锁: 俯仰角为 +/- 90度
        // 此时，可以将所有旋转分配给偏航角（或翻滚角），并将另一个设为0
        yaw = atan2f(-matRotation._13, matRotation._11);
        roll = 0.0f;
    }
    return D3DXVECTOR3(pitch, yaw, roll);
}
Camera::~Camera() = default;
