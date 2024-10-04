#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

#include "ItemData.h"
#include "Item.h"

BEGIN(Client)
class CUI;

class CUIGroup_DropItem final : public CUIGroup
{
public:
	typedef struct UIGroup_DropItem_Desc : public UIGROUP_DESC
	{
		_bool					isRend = { true };
		_bool					isItem;
		_bool					isWeapon;
		CItemData::ITEM_NAME	eItemName;
		wstring					wszTextureName;
	}UIGROUP_DROPITEM_DESC;

private:
	CUIGroup_DropItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_DropItem(const CUIGroup_DropItem& rhs);
	virtual ~CUIGroup_DropItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float					m_fRenderTimer = { 0.f };

	vector<CUI*>			m_vecUI;

private:
	HRESULT					Create_UI(CItemData::ITEM_NAME eItemName, wstring wstrTextureName, _bool isItem, _bool isWeapon);

public:
	static CUIGroup_DropItem*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END