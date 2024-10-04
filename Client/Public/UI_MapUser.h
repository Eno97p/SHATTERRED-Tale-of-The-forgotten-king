#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MapUser final : public CUI
{
private:
	CUI_MapUser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MapUser(const CUI_MapUser& rhs);
	virtual ~CUI_MapUser() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CUI_MapIcon*		m_pPlayerIcon = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Create_PlayerIcon();

	void	Update_Pos();

public:
	static CUI_MapUser*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END