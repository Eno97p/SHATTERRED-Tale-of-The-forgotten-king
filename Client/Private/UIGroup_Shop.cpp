#include "UIGroup_Shop.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_ShopBG.h"
#include "UI_Shop_SoulBG.h"
#include "UI_ShopSelect.h"

CUIGroup_Shop::CUIGroup_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Shop::CUIGroup_Shop(const CUIGroup_Shop& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Shop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Shop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Shop::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Shop::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend)
	{
		CUI_Manager::GetInstance()->Set_isShopOn(true);

		for (auto& pUI : m_vecUI)
		{
			if (!m_isRenderOnAnim && !(pUI->Get_RenderOnAnim()))
			{
				pUI->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pUI->Get_RenderOnAnim())
			{
				pUI->Resset_Animation(false);
			}

			pUI->Tick(fTimeDelta);

			isRender_End = pUI->isRender_End();
		}
		if (isRender_End)
			m_isRend = false;

		for (auto& pSlot : m_vecSlot)
		{
			if (!m_isRenderOnAnim && !(pSlot->Get_RenderOnAnim()))
			{
				pSlot->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pSlot->Get_RenderOnAnim())
			{
				pSlot->Resset_Animation(false);
			}

			pSlot->Tick(fTimeDelta);
		}
	}
	else
	{
		CUI_Manager::GetInstance()->Set_isShopOn(false);
	}

	m_iSelectIdx = Check_SelectIdx();
}

void CUIGroup_Shop::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		for (auto& pSlot : m_vecSlot)
			pSlot->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Shop::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Shop::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	// BG
	m_vecUI.emplace_back(dynamic_cast<CUI_ShopBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ShopBG"), &pDesc)));

	// SoulBG   
	m_vecUI.emplace_back(dynamic_cast<CUI_Shop_SoulBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Shop_SoulBG"), &pDesc)));

	// Slot
	if (FAILED(Create_Slot()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIGroup_Shop::Create_Slot()
{
	CUI_ShopSelect::UI_SHOPSELECT_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = 380.f;
	pDesc.fSizeX = 700.f;
	pDesc.fSizeY = 140.f;

	for (size_t i = 0; i < 3; ++i)
	{
		pDesc.iSlotIdx = i;
		pDesc.fY = 180.f + (i * 75);
		m_vecSlot.emplace_back(dynamic_cast<CUI_ShopSelect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ShopSelect"), &pDesc)));
	}

	return S_OK;
}

_int CUIGroup_Shop::Check_SelectIdx()
{
	// 몇 번째 select와 충돌 중인지 (충돌중이 아니라면 -1)
	vector< CUI_ShopSelect*>::iterator select = m_vecSlot.begin();
	for (size_t i = 0; i < m_vecSlot.size(); ++i)
	{
		if ((*select)->Get_Select()) // 충돌했다면
		{
			return i;
		}
		else
		{
			++select;
		}
	}

	return -1;
}

CUIGroup_Shop* CUIGroup_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Shop* pInstance = new CUIGroup_Shop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Shop");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Shop::Clone(void* pArg)
{
	CUIGroup_Shop* pInstance = new CUIGroup_Shop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Shop");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Shop::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	for (auto& pSlot : m_vecSlot)
		Safe_Release(pSlot);
}
