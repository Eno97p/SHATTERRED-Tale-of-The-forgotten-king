#include "UI_ShopBG.h"

#include "GameInstance.h"

CUI_ShopBG::CUI_ShopBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_ShopBG::CUI_ShopBG(const CUI_ShopBG& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_ShopBG::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_ShopBG::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fX = 380.f;
    m_fY = 260.f;
    m_fSizeX = 700.f; // 512  682.6
    m_fSizeY = 300.f; // 128   170.6

    Setting_Position();

    return S_OK;
}

void CUI_ShopBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ShopBG::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);
}

void CUI_ShopBG::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, SIXTH);
}

HRESULT CUI_ShopBG::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_ShopBG::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ShopBG"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_ShopBG::Bind_ShaderResources()
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

CUI_ShopBG* CUI_ShopBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_ShopBG* pInstance = new CUI_ShopBG(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_ShopBG");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_ShopBG::Clone(void* pArg)
{
    CUI_ShopBG* pInstance = new CUI_ShopBG(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_ShopBG");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_ShopBG::Free()
{
    __super::Free();
}
