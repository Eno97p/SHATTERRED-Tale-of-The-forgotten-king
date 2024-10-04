#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MonsterHPBar final : public CUI
{
private:
	CUI_MonsterHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MonsterHPBar(const CUI_MonsterHPBar& rhs);
	virtual ~CUI_MonsterHPBar() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Pos(_vector vMonsterPos);

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_MonsterHPBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END