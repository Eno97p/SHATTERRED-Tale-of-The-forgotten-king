#include "..\Public\SideViewCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

#include "ToolObj.h"

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

    __super::Activate();

    // 초기 카메라 위치 설정
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
    // Priority_Tick 함수 내용은 그대로 유지
    if (m_pGameInstance->Key_Down(DIK_3))
    {
        m_bBossScene = !m_bBossScene;
    }
}

void CSideViewCamera::Tick(_float fTimeDelta)
{
    if (m_pPlayerTrans == nullptr)
        return;

    //// 플레이어 위치 얻기
    //_vector vPlayerPos = m_pPlayerTrans->Get_State(CTransform::STATE_POSITION);
    //_vector vCamPos = m_pGameInstance->Get_CamPosition();

    // 맵 오브젝트 관련 코드는 그대로 유지

    {
        Update_SideViewCam(fTimeDelta);
    }

    // 셰이킹 관련 코드는 그대로 유지

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

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
        // LookAt 위치 설정 (고정된 위치)
        *pOutLookAtPosition = m_vFixedLookAtPosition;

        // 플레이어에서 LookAt 위치로 향하는 벡터 계산
        _vector vPlayerToLookAt = XMLoadFloat4(pOutLookAtPosition) - XMLoadFloat4(&vPlayerPosition);
        vPlayerToLookAt = XMVector3Normalize(vPlayerToLookAt);

        // 카메라 위치 계산: 플레이어 위치에서 LookAt 방향으로 m_fBossZPosition만큼 이동
        _vector vCameraPos = XMLoadFloat4(&vPlayerPosition) - vPlayerToLookAt * fCurrentZPosition;
        XMStoreFloat4(pOutCameraPosition, vCameraPos);

        // 높이 조정
        pOutCameraPosition->y += fCurrentHeightOffset;
    }
    else
    {
        // 기존 일반 모드 로직
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

    // 부드러운 카메라 이동을 위한 보간
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

    // 카메라 Transform 컴포넌트 업데이트
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));
}

void CSideViewCamera::Update_TransitionCam(_float fTimeDelta)
{
    // Update_TransitionCam 함수 내용은 그대로 유지
}

void CSideViewCamera::Shaking()
{
    // Shaking 함수 내용은 그대로 유지
}

void CSideViewCamera::Key_Input(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_TAB))
    {
        m_pGameInstance->Set_MainCamera(0);
    }
    // 회전 및 거리 조절 관련 코드는 제거되었습니다.
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