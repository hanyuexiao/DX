//
// Created by admin on 2025/5/22.
//

#ifndef DX_CTRANSFORM_H
#define DX_CTRANSFORM_H

#include "head.h"       // 包含基础头文件，例如 D3DX 相关的定义。
#include "Graphic.h"    // 假设这里有 CGraphic 和 MyImageInfo 的定义 (虽然在此文件中可能不是直接必需的)。

/**
 * @class Transform
 * @brief 管理游戏对象的位置、旋转和缩放。
 *
 * 此类封装了游戏对象在3D空间中的变换信息，包括：
 * - 位置 (position)
 * - 旋转 (orientation)，使用四元数表示
 * - 缩放 (scale)
 * 它提供了获取组合的世界矩阵以及独立设置和修改这些变换的方法。
 */
class Transform {
public:
    // 公有成员变量
    D3DXVECTOR3 position;       ///< 对象在世界空间中的位置 (x, y, z)。
    D3DXQUATERNION orientation; ///< 对象在世界空间中的旋转，使用四元数表示。
    D3DXVECTOR3 scale;          ///< 对象在各个轴上的缩放比例 (x, y, z)，默认为 (1,1,1)。

public:
    /**
     * @brief 构造函数。
     * 初始化位置为原点 (0,0,0)，旋转为单位四元数 (无旋转)，缩放为 (1,1,1)。
     */
    Transform();

    /**
     * @brief 获取对象的世界矩阵。
     * 此矩阵组合了对象的缩放、旋转和平移变换。
     * @return 返回计算得到的世界矩阵 (D3DXMATRIX)。
     */
    D3DXMATRIX GetWorldMatrix() const;

    // --- 设置器 (Setters) ---

    /**
     * @brief 设置对象的位置。
     * @param x X轴坐标。
     * @param y Y轴坐标。
     * @param z Z轴坐标。
     */
    void SetPosition(float x, float y, float z);

    /**
     * @brief 使用D3DXVECTOR3设置对象的位置。
     * @param vec 包含新位置的D3DXVECTOR3。
     */
    void SetPosition(const D3DXVECTOR3& vec);

    /**
     * @brief 设置对象的旋转（使用欧拉角）。
     * 欧拉角将转换为四元数存储。
     * @param yaw 偏航角 (绕Y轴旋转)，弧度制。
     * @param pitch 俯仰角 (绕X轴旋转)，弧度制。
     * @param roll 翻滚角 (绕Z轴旋转)，弧度制。
     */
    void SetRotationEuler(float yaw, float pitch, float roll);
    D3DXVECTOR3 GetEulerAngles() const;
    /**
     * @brief 设置对象的旋转（使用四元数）。
     * 传入的四元数将被归一化后存储。
     * @param quat 代表新旋转的D3DXQUATERNION。
     */
    void SetRotation(const D3DXQUATERNION& quat);

    /**
     * @brief 设置对象的缩放。
     * @param x X轴缩放比例。
     * @param y Y轴缩放比例。
     * @param z Z轴缩放比例。
     */
    void SetScale(float x, float y, float z);

    /**
     * @brief 使用D3DXVECTOR3设置对象的缩放。
     * @param vec 包含新缩放比例的D3DXVECTOR3。
     */
    void SetScale(const D3DXVECTOR3& vec);

    // --- 操作函数 (Modifiers/Operators) ---

    /**
     * @brief 平移对象。
     * 将当前位置与指定的偏移量相加。
     * @param offset 平移的向量。
     */
    void Translate(const D3DXVECTOR3& offset);

    /**
     * @brief 旋转对象。
     * 将当前旋转与指定的旋转（四元数）相乘，并归一化结果。
     * @param rot 要应用的旋转（四元数）。
     */
    void Rotate(const D3DXQUATERNION& rot);

    /**
     * @brief 围绕指定轴旋转对象。
     * 将当前旋转与绕指定轴旋转指定角度产生的旋转相乘，并归一化结果。
     * @param axis 旋转轴（应为单位向量）。
     * @param angle 旋转角度（弧度制）。
     */
    void RotateAxis(const D3DXVECTOR3& axis, float angle);

    // --- 获取器 (Getters) - 获取局部坐标轴在世界空间中的方向 ---

    /**
     * @brief 获取对象的前方向向量 (局部Z+轴在世界空间中的方向)。
     * @return 返回归一化的前方向向量。
     */
    D3DXVECTOR3 GetForward() const;

    /**
     * @brief 获取对象的右方向向量 (局部X+轴在世界空间中的方向)。
     * @return 返回归一化的右方向向量。
     */
    D3DXVECTOR3 GetRight() const;

    /**
     * @brief 获取对象的上方向向量 (局部Y+轴在世界空间中的方向)。
     * @return 返回归一化的上方向向量。
     */
    D3DXVECTOR3 GetUp() const;
};

#endif //DX_CTRANSFORM_H