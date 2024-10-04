#pragma once

#include "Weapon.h"
#include "Client_Defines.h"

BEGIN(Client)

class CWeapon_Sword_LGGun final : public CWeapon
{
private:
	CWeapon_Sword_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Sword_LGGun(const CWeapon_Sword_LGGun& rhs);
	virtual ~CWeapon_Sword_LGGun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
	_bool Get_IsParried() { return m_bIsParried; }
	void Set_IsParried(_bool isParried) { m_bIsParried = isParried; }
	virtual void Set_Active(_bool isActive) override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	class CPlayer* m_pPlayer = { nullptr };
	_bool m_bIsParried = false;

public:
	static CWeapon_Sword_LGGun*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END