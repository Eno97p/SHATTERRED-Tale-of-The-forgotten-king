#include "UIGroup_Character.h"

#include "GameInstance.h"

#include "UI_CharacterBG.h"
#include "UI_CharacterTop.h"
#include "UI_StateSoul.h"

CUIGroup_Character::CUIGroup_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Character::CUIGroup_Character(const CUIGroup_Character& rhs)
    : CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Character::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIGroup_Character::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUIGroup_Character::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Character::Tick(_float fTimeDelta)
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

void CUIGroup_Character::Late_Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Late_Tick(fTimeDelta);
    }
}

HRESULT CUIGroup_Character::Render()
{
    return S_OK;
}

HRESULT CUIGroup_Character::Create_UI()
{
    CUI::UI_DESC pDesc{};

    // Character BG 
    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_CharacterBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_CharacterBG"), &pDesc)));

    // Character Top 
    ZeroMemory(&pDesc, sizeof(pDesc));
    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_CharacterTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_CharacterTop"), &pDesc)));

    // Soul
    CUI_StateSoul::UI_SOUL_DESC pSoulDesc{};
    pSoulDesc.eLevel = LEVEL_STATIC;
    pSoulDesc.isSoulCntRend = false;
    pSoulDesc.isNextlevel = true;
    pSoulDesc.fX = 450.f;
    pSoulDesc.fY = 207.f;
    pSoulDesc.fSizeX = 24.f;
    pSoulDesc.fSizeY = 24.f;
    pSoulDesc.eUISort = NINETH;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pSoulDesc)));

    pSoulDesc.isSoulCntRend = true;
    pSoulDesc.isNextlevel = false;
    pSoulDesc.fX = 450.f;
    pSoulDesc.fY = 227.f;
    pSoulDesc.fSizeX = 24.f;
    pSoulDesc.fSizeY = 24.f;
    m_vecUI.emplace_back(dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pSoulDesc)));

    return S_OK;
}

CUIGroup_Character* CUIGroup_Character::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUIGroup_Character* pInstance = new CUIGroup_Character(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUIGroup_Character");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CUIGroup_Character::Clone(void* pArg)
{
    CUIGroup_Character* pInstance = new CUIGroup_Character(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIGroup_Character");
        return nullptr;
    }

    return pInstance;
}

void CUIGroup_Character::Free()
{
    __super::Free();

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);
}
