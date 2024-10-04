#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
#include "Heal_Ribbon.h"
#include "Charge_Vane.h"

BEGIN(Effect)
class CHammerSpawn final : public CGameObject
{
public:
	typedef struct HAMMERSPAWN
	{
		_float fLifeTime = 0.f;
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		CHeal_Ribbon::HEAL_RIBBON RibbonDesc{};
		Charge_Vane::VANE_DESC VaneDesc{};
	};

private:
	CHammerSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHammerSpawn(const CHammerSpawn& rhs);
	virtual ~CHammerSpawn() = default;
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
	shared_ptr<HAMMERSPAWN>			m_OwnDesc;
	vector<CGameObject*>			m_EffectClasses;

public:
	static CHammerSpawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END