#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_CharacterBG final : public CUI
{
public:
	enum ABILITY_TYPE { ABILITY_VITALITY, ABILITY_STAMINA, ABILITY_STRENGHT, ABILITY_MYSTICISM, ABILITY_KNOWLEDGE,
		ABILITY_HEALTH, ABILITY_STAMINA_STATE, ABILITY_ETHER, ABILITY_PHYSICALDMG, ABILITY_ETHERDMG, ABILITY_END };
private:
	CUI_CharacterBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CharacterBG(const CUI_CharacterBG& rhs);
	virtual ~CUI_CharacterBG() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Data();

private:
	// 출력 해야 하는 Text들 가지고 있기
	wstring			m_wstrLevel;
	
	wstring			m_wstrVitality;
	wstring			m_wstrStamina;
	wstring			m_wstrStrenght;
	wstring			m_wstrMysticism;
	wstring			m_wstrKnowledge;

	wstring			m_wstrHealth;
	wstring			m_wstrStamina_State;
	wstring			m_wstrEther;

	wstring			m_wstrPhysicalDmg;
	wstring			m_wstrEtheralDmg;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Render_Text();

	_vector Check_Difference(ABILITY_TYPE eAbilityType);

public:
	static CUI_CharacterBG*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END