#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CGroundSlash final : public CBlendObject
{
public:
	typedef struct GROUNDSLASH
	{
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float4 vDirection = { 0.f,0.f,0.f,1.f };
		_float3 vStartSize = { 0.5f,0.5f,0.5f };
		_float3 vEndSize = { 1.f,1.f,1.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 5.f;
		_float fThreadRatio = 0.2f;
		_float fSlowStrength = 0.1f;
		_float fSpeed = 0.f;
	};

private:
	CGroundSlash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGroundSlash(const CGroundSlash& rhs);
	virtual ~CGroundSlash() = default;

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
	HRESULT Add_ChildEffect();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

private:
	shared_ptr<GROUNDSLASH>			m_OwnDesc;
	vector<CGameObject*>			ChildEffects;
	_float							m_fCurLifeTime = 0.f;
	_float							m_fLifeTimeRatio = 0.f;
	const _float4x4*				m_OwnMat = nullptr;
	_bool							m_ParticleSpawn = false;


public:
	static CGroundSlash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END