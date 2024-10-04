#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
#include "TornadoWind.h"
#include "Tornado_Root.h"
#include "Tornado_Ring.h"

BEGIN(Effect)
class CTornadoEffect : public CGameObject
{
public:
	typedef struct TORNADODESC
	{
		_float fSpeed = 0.f;
		_float fRotationSpeed = 0.f;
		_float fLifeTime = 0.f;
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float fRootInterval = 0.f;
		CGameObject* pTarget = nullptr;
		CTornado_Wind::TORNADO_WIND WindDesc{};
		CTornado_Wind::TORNADO_WIND WindDesc2{};
		CTornado_Wind::TORNADO_WIND WindDesc3{};
		CTornado_Root::TORNADO_ROOT RootDesc{};
		CTornado_Ring::TORNADO_RING RingDesc1{};
		CTornado_Ring::TORNADO_RING RingDesc2{};
		_int NumParticle = 0;
	};

private:
	CTornadoEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTornadoEffect(const CTornadoEffect& rhs);
	virtual ~CTornadoEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_Components();
	HRESULT Add_Child_Effects();
	void Generate_RootWind();

private:
	shared_ptr<TORNADODESC>			m_OwnDesc;
	vector<CGameObject*>			m_EffectClasses;
private:
	_float RootInterval = 0.f;

public:
	static CTornadoEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END