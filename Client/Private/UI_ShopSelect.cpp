#include "UI_ShopSelect.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "CMouse.h"

#include "UI_ItemIcon.h"
#include "UI_StateSoul.h"
#include "UI_Shop_RemainIcon.h"

CUI_ShopSelect::CUI_ShopSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Interaction{ pDevice, pContext }
{
}

CUI_ShopSelect::CUI_ShopSelect(const CUI_ShopSelect& rhs)
    : CUI_Interaction{ rhs }
{
}

HRESULT CUI_ShopSelect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_ShopSelect::Initialize(void* pArg)
{
    UI_SHOPSELECT_DESC* pDesc = static_cast<UI_SHOPSELECT_DESC*>(pArg);

    m_iSlotIdx = pDesc->iSlotIdx;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Setting_Position();

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUI_ShopSelect::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ShopSelect::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);

    m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.3f),
                        LONG(m_fY - m_fSizeY * 0.2f),
                        LONG(m_fX + m_fSizeX * 0.3f),
                        LONG(m_fY + m_fSizeY * 0.2f) };

    m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());


    // Soul이 부족하지 않으면 구매
    if (m_iPrice <= CInventory::GetInstance()->Get_Soul())
    {
        if (m_isSelect && m_pGameInstance->Mouse_Down(DIM_LB))
        {
            Sell_Item();
        }

        m_vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);
    }
    else
    {
        m_vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
    }


    for (auto& pUI : m_vecUI)
    {
        pUI->Tick(fTimeDelta);
    }
}

void CUI_ShopSelect::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, SEVENTH);

    for (auto& pUI : m_vecUI)
    {
        pUI->Late_Tick(fTimeDelta);
    }
}

HRESULT CUI_ShopSelect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();

    if(m_isSelect)
        m_pVIBufferCom->Render();

    Rend_Font();

    return S_OK;
}

HRESULT CUI_ShopSelect::Add_Components()
{
    /* For. Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ShopSelect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_ShopSelect::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_ShopSelect::Create_UI()
{
    // ItemIcon / RemainIcon / Soul 생성 필요
    CUI_ItemIcon::UI_ITEMICON_DESC pItemIconDesc{};
    pItemIconDesc.eLevel = LEVEL_STATIC;
    pItemIconDesc.eUISort = EIGHT;
    pItemIconDesc.fX = m_fX - 190.f;
    pItemIconDesc.fY = m_fY;
    pItemIconDesc.fSizeX = 60.f;
    pItemIconDesc.fSizeY = 60.f;

    CUI_StateSoul::UI_SOUL_DESC pSoulDesc{};
    pSoulDesc.eLevel = LEVEL_STATIC;
    pSoulDesc.fX = m_fX + 120.f;
    pSoulDesc.fY = m_fY;
    pSoulDesc.fSizeX = 30.f;
    pSoulDesc.fSizeY = 30.f;
    pSoulDesc.eUISort = EIGHT;
    pSoulDesc.isSoulCntRend = false;

    CUI::UI_DESC pDesc{};
    pDesc.eLevel = LEVEL_STATIC;
    pDesc.fX = m_fX + 200.f;
    pDesc.fY = m_fY;
    pDesc.fSizeX = 32.f; // 128
    pDesc.fSizeY = 32.f;

    switch (m_iSlotIdx)
    {
    case 0:
    {
        pItemIconDesc.wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether"); // 마나 채워주는 템
        m_wstrItemName = TEXT("RADIANT ETHER");
        m_iPrice = 250;
        m_iRemainCnt = 5;
        break;
    }
    case 1:
    {
        pItemIconDesc.wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3"); // 버프 템
        m_wstrItemName = TEXT("SIGIL OF ETHER");
        m_iPrice = 400;
        m_iRemainCnt = 3;
        break;
    }
    case 2:
    {
        pItemIconDesc.wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0"); // 강화
        m_wstrItemName = TEXT("HADRONITE");
        m_iPrice = 500;
        m_iRemainCnt = 3;
        break;
    }
    default:
        break;
    }

    // ItemIcon
    m_vecUI.emplace_back(dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pItemIconDesc)));

    // Soul
    m_vecUI.emplace_back(dynamic_cast<CUI_StateSoul*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_StateSoul"), &pSoulDesc)));

    // RemainIcon
    m_vecUI.emplace_back(dynamic_cast<CUI_Shop_RemainIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Shop_RemainIcon"), &pDesc)));


    return S_OK;
}

void CUI_ShopSelect::Rend_Font()
{
    // Item Name
    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo"), m_wstrItemName, _float2(m_fX - 160.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
        return;

    // Price
    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), to_wstring(m_iPrice), _float2(m_fX + 70.f, m_fY - 10.f), m_vColor)))
        return;

    // Remain Count
    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), to_wstring(m_iRemainCnt), _float2(m_fX + 170.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
        return;
}

void CUI_ShopSelect::Sell_Item()
{
    if (0 >= m_iRemainCnt)
        return;

    // Inventory에서 Soul 빼가기
    CInventory::GetInstance()->Calcul_Soul(-m_iPrice);

    m_iRemainCnt--;

    // Inventory에 Item 추가
    if (0 == m_iSlotIdx)
    {
        CInventory::GetInstance()->Add_DropItem(CItem::ITEM_ETHER);

    }
    else if (1 == m_iSlotIdx)
    {
        CInventory::GetInstance()->Add_DropItem(CItem::ITEM_BUFF4);
    }
    else if (2 == m_iSlotIdx)
    {
        CInventory::GetInstance()->Add_DropItem(CItem::ITEM_UPGRADE1);
    }
}

CUI_ShopSelect* CUI_ShopSelect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_ShopSelect* pInstance = new CUI_ShopSelect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_ShopSelect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_ShopSelect::Clone(void* pArg)
{
    CUI_ShopSelect* pInstance = new CUI_ShopSelect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_ShopSelect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_ShopSelect::Free()
{
    __super::Free();

    //Safe_Release(m_pItemIcon);

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);
}
