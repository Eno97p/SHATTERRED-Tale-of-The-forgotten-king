#pragma once

#include "Npc.h"

BEGIN(Client)
class CBody_Valnir;

class CNpc_Valnir final : public CNpc
{
private:
	CNpc_Valnir(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc_Valnir(const CNpc_Valnir& rhs);
	virtual ~CNpc_Valnir() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool					m_isScriptOn = { false };
	_uint					m_iDialogCnt = { 0 };

	CBody_Valnir*			m_pBody = { nullptr };

	class CUIGroup_Shop*	m_pShopUI = { nullptr };

private:
	HRESULT				Add_PartObjects();

	virtual HRESULT		Create_Script() override;
	HRESULT				Create_Shop();

	void				Set_DialogText();

public:
	static CNpc_Valnir*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END