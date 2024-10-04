#pragma once

#include "Camera.h"
#include "MYMapTool_Defines.h"

BEGIN(MYMapTool)

class CDefault_Camera final : public CCamera
{
public:
	typedef struct DefaultCam_Desc : public CCamera::CAMERA_DESC
	{
		_float fSensor = {0.f};
	}DEFAULTCAM_DESC;

private:
	CDefault_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDefault_Camera(const CDefault_Camera& rhs);
	virtual ~CDefault_Camera() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	void			Key_Input(_float fTimeDelta);
	void			Mouse_Fix();
	void			Mouse_Move(_float fTimeDelta);

private:
	_bool			m_bFix = { false };
	_float			m_fSensor = {0.f};
	_float			m_fCamSpeed = 1.f;

public:
	static CDefault_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;
};

END