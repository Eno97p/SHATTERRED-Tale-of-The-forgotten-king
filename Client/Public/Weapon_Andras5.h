#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CWeapon_Andras5 final : public CWeapon
{
private:
	CWeapon_Andras5(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Andras5(const CWeapon_Andras5& rhs);
	virtual ~CWeapon_Andras5() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
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
	static CWeapon_Andras5* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END