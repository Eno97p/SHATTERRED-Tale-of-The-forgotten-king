#include "UIGroup_Upgrade.h"

#include "GameInstance.h"

#include "UI_UpgradeBG.h"
#include "UI_UpgradeForge.h"
#include "UI_UpgradeForge_Text.h"
#include "UI_UpgradeCrucible.h"

CUIGroup_Upgrade::CUIGroup_Upgrade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Upgrade::CUIGroup_Upgrade(const CUIGroup_Upgrade& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Upgrade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Upgrade::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Upgrade::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Upgrade::Tick(_float fTimeDelta)
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
	}
}

void CUIGroup_Upgrade::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Upgrade::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Upgrade::Create_UI()
{
	CUI::UI_DESC pDesc;

	pDesc.eLevel = LEVEL_STATIC;
	
	// Upgrade BG
	m_vecUI.emplace_back(dynamic_cast<CUI_UpgradeBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_UpgradeBG"), &pDesc)));

	// Upgrade Forge
	m_vecUI.emplace_back(dynamic_cast<CUI_UpgradeForge*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Upgrade_Forge"), &pDesc)));

	// Upgrade Forge Text
	m_vecUI.emplace_back(dynamic_cast<CUI_UpgradeForge_Text*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Upgrade_Forge_Text"), &pDesc)));

	// Upgrade Crucible
	m_vecUI.emplace_back(dynamic_cast<CUI_UpgradeCrucible*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Upgrade_Crucible"), &pDesc)));

	return S_OK;
}

CUIGroup_Upgrade* CUIGroup_Upgrade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Upgrade* pInstance = new CUIGroup_Upgrade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Upgrade");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Upgrade::Clone(void* pArg)
{
	CUIGroup_Upgrade* pInstance = new CUIGroup_Upgrade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Upgrade");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Upgrade::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
