#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
#include "BlackSphere.h"
#include "BlackHole_Ring.h"
#include "BlackHorizon.h"
#include "BlackHole_Ring_Bill.h"
BEGIN(Effect)
class CBlackHole : public CGameObject
{
public:
	typedef struct BLACKHOLE
	{
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float	vStartDelay = 0.2f;
		CBlackSphere::BLACKSPHERE SphereDesc{};
		CBlackHole_Ring::BLACKHOLE_RING RingDesc{};
		CBlackHole_Ring_Bill::BLACKHOLE_RING_BILL RingBill{};
		CBlackHorizon::HORIZON_DESC HorizonDesc{};
	};

private:
	CBlackHole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlackHole(const CBlackHole& rhs);
	virtual ~CBlackHole() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	void Set_Delete_BlackHole();

private:
	HRESULT Add_ChildEffects();

private:
	unique_ptr<BLACKHOLE>	m_OwnDesc;
	vector<CGameObject*>		m_ChildEffects;
	_bool					m_Delete = false;

private:
	_float					fLifeTime = 0.f;

public:
	static CBlackHole* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END