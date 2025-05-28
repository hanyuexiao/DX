//
// Created by admin on 2025/5/22.
//

#include "CTransform.h" // 包含Transform类的头文件定义

/**
 * @brief Transform类的构造函数。
 *
 * 初始化对象的变换属性：
 * - position: 设置为原点 (0, 0, 0)。
 * - scale: 设置为单位缩放 (1, 1, 1)。
 * - orientation: 初始化为单位四元数，表示没有旋转。
 */
Transform::Transform():position{0,0,0},scale{1,1,1} {
    // 使用D3DX库函数将方向四元数初始化为单位四元数（无旋转）。
    D3DXQuaternionIdentity(&orientation);
}

/**
 * @brief 计算并返回对象的世界变换矩阵。
 *
 * 此矩阵按顺序组合了缩放、旋转和平移变换 (S * R * T)。
 * 缩放矩阵根据 scale 成员创建。
 * 旋转矩阵根据 orientation 四元数创建。
 * 平移矩阵根据 position 成员创建。
 *
 * @return D3DXMATRIX 返回最终的世界变换矩阵。
 */
D3DXMATRIX Transform::GetWorldMatrix() const {
    D3DXMATRIX matScaling,matRotation,matTranslation,world;

    // 1. 根据方向四元数创建旋转矩阵。
    D3DXMatrixRotationQuaternion(&matRotation, &orientation);
    // 2. 根据缩放向量创建缩放矩阵。
    D3DXMatrixScaling(&matScaling, scale.x, scale.y, scale.z);
    // 3. 根据位置向量创建平移矩阵。
    D3DXMatrixTranslation(&matTranslation, position.x, position.y, position.z);

    // 4. 组合变换：缩放 -> 旋转 -> 平移。
    //    注意：矩阵乘法的顺序很重要。对于行向量坐标系 (如D3D中常见)，
    //    变换顺序是向量先乘以缩放矩阵，然后是旋转矩阵，最后是平移矩阵。
    world = matScaling * matRotation * matTranslation;

    return world;
}

/**
 * @brief 设置对象的位置。
 * @param x X轴坐标。
 * @param y Y轴坐标。
 * @param z Z轴坐标。
 */
void Transform::SetPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
}

/**
 * @brief 使用D3DXVECTOR3设置对象的位置。
 * @param vec 包含新位置的D3DXVECTOR3。
 */
void Transform::SetPosition(const D3DXVECTOR3 &vec) {
    position = vec;
}

/**
 * @brief 设置对象的缩放。
 * @param x X轴缩放比例。
 * @param y Y轴缩放比例。
 * @param z Z轴缩放比例。
 */
void Transform::SetScale(float x, float y, float z) {
    scale.x = x;
    scale.y = y;
    scale.z = z;
}

/**
 * @brief 使用D3DXVECTOR3设置对象的缩放。
 * @param vec 包含新缩放比例的D3DXVECTOR3。
 */
void Transform::SetScale(const D3DXVECTOR3 &vec) {
    scale = vec;
}

/**
 * @brief 根据欧拉角 (Yaw, Pitch, Roll) 设置对象的旋转。
 * 欧拉角将转换为四元数并存储在 orientation 成员中。
 * @param yaw 偏航角 (绕Y轴旋转)，以弧度为单位。
 * @param pitch 俯仰角 (绕X轴旋转)，以弧度为单位。
 * @param roll 翻滚角 (绕Z轴旋转)，以弧度为单位。
 */
void Transform::SetRotationEuler(float yaw, float pitch, float roll) {
    // 使用D3DX库函数从欧拉角创建四元数。
    D3DXQuaternionRotationYawPitchRoll(&orientation, yaw, pitch, roll);
}

/**
 * @brief 使用四元数设置对象的旋转。
 * 传入的四元数将被赋值给 orientation 成员，然后进行归一化。
 * 归一化确保四元数代表一个纯旋转，并有助于数值稳定性。
 * @param quad 代表新旋转的D3DXQUATERNION。
 */
void Transform::SetRotation(const D3DXQUATERNION &quad) {
    orientation = quad; // 1. 先将传入的四元数赋值给成员变量。
    D3DXQuaternionNormalize(&orientation, &orientation); // 2. 然后对成员变量自身进行归一化。
}

/**
 * @brief 沿给定向量平移对象。
 * 将指定的偏移量加到当前位置上。
 * @param offset 一个D3DXVECTOR3，表示在X, Y, Z轴上的平移量。
 */
void Transform::Translate(const D3DXVECTOR3 &offset) {
    position += offset; // 向量加法直接更新位置。
}

/**
 * @brief 将当前旋转与另一个旋转（四元数）复合。
 * 新的旋转 = 当前旋转 * 输入旋转 (rot)。
 * 结果四元数会被归一化。
 * @param rot 要应用的附加旋转（D3DXQUATERNION）。
 */
void Transform::Rotate(const D3DXQUATERNION &rot) {
    // 将当前方向四元数与输入的旋转四元数相乘。
    // D3DXQuaternionMultiply(&out, &q1, &q2) 计算 q1 * q2，表示先应用q2再应用q1。
    // 这里我们希望在当前旋转的基础上再应用rot，所以是 orientation = orientation * rot。
    D3DXQuaternionMultiply(&orientation, &orientation, &rot);
    // 保持四元数归一化。
    D3DXQuaternionNormalize(&orientation, &orientation);
}

/**
 * @brief 围绕指定轴旋转对象特定角度。
 * 当前旋转将与这个新的轴角旋转复合。
 * 结果四元数会被归一化。
 * @param axis 旋转轴（应为单位向量）。
 * @param angle 旋转角度（以弧度为单位）。
 */
void Transform::RotateAxis(const D3DXVECTOR3 &axis, float angle) {
    D3DXQUATERNION tempRotQuat;
    // 1. 根据旋转轴和角度创建临时旋转四元数。
    D3DXQuaternionRotationAxis(&tempRotQuat, &axis, angle);
    // 2. 将当前方向四元数与临时旋转四元数相乘。
    D3DXQuaternionMultiply(&orientation, &orientation, &tempRotQuat);
    // 3. 保持四元数归一化。
    D3DXQuaternionNormalize(&orientation, &orientation);
}

/**
 * @brief 获取对象在世界空间中的前方向向量。
 * 这是对象局部坐标系的Z轴正方向在世界空间中的表示。
 * 通常用于确定对象“朝向”哪里。
 * @return D3DXVECTOR3 返回归一化后的世界空间前方向向量。
 */
D3DXVECTOR3 Transform::GetForward() const {
    // 局部空间中的Z轴正方向 (例如，模型通常朝向Z+建模)。
    D3DXVECTOR3 localForward(0.0f, 0.0f, 1.0f);

    D3DXMATRIX rotationMatrix;
    // 从当前的方向四元数获取旋转矩阵。
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation);

    D3DXVECTOR3 worldForward;
    // 使用旋转矩阵变换局部前方向量到世界空间。
    // D3DXVec3TransformNormal 用于变换方向向量（它会忽略矩阵的平移部分）。
    D3DXVec3TransformNormal(&worldForward, &localForward, &rotationMatrix);

    // 确保返回的是单位向量。
    D3DXVec3Normalize(&worldForward, &worldForward);
    return worldForward;
}

/**
 * @brief 获取对象在世界空间中的右方向向量。
 * 这是对象局部坐标系的X轴正方向在世界空间中的表示。
 * @return D3DXVECTOR3 返回归一化后的世界空间右方向向量。
 */
D3DXVECTOR3 Transform::GetRight() const {
    // 局部空间中的X轴正方向。
    D3DXVECTOR3 localRight(1.0f, 0.0f, 0.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation);

    D3DXVECTOR3 worldRight;
    D3DXVec3TransformNormal(&worldRight, &localRight, &rotationMatrix);

    D3DXVec3Normalize(&worldRight, &worldRight);
    return worldRight;
}

/**
 * @brief 获取对象在世界空间中的上方向向量。
 * 这是对象局部坐标系的Y轴正方向在世界空间中的表示。
 * @return D3DXVECTOR3 返回归一化后的世界空间上方向向量。
 */
D3DXVECTOR3 Transform::GetUp() const {
    // 局部空间中的Y轴正方向。
    D3DXVECTOR3 localUp(0.0f, 1.0f, 0.0f);

    D3DXMATRIX rotationMatrix;
    D3DXMatrixRotationQuaternion(&rotationMatrix, &orientation);

    D3DXVECTOR3 worldUp;
    D3DXVec3TransformNormal(&worldUp, &localUp, &rotationMatrix);

    D3DXVec3Normalize(&worldUp, &worldUp);
    return worldUp;
}