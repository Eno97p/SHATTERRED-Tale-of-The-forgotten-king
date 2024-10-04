#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class Charge_Ribbon final : public CBlendObject
{
public:
	typedef struct LARGE_RIBBON
	{
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 vOffset = { 0.f,1.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 1.f;
		_float2 fThreadRatio = { 0.f,0.f };
		_float fSlowStrength = 0.5f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	Charge_Ribbon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Charge_Ribbon(const Charge_Ribbon& rhs);
	virtual ~Charge_Ribbon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	_bool Get_Effect_Dead() { return EffectDead; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	shared_ptr<LARGE_RIBBON>			m_OwnDesc;
	_float							m_fCurLifeTime = 0.f;
	_float							m_fLifeTimeRatio = 0.f;

private:
	_bool							EffectDead = false;

public:
	static Charge_Ribbon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END