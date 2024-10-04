#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
#include "Hedgehog.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CNeedleSpawner final : public CBlendObject
{
public:
	typedef struct NEEDLESPAWNER
	{
		_float3 vMinSize = { 1.f,1.f,1.f };
		_float3 vMaxSize = { 1.f,1.f,1.f };
		_float3 fColor = { 1.f,1.f,1.f };
		_float3 BloomColor = { 1.f,1.f,1.f };
		_float fBloomPower = 1.f;
		_float fMaxLifeTime = 0.5f;
		_float2 fThreadRatio = { 0.f,0.2f };
		_float fSlowStrength = 0.1f;
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float SpawnTiming = 0.5f;
		CHedgehog::HEDGEHOG ChildDesc{};
	};

private:
	CNeedleSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNeedleSpawner(const CNeedleSpawner& rhs);
	virtual ~CNeedleSpawner() = default;

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
	shared_ptr<NEEDLESPAWNER>			m_OwnDesc;
	_float							m_fCurLifeTime = 0.f;
	_float							m_fLifeTimeRatio = 0.f;

private:
	_bool							Spawned = false;

public:
	static CNeedleSpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END