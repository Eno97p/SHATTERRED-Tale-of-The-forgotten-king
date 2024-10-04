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
    m_fTotalTransitionTime = 1.0f; // �⺻�� ����, �ʿ信 ���� ���� ����

    return S_OK;
}

void CTransitionCamera::Tick(_float fTimeDelta)
{
    if (m_bTransitionFinished)
        return;

    m_fTransitionTime += fTimeDelta;
    float t = min(m_fTransitionTime / m_fTotalTransitionTime, 1.0f);

    vector<CCamera*> cams = (m_pGameInstance->Get_Cameras());

    // ���� ��� ����
    _matrix srcMat = dynamic_cast<CTransform*>(cams[1]->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
    _matrix dstMat = dynamic_cast<CTransform*>(cams[0]->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

    _matrix matInterpolated = XMMatrixSlerp(/*m_matStartWorld*//*XMMatrixIdentity()*/srcMat, /*m_matEndWorld*/dstMat, t);

    // ������ ��� ����
    m_pTransformCom->Set_WorldMatrix(matInterpolated);

    if (t >= 1.0f)
    {
        m_bTransitionFinished = true;
        m_pGameInstance->Set_MainCamera(0);
        m_pGameInstance->Erase(this);
        // ���⼭ ���� ī�޶�� ��ȯ�ϴ� ������ �߰��� �� �ֽ��ϴ�.
        // ��: m_pGameInstance->Set_MainCamera(m_pNextCamera);
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
        // �ʿ��� ��� ���� ī�޶� ������ ������ �� �ֽ��ϴ�.
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