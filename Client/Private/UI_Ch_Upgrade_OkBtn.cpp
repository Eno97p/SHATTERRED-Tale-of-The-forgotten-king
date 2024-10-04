#include "UI_Ch_Upgrade_OkBtn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"
#include "Camera.h"
#include "Player.h"

#include "UIGroup_Ch_Upgrade.h"

CUI_Ch_Upgrade_OkBtn::CUI_Ch_Upgrade_OkBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_Ch_Upgrade_OkBtn::CUI_Ch_Upgrade_OkBtn(const CUI_Ch_Upgrade_OkBtn& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_Ch_Upgrade_OkBtn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Ch_Upgrade_OkBtn::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fX = (g_iWinSizeX >> 1) - 60.f;
    m_fY = (g_iWinSizeY >> 1) + 120.f;
    m_fSizeX = 100.f; // 110
    m_fSizeY = 30.f; // 110

    Setting_Position();

    return S_OK;
}

void CUI_Ch_Upgrade_OkBtn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Ch_Upgrade_OkBtn::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);

    __super::Tick(fTimeDelta);

    m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

    if (m_isSelect && m_pGameInstance->Mouse_Down(DIM_LB))
    {
        // 창이 종료되고 Origin의 값들을 현재 값으로 적용
        CUI_Manager::GetInstance()->Get_UIGroup("Ch_Upgrade")->Set_RenderOnAnim(false);
        dynamic_cast<CUIGroup_Ch_Upgrade*>(CUI_Manager::GetInstance()->Get_UIGroup("Ch_Upgrade"))->Resset_OriginData();
   
        m_pGameInstance->Get_MainCamera()->Activate();
    }
}

void CUI_Ch_Upgrade_OkBtn::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_Ch_Upgrade_OkBtn::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

    _vector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

    if (m_isSelect)
        vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    else
        vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("OK"), _float2(m_fX - 20.f, m_fY - 15.f), vColor)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Ch_Upgrade_OkBtn::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ch_Upgrade_Btn"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Ch_Upgrade_OkBtn::Bind_ShaderResources()
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

CUI_Ch_Upgrade_OkBtn* CUI_Ch_Upgrade_OkBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Ch_Upgrade_OkBtn* pInstance = new CUI_Ch_Upgrade_OkBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_Ch_Upgrade_OkBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Ch_Upgrade_OkBtn::Clone(void* pArg)
{
    CUI_Ch_Upgrade_OkBtn* pInstance = new CUI_Ch_Upgrade_OkBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_Ch_Upgrade_OkBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Ch_Upgrade_OkBtn::Free()
{
    __super::Free();
}
