#pragma once
#include "Camera.h"

BEGIN(Engine)

class CTransitionCamera final : public CCamera
{
public:
    // TRANSITIONCAMERA_DESC 구조체 정의
    struct TRANSITIONCAMERA_DESC : public CCamera::CAMERA_DESC
    {
        _uint iStartCam = CAM_FREE; // 전환 전 카메라 인덱스
        _uint iEndCam = CAM_END; // 전환 후 카메라 인덱스
        _float fTransitionTime = 1.0f; //몇 초동안 보간할거임


       //_matrix matStartWorld;
       //_matrix matEndWorld;
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

    _uint m_iStartCam, m_iEndCam = 0;
    // CCamera* m_pNextCamera = nullptr; // 필요한 경우 다음 카메라 포인터 추가
};

END

