#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_UpGPage_MatSlot final : public CUI
{
public:
	enum MATSLOT_TYPE { MATSLOT_L, MATSLOT_R, MATSLOT_END };
	typedef struct UI_MatSlot_Desc : public UI_DESC
	{
		MATSLOT_TYPE	eMatSlotType;
	}UI_MATSLOT_DESC;
private:
	CUI_UpGPage_MatSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpGPage_MatSlot(const CUI_UpGPage_MatSlot& rhs);
	virtual ~CUI_UpGPage_MatSlot() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	MATSLOT_TYPE			m_eMatSlotType = { MATSLOT_END };

	class CUI_ItemIcon*		m_pItemIcon = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_ItemIcon();

public:
	static CUI_UpGPage_MatSlot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END