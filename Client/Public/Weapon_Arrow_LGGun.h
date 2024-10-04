#pragma once

#include "Weapon.h"
#include "Client_Defines.h"

BEGIN(Client)

class CWeapon_Arrow_LGGun final : public CWeapon
{
public:
	typedef struct ARROW_DESC : public WEAPON_DESC
	{
		_vector vDir;
		_vector vPos;
		class CLegionnaire_Gun* pParent = { nullptr };
	}ARROW_DESC;

private:
	CWeapon_Arrow_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Arrow_LGGun(const CWeapon_Arrow_LGGun& rhs);
	virtual ~CWeapon_Arrow_LGGun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
	_bool Get_IsParried() { return m_bIsParried; }

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	class CPlayer* m_pPlayer = { nullptr };
	_bool m_bIsParried = false;
	_float fAccParticle = 0.f;
	_float fMaxLifeTime = 3.f;
	class CLegionnaire_Gun* m_pParent = { nullptr };

public:
	static CWeapon_Arrow_LGGun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

END