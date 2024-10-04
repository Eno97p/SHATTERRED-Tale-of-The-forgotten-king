#include "UIGroup_State.h"

#include "GameInstance.h"

#include "UI_StateBG.h"
#include "UI_StateBar.h"
#include "UI_StateHP.h"
#include "UI_StateEnergy.h"
#include "UI_StateEther.h"
#include "UI_StateSoul.h"

CUIGroup_State::CUIGroup_State(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIGroup{pDevice, pContext}
{
}

CUIGroup_State::CUIGroup_State(const CUIGroup_State& rhs)
    : CUIGroup{rhs}
{
}

HRESULT CUIGroup_State::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIGroup_State::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUIGroup_State::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_State::Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Tick(fTimeDelta);

        for (auto& pState : m_vecStates)
            pState->Tick(fTimeDelta);

        m_pSoul->Tick(fTimeDelta);
    }
}

void CUIGroup_State::Late_Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Late_Tick(fTimeDelta);

        for (auto& pState : m_vecStates)
            pState->Late_Tick(fTimeDelta);

        m_pSoul->Late_Tick(fTimeDelta);
    }
}

HRESULT CUIGroup_State::Render()
{
    return S_OK;
}

void CUIGroup_State::Resset_Player()
{
    vector<CUI*>::iterator state = m_vecStates.begin();

    dynamic_cast<CUI_StateHP*>(*state)->Resset_Player();
    ++state;
    dynamic_cast<CUI_StateEnergy*>(*state)->Resset_Player();
    ++state;
    dynamic_cast<CUI_StateEther*>(*state)->Resset_Player();
}

void CUIGroup_State::Rend_Calcul(_int iSoul)
{
    m_pSoul->Rend_Calcul(iSoul);
}

HRESULT CUIGroup_State::Create_UI()
{
    // State BG
    CUI::UI_DESC pDesc;

    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateBG"), &pDesc)));

#pragma region Bar

    CUI_StateBar::UI_STATEBAR_DESC pBarDesc{};

    // Hp
    pBarDesc.eLevel = LEVEL_STATIC;
    pBarDesc.eBarType = CUI_StateBar::BAR_HP;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateBar*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateBar"), &pBarDesc)));

    // Energy
    ZeroMemory(&pBarDesc, sizeof(pBarDesc));
    pBarDesc.eLevel = LEVEL_STATIC;
    pBarDesc.eBarType = CUI_StateBar::BAR_ENERGY;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateBar*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateBar"), &pBarDesc)));

    // Ether
    ZeroMemory(&pBarDesc, sizeof(pBarDesc));
    pBarDesc.eLevel = LEVEL_STATIC;
    pBarDesc.eBarType = CUI_StateBar::BAR_ETHER;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateBar*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateBar"), &pBarDesc)));

#pragma endregion Bar

    // HP
    ZeroMemory(&pDesc, sizeof(pDesc));
    pDesc.eLevel = LEVEL_STATIC;
    m_vecStates.emplace_back(dynamic_cast<CUI_StateHP*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateHP"), &pDesc)));

    // Energy 
    ZeroMemory(&pDesc, sizeof(pDesc));
    pDesc.eLevel = LEVEL_STATIC;
    m_vecStates.emplace_back(dynamic_cast<CUI_StateEnergy*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateEnergy"), &pDesc)));
    
    // Ether 
    ZeroMemory(&pDesc, sizeof(pDesc));
    pDesc.eLevel = LEVEL_STATIC;
    m_vecStates.emplace_back(dynamic_cast<CUI_StateEther*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateEther"), &pDesc)));

    // Soul 
    CUI_StateSoul::UI_SOUL_DESC pSoulDesc{};
    pSoulDesc.eLevel = LEVEL_STATIC;
    pSoulDesc.fX = 200.f;
    pSoulDesc.fY = 100.f;
    pSoulDesc.fSizeX = 32.f;
    pSoulDesc.fSizeY = 32.f;
    pSoulDesc.eUISort = SECOND;
    m_pSoul = dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pSoulDesc));

    return S_OK;
}

CUIGroup_State* CUIGroup_State::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUIGroup_State* pInstance = new CUIGroup_State(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUIGroup_State");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CUIGroup_State::Clone(void* pArg)
{
    CUIGroup_State* pInstance = new CUIGroup_State(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIGroup_State");
        return nullptr;
    }

    return pInstance;
}

void CUIGroup_State::Free()
{
    __super::Free();

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);

    for (auto& pState : m_vecStates)
        Safe_Release(pState);

    Safe_Release(m_pSoul);
}
