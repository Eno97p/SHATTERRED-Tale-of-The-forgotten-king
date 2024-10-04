#include "TransitionCamera.h"
#include "GameInstance.h"

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

    m_fTransitionTime = 0.0f;
    m_fTotalTransitionTime = 1.0f; // 기본값 설정, 필요에 따라 조정 가능

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
    _matrix srcMat = dynamic_cast<CTransform*>(cams[1]->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
    _matrix dstMat = dynamic_cast<CTransform*>(cams[0]->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

    _matrix matInterpolated = XMMatrixSlerp(/*m_matStartWorld*//*XMMatrixIdentity()*/srcMat, /*m_matEndWorld*/dstMat, t);

    // 보간된 행렬 적용
    m_pTransformCom->Set_WorldMatrix(matInterpolated);

    if (t >= 1.0f)
    {
        m_bTransitionFinished = true;
        m_pGameInstance->Set_MainCamera(0);
        m_pGameInstance->Erase(this);
        // 여기서 다음 카메라로 전환하는 로직을 추가할 수 있습니다.
        // 예: m_pGameInstance->Set_MainCamera(m_pNextCamera);
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
    if (pDesc)
    {
        pInstance->m_matStartWorld = pDesc->matStartWorld;
        pInstance->m_matEndWorld = pDesc->matEndWorld;
        pInstance->m_fTotalTransitionTime = pDesc->fTransitionTime;
        // 필요한 경우 다음 카메라 정보도 저장할 수 있습니다.
        // pInstance->m_pNextCamera = pDesc->pNextCamera;
    }

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