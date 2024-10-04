#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CAndrasSphere final : public CBlendObject
{
public:
	typedef struct ANDRASSPHERE
	{
		_float3 vSize = { 1.f,1.f,1.f };
		_float3 vEndSized = { 10.f,10.f,10.f };
		_float3 vOffset = { 0.f,0.f,0.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 0.5f;
		_float fUVSpeed = 0.5f;
		_float OppacityPower = 0.5f;
		_float2 fThreadRatio = { 0.7f,1.f };
		_float fSlowStrength = 0.1f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CAndrasSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrasSphere(const CAndrasSphere& rhs);
	virtual ~CAndrasSphere() = default;

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
	shared_ptr<ANDRASSPHERE>			m_OwnDesc;
	_float							m_fCurLifeTime = 0.f;
	_float							m_fLifeTimeRatio = 0.f;
	_float							m_UvTime = 0.f;

public:
	static CAndrasSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END