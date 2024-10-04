#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_WeaponSlotBG final : public CUI
{
private:
	CUI_WeaponSlotBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_WeaponSlotBG(const CUI_WeaponSlotBG& rhs);
	virtual ~CUI_WeaponSlotBG() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void	Create_UI(_bool isSkill);

private:
	class CUI_HUDEffect*		m_pEffect = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_WeaponSlotBG*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END