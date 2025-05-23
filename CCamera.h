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
    // ͶӰ����
    float fieldOfViewY;
    float aspectRatio;
    float nearPlane;
    float farPlane;

private:
    // ˽�г�Ա����
    D3DXMATRIX m_matView;
    D3DXMATRIX m_matProjection;

public:
    // ���캯��
    Camera(const std::string& name = "Camera");

    // ������º���
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    // ��ȡ��
    const D3DXMATRIX& GetViewMatrix() const;
    const D3DXMATRIX& GetProjectionMatrix() const;

    // ��������ƺ��� (��Щ���޸ļ̳����� transform)
    void MoveForward(float distance);
    void Strafe(float distance);
    void Fly(float distance); // ͨ��ָ������Y�������Y���ƶ�
    void RotateYaw(float angle);   // Χ������Y�������Y����ת
    void RotatePitch(float angle); // Χ������X����ת

    // ��д GameObject ���麯��
    virtual void Update(float deltaTime) override;
    // virtual void Render() override; // Camera ͨ����ֱ����Ⱦ������
};

#endif //DX_CCAMERA_H
