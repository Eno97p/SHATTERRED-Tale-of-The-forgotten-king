#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CHeal_Spiral final : public CBlendObject
{
public:
	typedef struct HEAL_SPIRAL
	{
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 0.f,1.f,0.f };
		_float fBloomPower = 0.f;
		_float fMaxLifeTime = 1.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CHeal_Spiral(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHeal_Spiral(const CHeal_Spiral& rhs);
	virtual ~CHeal_Spiral() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	shared_ptr<HEAL_SPIRAL>		m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;

private:
	_bool						EffectDead = false;

public:
	static CHeal_Spiral* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END