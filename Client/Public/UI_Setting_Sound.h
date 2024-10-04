#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

// 얘 버튼 스타일로 바꾸자
class CUI_Setting_Sound final : public CUI_Interaction
{
	const _float fRANGE = 270.f;
private:
	CUI_Setting_Sound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Setting_Sound(const CUI_Setting_Sound& rhs);
	virtual ~CUI_Setting_Sound() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CUI_Setting_SoundBar*		m_pBar = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Bar();
	void	Update_Position();

public:
	static CUI_Setting_Sound*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END