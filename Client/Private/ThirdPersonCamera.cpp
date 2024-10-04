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

    // 초기 카메라 위치 설정
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
        m_fShakeAmount = m_fInitialShakeAmount; // 초기 셰이크 강도로 리셋
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

    // 플레이어 위치 가져오기
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    // 목표 카메라 위치 및 시선 계산
    _float4 vTargetCameraPosition, vTargetLookAtPosition;

    if (m_bIsTargetLocked)
    {
        // 타겟 락온 상태 처리
        if (m_pTarget != nullptr)
        {
            CMonster* pMonster = dynamic_cast<CMonster*>(m_pTarget);
            if (pMonster != nullptr)
            {
                _vector vTP = pMonster->Get_MonsterPos();
                _vector vCamPos = XMLoadFloat4(&m_vCameraPosition);

                // 카메라와 타겟 사이의 직선 방향 벡터
                _vector vCamToTarget = XMVector3Normalize(vTP - vCamPos);

                // 카메라에서 타겟 방향으로 일정 거리(예: 10.0f) 떨어진 점
                _vector vProjectedPoint = vCamPos + vCamToTarget * 10.0f;

                // 투영점과 타겟 사이의 거리 계산
                _float distToRay = XMVectorGetX(XMVector3Length(vTP - vProjectedPoint));

                // 카메라와 투영점 사이의 거리 계산 (깊이)
                _float depthOnRay = 10.0f;

                // 거리에 가중치를 주어 깊이도 고려 (예: 깊이에 0.5 가중치)
                _float weightedDistance = distToRay + depthOnRay * 0.5f;

                // 가중 거리를 기준으로 락온 해제 판단
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

            // 전환 완료 여부 확인
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

        // 레이캐스트 수행 (락온 상태가 아닐 때만)
        _float4 hitPoint;
        _float hitDistance;
        bool bHitObstacle = RaycastFromCameraToPlayer(vTargetCameraPosition, vPlayerPosition, &hitPoint, &hitDistance);

        // 목표 거리 계산
        _float fTargetDistance = m_fDistance;
        if (bHitObstacle && hitDistance < m_fDistance)
        {
            fTargetDistance = max(hitDistance * 0.7f, m_fMinDistance * 0.66f);
        }

        // 거리 부드럽게 조정
        _float fDistanceLerpSpeed = 5.0f;
        m_fDistance = XMVectorGetX(XMVectorLerp(XMLoadFloat(&m_fDistance), XMLoadFloat(&fTargetDistance), 1.0f - exp(-fDistanceLerpSpeed * fTimeDelta)));

        // 새로운 카메라 위치 계산
        _float4 cameraDirection;
        XMStoreFloat4(&cameraDirection, XMVector4Normalize(XMLoadFloat4(&vTargetCameraPosition) - XMLoadFloat4(&vPlayerPosition)));
        XMStoreFloat4(&vTargetCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&cameraDirection) * m_fDistance);

        // 카메라 높이 제한
        vTargetCameraPosition.y = max(vTargetCameraPosition.y, vPlayerPosition.y + m_fMinHeight * 0.5f);

        // 부드러운 카메라 이동
        _float fPositionLerpSpeed = 10.0f;
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
            XMLoadFloat4(&m_vCameraPosition),
            XMLoadFloat4(&vTargetCameraPosition),
            1.0f - exp(-fPositionLerpSpeed * fTimeDelta)
        ));

        // LookAt 위치 업데이트
        XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
            XMLoadFloat4(&m_vLookAtPosition),
            XMLoadFloat4(&vTargetLookAtPosition),
            1.0f - exp(-fPositionLerpSpeed * fTimeDelta)
        ));
    }

    // 줌인/아웃 상태 처리
    if (m_bZoomIn)
    {
        ParryingZoomIn(fTimeDelta);
    }
    else if (m_bZoomOut)
    {
        ParryingZoomOut(fTimeDelta);
    }


    Update_Shake(fTimeDelta);

    // 셰이크 효과 적용
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

    // 최종 카메라 위치 및 방향 설정
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

            // 가중치 거리가 30 이하이고 실제 거리도 30 이하인 경우에만 타겟 설정
            if (closestMonster != nullptr && closestWeightedDistance <= 30.0f && closestActualDistance <= 30.0f)
            {
                // 이전 타겟이 있다면 레퍼런스 카운트 감소
                if (m_pTarget != nullptr)
                    Safe_Release(m_pTarget);

                // 새로운 타겟 설정 및 레퍼런스 카운트 증가
                m_pTarget = closestMonster;
                Safe_AddRef(m_pTarget);

                // 타겟 락온
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
                // 가장 가까운 몬스터가 없거나 거리 조건을 만족하지 않는 경우
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
            // 이미 타겟이 락온된 상태에서 다시 클릭한 경우
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
    // 플레이어와 타겟 사이의 방향 벡터 계산
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // 플레이어와 타겟 사이의 거리 계산
    _float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // 카메라 거리 조정 (플레이어-타겟 거리에 따라)
    _float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.5f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // 플레이어 뒤쪽 방향 계산
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // 카메라 위치 계산 (플레이어 뒤쪽)
    XMStoreFloat4(pOutCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);
    pOutCameraPosition->y += m_fHeightOffset * 2.5f;

    // LookAt 위치 계산 (플레이어와 타겟의 중간점)
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

    // 보간 factor 계산
    _float fLerpFactor = 1.0f - exp(-m_fFollowSpeed * fTimeDelta);

    // 카메라 위치 보간
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        fLerpFactor
    ));

    // 시선 위치 보간
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
    // 거리와 FOV 조정
    m_fDistance -= 30.f * fTimeDelta;
    m_fFovy -= XMConvertToRadians(1.f);

    // 최소값 제한
    m_fDistance = max(m_fDistance, 4.5f);
    m_fFovy = max(m_fFovy, XMConvertToRadians(45.f));

    _float4 vPlayerPosition;

    if (!m_bIsTargetLocked)
    {
        // 플레이어 위치와 Look 벡터 가져오기
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
        _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

        // 플레이어 뒤쪽 방향으로 Yaw 조정
        _float3 vPlayerLookFloat3;
        XMStoreFloat3(&vPlayerLookFloat3, vPlayerLook);
        m_fYaw = XMConvertToDegrees(atan2f(vPlayerLookFloat3.x, vPlayerLookFloat3.z)); // 180도 추가하여 뒤쪽을 바라보도록 함

        // Get_ThirdCamPos 함수를 사용하여 목표 카메라 위치 계산
        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

        // 현재 카메라 위치에서 목표 위치로 보간
        _float fLerpFactor = 5.f * fTimeDelta; // 보간 속도 조절
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&vTargetCameraPosition), fLerpFactor));

        // LookAt 위치 업데이트
        XMStoreFloat4(&m_vLookAtPosition, XMLoadFloat4(&vTargetLookAtPosition));
    }
    else
    {
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_LockOnCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);
        // 플레이어 위치와 Look 벡터 가져오기
        _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

        // 플레이어 뒤쪽 방향으로 카메라 위치 조정
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


    // 줌인 완료 체크
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

    // 두 조건이 모두 만족되면 m_bZoomOut을 false로 설정
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

    // 충돌이 없으면 플레이어 위치를 반환
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
            // 원래 각도로 복귀
            _float t = m_fTiltTimer / m_fTiltRecoveryDuration;
            m_fRollAngle = LerpFloat(m_fStartRollAngle, 0.f, SmoothStepFloat(t));
        }
        else
        {
            // Tilt 종료
            m_bTilt = false;
            m_fRollAngle = 0.f;
            //return;
        }
    }

    // Look 벡터 계산
    _vector vLook = XMLoadFloat4(&m_vLookAtPosition) - XMLoadFloat4(&m_vCameraPosition);
    vLook = XMVector3Normalize(vLook);

    // Right 벡터 계산
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    vRight = XMVector3Normalize(vRight);

    // Up 벡터 계산
    _vector vUp = XMVector3Cross(vLook, vRight);

    // Roll 회전 적용
    XMMATRIX matRoll = XMMatrixRotationAxis(vLook, XMConvertToRadians(m_fRollAngle));
    vRight = XMVector3TransformNormal(vRight, matRoll);
    vUp = XMVector3TransformNormal(vUp, matRoll);

    // 변환 행렬 생성 및 적용
    XMMATRIX matView = XMMatrixLookToLH(XMLoadFloat4(&m_vCameraPosition), vLook, vUp);
    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, matView);

    // Transform 컴포넌트 업데이트
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
}

void CThirdPersonCamera::StartTilt(_float targetAngle, _float duration, _float recoveryDuration)
{
    m_bTilt = true;
    m_fTiltTimer = 0.f;
    m_fStartRollAngle = m_fRollAngle;  // 현재 각도를 시작 각도로 설정
    m_fTargetRollAngle = targetAngle;
    m_fTiltDuration = duration;
    m_fTiltRecoveryDuration = recoveryDuration;
    m_bTiltingToTarget = true;  // 목표 각도로 Tilt 중임을 표시
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
// TargetLock_On 함수 수정

void CThirdPersonCamera::TargetLock_On(_vector vTargetPos)
{
    XMStoreFloat4(&m_vLockedTargetPos, vTargetPos);
    m_bIsTargetLocked = true;
    m_bIsTransitioning = true;
    m_bIsFirstUpdate = true;
    m_fLockOnTransitionSpeed = 3.0f;

    // 전환 종료 위치 설정
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
    Get_LockOnCamPos(vPlayerPosition, &m_vTransitionEndPos, &m_vTransitionEndLookAt);
}

// TargetLock_Off 함수 수정
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

    if (lineLengthSq < 1e-12f)  // 라인의 길이가 거의 0인 경우
    {
        return lineStart;
    }

    _vector toPoint = point - lineStart;
    _float t = XMVectorGetX(XMVector3Dot(toPoint, lineDirection)) / lineLengthSq;

    // t를 0과 1 사이로 제한 (선분에 대한 투영을 위해)
    t = max(0.f, min(1.f, t));

    return XMVectorAdd(lineStart, XMVectorScale(lineDirection, t));
}

void CThirdPersonCamera::TargetLockView(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    // 플레이어와 타겟 사이의 방향 벡터 계산
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // 플레이어와 타겟 사이의 거리 계산
    _float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // 카메라 거리 조정 (플레이어-타겟 거리에 따라)
    _float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.7f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // 플레이어 뒤쪽 방향 계산
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // 카메라 위치 계산 (플레이어 뒤쪽)
    _float4 vIdealCameraPos;
    XMStoreFloat4(&vIdealCameraPos, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);

    // 카메라 높이 조정
    vIdealCameraPos.y += m_fHeightOffset * 1.75f;

    // 현재 카메라 위치에서 이상적인 위치로 부드럽게 이동
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vIdealCameraPos),
        1.0f - exp(-m_fLerpFactor * fTimeDelta)
    ));

    // LookAt 위치 계산 (타겟 위치)
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

    // 정규화된 값을 -1에서 1 사이로 매핑
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


    /*    카메라 회전
       if (m_pGameInstance->Key_Pressing(DIK_LEFT))
           RotateCamera(-90.0f * fTimeDelta);
       else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
           RotateCamera(90.0f * fTimeDelta);

        거리 조절 (선택적)
       if (m_pGameInstance->Key_Pressing(DIK_UP))
           m_fDistance = max(2.0f, m_fDistance - 5.0f * fTimeDelta);
       else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
           m_fDistance = min(10.0f, m_fDistance + 5.0f * fTimeDelta);*/
}

// ThirdPersonCamera.cpp 수정
void CThirdPersonCamera::Mouse_Move(_float fTimeDelta)
{
    if (m_bIsTargetLocked) return;
    _long MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
    _long MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);

    if (MouseMoveX != 0 || MouseMoveY != 0)
    {
        RotateCamera(MouseMoveX * m_fSensorX, MouseMoveY * m_fSensorY);

        // 원하는 카메라 위치 및 거리 계산
        _float4 vPlayerPosition;
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
        Get_ThirdCamPos(vPlayerPosition, &m_vDesiredCameraPosition, &m_vDesiredLookAtPosition);
        m_fDesiredDistance = m_fDistance;

        // 마우스를 화면 중앙으로 고정
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

    // 주파수를 사용하여 시간에 따른 변화 계산
    _float time = m_fShakeTimer * m_fShakeFrequency;

    // Position shake
    _float x = sin(time * 1.0f) * m_fPositionShakeAmount * decreaseAmount;
    _float y = sin(time * 1.1f) * m_fPositionShakeAmount * decreaseAmount;
    _float z = sin(time * 1.2f) * m_fPositionShakeAmount * decreaseAmount;

    // 랜덤성 추가
    x += RandomFloat(-0.5f, 0.5f) * m_fPositionShakeAmount * decreaseAmount;
    y += RandomFloat(-0.5f, 0.5f) * m_fPositionShakeAmount * decreaseAmount;
    z += RandomFloat(-0.5f, 0.5f) * m_fPositionShakeAmount * decreaseAmount;

    m_vShakeOffset = { x, y, z };

    // Rotation shake
    _float pitch = sin(time * 1.3f) * m_fRotationShakeAmount * decreaseAmount;
    _float yaw = sin(time * 1.4f) * m_fRotationShakeAmount * decreaseAmount;
    _float roll = sin(time * 1.5f) * m_fRotationShakeAmount * decreaseAmount;

    // 랜덤성 추가
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
    // 다른 필요한 변수들도 여기에서 설정
}

void CThirdPersonCamera::RotateCamera(_float fDeltaX, _float fDeltaY)
{
    m_fYaw += fDeltaX;
    m_fPitch += fDeltaY;

    // 각도를 0-360 범위 내로 유지
    if (m_fYaw > 360.0f)
        m_fYaw -= 360.0f;
    else if (m_fYaw < 0.0f)
        m_fYaw += 360.0f;

    // 수직 각도 제한
    m_fPitch = max(m_fMinPitch, min(m_fMaxPitch, m_fPitch));

    // 수직 각도에 따른 거리 조정 (보간 함수 사용)
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
    // GROUP_PLAYER와 충돌 시 무시
    if (filterData.word0 & GROUP_PLAYER)
    {
        return physx::PxQueryHitType::eNONE;
    }
    // 다른 그룹과 충돌 시 확대 (hit 반환)
    return physx::PxQueryHitType::eBLOCK;
}

physx::PxQueryHitType::Enum CCameraQueryFilterCallback::postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor)
{
    // postFilter에서 추가적인 필터링 로직을 구현할 수 있습니다.
    // 예를 들어, 특정 조건에 따라 hit를 무시하거나 허용할 수 있습니다.
    return physx::PxQueryHitType::eBLOCK;
}

