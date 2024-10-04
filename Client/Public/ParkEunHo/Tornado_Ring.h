#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CTornado_Ring final : public CBlendObject
{
public:
	typedef struct TORNADO_RING
	{
		_float3 vMaxSize = { 1.f,1.f,1.f };
		_float3 vOffset = { 0.f,0.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 fColor2 = { 1.f,1.f,1.f };
		_float fBloomPower = 0.f;
		_float fRotationSpeed = 360.f;
		_float fMaxLifeTime = 10.f;
		_float fUVSpeed = 1.f;
		_float RadicalStrength = 1.f;
		_float fGrowSpeed = 3.f;
		_bool Opacity = false;
		_bool IsDistortion = false;
		_float OpacityPower = 0.5f;
		_int NumDesolve = 0;
		_bool StartWithBody = false;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CTornado_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTornado_Ring(const CTornado_Ring& rhs);
	virtual ~CTornado_Ring() = default;

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
	HRESULT Bind_BloomResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	shared_ptr<TORNADO_RING>	m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;
	_float3						m_CurrentSize{};

private:
	_bool						EffectDead = false;

public:
	static CTornado_Ring* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END