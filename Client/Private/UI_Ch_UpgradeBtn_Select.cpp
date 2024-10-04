#include "UI_Ch_UpgradeBtn_Select.h"

#include "GameInstance.h"

CUI_Ch_UpgradeBtn_Select::CUI_Ch_UpgradeBtn_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_Ch_UpgradeBtn_Select::CUI_Ch_UpgradeBtn_Select(const CUI_Ch_UpgradeBtn_Select& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_Ch_UpgradeBtn_Select::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Ch_UpgradeBtn_Select::Initialize(void* pArg)
{
    UI_CHUP_BTNSELECT_DESC* pDesc = static_cast<UI_CHUP_BTNSELECT_DESC*>(pArg);

    m_wstrSign = pDesc->wstrSign;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Setting_Position();

    return S_OK;
}

void CUI_Ch_UpgradeBtn_Select::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Ch_UpgradeBtn_Select::Tick(_float fTimeDelta)
{
}

void CUI_Ch_UpgradeBtn_Select::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_Ch_UpgradeBtn_Select::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), m_wstrSign, _float2(m_fX - 7.f, m_fY - 12.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Ch_UpgradeBtn_Select::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ch_Upgrade_BtnSelect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Ch_UpgradeBtn_Select::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CUI_Ch_UpgradeBtn_Select* CUI_Ch_UpgradeBtn_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Ch_UpgradeBtn_Select* pInstance = new CUI_Ch_UpgradeBtn_Select(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_Ch_UpgradeBtn_Select");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Ch_UpgradeBtn_Select::Clone(void* pArg)
{
    CUI_Ch_UpgradeBtn_Select* pInstance = new CUI_Ch_UpgradeBtn_Select(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_Ch_UpgradeBtn_Select");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Ch_UpgradeBtn_Select::Free()
{
    __super::Free();
}
