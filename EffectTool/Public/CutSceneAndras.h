#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
#include "DefaultCylinder.h"
#include "AndrasCylinder.h"
#include "AndrasSphere.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)
class CutSceneAndras : public CGameObject
{
public:
	typedef struct CUTSCENEANDRAS
	{
		_float fLifeTime = 15.f;
		_float2 EyeChangeBetwin = { 3.f, 5.f };
		_float4 vPosition = { 0.f,0.f,0.f,1.f };
		_float3 vColor = { 1.f,1.f,1.f };
		_float3 vBloomColor = { 1.f,0.f,0.f };
		_float	fStartEffect = 8.f;
		CDefaultCylinder::DEFAULTCYLINDER Cylinder{};
		CAndrasCylinder::ANDRASCYLINDER AndrasPillar{};
		CAndrasSphere::ANDRASSPHERE SphereDesc{};
	};

private:
	CutSceneAndras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CutSceneAndras(const CutSceneAndras& rhs);
	virtual ~CutSceneAndras() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	unique_ptr<CUTSCENEANDRAS> m_OwnDesc;
	_float m_AccTime = 0.f;
	_float m_EyechangeRatio = 0.f;
	_bool	m_EyeChanged = false;
	_bool	m_EffectSpawned = false;
private:
	CGameObject* Head = nullptr;
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BloomResources();
	HRESULT Add_Child_Effects();
public:
	static CutSceneAndras* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END