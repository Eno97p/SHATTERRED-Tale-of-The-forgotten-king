#pragma once

#include "UI.h"
#include "ItemData.h"

BEGIN(Client)

class CUI_DropItemBG final : public CUI
{
public:
	typedef struct UI_DropItemBG_Desc : public UI_DESC
	{
		//_bool is >> 추후 분기처리 위한 데이터 추가해줄 것
		_bool					isItem;
		_bool					isWeapon;
		CItemData::ITEM_NAME	eItemName;
		wstring wstrTextureName;
	}UI_DROPITEM_DESC;

private:
	CUI_DropItemBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DropItemBG(const CUI_DropItemBG& rhs);
	virtual ~CUI_DropItemBG() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	wstring			m_wstrItemName;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_DropItemBG*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END