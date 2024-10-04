#include "TransitionCamera.h"
#include "GameInstance.h"
#include "CutSceneCamera.h"
#include "UI_Manager.h"
CTransitionCamera::CTransitionCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CTransitionCamera::CTransitionCamera(const CTransitionCamera& rhs)
    : CCamera{ rhs }
{
}

HRESULT CTransitionCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTransitionCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    TRANSITIONCAMERA_DESC* desc = static_cast<TRANSITIONCAMERA_DESC*>(pArg);

    m_iStartCam = desc->iStartCam;
    m_iEndCam = desc->iEndCam;
    m_fTotalTransitionTime = desc->fTransitionTime;

    m_fTransitionTime = 0.01f;

    return S_OK;
}

void CTransitionCamera::Tick(_float fTimeDelta)
{

    if (m_bTransitionFinished)
        return;

    m_fTransitionTime += fTimeDelta;
    float t = min(m_fTransitionTime / m_fTotalTransitionTime, 1.0f);

    vector<CCamera*> cams = (m_pGameInstance->Get_Cameras());
    
    // 월드 행렬 보간
    _matrix srcMat = dynamic_cast<CTransform*>(cams[m_iStartCam]->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
    _matrix dstMat = dynamic_cast<CTransform*>(cams[m_iEndCam]->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

    _matrix matInterpolated = XMMatrixSlerp(/*m_matStartWorld*//*XMMatrixIdentity()*/srcMat, /*m_matEndWorld*/dstMat, t);

    // 보간된 행렬 적용
    m_pTransformCom->Set_WorldMatrix(matInterpolated);

    if (t >= 1.0f)
    {
        m_bTransitionFinished = true;
        if ((CAMERA_INDEX)m_iEndCam == CAM_CUTSCENE)
        {
            CUI_Manager::GetInstance()->Setting_Cinematic(false); // 컷씬 시작
            dynamic_cast<CCutSceneCamera*>(m_pGameInstance->Get_Cameras()[CAM_CUTSCENE])->Play_CutScene();
        }
        m_pGameInstance->Set_MainCamera(m_iEndCam);

        m_pGameInstance->Erase(this);
    }
    else
    {
        //m_pGameInstance->Set_MainCamera(m_iEndCam);
        m_bCamActivated = true;

    }

    __super::Tick(fTimeDelta);
}

CTransitionCamera* CTransitionCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTransitionCamera* pInstance = new CTransitionCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTransitionCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTransitionCamera::Clone(void* pArg)
{
    CTransitionCamera* pInstance = new CTransitionCamera(*this);

    TRANSITIONCAMERA_DESC* pDesc = (TRANSITIONCAMERA_DESC*)pArg;

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CTransitionCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTransitionCamera::Free()
{
    __super::Free();
}