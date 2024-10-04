#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CNewAspiration final : public CBlendObject
{
public:
	typedef struct NEWASPIRATION
	{
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 vOffset = { 0.f,1.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 0.f,1.f,0.f };
		_float fBloomPower = 0.f;
		_float fMaxLifeTime = 1.f;
		_float2 fThreadRatio = { 0.f,0.f };
		_float fSlowStrength = 0.5f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CNewAspiration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNewAspiration(const CNewAspiration& rhs);
	virtual ~CNewAspiration() = default;

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
	shared_ptr<NEWASPIRATION>		m_OwnDesc;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fLifeTimeRatio = 0.f;


public:
	static CNewAspiration* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END