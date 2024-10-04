#include "UI_Setting_BackBtn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"

#include "UIGroup_Menu.h"

CUI_Setting_BackBtn::CUI_Setting_BackBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Interaction{ pDevice, pContext }
{
}

CUI_Setting_BackBtn::CUI_Setting_BackBtn(const CUI_Setting_BackBtn& rhs)
    : CUI_Interaction{ rhs }
{
}

HRESULT CUI_Setting_BackBtn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Setting_BackBtn::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fX = (g_iWinSizeX >> 1);
    m_fY = g_iWinSizeY - 120.f;
    m_fSizeX = 400.f; // 512
    m_fSizeY = 64.f; // 64

    Setting_Position();

    return S_OK;
}

void CUI_Setting_BackBtn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Setting_BackBtn::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);

    __super::Tick(fTimeDelta);

    m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

    if (m_isSelect && m_pGameInstance->Mouse_Down(DIM_LB))
    {
        CUI_Manager::GetInstance()->Get_UIGroup("Setting")->Set_RenderOnAnim(false);
        dynamic_cast<CUIGroup_Menu*>(CUI_Manager::GetInstance()->Get_UIGroup("Menu"))->Set_MenuPageState(false);
    }
}

void CUI_Setting_BackBtn::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_Setting_BackBtn::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();

    if(m_isSelect)
        m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("BACK"), _float2(m_fX - 30.f, m_fY - 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Setting_BackBtn::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_BackBtn"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Setting_BackBtn::Bind_ShaderResources()
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

CUI_Setting_BackBtn* CUI_Setting_BackBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Setting_BackBtn* pInstance = new CUI_Setting_BackBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_Setting_BackBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Setting_BackBtn::Clone(void* pArg)
{
    CUI_Setting_BackBtn* pInstance = new CUI_Setting_BackBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_Setting_BackBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Setting_BackBtn::Free()
{
    __super::Free();
}
