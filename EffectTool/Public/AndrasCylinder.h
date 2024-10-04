#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CAndrasCylinder final : public CBlendObject
{
public:
	typedef struct ANDRASCYLINDER
	{
		_float3 vSize = { 1.5f,1.f,1.5f };
		_float3 vEndSized = { 0.f,1.f,0.f };
		_float3 vOffset = { 0.f,0.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 0.5f;
		_float fUVSpeed = 0.5f;
		_float2 fThreadRatio = { 0.7f,1.f };
		_float fSlowStrength = 0.1f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CAndrasCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrasCylinder(const CAndrasCylinder& rhs);
	virtual ~CAndrasCylinder() = default;

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
	shared_ptr<ANDRASCYLINDER>			m_OwnDesc;
	_float							m_fCurLifeTime = 0.f;
	_float							m_fLifeTimeRatio = 0.f;
	_float							m_UvTime = 0.f;

public:
	static CAndrasCylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END