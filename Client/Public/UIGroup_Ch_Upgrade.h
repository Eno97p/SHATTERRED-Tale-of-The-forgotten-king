#pragma once

#include "UIGroup.h"

BEGIN(Client)

class CUIGroup_Ch_Upgrade final : public CUIGroup
{
public:
	typedef struct Origin_Data : public UIGROUP_DESC
	{
		_uint					iOriginLv = { 0 };
		_uint					iOriginVitalityLv = { 0 };
		_uint					iOriginStaminaLv = { 0 };
		_uint					iOriginStrenghtLv = { 0 };
		_uint					iOriginMysticismLv = { 0 };
		_uint					iOriginKnowledgeLv = { 0 };
		_uint					iOriginHealth = { 0 };
		_uint					iOriginStamina_State = { 0 };
		_uint					iOriginEther = { 0 };
		_uint					iOriginPhysicalDmg = { 0 };
		_uint					iOriginEtherDmg = { 0 };
	}ORIGIN_DATA;
private:
	CUIGroup_Ch_Upgrade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Ch_Upgrade(const CUIGroup_Ch_Upgrade& rhs);
	virtual ~CUIGroup_Ch_Upgrade() = default;

public:
	ORIGIN_DATA					Get_OriginData() { return m_tOriginData; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Resset_OriginData();

private:
	ORIGIN_DATA				m_tOriginData;

	vector<class CUI*>		m_vecUI;
	vector<class CUI*>		m_vecBtn;

private:
	HRESULT					Create_UI();
	HRESULT					Create_Btn();

public:
	static CUIGroup_Ch_Upgrade*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END