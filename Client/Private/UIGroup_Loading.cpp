#include "UIGroup_Loading.h"

#include "GameInstance.h"

#include "UI_LoadingBG.h"
#include "UI_LoadingCircle.h"

CUIGroup_Loading::CUIGroup_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Loading::CUIGroup_Loading(const CUIGroup_Loading& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Loading::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Loading::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Loading::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Loading::Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			pUI->Tick(fTimeDelta);
		}
	}
}

void CUIGroup_Loading::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			pUI->Late_Tick(fTimeDelta);
		}
	}
}

HRESULT CUIGroup_Loading::Render()
{
	return S_OK;
}

void CUIGroup_Loading::Setting_Data()
{
	vector<CUI*>::iterator circle = m_vecUI.begin();
	++circle;
	++circle;

	dynamic_cast<CUI_LoadingCircle*>(*circle)->Setting_Data();
}

HRESULT CUIGroup_Loading::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	// LoadingBG 
	m_vecUI.emplace_back(dynamic_cast<CUI_LoadingBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_LoadingBG"), &pDesc)));

	// LoadingCircle 1 

	CUI_LoadingCircle::UI_CIRCLE_DESC pCircleDesc{};
	pCircleDesc.eLevel = LEVEL_STATIC;
	pCircleDesc.fX = g_iWinSizeX >> 1;
	pCircleDesc.fY = g_iWinSizeY >> 1;
	pCircleDesc.fSizeX = 512.f;
	pCircleDesc.fSizeY = 512.f;
	pCircleDesc.eCircleType = CUI_LoadingCircle::CIRCLE_ONE;
	m_vecUI.emplace_back(dynamic_cast<CUI_LoadingCircle*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_LoadingCircle"), &pCircleDesc)));

	// LoadingCircle 2
	pCircleDesc.eCircleType = CUI_LoadingCircle::CIRCLE_TWO;
	m_vecUI.emplace_back(dynamic_cast<CUI_LoadingCircle*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_LoadingCircle"), &pCircleDesc)));


	return S_OK;
}

CUIGroup_Loading* CUIGroup_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Loading* pInstance = new CUIGroup_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Loading");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Loading::Clone(void* pArg)
{
	CUIGroup_Loading* pInstance = new CUIGroup_Loading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Loading");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Loading::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
