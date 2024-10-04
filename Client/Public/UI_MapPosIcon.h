#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MapPosIcon final : public CUI
{
	const _float ANIM_Y = 10.f;
private:
	CUI_MapPosIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MapPosIcon(const CUI_MapPosIcon& rhs);
	virtual ~CUI_MapPosIcon() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isUpAnim = { false };

	_float			m_fDefaultY = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	UpAndDown_Anim(_float fTimeDelta);
	void	Setting_AnimData();

public:
	static CUI_MapPosIcon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END