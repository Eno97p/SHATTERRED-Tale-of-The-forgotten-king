#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MapIcon final : public CUI
{
public:
	typedef struct UI_MapIcon_Desc : public UI_DESC
	{
		_uint		iIndex;
	}UI_MAPICON_DESC;

private:
	CUI_MapIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MapIcon(const CUI_MapIcon& rhs);
	virtual ~CUI_MapIcon() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Pos(_float fX, _float fY);

private:
	_uint			m_iIndex = { 0 };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_MapIcon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END