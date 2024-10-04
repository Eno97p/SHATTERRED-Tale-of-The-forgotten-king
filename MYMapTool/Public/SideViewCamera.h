#pragma once

#include "MYMapTool_Defines.h"
#include "Camera.h"
#include "GameObject.h"

BEGIN(MYMapTool)

class CSideViewCamera final : public CCamera
{
public:
	enum class CAMERA_PHASE { CAMERA_FOLLOW = 0, CAMERA_REVOLUTION, CAMERA_RETURN, CAMERA_ZOOMIN, CAMERA_END };

public:
	typedef struct THIRDPERSONCAMERA_DESC : public CCamera::CAMERA_DESC
	{
		_float		fSensor = { 0.f };
		CTransform* pPlayerTrans = { nullptr };
	}SIDEVIEWCAMERA_DESC;
private:
	CSideViewCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSideViewCamera(const CSideViewCamera& rhs);
	virtual ~CSideViewCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(_float fTimeDelta) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual void Late_Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Get_SideViewCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition);
    void Update_SideViewCam(_float fTimeDelta);

    void Update_TransitionCam(_float fTimeDelta);

public:
    void Set_CameraHeight(_float fHeight) { m_fHeightOffset = fHeight; }
public:
    void Set_BossScene(bool bBossScene) { m_bBossScene = bBossScene; }

private:
    _float      m_fSensor = { 0.0f };
    CTransform* m_pPlayerTrans = { nullptr };

    _float4 m_vCameraPosition;
    _float4 m_vLookAtPosition;

    _float m_fDistance = 8.5f;        // 플레이어로부터의 거리
    _float m_fHeightOffset = 8.5f;    // 높이 오프셋
    _float m_fFixedZPosition = 12.5f;  // 고정된 Z 위치
    _float m_fLookAtOffset = 1.0f;    // 카메라가 바라보는 위치의 오프셋
    _float m_fFollowSpeed = 5.0f;     // 카메라 추적 속도

    bool m_bIsShaking = false;
    _float m_fShakeDuration = 0.5f;
    _float m_fShakeTimer = 0.0f;
    _float m_fShakeIntensity = 0.1f;
    _float m_fShakeInterval = 0.05f;
    _float m_fShakeIntervalTimer = 0.0f;
    _float4 m_vShakeStart = { 0.f, 0.f, 0.f, 1.f };
    _float4 m_vShakeTarget = { 0.f, 0.f, 0.f, 1.f };
private:
    bool m_bBossScene = false;
    _float m_fNormalHeightOffset = 3.5f;
    _float m_fNormalZPosition = -7.5f;
    _float m_fBossHeightOffset = 8.5f;
    _float m_fBossZPosition = 12.5f;
    _float4 m_vFixedLookAtPosition = { -422.f, 73.f, -5.f, 1.f };


public:
	static CSideViewCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
	void Shaking();
	void Key_Input(_float fTimeDelta);
};

END