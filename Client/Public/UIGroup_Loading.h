#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI;

class CUIGroup_Loading final : public CUIGroup
{
private:
	CUIGroup_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Loading(const CUIGroup_Loading& rhs);
	virtual ~CUIGroup_Loading() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Setting_Data();

private:

	vector<CUI*>			m_vecUI;

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_Loading*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END