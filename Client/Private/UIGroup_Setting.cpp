#include "UIGroup_Setting.h"

#include "GameInstance.h"

#include "UI_MenuPageBG.h"
#include "UI_Setting_Star.h"
#include "UI_Setting_Overlay.h"
#include "UI_Setting_Btn.h"
#include "UI_Setting_BackBtn.h"
#include "UI_Setting_Sound.h"

CUIGroup_Setting::CUIGroup_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Setting::CUIGroup_Setting(const CUIGroup_Setting& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Setting::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Setting::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Setting::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Setting::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend)
	{
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

		for (auto& pBtn : m_mapBtn)
		{
			if (!m_isRenderOnAnim && !(pBtn.second->Get_RenderOnAnim()))
			{
				pBtn.second->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pBtn.second->Get_RenderOnAnim())
			{
				pBtn.second->Resset_Animation(false);
			}

			pBtn.second->Tick(fTimeDelta);
		}
	}
}

void CUIGroup_Setting::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		for (auto& pBtn : m_mapBtn)
			pBtn.second->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Setting::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Setting::Create_UI()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	// BG
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuPageBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPageBG"), &pDesc)));

	// Star 
	m_vecUI.emplace_back(dynamic_cast<CUI_Setting_Star*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Setting_Star"), &pDesc)));

	// Overlay 
	m_vecUI.emplace_back(dynamic_cast<CUI_Setting_Overlay*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Setting_Overlay"), &pDesc)));

	if (FAILED(Create_Btn()))
		return E_FAIL;

	// BackBtn 
	m_vecUI.emplace_back(dynamic_cast<CUI_Setting_BackBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Setting_BackBtn"), &pDesc)));

	// Sound
	m_vecUI.emplace_back(dynamic_cast<CUI_Setting_Sound*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Setting_Sound"), &pDesc)));

	return S_OK;
}

HRESULT CUIGroup_Setting::Create_Btn()
{
	CUI_Setting_Btn::UI_SETTINGBTN_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	string strBtnKey[4] = {"test1", "test2", "test3", "apply"};

	for (size_t i = 0; i < 4; ++i)
	{
		pDesc.iBtnIdx = i;
		pDesc.fX = (g_iWinSizeX >> 1) + 100.f;
		pDesc.fY = 200.f + (i * 70.f);
		pDesc.fSizeX = 170.6f; // 512
		pDesc.fSizeY = 85.3f; // 256

		m_mapBtn.emplace(strBtnKey[i], dynamic_cast<CUI_Setting_Btn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Setting_Btn"), &pDesc)));
	}

	return S_OK;
}

CUIGroup_Setting* CUIGroup_Setting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Setting* pInstance = new CUIGroup_Setting(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Setting");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Setting::Clone(void* pArg)
{
	CUIGroup_Setting* pInstance = new CUIGroup_Setting(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Setting");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Setting::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	for (auto& pBtn : m_mapBtn)
		Safe_Release(pBtn.second);
}
