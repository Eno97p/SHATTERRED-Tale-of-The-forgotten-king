#pragma once

#include "Weapon.h"
#include "Client_Defines.h"

BEGIN(Client)

class CWeapon_Gun_LGGun final : public CWeapon
{
public:
	typedef struct GUN_DESC : public CWeapon::WEAPON_DESC
	{
		CLegionnaire_Gun* pParent = { nullptr };
	}GUN_DESC;

private:
	CWeapon_Gun_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Gun_LGGun(const CWeapon_Gun_LGGun& rhs);
	virtual ~CWeapon_Gun_LGGun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
	virtual void Set_Active(_bool isActive) override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	class CPlayer* m_pPlayer = { nullptr };
	CLegionnaire_Gun* m_pParent;

public:
	static CWeapon_Gun_LGGun*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END