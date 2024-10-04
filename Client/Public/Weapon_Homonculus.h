#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CWeapon_Homonculus final : public CWeapon
{
private:
	CWeapon_Homonculus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Homonculus(const CWeapon_Homonculus& rhs);
	virtual ~CWeapon_Homonculus() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	_bool Get_IsParried() { return m_bIsParried; }
	void Set_IsParried(_bool isParried) { m_bIsParried = isParried; }
	virtual void Set_Active(_bool isActive) override;

public:
	HRESULT Add_Components();

private:
	class CPlayer* m_pPlayer = { nullptr };
	_bool m_bIsParried = false;

public:
	static CWeapon_Homonculus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END