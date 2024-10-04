#pragma once
#include "Camera.h"

BEGIN(Engine)

class CTransitionCamera final : public CCamera
{
public:
    // TRANSITIONCAMERA_DESC ����ü ����
    struct TRANSITIONCAMERA_DESC : public CCamera::CAMERA_DESC
    {
        _matrix matStartWorld;
        _matrix matEndWorld;
        _float fTransitionTime = 1.0f;
        // CCamera* pNextCamera = nullptr; // �ʿ��� ��� ���� ī�޶� ������ �߰�
    };

private:
    CTransitionCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTransitionCamera(const CTransitionCamera& rhs);
    virtual ~CTransitionCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_float fTimeDelta) override;

public:
    static CTransitionCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

private:
    _matrix m_matStartWorld;
    _matrix m_matEndWorld;
    _float m_fTransitionTime = 0.0f;
    _float m_fTotalTransitionTime = 1.0f;
    bool m_bTransitionFinished = false;
    // CCamera* m_pNextCamera = nullptr; // �ʿ��� ��� ���� ī�޶� ������ �߰�
};

END

