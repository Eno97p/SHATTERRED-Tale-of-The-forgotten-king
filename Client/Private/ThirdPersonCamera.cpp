#include "..\Public\ThirdPersonCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

_bool CThirdPersonCamera::m_bIsTargetLocked = false;
_float4 CThirdPersonCamera::m_vLockedTargetPos = { 0.f, 10.f, 0.f, 1.f };



CThirdPersonCamera::CThirdPersonCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CThirdPersonCamera::CThirdPersonCamera(const CThirdPersonCamera& rhs)
    : CCamera{ rhs }
{
}

HRESULT CThirdPersonCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CThirdPersonCamera::Initialize(void* pArg)
{
    THIRDPERSONCAMERA_DESC* pDesc = (THIRDPERSONCAMERA_DESC*)pArg;
    m_fSensor = pDesc->fSensor;
    m_pPlayerTrans = pDesc->pPlayerTrans;
    Safe_AddRef(m_pPlayerTrans);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // �ʱ� ī�޶� ��ġ ����
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vInitialCameraPosition, vInitialLookAtPosition;
    Get_ThirdCamPos(vPlayerPosition, &vInitialCameraPosition, &vInitialLookAtPosition);

    m_vCameraPosition = vInitialCameraPosition;
    m_vLookAtPosition = vInitialLookAtPosition;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

    return S_OK;
}


void CThirdPersonCamera::Priority_Tick(_float fTimeDelta)
{

    if (!m_bCamActivated) return;

    if (m_pGameInstance->Key_Down(DIK_GRAVE))
    {
        m_bFix = !m_bFix;
    }

    if (!m_bFix)
    {
        Mouse_Move(fTimeDelta);
        Key_Input(fTimeDelta);
    }


    //if (m_pGameInstance->Mouse_Down(DIM_RB))
    if (m_pGameInstance->Key_Down(DIK_M))
    {
        //Shake_Camera(false, 0.5f);
        m_bIsShaking = true;
        m_fShakeTimer = 0.f;
        m_fShakeIntervalTimer = 0.f;
        m_fShakeInterval = m_fInitialShakeInterval;
        m_fShakeAmount = m_fInitialShakeAmount; // �ʱ� ����ũ ������ ����
        XMStoreFloat4(&m_qShakeRotation, XMQuaternionIdentity());
    }

}

void CThirdPersonCamera::Tick(_float fTimeDelta)
{
    if (!m_bCamActivated || m_pPlayerTrans == nullptr)
    {
        if (CAM_SIDEVIEW == (CAMERA_INDEX)m_pGameInstance->Get_MainCameraIdx())
        {
            m_pTransformCom->Set_WorldMatrix(dynamic_cast<CTransform*>(m_pGameInstance->Get_MainCamera()->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix());
            return;
        }
    }

    // �÷��̾� ��ġ ��������
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    // ��ǥ ī�޶� ��ġ �� �ü� ���
    _float4 vTargetCameraPosition, vTargetLookAtPosition;

    if (m_bIsTargetLocked)
    {
        // Ÿ�� ���� ���� ó��
        if (m_pTarget != nullptr)
        {
            CMonster* pMonster = dynamic_cast<CMonster*>(m_pTarget);
            if (pMonster != nullptr)
            {
                _vector vTP = pMonster->Get_MonsterPos();
                _vector vCamPos = XMLoadFloat4(&m_vCameraPosition);

                // ī�޶�� Ÿ�� ������ ���� ���� ����
                _vector vCamToTarget = XMVector3Normalize(vTP - vCamPos);

                // ī�޶󿡼� Ÿ�� �������� ���� �Ÿ�(��: 10.0f) ������ ��
                _vector vProjectedPoint = vCamPos + vCamToTarget * 10.0f;

                // �������� Ÿ�� ������ �Ÿ� ���
                _float distToRay = XMVectorGetX(XMVector3Length(vTP - vProjectedPoint));

                // ī�޶�� ������ ������ �Ÿ� ��� (����)
                _float depthOnRay = 10.0f;

                // �Ÿ��� ����ġ�� �־� ���̵� ��� (��: ���̿� 0.5 ����ġ)
                _float weightedDistance = distToRay + depthOnRay * 0.5f;

                // ���� �Ÿ��� �������� ���� ���� �Ǵ�
                if (40.0f < weightedDistance || (m_pTarget->Get_Dead()))
                {
                    dynamic_cast<CMonster*>(m_pTarget)->Set_Lock(false);
                    Safe_Release(m_pTarget);
                    m_pTarget = nullptr;
                    TargetLock_Off();
                }
                else
                {
                    XMStoreFloat4(&m_vLockedTargetPos, vTP);
                }
            }
        }

        Get_LockOnCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

        if (m_bIsTransitioning)
        {
            _float t = 1.0f - exp(-m_fTransitionSpeed * fTimeDelta);

            XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
                XMLoadFloat4(&m_vCameraPosition),
                XMLoadFloat4(&vTargetCameraPosition),
                t
            ));

            XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
                XMLoadFloat4(&m_vLookAtPosition),
                XMLoadFloat4(&vTargetLookAtPosition),
                t
            ));

            // ��ȯ �Ϸ� ���� Ȯ��
            if (XMVector4NearEqual(XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&vTargetCameraPosition), XMVectorReplicate(m_fTransitionThreshold)) &&
                XMVector4NearEqual(XMLoadFloat4(&m_vLookAtPosition), XMLoadFloat4(&vTargetLookAtPosition), XMVectorReplicate(m_fTransitionThreshold)))
            {
                m_bIsTransitioning = false;
            }
        }
        else
        {
            TargetLockView(fTimeDelta);
        }
    }
    else
    {
        Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

        // ����ĳ��Ʈ ���� (���� ���°� �ƴ� ����)
        _float4 hitPoint;
        _float hitDistance;
        bool bHitObstacle = RaycastFromCameraToPlayer(vTargetCameraPosition, vPlayerPosition, &hitPoint, &hitDistance);

        // ��ǥ �Ÿ� ���
        _float fTargetDistance = m_fDistance;
        if (bHitObstacle && hitDistance < m_fDistance)
        {
            fTargetDistance = max(hitDistance * 0.7f, m_fMinDistance * 0.66f);
        }

        // �Ÿ� �ε巴�� ����
        _float fDistanceLerpSpeed = 5.0f;
        m_fDistance = XMVectorGetX(XMVectorLerp(XMLoadFloat(&m_fDistance), XMLoadFloat(&fTargetDistance), 1.0f - exp(-fDistanceLerpSpeed * fTimeDelta)));

        // ���ο� ī�޶� ��ġ ���
        _float4 cameraDirection;
        XMStoreFloat4(&cameraDirection, XMVector4Normalize(XMLoadFloat4(&vTargetCameraPosition) - XMLoadFloat4(&vPlayerPosition)));
        XMStoreFloat4(&vTargetCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&cameraDirection) * m_fDistance);

        // ī�޶� ���� ����
        vTargetCameraPosition.y = max(vTargetCameraPosition.y, vPlayerPosition.y + m_fMinHeight * 0.5f);

        // �ε巯�� ī�޶� �̵�
        _float fPositionLerpSpeed = 10.0f;
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
            XMLoadFloat4(&m_vCameraPosition),
            XMLoadFloat4(&vTargetCameraPosition),
            1.0f - exp(-fPositionLerpSpeed * fTimeDelta)
        ));

        // LookAt ��ġ ������Ʈ
        XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
            XMLoadFloat4(&m_vLookAtPosition),
            XMLoadFloat4(&vTargetLookAtPosition),
            1.0f - exp(-fPositionLerpSpeed * fTimeDelta)
        ));
    }

    // ����/�ƿ� ���� ó��
    if (m_bZoomIn)
    {
        ParryingZoomIn(fTimeDelta);
    }
    else if (m_bZoomOut)
    {
        ParryingZoomOut(fTimeDelta);
    }


    Update_Shake(fTimeDelta);

    // ����ũ ȿ�� ����
    if (m_bIsShaking)
    {
        m_pGameInstance->Set_MotionBlur(true);
        _vector vShakePosition = XMLoadFloat4(&m_vCameraPosition) + XMLoadFloat3(&m_vShakeOffset);
        XMStoreFloat4(&m_vCameraPosition, vShakePosition);

        _vector vLookAt = XMLoadFloat4(&m_vLookAtPosition);
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_vShakeRotation.x, m_vShakeRotation.y, m_vShakeRotation.z);
        _vector vShakeLookAt = XMVector3TransformNormal(XMVectorSubtract(vLookAt, vShakePosition), rotationMatrix);
        vShakeLookAt = XMVectorAdd(vShakePosition, vShakeLookAt);
        XMStoreFloat4(&m_vLookAtPosition, vShakeLookAt);
    }

    // ���� ī�޶� ��ġ �� ���� ����
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

    TiltAdjust(fTimeDelta);

    __super::Tick(fTimeDelta);
}

void CThirdPersonCamera::Late_Tick(_float fTimeDelta)
{
    if (!m_bCamActivated)
    {
        return;
    }



    if (m_pGameInstance->Mouse_Down(DIM_MB))
    {
        if (!m_bIsTargetLocked)
        {
            _vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            _vector vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
            _vector vCamLookNormalized = XMVector3Normalize(vCamLook);

            _vector vRayEnd = XMVectorAdd(vCamPos, XMVectorScale(vCamLookNormalized, 100.0f));

            list<CGameObject*> monsters = (m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster")));

            _float closestWeightedDistance = FLT_MAX;
            _float closestActualDistance = FLT_MAX;
            CGameObject* closestMonster = nullptr;

            for (auto& monster : monsters)
            {
                CMonster* pMonster = dynamic_cast<CMonster*>(monster);
                if (pMonster == nullptr)
                    continue;

                _vector vMonsterPos = pMonster->Get_MonsterPos();

                if (m_pGameInstance->isIn_WorldFrustum(vMonsterPos, 5.f))
                {
                    _vector projectedPoint = ProjectPointLine(vMonsterPos, vCamPos, vRayEnd);

                    _float distToRay = XMVectorGetX(XMVector3Length(vMonsterPos - projectedPoint));
                    _float depthOnRay = XMVectorGetX(XMVector3Length(projectedPoint - vCamPos));
                    _float weightedDistance = distToRay + depthOnRay * 0.5f;

                    _float actualDistance = XMVectorGetX(XMVector3Length(vMonsterPos - vCamPos));

                    if (weightedDistance < closestWeightedDistance)
                    {
                        closestWeightedDistance = weightedDistance;
                        closestActualDistance = actualDistance;
                        closestMonster = monster;
                    }
                }
            }

            // ����ġ �Ÿ��� 30 �����̰� ���� �Ÿ��� 30 ������ ��쿡�� Ÿ�� ����
            if (closestMonster != nullptr && closestWeightedDistance <= 30.0f && closestActualDistance <= 30.0f)
            {
                // ���� Ÿ���� �ִٸ� ���۷��� ī��Ʈ ����
                if (m_pTarget != nullptr)
                    Safe_Release(m_pTarget);

                // ���ο� Ÿ�� ���� �� ���۷��� ī��Ʈ ����
                m_pTarget = closestMonster;
                Safe_AddRef(m_pTarget);

                // Ÿ�� ����
                CMonster* pMonster = dynamic_cast<CMonster*>(m_pTarget);
                if (pMonster != nullptr)
                {
                    _vector vTargetPos = pMonster->Get_MonsterPos();
                    TargetLock_On(vTargetPos);
                    pMonster->Set_Lock(true);
                }
            }
            else
            {
                // ���� ����� ���Ͱ� ���ų� �Ÿ� ������ �������� �ʴ� ���
                if (m_pTarget != nullptr)
                {
                    dynamic_cast<CMonster*>(m_pTarget)->Set_Lock(false);
                    Safe_Release(m_pTarget);
                    m_pTarget = nullptr;
                }
                TargetLock_Off();
            }
        }
        else
        {
            // �̹� Ÿ���� ���µ� ���¿��� �ٽ� Ŭ���� ���
            if (m_pTarget != nullptr)
            {
                dynamic_cast<CMonster*>(m_pTarget)->Set_Lock(false);
                Safe_Release(m_pTarget);
                m_pTarget = nullptr;
            }
            TargetLock_Off();
        }
    }



}

HRESULT CThirdPersonCamera::Render()
{
    return S_OK;
}

void CThirdPersonCamera::Get_LockOnCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition)
{
    // �÷��̾�� Ÿ�� ������ ���� ���� ���
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // �÷��̾�� Ÿ�� ������ �Ÿ� ���
    _float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // ī�޶� �Ÿ� ���� (�÷��̾�-Ÿ�� �Ÿ��� ����)
    _float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.5f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // �÷��̾� ���� ���� ���
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // ī�޶� ��ġ ��� (�÷��̾� ����)
    XMStoreFloat4(pOutCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);
    pOutCameraPosition->y += m_fHeightOffset * 2.5f;

    // LookAt ��ġ ��� (�÷��̾�� Ÿ���� �߰���)
    if (m_bZoomIn)
    {
        XMStoreFloat4(pOutLookAtPosition, XMLoadFloat4(&m_vLockedTargetPos));
    }
    else
    {
        XMStoreFloat4(pOutLookAtPosition, (XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&m_vLockedTargetPos)) * 0.5f);
    }
}

void CThirdPersonCamera::Get_ThirdCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition)
{
    _float4 vCameraOffset;
    XMStoreFloat4(&vCameraOffset, XMVectorSet(
        -m_fDistance * sinf(XMConvertToRadians(m_fYaw)) * cosf(XMConvertToRadians(m_fPitch)),
        m_fDistance * sinf(XMConvertToRadians(m_fPitch)) + m_fHeightOffset,
        -m_fDistance * cosf(XMConvertToRadians(m_fYaw)) * cosf(XMConvertToRadians(m_fPitch)),
        0.f
    ));

    XMStoreFloat4(pOutCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vCameraOffset));
    *pOutLookAtPosition = vPlayerPosition;
    pOutLookAtPosition->y += m_fHeightOffset;
}

void CThirdPersonCamera::Update_LockOnCam(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_LockOnCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    _float fLerpFactor = 1.0f - exp(-m_fLockOnTransitionSpeed * fTimeDelta);

    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        fLerpFactor
    ));

    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        fLerpFactor
    ));
}

void CThirdPersonCamera::Update_ThirdCam(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    // ���� factor ���
    _float fLerpFactor = 1.0f - exp(-m_fFollowSpeed * fTimeDelta);

    // ī�޶� ��ġ ����
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        fLerpFactor
    ));

    // �ü� ��ġ ����
    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        fLerpFactor
    ));
}

void CThirdPersonCamera::Update_TransitionCam(_float fTimeDelta)
{
    m_fTransitionProgress += fTimeDelta;
    _float t = min(m_fTransitionProgress / m_fTransitionDuration, 1.0f);
    t = t * t * (3.0f - 2.0f * t); // Smoothstep interpolation

    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLastLockedPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        t
    ));

    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        t
    ));
}

void CThirdPersonCamera::ParryingZoomIn(_float fTimeDelta)
{
    // �Ÿ��� FOV ����
    m_fDistance -= 30.f * fTimeDelta;
    m_fFovy -= XMConvertToRadians(1.f);

    // �ּҰ� ����
    m_fDistance = max(m_fDistance, 4.5f);
    m_fFovy = max(m_fFovy, XMConvertToRadians(45.f));

    _float4 vPlayerPosition;

    if (!m_bIsTargetLocked)
    {
        // �÷��̾� ��ġ�� Look ���� ��������
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
        _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

        // �÷��̾� ���� �������� Yaw ����
        _float3 vPlayerLookFloat3;
        XMStoreFloat3(&vPlayerLookFloat3, vPlayerLook);
        m_fYaw = XMConvertToDegrees(atan2f(vPlayerLookFloat3.x, vPlayerLookFloat3.z)); // 180�� �߰��Ͽ� ������ �ٶ󺸵��� ��

        // Get_ThirdCamPos �Լ��� ����Ͽ� ��ǥ ī�޶� ��ġ ���
        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

        // ���� ī�޶� ��ġ���� ��ǥ ��ġ�� ����
        _float fLerpFactor = 5.f * fTimeDelta; // ���� �ӵ� ����
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&vTargetCameraPosition), fLerpFactor));

        // LookAt ��ġ ������Ʈ
        XMStoreFloat4(&m_vLookAtPosition, XMLoadFloat4(&vTargetLookAtPosition));
    }
    else
    {
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_LockOnCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);
        // �÷��̾� ��ġ�� Look ���� ��������
        _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

        // �÷��̾� ���� �������� ī�޶� ��ġ ����
        _float3 vPlayerLookFloat3;
        XMStoreFloat3(&vPlayerLookFloat3, vPlayerLook);
        _float4 vPlayerBackward;
        XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat3(&vPlayerLookFloat3) * -1));
        XMStoreFloat4(&vTargetCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * m_fDistance);
        vTargetCameraPosition.y += m_fHeightOffset;


        _float fLerpFactor = 1.0f - exp(-m_fLockOnTransitionSpeed * fTimeDelta * 5.f);

        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
            XMLoadFloat4(&m_vCameraPosition),
            XMLoadFloat4(&vTargetCameraPosition),
            fLerpFactor
        ));

        XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
            XMLoadFloat4(&m_vLookAtPosition),
            XMLoadFloat4(&vTargetLookAtPosition),
            fLerpFactor
        ));

    }


    // ���� �Ϸ� üũ
    if (m_fDistance <= 4.5f && m_fFovy <= XMConvertToRadians(45.f))
    {
        m_bZoomIn = false;
    }
}

void CThirdPersonCamera::ParryingZoomOut(_float fTimeDelta)
{
    bool bDistanceReached = false;
    bool bFovyReached = false;

    m_fDistance += 6.f * fTimeDelta;
    m_fFovy += XMConvertToRadians(1.f);

    if (m_fDistance > 5.3f)
    {
        m_fDistance = 5.3f;
        bDistanceReached = true;
    }

    if (m_fFovy > XMConvertToRadians(60.f))
    {
        m_fFovy = XMConvertToRadians(60.f);
        bFovyReached = true;
    }

    // �� ������ ��� �����Ǹ� m_bZoomOut�� false�� ����
    if (bDistanceReached && bFovyReached)
    {
        m_bZoomOut = false;
        m_fShakeDuration = 0.4f;
    }
}

_bool CThirdPersonCamera::RaycastFromCameraToPlayer(const _float4& cameraPosition, const _float4& playerPosition, _float4* hitPoint, _float* hitDistance)
{
    if (m_pGameInstance->GetScene() == nullptr)
        return false;

    physx::PxVec3 origin(cameraPosition.x, cameraPosition.y, cameraPosition.z);
    physx::PxVec3 direction = physx::PxVec3(playerPosition.x - cameraPosition.x,
        playerPosition.y - cameraPosition.y,
        playerPosition.z - cameraPosition.z);
    _float maxDistance = direction.magnitude();
    direction.normalize();

    physx::PxRaycastBuffer hit;
    physx::PxQueryFilterData filterData;
    filterData.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::ePOSTFILTER;

    bool status = m_pGameInstance->GetScene()->raycast(origin, direction, maxDistance, hit, physx::PxHitFlag::eDEFAULT, filterData, &m_QueryFilterCallback);

    if (status && hit.hasBlock)
    {
        physx::PxVec3 hitPos = hit.block.position;
        *hitPoint = _float4(hitPos.x, hitPos.y, hitPos.z, 1.0f);
        *hitDistance = hit.block.distance;
        return true;
    }

    // �浹�� ������ �÷��̾� ��ġ�� ��ȯ
    *hitPoint = playerPosition;
    *hitDistance = maxDistance;
    return false;
}


void CThirdPersonCamera::TiltAdjust(_float fTimeDelta)
{
    if (!m_bTilt)
        return;

    m_fTiltTimer += fTimeDelta;

    if (m_bTiltingToTarget)
    {
        if (m_fTiltTimer <= m_fTiltDuration)
        {
            _float t = m_fTiltTimer / m_fTiltDuration;
            m_fRollAngle = LerpFloat(m_fStartRollAngle, m_fTargetRollAngle, SmoothStepFloat(t));
        }
        else
        {
            m_bTiltingToTarget = false;
            m_fTiltTimer = 0.f;
            m_fStartRollAngle = m_fTargetRollAngle;
        }
    }
    else
    {
        if (m_fTiltTimer <= m_fTiltRecoveryDuration)
        {
            // ���� ������ ����
            _float t = m_fTiltTimer / m_fTiltRecoveryDuration;
            m_fRollAngle = LerpFloat(m_fStartRollAngle, 0.f, SmoothStepFloat(t));
        }
        else
        {
            // Tilt ����
            m_bTilt = false;
            m_fRollAngle = 0.f;
            //return;
        }
    }

    // Look ���� ���
    _vector vLook = XMLoadFloat4(&m_vLookAtPosition) - XMLoadFloat4(&m_vCameraPosition);
    vLook = XMVector3Normalize(vLook);

    // Right ���� ���
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    vRight = XMVector3Normalize(vRight);

    // Up ���� ���
    _vector vUp = XMVector3Cross(vLook, vRight);

    // Roll ȸ�� ����
    XMMATRIX matRoll = XMMatrixRotationAxis(vLook, XMConvertToRadians(m_fRollAngle));
    vRight = XMVector3TransformNormal(vRight, matRoll);
    vUp = XMVector3TransformNormal(vUp, matRoll);

    // ��ȯ ��� ���� �� ����
    XMMATRIX matView = XMMatrixLookToLH(XMLoadFloat4(&m_vCameraPosition), vLook, vUp);
    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, matView);

    // Transform ������Ʈ ������Ʈ
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
}

void CThirdPersonCamera::StartTilt(_float targetAngle, _float duration, _float recoveryDuration)
{
    m_bTilt = true;
    m_fTiltTimer = 0.f;
    m_fStartRollAngle = m_fRollAngle;  // ���� ������ ���� ������ ����
    m_fTargetRollAngle = targetAngle;
    m_fTiltDuration = duration;
    m_fTiltRecoveryDuration = recoveryDuration;
    m_bTiltingToTarget = true;  // ��ǥ ������ Tilt ������ ǥ��
}

_float CThirdPersonCamera::Approach(_float fCurrent, _float fTarget, _float fDelta)
{
    _float fDifference = fTarget - fCurrent;

    if (fDifference > fDelta)
        return fCurrent + fDelta;
    if (fDifference < -fDelta)
        return fCurrent - fDelta;

    return fTarget;
}
// TargetLock_On �Լ� ����

void CThirdPersonCamera::TargetLock_On(_vector vTargetPos)
{
    XMStoreFloat4(&m_vLockedTargetPos, vTargetPos);
    m_bIsTargetLocked = true;
    m_bIsTransitioning = true;
    m_bIsFirstUpdate = true;
    m_fLockOnTransitionSpeed = 3.0f;

    // ��ȯ ���� ��ġ ����
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
    Get_LockOnCamPos(vPlayerPosition, &m_vTransitionEndPos, &m_vTransitionEndLookAt);
}

// TargetLock_Off �Լ� ����
void CThirdPersonCamera::TargetLock_Off()
{
    m_bIsTargetLocked = false;
    m_bIsTransitioning = false;
    m_bIsFirstUpdate = true;
}

_float CThirdPersonCamera::DistancePointLine(_vector point, _vector lineStart, _vector lineEnd)
{
    _vector projection = ProjectPointLine(point, lineStart, lineEnd);
    return XMVectorGetX(XMVector3Length(projection - point));
}

_vector CThirdPersonCamera::ProjectPointLine(_vector point, _vector lineStart, _vector lineEnd)
{
    _vector lineDirection = lineEnd - lineStart;
    _float lineLengthSq = XMVectorGetX(XMVector3LengthSq(lineDirection));

    if (lineLengthSq < 1e-12f)  // ������ ���̰� ���� 0�� ���
    {
        return lineStart;
    }

    _vector toPoint = point - lineStart;
    _float t = XMVectorGetX(XMVector3Dot(toPoint, lineDirection)) / lineLengthSq;

    // t�� 0�� 1 ���̷� ���� (���п� ���� ������ ����)
    t = max(0.f, min(1.f, t));

    return XMVectorAdd(lineStart, XMVectorScale(lineDirection, t));
}

void CThirdPersonCamera::TargetLockView(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    // �÷��̾�� Ÿ�� ������ ���� ���� ���
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // �÷��̾�� Ÿ�� ������ �Ÿ� ���
    _float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // ī�޶� �Ÿ� ���� (�÷��̾�-Ÿ�� �Ÿ��� ����)
    _float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.7f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // �÷��̾� ���� ���� ���
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // ī�޶� ��ġ ��� (�÷��̾� ����)
    _float4 vIdealCameraPos;
    XMStoreFloat4(&vIdealCameraPos, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);

    // ī�޶� ���� ����
    vIdealCameraPos.y += m_fHeightOffset * 1.75f;

    // ���� ī�޶� ��ġ���� �̻����� ��ġ�� �ε巴�� �̵�
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vIdealCameraPos),
        1.0f - exp(-m_fLerpFactor * fTimeDelta)
    ));

    // LookAt ��ġ ��� (Ÿ�� ��ġ)
    XMStoreFloat4(&m_vLookAtPosition, XMLoadFloat4(&m_vLockedTargetPos));
}

_float CThirdPersonCamera::PerlinNoise(_float x, _float y, int octaves, _float persistence)
{
    _float total = 0;
    _float frequency = 1;
    _float amplitude = 1;
    _float maxValue = 0;

    for (int i = 0; i < octaves; i++)
    {
        total += InterpolatedNoise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    // ����ȭ�� ���� -1���� 1 ���̷� ����
    _float normalizedValue = total / maxValue;
    return normalizedValue;
}

_float CThirdPersonCamera::InterpolatedNoise(_float x, _float y)
{
    int intX = static_cast<int>(floor(x));
    _float fracX = x - intX;
    int intY = static_cast<int>(floor(y));
    _float fracY = y - intY;

    _float v1 = SmoothNoise(intX, intY);
    _float v2 = SmoothNoise(intX + 1, intY);
    _float v3 = SmoothNoise(intX, intY + 1);
    _float v4 = SmoothNoise(intX + 1, intY + 1);

    _float i1 = CosineInterpolate(v1, v2, fracX);
    _float i2 = CosineInterpolate(v3, v4, fracX);

    return CosineInterpolate(i1, i2, fracY);
}

_float CThirdPersonCamera::SmoothNoise(int x, int y)
{
    _float corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16.0f;
    _float sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8.0f;
    _float center = Noise(x, y) / 4.0f;
    return corners + sides + center;
}

_float CThirdPersonCamera::Noise(int x, int y)
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

_float CThirdPersonCamera::CosineInterpolate(_float a, _float b, _float t)
{
    _float ft = t * 3.1415927f;
    _float f = (1 - cos(ft)) * 0.5f;
    return a * (1 - f) + b * f;
}

void CThirdPersonCamera::Key_Input(_float fTimeDelta)
{


    /*    ī�޶� ȸ��
       if (m_pGameInstance->Key_Pressing(DIK_LEFT))
           RotateCamera(-90.0f * fTimeDelta);
       else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
           RotateCamera(90.0f * fTimeDelta);

        �Ÿ� ���� (������)
       if (m_pGameInstance->Key_Pressing(DIK_UP))
           m_fDistance = max(2.0f, m_fDistance - 5.0f * fTimeDelta);
       else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
           m_fDistance = min(10.0f, m_fDistance + 5.0f * fTimeDelta);*/
}

// ThirdPersonCamera.cpp ����
void CThirdPersonCamera::Mouse_Move(_float fTimeDelta)
{
    if (m_bIsTargetLocked) return;
    _long MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
    _long MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);

    if (MouseMoveX != 0 || MouseMoveY != 0)
    {
        RotateCamera(MouseMoveX * m_fSensorX, MouseMoveY * m_fSensorY);

        // ���ϴ� ī�޶� ��ġ �� �Ÿ� ���
        _float4 vPlayerPosition;
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
        Get_ThirdCamPos(vPlayerPosition, &m_vDesiredCameraPosition, &m_vDesiredLookAtPosition);
        m_fDesiredDistance = m_fDistance;

        // ���콺�� ȭ�� �߾����� ����
        POINT ptCenter = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
        ClientToScreen(g_hWnd, &ptCenter);
        SetCursorPos(ptCenter.x, ptCenter.y);
    }
}
void CThirdPersonCamera::Update_Shake(_float fTimeDelta)
{
    if (!m_bIsShaking)
        return;

    m_fShakeTimer += fTimeDelta;

    if (m_fShakeTimer > m_fShakeDuration)
    {
        m_bIsShaking = false;
        m_vShakeOffset = { 0.f, 0.f, 0.f };
        m_vShakeRotation = { 0.f, 0.f, 0.f };
        return;
    }

    _float progress = m_fShakeTimer / m_fShakeDuration;
    _float decreaseAmount = 1.0f - powf(progress, m_fDecayRate);

    // ���ļ��� ����Ͽ� �ð��� ���� ��ȭ ���
    _float time = m_fShakeTimer * m_fShakeFrequency;

    // Position shake
    _float x = sin(time * 1.0f) * m_fPositionShakeAmount * decreaseAmount;
    _float y = sin(time * 1.1f) * m_fPositionShakeAmount * decreaseAmount;
    _float z = sin(time * 1.2f) * m_fPositionShakeAmount * decreaseAmount;

    // ������ �߰�
    x += RandomFloat(-0.5f, 0.5f) * m_fPositionShakeAmount * decreaseAmount;
    y += RandomFloat(-0.5f, 0.5f) * m_fPositionShakeAmount * decreaseAmount;
    z += RandomFloat(-0.5f, 0.5f) * m_fPositionShakeAmount * decreaseAmount;

    m_vShakeOffset = { x, y, z };

    // Rotation shake
    _float pitch = sin(time * 1.3f) * m_fRotationShakeAmount * decreaseAmount;
    _float yaw = sin(time * 1.4f) * m_fRotationShakeAmount * decreaseAmount;
    _float roll = sin(time * 1.5f) * m_fRotationShakeAmount * decreaseAmount;

    // ������ �߰�
    pitch += RandomFloat(-0.5f, 0.5f) * m_fRotationShakeAmount * decreaseAmount;
    yaw += RandomFloat(-0.5f, 0.5f) * m_fRotationShakeAmount * decreaseAmount;
    roll += RandomFloat(-0.5f, 0.5f) * m_fRotationShakeAmount * decreaseAmount;

    m_vShakeRotation = { pitch, yaw, roll };
}

void CThirdPersonCamera::Set_TPS_DESC(const THIRDPERSONCAMERA_DESC& tpsDesc)
{
    m_fInitialShakeAmount = tpsDesc.fInitialShakeAmount;
    m_fShakeSpeed = tpsDesc.fShakeSpeed;
    m_fShakeDuration = tpsDesc.fShakeDuration;
    m_fInitialShakeInterval = tpsDesc.fInitialShakeInterval;
    // �ٸ� �ʿ��� �����鵵 ���⿡�� ����
}

void CThirdPersonCamera::RotateCamera(_float fDeltaX, _float fDeltaY)
{
    m_fYaw += fDeltaX;
    m_fPitch += fDeltaY;

    // ������ 0-360 ���� ���� ����
    if (m_fYaw > 360.0f)
        m_fYaw -= 360.0f;
    else if (m_fYaw < 0.0f)
        m_fYaw += 360.0f;

    // ���� ���� ����
    m_fPitch = max(m_fMinPitch, min(m_fMaxPitch, m_fPitch));

    // ���� ������ ���� �Ÿ� ���� (���� �Լ� ���)
    _float fPitchRatio = (m_fPitch - m_fMinPitch) / (m_fMaxPitch - m_fMinPitch);
    m_fDistance = m_fMinDistance + (m_fMaxDistance - m_fMinDistance) * (1.0f - fPitchRatio * fPitchRatio);
}



CThirdPersonCamera* CThirdPersonCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CThirdPersonCamera* pInstance = new CThirdPersonCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CThirdPersonCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CThirdPersonCamera::Clone(void* pArg)
{
    CThirdPersonCamera* pInstance = new CThirdPersonCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CThirdPersonCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThirdPersonCamera::Free()
{
    Safe_Release(m_pPlayerTrans);
    //Safe_Release(m_pTargetTrans);
    Safe_Release(m_pTarget);

    __super::Free();
}

physx::PxQueryHitType::Enum CCameraQueryFilterCallback::preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags)
{
    // GROUP_PLAYER�� �浹 �� ����
    if (filterData.word0 & GROUP_PLAYER)
    {
        return physx::PxQueryHitType::eNONE;
    }
    // �ٸ� �׷�� �浹 �� Ȯ�� (hit ��ȯ)
    return physx::PxQueryHitType::eBLOCK;
}

physx::PxQueryHitType::Enum CCameraQueryFilterCallback::postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor)
{
    // postFilter���� �߰����� ���͸� ������ ������ �� �ֽ��ϴ�.
    // ���� ���, Ư�� ���ǿ� ���� hit�� �����ϰų� ����� �� �ֽ��ϴ�.
    return physx::PxQueryHitType::eBLOCK;
}

