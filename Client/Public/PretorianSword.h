#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CPretorianSword final : public CWeapon
{
private:
	CPretorianSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPretorianSword(const CPretorianSword& rhs);
	virtual ~CPretorianSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection() override;
	virtual HRESULT Render_LightDepth() override;
	virtual HRESULT Render_Bloom() override;
	virtual CCollider* Get_Collider();
	virtual void Set_Active(_bool isActive) override;

private:
	CCollider* m_pSpecialColliderCom = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPretorianSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END