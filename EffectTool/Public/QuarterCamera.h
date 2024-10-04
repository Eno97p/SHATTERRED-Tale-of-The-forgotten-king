#pragma once

#include "Effect_Define.h"
#include "Camera.h"

BEGIN(Effect)

class CQuarterCamera final : public CCamera
{
public:
	typedef struct FCD : public CCamera::CAMERA_DESC
	{
		_float		fSensor = { 0.f };
	}FREE_CAMERA_DESC;
private:
	CQuarterCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuarterCamera(const CQuarterCamera& rhs);
	virtual ~CQuarterCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Target(class CGameObject* m_Target)
	{
		m_pTarget = m_Target;
		m_isTargetChanging = false;
	}
	void Change_Target(class CGameObject* m_Target);
	void Distance_Up(_float fTimeDelta)
	{
		fDistance += fTimeDelta;
	}

public:
	void Shaking();
	void Set_ShakeCamera(_float _fIntensity, _float _fShakeTime, _bool _bLockWidth)
	{
		m_fShakeAccTime = 0.f;
		m_fIntensity = _fIntensity;
		m_fShakeTime = _fShakeTime;
		m_bLockWidth = _bLockWidth;
	}

	void Key_Input(_float fTimeDelta);
	void Free_Key_Input(_float fTimeDelta);
	void Change_Camera();

private:
	_float		m_fSensor = { 0.0f };

private:
	class CGameObject* m_pTarget = nullptr;
	_float4 vEye;
	_float4 vDir;
	_float	fSpeed = 0.f;
	_float4 vAt;
	_float  fDistance = 0.f;
	_float	m_fHeight = 0.f;
	_float	m_fAngle = 0.f;
	_bool m_bIsMove = false;
	_bool m_bZoom = false;
private:
	_bool m_isTargetChanging = false;

private:
	_float	m_fIntensity;	//카메라 흔들림 강도
	_float	m_fShakeTime;	//진동 시간
	_float	m_fShakeAccTime;		//누적 시간
	_bool	m_bLockWidth;

private:
	_bool	m_FreeCamera = false;
	_bool	m_bStop = false;
 
public:
	static CQuarterCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END