#pragma once
#include "Camera.h"

BEGIN(Client)

class CCameraQueryFilterCallback : public physx::PxQueryFilterCallback
{
public:
    virtual physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags) override;
    virtual physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor) override;
};

class CThirdPersonCamera final : public CCamera
{
public:
    enum class CAMERA_PHASE { CAMERA_FOLLOW = 0, CAMERA_REVOLUTION, CAMERA_RETURN, CAMERA_ZOOMIN, CAMERA_END };

    typedef struct THIRDPERSONCAMERA_DESC : public CCamera::CAMERA_DESC
    {
        _float fInitialShakeAmount = 0.1f;
        _float fShakeSpeed = 1.0f;
        _float fShakeDuration = 0.5f;
        _float fInitialShakeInterval = 0.05f;
        _float fSensor = 0.f;
        CTransform* pPlayerTrans = nullptr;
    } THIRDPERSONCAMERA_DESC;

private:
    CThirdPersonCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CThirdPersonCamera(const CThirdPersonCamera& rhs);
    virtual ~CThirdPersonCamera() = default;

public:
    // �ʱ�ȭ �� ������Ʈ �Լ�
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(_float fTimeDelta) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual void Late_Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    // ī�޶� ��ġ �� ���� ��� �Լ�
    void Get_LockOnCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition);
    void Get_ThirdCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition);
    void Update_LockOnCam(_float fTimeDelta);
    void Update_ThirdCam(_float fTimeDelta);
    void Update_TransitionCam(_float fTimeDelta);
    void RotateCamera(_float fDeltaX, _float fDeltaY);

    // Ÿ�� ���� ���� �Լ�
    void TargetLock_On(_vector vTargetPos);
    void TargetLock_Off();
    void TargetLockView(_float fTimeDelta);

    // ī�޶� �� ���� �Լ�
    void ParryingZoomIn(_float fTimeDelta);
    void ParryingZoomOut(_float fTimeDelta);
    void Set_ZoomIn() { m_bZoomIn = true; }
    void Set_ZoomOut() { m_bZoomOut = true; }

    // ī�޶� ȿ�� �Լ�
    void Shake_Camera(_float duration, _float positionAmount, _float rotationAmount, _float frequency)
    {
        m_bIsShaking = true;
        m_fShakeTimer = 0.f;
        m_fShakeDuration = duration;
        m_fPositionShakeAmount = positionAmount;
        m_fRotationShakeAmount = rotationAmount;
        m_fShakeFrequency = frequency;

    }
    void Update_Shake(_float fTimeDelta);
    void StartTilt(_float targetAngle, _float duration, _float recoveryDuration);
    void TiltAdjust(_float fTimeDelta);

    // �浹 �� ����ĳ��Ʈ �Լ�
    _bool RaycastFromCameraToPlayer(const _float4& cameraPosition, const _float4& playerPosition, _float4* hitPoint, _float* hitDistance);

    // Ư�� ī�޶� ������ �Լ�
    void Revolution360(_float fTime);

    // ���� �Լ�
    void Set_TPS_DESC(const THIRDPERSONCAMERA_DESC& tpsDesc);
    void Set_Camera_Phase(CAMERA_PHASE ePhase);
    void Set_Revolution_Time(_float fTime) { m_fRevolutionTime = fTime; }
    void Set_Height(_float fHeight) { m_fHeight = fHeight; }
    void Set_ZoomSpeed(_float fZoomSpeed) { m_fZoomSpeed = fZoomSpeed; }
    void Set_MaxZoomDistance(_float fMaxZoomDistance) { m_fMaxZoomDistance = fMaxZoomDistance; }

    // getter �Լ�
    _float Get_ShakeTimer() const { return m_fShakeTimer; }
    _float Get_ShakeDuration() const { return m_fShakeDuration; }
    _bool Get_isShaking() const { return m_bIsShaking; }

private:
    // ��ƿ��Ƽ �Լ�
    _float DistancePointLine(_vector point, _vector lineStart, _vector lineEnd);
    _vector ProjectPointLine(_vector point, _vector lineStart, _vector lineEnd);
    _float Approach(_float fCurrent, _float fTarget, _float fDelta);
    _float PerlinNoise(_float x, _float y, int octaves, _float persistence);
    _float InterpolatedNoise(_float x, _float y);
    _float SmoothNoise(int x, int y);
    _float Noise(int x, int y);
    _float CosineInterpolate(_float a, _float b, _float t);
    _float LinearInterpolate(_float a, _float b, _float x);

    // �Է� ó�� �Լ�
    void Key_Input(_float fTimeDelta);
    void Mouse_Move(_float fTimeDelta);

public:
    static _bool m_bIsTargetLocked;
    static _float4 m_vLockedTargetPos;

private:
#pragma region 3��Ī ī�޶� �⺻ ������
    // ī�޶� �⺻ ����
    _float m_fYaw = 0.0f;
    _float m_fPitch = 0.0f;
    _float m_fDistance = 4.5f; //�÷��̾�, ī�޶� �����Ÿ�
    _float m_fMinHeight = 2.5f;
    _float m_fMinPitch = -45.f;
    _float m_fMaxPitch = 45.f;
    _float m_fMinDistance = 3.0f;
    _float m_fMaxDistance = 6.5f;
    _float m_fHeightOffset = 1.8f;
    _float m_fSensorX = 0.1f;
    _float m_fSensorY = 0.05f;
    _float m_fSensor = { 0.0f };
    _float m_fHeight = 0.f;
#pragma endregion 3��Ī ī�޶� �⺻ ������

#pragma region ī�޶� ��ġ, ����
    _float4 m_vCameraPosition; //ī�޶� ������ �����ϴ� ����
    _float4 m_vLookAtPosition; //ī�޶� Look �����ϴ� ����
    _float4 m_vDesiredCameraPosition;
    _float4 m_vDesiredLookAtPosition;
    _float m_fDesiredDistance;
    const _vector m_HeightOffset = { 0.f, 10.f, 0.f, 0.f };
    _vector m_vLerpPos = { 0.f, };
    _vector LerpAt;
#pragma endregion ī�޶� ��ġ, ����

#pragma region �÷��̾�, Ÿ�ٶ� �� ���۷���
    // �÷��̾� �� Ÿ�� ����
    CTransform* m_pPlayerTrans = { nullptr };
    class CGameObject* m_pTarget = nullptr;
    CTransform* m_pTargetTrans = nullptr;

#pragma endregion �÷��̾�, Ÿ�ٶ� �� ���۷���

#pragma region ī�޶� ����
    _bool m_bFix = false;
    _bool m_bIsMove = false;
    _bool IsNear = false;
    _bool m_bZoomIn = false;
    _bool m_bZoomOut = false;
    _bool m_bRevolution360 = false;
    _bool m_bTiltAdjust = false;
#pragma endregion ī�޶� ����

#pragma region ī�޶� Transition
    _float m_fLockOnTransitionSpeed = 3.0f;
    bool m_bIsFirstUpdate = true;
    bool m_bIsTransitioning = false;
    _float m_fTransitionSpeed = 2.0f;
    _float m_fTransitionThreshold = 0.1f;
    _float4 m_vTransitionEndPos;
    _float4 m_vTransitionEndLookAt;

    _float4 m_vLastLockedPosition;
    _float m_fTransitionProgress = 0.0f;
    const _float m_fTransitionDuration = 1.0f; // ��ȯ�� �ɸ��� �ð� (��)
#pragma endregion ī�޶� Transition

#pragma region ī�޶� Shaking
    bool m_bIsShaking = false;
    _float m_fShakeTimer = 0.0f;
    _float m_fIntensity = 0.1f;
    _float m_fShakeDuration = 0.4f;
    _float m_fShakeSpeed = 1.5f;
    _float m_fShakeAmount = 3.f;
    _float m_fShakeTargetThreshold = 0.01f;
    _vector m_vBeforeShakePos = { 0.f, 0.f, 0.f, 1.f };
    _float4 m_vShakeTargetPosition = { 0.f, 0.f, 0.f, 1.f };
    _float4 m_qShakeRotation = { 0.f, 0.f, 0.f, 1.f };
    _float4 m_vShakeStart = { 0.f, 0.f, 0.f, 1.f };
    _float4 m_vShakeTarget = { 0.f, 0.f, 0.f, 1.f };
    bool m_bLockWidth = false;
    _float m_fPositionShakeAmount = 0.1f;
    _float m_fRotationShakeAmount = 0.1f;
    _float m_fShakeFrequency = 25.f;
    _float m_fDecayRate = 0.8f;
    _float3 m_vShakeOffset = { 0.f, 0.f, 0.f };
    _float3 m_vShakeRotation = { 0.f, 0.f, 0.f };
    _float m_fInitialShakeInterval = 0.1f;
    _float m_fInitialShakeAmount = 3.f;
    _float m_fShakeInterval = 0.1f;
    _float m_fShakeIntervalTimer = 0.0f;
#pragma endregion ī�޶� Shaking

#pragma region ī�޶� Tilt
    // ī�޶� ƿƮ ȿ��
    _float m_fRollAngle = 30.f;
    _float m_fRollSpeed = 30.f;
    const _float m_fMaxRollAngle = 45.f;
    bool m_bTilt = false;
    _float m_fTiltTimer = 0.f;
    _float m_fTiltDuration = 0.5f;
    _float m_fTiltRecoveryDuration = 0.5f;
    _float m_fTargetRollAngle = 0.f;
    _float m_fStartRollAngle = 0.f;
    _float m_fStartTiltTimer = 0.f;
    bool m_bTiltingToTarget = false;
#pragma endregion ī�޶� Tilt


    // ī�޶� �� �� ȸ�� ����
    _float m_fZoomDistance = 0.f;
    _float m_fRevolutionTime = 3.f;
    _float m_fRevolutionAccTime = 0.f;
    _float m_fTargetHeight = 6.f;
    _float m_fZoomSpeed = 5.f;
    _float m_fHeightChangeSpeed = 2.f;
    _float m_fZoomAccTime = 0.f;
    _float m_fMaxZoomDistance = 10.f;
    _float m_fZoomDuration = 3.f;
    _float m_fZoomDelay = 0.f;

    // ��Ÿ ����
    CAMERA_PHASE m_eCameraPhase = CAMERA_PHASE::CAMERA_FOLLOW;
    _float m_fLerpFactor = 8.0f;
    _float m_fFollowSpeed = 10.0f;
    _float4 vEye;
    _float4 vDir;
    _float fSpeed = 0.f;
    _float4 vAt;
    _float fDistance = 20.f;
    _float m_fAngle = 0.f;
    _float4 m_vCameraRotation = { 0.0f, 0.0f, 0.0f, 1.0f };

    // �浹 ó��
    CCameraQueryFilterCallback m_QueryFilterCallback;

public:
    static CThirdPersonCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END