#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CShieldSphere final : public CBlendObject
{
public:
	typedef struct SHIELD_DESC
	{
		_float3 vMaxSize = { 1.5f,1.5f,1.5f };
		_float3 vOffset = { 0.f,1.f,0.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 fBloomColor = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 0.2f;
		_float fUVSpeed = 1.f;
		const _float4x4* ParentMatrix = nullptr;
	};

private:
	CShieldSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShieldSphere(const CShieldSphere& rhs);
	virtual ~CShieldSphere() = default;

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

private:
	shared_ptr<SHIELD_DESC> m_OwnDesc;

private:
	_float				m_fCurLifeTime = 0.f;
	_float				m_fRatio = 0.f;

public:
	static CShieldSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END