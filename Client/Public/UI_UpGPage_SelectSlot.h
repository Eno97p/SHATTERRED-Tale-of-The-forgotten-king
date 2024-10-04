#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_UpGPage_SelectSlot final : public CUI
{
private:
	CUI_UpGPage_SelectSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpGPage_SelectSlot(const CUI_UpGPage_SelectSlot& rhs);
	virtual ~CUI_UpGPage_SelectSlot() = default;

public:
	void			Set_ItemName(wstring wstrItemName) { m_wstrItemName = wstrItemName; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Data(_uint iCurSlotIdx);

private:
	_uint								m_iLevel = { 0 };
	_uint								m_iAddDamage = { 0 };
	wstring								m_wstrItemName;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Rend_Font();
	
public:
	static CUI_UpGPage_SelectSlot*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END