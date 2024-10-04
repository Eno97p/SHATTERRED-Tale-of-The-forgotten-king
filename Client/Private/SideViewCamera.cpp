#include "..\Public\SideViewCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

CSideViewCamera::CSideViewCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CSideViewCamera::CSideViewCamera(const CSideViewCamera& rhs)
    : CCamera{ rhs }
{
}

HRESULT CSideViewCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSideViewCamera::Initialize(void* pArg)
{
    SIDEVIEWCAMERA_DESC* pDesc = (SIDEVIEWCAMERA_DESC*)pArg;
    m_fSensor = pDesc->fSensor;
    m_pPlayerTrans = pDesc->pPlayerTrans;
    Safe_AddRef(m_pPlayerTrans);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    //__super::Activate();

    // �ʱ� ī�޶� ��ġ ����
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vInitialCameraPosition, vInitialLookAtPosition;
    Get_SideViewCamPos(vPlayerPosition, &vInitialCameraPosition, &vInitialLookAtPosition);

    m_vCameraPosition = vInitialCameraPosition;
    m_vLookAtPosition = vInitialLookAtPosition;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

    return S_OK;
}

void CSideViewCamera::Priority_Tick(_float fTimeDelta)
{
    switch (m_eSideViewState)
    {
    case SV_UP:
    {
        m_fBossHeightOffset += fTimeDelta * 10.f;
        if (m_fBossHeightOffset > 25.f)
        {
            m_eSideViewState = SV_IDLE;
            m_fBossHeightOffset = 25.f;
            return;
        }
    }
        break;
    case SV_DOWN:
    {
        m_fBossHeightOffset -= fTimeDelta * 15.f;

        if (m_fBossHeightOffset <= 8.5f)
        {
            m_fBossHeightOffset = 8.5f;
            m_vFixedLookAtPosition = { -422.f, 73.f, -5.f, 1.f };
            m_eSideViewState = SV_IDLE;
            return;
        }
    }
        break;
    default:
        break;
    }

}

void CSideViewCamera::Tick(_float fTimeDelta)
{
    if (m_pPlayerTrans == nullptr)
        return;

    if (m_pGameInstance->Key_Down(DIK_M))
    {
        m_fShakeTime = 0.5f;  // ����ŷ ���� �ð� (0.5��)
        m_fShakeAccTime = 0.f;
        m_fIntensity = 0.1f;  // ����ŷ ����
    }

    {
        Update_SideViewCam(fTimeDelta);
    }

    if (m_fShakeTime > m_fShakeAccTime)
    {
        m_fShakeAccTime += fTimeDelta;
        Shaking();
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

    if (!m_bCamActivated) return;

    __super::Tick(fTimeDelta);
}

void CSideViewCamera::Late_Tick(_float fTimeDelta)
{
    if (!m_bCamActivated) return;
    Key_Input(fTimeDelta);
}

HRESULT CSideViewCamera::Render()
{
    return S_OK;
}

void CSideViewCamera::Get_SideViewCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition)
{
    _float fCurrentHeightOffset = m_bBossScene ? m_fBossHeightOffset : m_fNormalHeightOffset;
    _float fCurrentZPosition = m_bBossScene ? m_fBossZPosition : m_fNormalZPosition;

    if (m_bBossScene)
    {
        // LookAt ��ġ ���� (������ ��ġ)
        *pOutLookAtPosition = m_vFixedLookAtPosition;

        // �÷��̾�� LookAt ��ġ�� ���ϴ� ���� ���
        _vector vPlayerToLookAt = XMLoadFloat4(pOutLookAtPosition) - XMLoadFloat4(&vPlayerPosition);
        vPlayerToLookAt = XMVector3Normalize(vPlayerToLookAt);

        // ī�޶� ��ġ ���: �÷��̾� ��ġ���� LookAt �������� m_fBossZPosition��ŭ �̵�
        _vector vCameraPos = XMLoadFloat4(&vPlayerPosition) - vPlayerToLookAt * fCurrentZPosition;
        XMStoreFloat4(pOutCameraPosition, vCameraPos);

        // ���� ����
        pOutCameraPosition->y += fCurrentHeightOffset;
    }
    else
    {
        // ���� �Ϲ� ��� ����
        pOutCameraPosition->x = vPlayerPosition.x;
        pOutCameraPosition->y = vPlayerPosition.y + fCurrentHeightOffset;
        pOutCameraPosition->z = vPlayerPosition.z - fCurrentZPosition;
        *pOutLookAtPosition = vPlayerPosition;
    }

    pOutCameraPosition->w = 1.f;
}




void CSideViewCamera::Update_SideViewCam(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_SideViewCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    // �ε巯�� ī�޶� �̵��� ���� ����
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        1.0f - exp(-m_fFollowSpeed * fTimeDelta)
    ));

    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        1.0f - exp(-m_fFollowSpeed * fTimeDelta)
    ));

    // ī�޶� Transform ������Ʈ ������Ʈ
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));
}

void CSideViewCamera::Update_TransitionCam(_float fTimeDelta)
{
    // Update_TransitionCam �Լ� ������ �״�� ����
}

void CSideViewCamera::Phase_Two_Height_Offset()
{
    m_vFixedLookAtPosition.y += 20.f;
    m_eSideViewState = SV_UP;
}

void CSideViewCamera::Phase_Two_Back_To_Origin()
{
    m_eSideViewState = SV_DOWN;
}



_float CSideViewCamera::RandomFloat(_float min, _float max)
{
    return min + static_cast <_float> (rand()) / (static_cast <_float> (RAND_MAX / (max - min)));
}

void CSideViewCamera::Shaking()
{
    _vector vLook;
    _vector vRight;
    _vector vUp;

    vLook = XMVector3Normalize(XMLoadFloat4(&m_vLookAtPosition) - XMLoadFloat4(&m_vCameraPosition));
    vRight = XMVector3Cross(vLook, m_pTransformCom->Get_State(CTransform::STATE_UP));
    vUp = XMVector3Cross(vLook, vRight);

    // -1.5f ~ 1.5f ������ �� ����
    float offsetY = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
    _vector eye = XMLoadFloat4(&m_vCameraPosition);
    _vector at = XMLoadFloat4(&m_vLookAtPosition);
    eye += vUp * offsetY;
    at += vUp * offsetY;

    if (!m_bLockWidth)
    {
        float offsetX = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
        eye += vRight * offsetX;
        at += vRight * offsetX;
    }

    XMStoreFloat4(&m_vCameraPosition, eye);
    XMStoreFloat4(&m_vLookAtPosition, at);
}
void CSideViewCamera::Key_Input(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_TAB))
    {
        m_pGameInstance->Set_MainCamera(0);
    }
    // ȸ�� �� �Ÿ� ���� ���� �ڵ�� ���ŵǾ����ϴ�.
}

CSideViewCamera* CSideViewCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSideViewCamera* pInstance = new CSideViewCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CSideViewCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSideViewCamera::Clone(void* pArg)
{
    CSideViewCamera* pInstance = new CSideViewCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CSideViewCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSideViewCamera::Free()
{
    Safe_Release(m_pPlayerTrans);

    __super::Free();
}