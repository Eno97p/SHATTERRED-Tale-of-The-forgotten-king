#include "UIGroup_DropItem.h"

#include "GameInstance.h"
#include "UI_DropItemBG.h"
#include "UI_ItemIcon.h"

CUIGroup_DropItem::CUIGroup_DropItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIGroup{ pDevice, pContext }
{
}

CUIGroup_DropItem::CUIGroup_DropItem(const CUIGroup_DropItem& rhs)
    : CUIGroup{ rhs }
{
}

HRESULT CUIGroup_DropItem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIGroup_DropItem::Initialize(void* pArg)
{
    UIGROUP_DROPITEM_DESC* pDesc = static_cast<UIGROUP_DROPITEM_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (pDesc->isRend)
    {
        if (FAILED(Create_UI(pDesc->eItemName, pDesc->wszTextureName, pDesc->isItem, pDesc->isWeapon)))
            return E_FAIL;
    }

    return S_OK;
}

void CUIGroup_DropItem::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_DropItem::Tick(_float fTimeDelta)
{
    //_bool isRender_End = false;
    
    for (auto& pUI : m_vecUI)
    {
        pUI->Tick(fTimeDelta);
    }

    m_fRenderTimer += fTimeDelta;
    if (3.f <= m_fRenderTimer)
        m_pGameInstance->Erase(this);
}

void CUIGroup_DropItem::Late_Tick(_float fTimeDelta)
{
    for (auto& pUI : m_vecUI)
        pUI->Late_Tick(fTimeDelta);
}

HRESULT CUIGroup_DropItem::Render()
{
    return S_OK;
}

HRESULT CUIGroup_DropItem::Create_UI(CItemData::ITEM_NAME eItemName, wstring wstrTextureName, _bool isItem, _bool isWeapon)
{
    //CUI::UI_DESC pDesc{};
    CUI_DropItemBG::UI_DROPITEM_DESC pDesc{};

    pDesc.eLevel = LEVEL_STATIC;
    pDesc.eItemName = eItemName;
    pDesc.isItem = isItem;
    pDesc.isWeapon = isWeapon;
    // BG
    m_vecUI.emplace_back(dynamic_cast<CUI_DropItemBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_DropItemBG"), &pDesc)));

    // Icon
    CUI_ItemIcon::UI_ITEMICON_DESC pUIDesc{};
    pUIDesc.eLevel = LEVEL_STATIC;
    pUIDesc.fX = 73.f;
    pUIDesc.fY = (g_iWinSizeY >> 1) + 70.f;
    pUIDesc.fSizeX = 64.f;
    pUIDesc.fSizeY= 64.f;
    pUIDesc.wszTexture = wstrTextureName;
    m_vecUI.emplace_back(dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pUIDesc)));

    return S_OK;
}

CUIGroup_DropItem* CUIGroup_DropItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUIGroup_DropItem* pInstance = new CUIGroup_DropItem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUIGroup_DropItem");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CUIGroup_DropItem::Clone(void* pArg)
{
    CUIGroup_DropItem* pInstance = new CUIGroup_DropItem(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIGroup_DropItem");
        return nullptr;
    }

    return pInstance;
}

void CUIGroup_DropItem::Free()
{
    __super::Free();

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);
}
