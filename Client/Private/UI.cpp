#include "UI.h"

#include "GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject{rhs}
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
    UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    if (nullptr != pArg)
    {
        m_fX = pDesc->fX;
        m_fY = pDesc->fY;
        m_fSizeX = pDesc->fSizeX;
        m_fSizeY = pDesc->fSizeY;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUI::Priority_Tick(_float fTimeDelta)
{
}

void CUI::Tick(_float fTimeDelta)
{
}

void CUI::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::Setting_Position()
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.0f));
}

void CUI::Render_Animation(_float fTimeDelta, _float fSpeed)
{
    m_fRenderTimer += fTimeDelta * fSpeed;

    if (m_fRenderTimer >= 1.f) // 애니메이션 종료 시
    {
        m_fRenderTimer = 1.f;
        m_isRenderAnimFinished = true;
    }
}

void CUI::Resset_Animation(_bool isOffAnim)
{
    m_fRenderTimer = 0.f;
    m_isRenderOffAnim = isOffAnim;
    m_isRenderAnimFinished = false;
}

_bool CUI::isRender_End()
{
    if (m_isRenderOffAnim && m_isRenderAnimFinished)
        return true;
    
    return false;
}

void CUI::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
