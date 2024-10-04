#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CJuggulus_Hammer final : public CWeapon
{
private:
	CJuggulus_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJuggulus_Hammer(const CJuggulus_Hammer& rhs);
	virtual ~CJuggulus_Hammer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_LightDepth() override;
	virtual void Set_Active(_bool isActive) override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float m_fHammerDelay = 4.f;
	_bool m_bSpawnEffect = false;

public:
	static CJuggulus_Hammer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END