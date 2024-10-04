#include "..\Public\Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	CAMERA_DESC*		pDesc = (CAMERA_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 카메라의 트랜스폼에 카메라의 초기 상태를 셋팅했다. */
	/* 카메라의 월드행렬의 역행렬 == 뷰 스페이스변환 행렬. */
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

    m_fFovy = pDesc->fFovy;
	m_fAspect = pDesc->fAspect;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	return S_OK;
}

void CCamera::Priority_Tick(_float fTimeDelta)
{
}

void CCamera::Tick(_float fTimeDelta)
{
    if (!m_bCamActivated) return;
    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix()));

    if (m_bZoom)
    {
        m_fZoomTimer += fTimeDelta;

        if (m_bZooming)
        {
            // Zooming to target FOV
            float t = min(m_fZoomTimer / m_fMaxZoomTime, 1.0f);
            m_fCurrentFovy = LerpFloat(m_fStartFovy, m_fTargetFovy, SmoothStepFloat(t));

            if (t >= 1.0f)
            {
                m_bZooming = false;
                m_fZoomTimer = 0.f;
                m_fStartFovy = m_fTargetFovy;
            }
        }
        else
        {
            // Recovering to original FOV
            float t = min(m_fZoomTimer / m_fRecoverTime, 1.0f);
            m_fCurrentFovy = LerpFloat(m_fStartFovy, m_fFovy, SmoothStepFloat(t));

            if (t >= 1.0f)
            {
                m_bZoom = false;
                m_fCurrentFovy = m_fFovy;
            }
        }

        m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fCurrentFovy, m_fAspect, m_fNear, m_fFar));
    }
    else
    {
        m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
    }
}
void CCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{

	return S_OK;
}

_vector CCamera::Get_CamLook()
{
	return m_pTransformCom->Get_State(CTransform::STATE_LOOK);
}

void CCamera::Zoom(_float fTargetFov, _float fMaxZoomTime, _float fRecoverTime)
{
    _float fNewTargetFovy = XMConvertToRadians(fTargetFov);

    if (m_bZoom)
    {
        // 이미 줌 중이라면 현재 상태에서 새로운 목표로 부드럽게 전환
        m_fStartFovy = m_fCurrentFovy;
        _float remainingTime = m_bZooming ? (m_fMaxZoomTime - m_fZoomTimer) : (m_fRecoverTime - m_fZoomTimer);
        m_fZoomTimer = 0.f;
        m_fMaxZoomTime = fMaxZoomTime;
        m_fRecoverTime = fRecoverTime;
    }
    else
    {
        m_fMaxZoomTime = fMaxZoomTime;
        m_fRecoverTime = fRecoverTime;
        // 새로운 줌 시작
        m_fStartFovy = m_fFovy;
        m_fCurrentFovy = m_fFovy;
        m_fZoomTimer = 0.f;
        m_bZoom = true;
    }

    m_fTargetFovy = fNewTargetFovy;
    m_bZooming = true;
}

void CCamera::Free()
{
	__super::Free();

}
