#include "UIGroup_Map.h"

#include "GameInstance.h"

#include "UI_MapBG.h"
#include "UI_MapArea.h"
#include "UI_MapDetail.h"
#include "UI_MapUser.h"
#include "UI_MapPosIcon.h"

CUIGroup_Map::CUIGroup_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Map::CUIGroup_Map(const CUIGroup_Map& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Map::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Map::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Map::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Map::Tick(_float fTimeDelta)
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

void CUIGroup_Map::Late_Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Late_Tick(fTimeDelta);
    }
}

HRESULT CUIGroup_Map::Render()
{
    return S_OK;
}

HRESULT CUIGroup_Map::Create_UI()
{
    CUI::UI_DESC pDesc{};

    // BG 
    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_MapBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MapBG"), &pDesc)));

    // Area   
    m_vecUI.emplace_back(dynamic_cast<CUI_MapArea*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MapArea"), &pDesc)));

    // Detail
    CUI_MapDetail::UI_MAPDETAIL_DESC pDetailDesc{};
    pDetailDesc.eLevel = LEVEL_STATIC;
    for (size_t i = 0; i < 5; ++i)
    {
        pDetailDesc.iIdx = i;
        m_vecUI.emplace_back(dynamic_cast<CUI_MapDetail*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MapDetail"), &pDetailDesc)));
    }

    // User 
    m_vecUI.emplace_back(dynamic_cast<CUI_MapUser*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MapUser"), &pDetailDesc)));

    // MapPosIcon 
    m_vecUI.emplace_back(dynamic_cast<CUI_MapPosIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MapPosIcon"), &pDesc)));

    return S_OK;
}

CUIGroup_Map* CUIGroup_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUIGroup_Map* pInstance = new CUIGroup_Map(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUIGroup_Map");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CUIGroup_Map::Clone(void* pArg)
{
    CUIGroup_Map* pInstance = new CUIGroup_Map(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIGroup_Map");
        return nullptr;
    }

    return pInstance;
}

void CUIGroup_Map::Free()
{
    __super::Free();

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);
}
