#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_StateEther final : public CUI
{
	const _float ORIGIN_SIZEX = 768.f;
	const _float ORIGIN_X = 390.f;
private:
	CUI_StateEther(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StateEther(const CUI_StateEther& rhs);
	virtual ~CUI_StateEther() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;
	void Set_Ratio(_float fRatio) { m_fCurrentRatio = fRatio; }

	void			Resset_Player();

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float m_fCurrentRatio = 1.f;
	_float m_fPastRatio = 1.f;
	_float m_fHudRatio = 0.8f;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_StateEther*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END