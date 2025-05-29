// CPlayer.h
// 确保包含了 GameObject.h, CCamera.h (如果需要), 以及其他必要的头文件如 d3dx9.h, string, vector
#ifndef DX_CPLAYER_H
#define DX_CPLAYER_H

#include "CGameObject.h" // 包含基类 GameObject 的定义
#include "Graphic.h"     // 为了 MyImageInfo 和 ModelType (假设定义在这里或 common.h)
// #include "CCamera.h"  // 如果玩家需要直接操作或关联摄像机


#define D3DFVF_FBXVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)


class Player : public GameObject {
public:
    // 公有成员变量
    float m_moveSpeed;
    float m_rotationSpeed; // 用于键盘旋转
    bool isActive;

    // 鼠标控制相关
    float m_mouseSensitivity;
    bool m_isMouseRotating;
    POINT m_lastMousePos;

public:
    explicit Player(const std::string& name = "Player");
    virtual ~Player();

    virtual void Update(float deltaTime) override;
    void Render(LPDIRECT3DDEVICE9 pd3dDevice); // 注意：基类Render可能是无参的，这里是隐藏

//    void HandleInput(float deltaTime); // 处理键盘输入

    // 鼠标控制方法
    void StartMouseRotation(int x, int y);
    void StopMouseRotation();
    void ProcessMouseMovement(int deltaX, int deltaY); // deltaTime 通常不直接用于鼠标的delta计算

    // 移动方法
    void MoveForward(float deltaTime);
    void MoveBackward(float deltaTime);
    void StrafeLeft(float deltaTime);
    void StrafeRight(float deltaTime);
    void RotateLeft(float deltaTime);  // 键盘控制的左旋
    void RotateRight(float deltaTime); // 键盘控制的右旋

    // 模型加载与资源管理
    bool LoadXModelFromFile(const std::string& modelFilePath);
    bool LoadFBXModelFromFile(const std::string& modelFilePath);
    bool LoadGenericModel(const std::string& modelFilePath);
    void ReleaseResources();

    // 其他示例方法 (根据需要实现)
    // int GetHealth() const;
    // void TakeDamage(int amount);

private:
    ModelType m_modelType;
    std::string m_filePath;

    // .X 模型数据
    LPD3DXMESH m_pMesh; // 用于.X模型

    // .FBX 模型数据 (Assimp加载)
    struct SubMesh {
        LPDIRECT3DVERTEXBUFFER9 vb = nullptr;
        LPDIRECT3DINDEXBUFFER9  ib = nullptr;
        UINT numVertices = 0;
        UINT numFaces = 0;
        UINT materialIndex = 0; // 对应 m_vMaterials 和 m_vTextures 中的索引
    };
    std::vector<SubMesh> m_vSubMeshes;
    DWORD m_dwFBXVertexFVF; // FBX顶点的FVF

    // 通用材质和纹理数据
    DWORD m_dwNumMaterials;
    std::vector<D3DMATERIAL9> m_vMaterials;
    std::vector<MyImageInfo> m_vTextures; // MyImageInfo 来自 CGraphic.h

    // 玩家状态 (示例)
    // int m_health;
    // int m_score;

    // 鼠标控制的旋转角度
    float m_currentYaw;   // 当前偏航角 (绕Y轴)
    float m_currentPitch; // 当前俯仰角 (绕X轴)
};

#endif //DX_CPLAYER_H
