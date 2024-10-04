#pragma once
#include "Effect_Define.h"
#include "Particle.h"
#include "PhysXParticle.h"
BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Effect)
class CParticle_PhysX : public CBlendObject
{
private:
	enum BoolValue { BOOL_ALPHA, BOOL_COLORMAP, BOOL_BLOOM, BOOL_DESOLVE, BOOL_END};
public:
	typedef struct PARTICLE_PHYSXDESC
	{
		EFFECTMODELTYPE						eModelType = EFFECTMODELTYPE::CUBE;
		CPhysXParticle::PhysX_Particle_Desc	PhysXDesc{};
		_float4								vStartPos = {0.f,0.f,0.f,1.f};
		_float3								vStartColor = {1.f,1.f,1.f};
		_float3								vEndColor = { 1.f,1.f,1.f };
		_float3								vBloomColor = { 1.f,1.f,1.f };
		_float3								vDesolveColor = { 1.f,1.f,1.f };
		_float								fDesolveLength = 0.f;
		_float								fBloomPower = 0.f;
		_int								NumDesolve = 0;
		_bool								bValues[BoolValue::BOOL_END] = {false,false,false,false};
	};
private:
	CParticle_PhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_PhysX(const CParticle_PhysX& rhs);
	virtual ~CParticle_PhysX() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CModel*			m_InstModelCom = { nullptr };
	CShader*		m_pShaderCom = nullptr;
	CTexture*		m_pDesolveTexture = { nullptr };

private:
	HRESULT Add_Components(const wstring& strModelPrototype);
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();
	const wchar_t* m_ModelPrototypeTag = nullptr;

private:
	unique_ptr<PARTICLE_PHYSXDESC> OwnDesc;

public:
	static CParticle_PhysX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END