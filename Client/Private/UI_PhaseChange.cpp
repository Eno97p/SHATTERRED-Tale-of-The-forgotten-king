#include "UI_PhaseChange.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CUI_PhaseChange::CUI_PhaseChange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_PhaseChange::CUI_PhaseChange(const CUI_PhaseChange& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_PhaseChange::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_PhaseChange::Initialize(void* pArg)
{
    UI_PHASECHANGE_DESC* pDesc = static_cast<UI_PHASECHANGE_DESC*>(pArg);

    m_isFadeIn = pDesc->isFadeIn;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fX = g_iWinSizeX >> 1;
    m_fY = g_iWinSizeY >> 1;
    m_fSizeX = g_iWinSizeX;
    m_fSizeY = g_iWinSizeY;

    Setting_Position();
    
    return S_OK;
}

void CUI_PhaseChange::Priority_Tick(_float fTimeDelta)
{
}

void CUI_PhaseChange::Tick(_float fTimeDelta)
{
    m_fAlphaTimer += fTimeDelta;
    if (1.f <= m_fAlphaTimer)
    {
        m_fAlphaTimer = 1.f;
        if (!m_isFadeIn)
        {
            m_fDeadTimer += fTimeDelta;
            if (1.f <= m_fDeadTimer)
            {
                CUI_Manager::GetInstance()->Create_PhaseChange(true);

                m_isFadeAnimEnd = true;
            }
        }
        else
        {
            m_isFadeAnimEnd = true;
        }
    }
}

void CUI_PhaseChange::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, SIXTEENTH); // 더 추가될 수 있음
}

HRESULT CUI_PhaseChange::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_PhaseChange::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PhaseChange"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_PhaseChange::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaTimer, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isFadeIn, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

CUI_PhaseChange* CUI_PhaseChange::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_PhaseChange* pInstance = new CUI_PhaseChange(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_PhaseChange");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_PhaseChange::Clone(void* pArg)
{
    CUI_PhaseChange* pInstance = new CUI_PhaseChange(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_PhaseChange");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_PhaseChange::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
