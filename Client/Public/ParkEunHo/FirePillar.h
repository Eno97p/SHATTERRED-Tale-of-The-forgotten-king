#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "FirePillarEffect.h"
#include "FirePillar_Bottom.h"
#include "FirePillar_Charge.h"
#include "Rock_Ground.h"
BEGIN(Client)
class CFirePillar : public CGameObject
{
public:
	typedef struct FIREPILLAR
	{
		_float fLifeTime = 3.f;
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float3 vScale = { 0.7f,3.f,0.7f };
		_float Interval = 0.1f;
		CFirePillarEffect::FIREPILLAREFFECTDESC pillar1{};
		CFirePillarEffect::FIREPILLAREFFECTDESC pillar2{};
		CFirePillarEffect::FIREPILLAREFFECTDESC pillar4{};
		CFirePillar_Bottom::FIREPILLAR_BOTTOM	Bottom{};
		CFirePillar_Charge::FIREPILLAR_CHARGE	Charge{};
		CRock_Ground::ROCK_GROUND				Ground{};
	};

private:
	CFirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFirePillar(const CFirePillar& rhs);
	virtual ~CFirePillar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_Components();
	HRESULT Add_Child_Effects();

private:
	shared_ptr<FIREPILLAR> m_OwnDesc;

private:
	vector<CGameObject*>	m_ChildEffects;
	_bool					m_bEffectOn = false;

public:
	static CFirePillar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END