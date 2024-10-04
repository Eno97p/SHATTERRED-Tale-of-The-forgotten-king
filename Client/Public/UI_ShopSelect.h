#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_ShopSelect final : public CUI_Interaction
{
public:
	typedef struct UI_ShopSelect_Desc : public UI_DESC
	{
		_uint		iSlotIdx;
	}UI_SHOPSELECT_DESC;

private:
	CUI_ShopSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShopSelect(const CUI_ShopSelect& rhs);
	virtual ~CUI_ShopSelect() = default;



public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_uint					m_iSlotIdx = { 0 };
	_int					m_iPrice = { 0 };
	_uint					m_iRemainCnt = { 0 }; // 남은 개수
	wstring					m_wstrItemName;
	_vector					m_vColor;

	vector<class CUI*>		m_vecUI;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_UI();

	void	Rend_Font();

	void	Sell_Item();

public:
	static CUI_ShopSelect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END