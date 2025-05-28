//
// Created by admin on 2025/5/22.
//

#ifndef DX_CPLAYER_H
#define DX_CPLAYER_H

#include "CGameObject.h" // 包含基类 GameObject 的定义
#include "CCamera.h"     // 如果玩家需要直接操作或关联摄像机

/**
 * @class Player
 * @brief 代表游戏中的玩家角色。
 *
 * Player 类继承自 GameObject，并添加了玩家特有的行为，
 * 例如响应输入、移动、以及可能的其他游戏逻辑。
 */
class Player : public GameObject {
public:
    // 公有成员变量
    // Camera* m_pCamera; // 如果玩家拥有或紧密关联一个摄像机，可以取消注释

    // 移动和旋转速度
    float m_moveSpeed;      ///< 玩家的移动速度 (单位/秒)
    float m_rotationSpeed;  ///< 玩家的旋转速度 (弧度/秒)
    bool isActive = true; // 玩家是否处于活动状态
public:
    /**
     * @brief Player 类的构造函数。
     * @param name 玩家对象的名称，默认为 "Player"。
     */
    explicit Player(const std::string& name = "Player");

    /**
     * @brief Player 类的析构函数。
     */
    virtual ~Player();

    // --- 核心更新与渲染 ---
    /**
     * @brief 每帧更新玩家的状态。
     * 重写自 GameObject::Update。这里将处理输入并更新玩家位置/朝向。
     * @param deltaTime 上一帧到当前帧的时间差（秒）。
     */
    virtual void Update(float deltaTime) override;

    /**
     * @brief 渲染玩家。
     * 重写自 GameObject::Render。如果玩家有自己的模型，在这里进行渲染。
     * 注意：如果Player类不直接负责渲染（例如，渲染由附加的ModelComponent处理），
     * 这个函数可能只是调用基类的Render或子组件的Render。
     */

    // --- 输入处理与移动 ---
    /**
     * @brief 处理玩家输入。
     * 在 Update 函数中被调用，检查按键并触发相应的移动/旋转。
     * @param deltaTime 上一帧到当前帧的时间差。
     */
    void HandleInput(float deltaTime);

    /**
     * @brief 使玩家向前移动。
     * @param deltaTime 时间增量，用于实现帧独立移动。
     */
    void MoveForward(float deltaTime);

    /**
     * @brief 使玩家向后移动。
     * @param deltaTime 时间增量。
     */
    void MoveBackward(float deltaTime);

    /**
     * @brief 使玩家向左平移。
     * @param deltaTime 时间增量。
     */
    void StrafeLeft(float deltaTime);

    /**
     * @brief 使玩家向右平移。
     * @param deltaTime 时间增量。
     */
    void StrafeRight(float deltaTime);

    /**
     * @brief 使玩家向左旋转（偏航）。
     * @param deltaTime 时间增量。
     */
    void RotateLeft(float deltaTime);

    /**
     * @brief 使玩家向右旋转（偏航）。
     * @param deltaTime 时间增量。
     */
    void RotateRight(float deltaTime);

    // --- 其他玩家特定功能 ---
    // 例如：
     void Attack();
     void Interact();
// This function returns the health of the object
     int GetHealth() const;
     void TakeDamage(int amount);

    bool LoadXModelFromFile(const std::string& modelFilePath);

    bool LoadFBXModelFromFile(const std::string& modelFilePath);

    bool LoadGenericModel(const std::string& modelFilePath); // 可选

    void Render(LPDIRECT3DDEVICE9 pd3dDevice);

    void ReleaseResources();
private:
    // 可以在这里添加私有成员变量，例如玩家的生命值、得分等
     ModelType m_modelType;

     std::string m_filePath;

    // .X 模型数据
    LPD3DXMESH m_pMesh;

    // .FBX 模型数据
    struct SubMesh {
        LPDIRECT3DVERTEXBUFFER9 vb = nullptr;
        LPDIRECT3DINDEXBUFFER9  ib = nullptr;
        UINT numVertices = 0;
        UINT numFaces = 0;
        UINT materialIndex = 0;
    };
    std::vector<SubMesh> m_vSubMeshes;
    DWORD m_dwFBXVertexFVF;

    // 通用材质和纹理数据
    DWORD m_dwNumMaterials;
    std::vector<D3DMATERIAL9> m_vMaterials;
    // 修改 m_vTextures 以存储 CGraphic::MyImageInfo
    std::vector<MyImageInfo> m_vTextures;

     int m_health;
     int m_score;
};

#endif //DX_CPLAYER_H
