#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CAndrasLazerCylinder final : public CBlendObject
{
public:
	typedef struct ANDRAS_LASER_CYLINDER_DESC
	{
		_float3 vMaxSize = { 1.f,1.f,3.f };
		_float3 vOffset = { 0.f,1.f,1.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float fRotationSpeed = 360.f;
		_float fBloomPower = 0.f;
		_float fDistortionPower = 0.f;
		_float fMaxLifeTime = 0.f;
		_float fUVSpeed = 2.f;
		_int NumDesolve = 0;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CAndrasLazerCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrasLazerCylinder(const CAndrasLazerCylinder& rhs);
	virtual ~CAndrasLazerCylinder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Distortion() override;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	shared_ptr<ANDRAS_LASER_CYLINDER_DESC> m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

public:
	static CAndrasLazerCylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END