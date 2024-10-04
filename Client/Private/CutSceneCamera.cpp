#include "CutSceneCamera.h"
#include "GameInstance.h"
#include "TransitionCamera.h"
#include "EventTrigger.h"
#include "Boss_Juggulus.h"
#include "Andras.h"
#include "Malkhel.h"
#include "Cloud.h"

#include"UI_Manager.h"

CCutSceneCamera::CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCutSceneCamera::CCutSceneCamera(const CCutSceneCamera& rhs)
    : CCamera{ rhs }
{
}

HRESULT CCutSceneCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCutSceneCamera::Initialize(void* pArg)
{
    CUTSCENECAMERA_DESC* pDesc = (CUTSCENECAMERA_DESC*)pArg;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_KeyFrames = pDesc->KeyFrames;

    Load_CameraKeyFrames();
    Set_CutSceneIdx(0);

    vector<CCamera::CameraKeyFrame> vec;

    Add_CutScene(m_AllCutScenes[SCENE_MANTARI]);

    return S_OK;
}

void CCutSceneCamera::Priority_Tick(_float fTimeDelta)
{

}

void CCutSceneCamera::Tick(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_M))
    {
        StartIntenseShaking(2.0f, 5.f); // 2초 동안 5배 강한 셰이킹 적용
    }

    fTimeDelta /= fSlowValue;

    if (!m_bCamActivated || m_AllCutScenes.empty() || m_iCurrentCutSceneIdx >= m_AllCutScenes.size())
        return;

    list<CGameObject*> qte = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_QTESword"));
    if (qte.empty())
    {
        m_pProjectileMatrix = nullptr;
    }

    // 투사체가 사라졌는지 확인
    if (m_iCurrentCutSceneIdx == SCENE_ANDRAS_PROJECTILE && !m_pProjectileMatrix)
    {
        EndCutScene();
        return;
    }

    vector<CameraKeyFrame>& currentCutScene = m_AllCutScenes[m_iCurrentCutSceneIdx];

    if (currentCutScene.empty())
        return;

    if (m_bAnimationFinished || m_bPaused)
    {
        // m_pGameInstance->Set_MainCamera(CAM_THIRDPERSON);

        CUI_Manager::GetInstance()->Setting_Cinematic(true); // 컷씬 끝날 때
        //컷씬 트렌지션
        CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

        pTCDesc.fFovy = XMConvertToRadians(60.f);
        pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
        pTCDesc.fNear = 0.1f;
        pTCDesc.fFar = m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]->Get_Far();

        pTCDesc.fSpeedPerSec = 40.f;
        pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

        pTCDesc.iStartCam = CAM_CUTSCENE;
        pTCDesc.iEndCam = CAM_THIRDPERSON;
        pTCDesc.fTransitionTime = 1.f;

        switch (m_iCurrentCutSceneIdx)
        {
        case SCENE_MANTARI:
            // UI BossText 생성
            CUI_Manager::GetInstance()->Create_BossText(true);
            break;
        case SCENE_JUGGULAS:
        {
            CPhysXComponent_Character* pBossCom = dynamic_cast<CPhysXComponent_Character*>(m_pGameInstance->Get_Component(LEVEL_JUGGLAS, TEXT("Layer_Boss"), TEXT("Com_PhysX")));
            if(pBossCom)
                pBossCom->Set_Position({ -450.f , 56.f , -3.f, 1.f });
            
            CTransform* pBossTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_JUGGLAS, TEXT("Layer_Boss"), TEXT("Com_Transform")));
            if (pBossTransform)
				pBossTransform->Set_State(CTransform::STATE_POSITION, { -450.f , 56.f , -3.f, 1.f });

            // UI BossText 생성
            CUI_Manager::GetInstance()->Create_BossText(true);
        }
        break;
        case SCENE_ANDRAS_PHASE2:
        {
            dynamic_cast<CAndras*>(m_pGameInstance->Get_GameObjects_Ref(LEVEL_ANDRASARENA, TEXT("Layer_Monster")).front())->Phase_Two();
        }
            break;
        case SCENE_BLOODMOON:
        {
            Set_CutSceneIdx(SCENE_MALKHEL_DESCEND);
            return;
        }
            break;
        case SCENE_MALKHEL_DESCEND:
        {
            dynamic_cast<CMalkhel*>(m_pGameInstance->Get_GameObjects_Ref(LEVEL_GRASSLAND, TEXT("Layer_Monster")).front())->Activate_Malkhel();

            // UI BossText 생성
            CUI_Manager::GetInstance()->Create_BossText(true);
        }
        break;
        case SCENE_GRASSLAND_HANGAROUND:
        {
            list<CGameObject*> cloud = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GRASSLAND, TEXT("Layer_Cloud"));
            dynamic_cast<CCloud*>(cloud.front())->Set_ShaderPath(3);
        }
            break;
        default:
        {
        }
        break;
        }

        if (FAILED(m_pGameInstance->Add_Camera(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
        {
            MSG_BOX("FAILED");
            return;
        }

        return;
    }

    if (!m_bPaused)
    {
        if (m_iCurrentKeyFrame < currentCutScene.size())
        {
            CameraKeyFrame& currentKeyFrame = currentCutScene[m_iCurrentKeyFrame];
            CameraKeyFrame& nextKeyFrame = (m_iCurrentKeyFrame + 1 < currentCutScene.size())
                ? currentCutScene[m_iCurrentKeyFrame + 1]
                : currentKeyFrame;

            float totalDuration = nextKeyFrame.fTime - currentKeyFrame.fTime;
            float t = (totalDuration > 0) ? m_fKeyFrameTime / totalDuration : 1.0f;

            // 속도 변화 로직 적용
            float speedMultiplier = CalculateSpeedMultiplier(currentKeyFrame, t);

            // 부드러운 이징 적용
            t = EaseInOutCubic(t);

            m_fKeyFrameTime += fTimeDelta * speedMultiplier;

            _matrix matCurrent, matTarget;

            bool projectileDisappeared = false;

            // 현재 키프레임에 대한 처리
            if (m_iCurrentCutSceneIdx == SCENE_ANDRAS_PROJECTILE &&
                (currentKeyFrame.bTrackProjectile || m_iCurrentKeyFrame == 2))
            {
                if (m_pProjectileMatrix && m_pPlayerMatrix)
                {
                    matCurrent = CalculateCameraMatrixFromProjectile(*m_pProjectileMatrix, *m_pPlayerMatrix, currentKeyFrame.offset, currentKeyFrame.lookOffset);
                }
                else
                {
                    projectileDisappeared = true;
                }
            }
            else
            {
                matCurrent = XMLoadFloat4x4(&currentKeyFrame.matWorld);
            }

            // 다음 키프레임에 대한 처리
            if (m_iCurrentCutSceneIdx == SCENE_ANDRAS_PROJECTILE &&
                (nextKeyFrame.bTrackProjectile || m_iCurrentKeyFrame == 1))
            {
                if (m_pProjectileMatrix && m_pPlayerMatrix)
                {
                    matTarget = CalculateCameraMatrixFromProjectile(*m_pProjectileMatrix, *m_pPlayerMatrix, nextKeyFrame.offset, nextKeyFrame.lookOffset);
                }
                else
                {
                    projectileDisappeared = true;
                }
            }
            else
            {
                matTarget = XMLoadFloat4x4(&nextKeyFrame.matWorld);
            }
            _matrix matLerpedWorld;

            if (projectileDisappeared)
            {
                // 투사체가 사라진 경우, 즉시 다음 키프레임으로 전환
                m_iCurrentKeyFrame++;
                m_fKeyFrameTime = 0.0f;

                // 다음 키프레임이 존재하는 경우 그 키프레임의 행렬을 사용
                if (m_iCurrentKeyFrame < currentCutScene.size())
                {
                    matLerpedWorld = XMLoadFloat4x4(&currentCutScene[m_iCurrentKeyFrame].matWorld);
                }
                else
                {
                    // 마지막 키프레임이었다면 컷신 종료
                    m_bAnimationFinished = true;
                    return;
                }
            }
            else
            {
                // 정상적인 경우의 행렬 보간
                matLerpedWorld = XMMatrixSlerp(matCurrent, matTarget, t);

                // 셰이킹 및 추가 보간 ...
                ApplyShaking(matLerpedWorld, fTimeDelta);
            }

            // 보간된 월드 행렬 설정
            m_pTransformCom->Set_WorldMatrix(matLerpedWorld);

            // 보간된 시야각 설정
            _float fLerpedFovy = XMConvertToRadians(XMConvertToDegrees(currentKeyFrame.fFovy) +
                (XMConvertToDegrees(nextKeyFrame.fFovy) - XMConvertToDegrees(currentKeyFrame.fFovy)) * t);
            Set_Fovy(fLerpedFovy);

            if (t >= 1.0f)
            {
                m_iCurrentKeyFrame++;
                m_fKeyFrameTime = 0.0f;

                if (m_iCurrentKeyFrame >= currentCutScene.size())
                {
                    m_bAnimationFinished = true;
                }
            }
        }
    }

    __super::Tick(fTimeDelta);
}
// 속도 계산을 위한 보조 함수
float CCutSceneCamera::CalculateSpeedMultiplier(const CameraKeyFrame& keyFrame, float t)
{
    float speedMultiplier = 1.0f;
    float prevSpeedMultiplier = 1.0f;

    for (const auto& [changeTime, changeSpeed, smoothOffset] : keyFrame.speedChanges)
    {
        if (t >= changeTime - smoothOffset && t < changeTime + smoothOffset)
        {
            float lerpT = (t - (changeTime - smoothOffset)) / (2.0f * smoothOffset);
            speedMultiplier = prevSpeedMultiplier + (changeSpeed - prevSpeedMultiplier) * lerpT;
            break;
        }
        else if (t >= changeTime + smoothOffset)
        {
            prevSpeedMultiplier = changeSpeed;
            speedMultiplier = changeSpeed;
        }
    }

    return speedMultiplier;
}

void CCutSceneCamera::ApplyShaking(_matrix& matLerpedWorld, float fTimeDelta)
{
    static float timeAccumulator = 0.0f;
    timeAccumulator += fTimeDelta;

    // 강한 셰이킹 타이머 업데이트
    if (m_bIntenseShaking)
    {
        m_fIntenseShakeTimer += fTimeDelta;
        if (m_fIntenseShakeTimer >= m_fIntenseShakeDuration)
        {
            m_bIntenseShaking = false;
            m_fIntenseShakeTimer = 0.0f;
        }
    }

    float currentShakeStrength = m_bIntenseShaking ? m_fIntenseShakeStrength : m_fNormalShakeStrength;
    float currentShakeFrequency = m_bIntenseShaking ? m_fIntenseShakeFrequency : m_fNormalShakeFrequency;

    // 다중 주파수 노이즈 생성 (주파수 조정)
    float noise_low1 = PerlinNoise(timeAccumulator * currentShakeFrequency * 0.3f, 0, 2, 0.5f);
    float noise_low2 = PerlinNoise(0, timeAccumulator * currentShakeFrequency * 0.3f, 2, 0.5f);
    float noise_mid1 = PerlinNoise(timeAccumulator * currentShakeFrequency * 1.0f, 0, 4, 0.5f);
    float noise_mid2 = PerlinNoise(0, timeAccumulator * currentShakeFrequency * 1.0f, 4, 0.5f);
    float noise_high1 = PerlinNoise(timeAccumulator * currentShakeFrequency * 4.0f, 0, 4, 0.5f);
    float noise_high2 = PerlinNoise(0, timeAccumulator * currentShakeFrequency * 4.0f, 4, 0.5f);

    // 시간에 따른 강도 변화 (주파수 조정)
    float intensityVariation = (sinf(timeAccumulator * currentShakeFrequency * 0.05f) + 1.0f) * 0.5f;

    // 위치 셰이킹 적용
    float shakeX = (noise_low1 * 0.6f + noise_mid1 * 0.3f + noise_high1 * 0.1f) * 0.03f * intensityVariation * currentShakeStrength;
    float shakeY = (noise_low2 * 0.6f + noise_mid2 * 0.3f + noise_high2 * 0.1f) * 0.02f * intensityVariation * currentShakeStrength;
    float shakeZ = (noise_mid1 * 0.7f + noise_high1 * 0.3f) * 0.01f * intensityVariation * currentShakeStrength;

    _vector shakeOffset = XMVectorSet(shakeX, shakeY, shakeZ, 0.0f);
    matLerpedWorld.r[3] = XMVectorAdd(matLerpedWorld.r[3], shakeOffset);

    // 회전 셰이킹 적용
    float rotX = (noise_low1 * 0.7f + noise_mid1 * 0.3f) * 0.005f * intensityVariation * currentShakeStrength;
    float rotY = (noise_low2 * 0.7f + noise_mid2 * 0.3f) * 0.005f * intensityVariation * currentShakeStrength;
    float rotZ = (noise_mid1 * 0.8f + noise_high1 * 0.2f) * 0.003f * intensityVariation * currentShakeStrength;

    _matrix rotShake = XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
    matLerpedWorld = XMMatrixMultiply(matLerpedWorld, rotShake);

    // 부드러운 카메라 움직임을 위한 추가 보간
    static _matrix prevMatrix = matLerpedWorld;
    float smoothFactor = m_bIntenseShaking ? 0.3f : (0.1f + 0.05f * intensityVariation);
    matLerpedWorld = XMMatrixSlerp(prevMatrix, matLerpedWorld, smoothFactor);
    prevMatrix = matLerpedWorld;
}
// 부드러운 이징 함수
float CCutSceneCamera::EaseInOutCubic(float t)
{
    return t < 0.5f ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
}
void CCutSceneCamera::Late_Tick(_float fTimeDelta)
{
    Key_Input(fTimeDelta);
}

HRESULT CCutSceneCamera::Render()
{
    return S_OK;
}


void CCutSceneCamera::Load_CameraKeyFrames()
{
    wstring filePath = L"../Bin/CutSceneData/CutSceneKeyFrames.dat";

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed to open file for camera keyframes.");
        return;
    }

    DWORD dwByte = 0;

    // 컷씬 개수 로드
    _uint iCutSceneCount = 0;
    ReadFile(hFile, &iCutSceneCount, sizeof(_uint), &dwByte, nullptr);

    // 기존 컷씬 클리어
    Clear_CutScenes();

    // 각 컷씬의 키프레임 로드
    for (_uint i = 0; i < iCutSceneCount; ++i)
    {
        vector<CCamera::CameraKeyFrame> cutScene;

        // 키프레임 개수 로드
        _uint iKeyFrameCount = 0;
        ReadFile(hFile, &iKeyFrameCount, sizeof(_uint), &dwByte, nullptr);

        // 각 키프레임 정보 로드
        for (_uint j = 0; j < iKeyFrameCount; ++j)
        {
            CCamera::CameraKeyFrame keyFrame;
            ReadFile(hFile, &keyFrame.fTime, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.matWorld, sizeof(_float4x4), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fFovy, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fNear, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fFar, sizeof(float), &dwByte, nullptr);

            // 속도 변화 정보 로드
            _uint iSpeedChangeCount = 0;
            ReadFile(hFile, &iSpeedChangeCount, sizeof(_uint), &dwByte, nullptr);
            for (_uint k = 0; k < iSpeedChangeCount; ++k)
            {
                float time, speed, smoothOffset;
                ReadFile(hFile, &time, sizeof(float), &dwByte, nullptr);
                ReadFile(hFile, &speed, sizeof(float), &dwByte, nullptr);
                ReadFile(hFile, &smoothOffset, sizeof(float), &dwByte, nullptr);
                keyFrame.speedChanges.emplace_back(time, speed, smoothOffset);
            }

            cutScene.push_back(keyFrame);
        }

        Add_CutScene(cutScene);
    }

    CloseHandle(hFile);
    //MSG_BOX("Camera KeyFrames Data Loaded");

}

void CCutSceneCamera::Set_CutSceneIdx(_uint idx)
{
    if (idx >= m_AllCutScenes.size())
        return;

    m_iCurrentCutSceneIdx = idx;
    m_iCurrentKeyFrame = 0;
    m_fKeyFrameTime = 0.0f;
    m_bAnimationFinished = false;
    m_bPaused  = false;

    vector<CameraKeyFrame>& currentCutScene = m_AllCutScenes[idx];

    if (currentCutScene.size() < 2)  // 최소 2개의 키프레임이 필요합니다.
        return;

    CameraKeyFrame& currentKeyFrame = currentCutScene[0];
    CameraKeyFrame& nextKeyFrame = currentCutScene[1];

    // 0.01초 동안의 보간 수행
    float fTimeDelta = 0.01f;
    float t = fTimeDelta / (nextKeyFrame.fTime - currentKeyFrame.fTime);

    // 속도 변화 적용 (Tick 함수의 로직과 유사)
    float speedMultiplier = 1.0f;
    for (const auto& [changeTime, changeSpeed, smoothOffset] : currentKeyFrame.speedChanges)
    {
        if (t >= changeTime - smoothOffset && t < changeTime + smoothOffset)
        {
            float lerpT = (t - (changeTime - smoothOffset)) / (2.0f * smoothOffset);
            speedMultiplier = 1.0f + (changeSpeed - 1.0f) * lerpT;
            break;
        }
        else if (t >= changeTime + smoothOffset)
        {
            speedMultiplier = changeSpeed;
        }
    }

    // 월드 행렬 보간
    _matrix matCurrent = XMLoadFloat4x4(&currentKeyFrame.matWorld);
    _matrix matTarget = XMLoadFloat4x4(&nextKeyFrame.matWorld);
    _matrix matLerpedWorld = XMMatrixSlerp(matCurrent, matTarget, t * speedMultiplier);

    // 보간된 월드 행렬 설정
    m_pTransformCom->Set_WorldMatrix(matLerpedWorld);

    // FOV, Near, Far 보간
    float fLerpedFovy = XMConvertToRadians(
        XMConvertToDegrees(currentKeyFrame.fFovy) +
        (XMConvertToDegrees(nextKeyFrame.fFovy) - XMConvertToDegrees(currentKeyFrame.fFovy)) * t * speedMultiplier
    );
    Set_Fovy(fLerpedFovy);

    // Near와 Far 값 설정이 필요한 경우 주석 해제
    // float fLerpedNear = currentKeyFrame.fNear + (nextKeyFrame.fNear - currentKeyFrame.fNear) * t * speedMultiplier;
    // float fLerpedFar = currentKeyFrame.fFar + (nextKeyFrame.fFar - currentKeyFrame.fFar) * t * speedMultiplier;
    // Set_Near(fLerpedNear);
    // Set_Far(fLerpedFar);

    // 키프레임 시간 업데이트
    m_fKeyFrameTime = fTimeDelta * speedMultiplier;
}

_float CCutSceneCamera::PerlinNoise(_float x, _float y, int octaves, _float persistence)
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

_float CCutSceneCamera::InterpolatedNoise(_float x, _float y)
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

_float CCutSceneCamera::SmoothNoise(int x, int y)
{
    _float corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16.0f;
    _float sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8.0f;
    _float center = Noise(x, y) / 4.0f;
    return corners + sides + center;
}

_float CCutSceneCamera::Noise(int x, int y)
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

_float CCutSceneCamera::CosineInterpolate(_float a, _float b, _float t)
{
    _float ft = t * 3.1415927f;
    _float f = (1 - cos(ft)) * 0.5f;
    return a * (1 - f) + b * f;
}

void CCutSceneCamera::ZoomIn(_float fTimeDelta)
{
    m_fZoomDistance += m_fZoomSpeed * fTimeDelta;

    _vector CurAt = XMLoadFloat4(&vAt);
    _vector vZoomDir = XMVector3Normalize(CurAt - XMLoadFloat4(&vEye));
    vEye.x = vAt.x - XMVectorGetX(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.y = vAt.y - XMVectorGetY(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.z = vAt.z - XMVectorGetZ(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.w = 1.f;
}

void CCutSceneCamera::Revolution360(_float fTimeDelta)
{
    if (m_fRevolutionAccTime < m_fRevolutionTime)
    {
        m_fRevolutionAccTime += fTimeDelta;
        float fRevolutionAngle = m_fRevolutionAccTime / m_fRevolutionTime * XM_2PI;

        vEye.x = vAt.x + cosf(fRevolutionAngle) * fDistance;
        vEye.y = vAt.y + m_fHeight;
        vEye.z = vAt.z + sinf(fRevolutionAngle) * fDistance;
        vEye.w = 1.f;
    }
}

void CCutSceneCamera::TiltAdjust(_float fTimeDelta)
{
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Cross(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye), vUp);
    vUp = XMVector3Cross(vRight, XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye));

    _matrix matRoll = XMMatrixRotationAxis(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye), XMConvertToRadians(m_fRollAngle));
    vUp = XMVector3TransformNormal(vUp, matRoll);

    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
}

void CCutSceneCamera::HeightChange(_float fTimeDelta)
{
    if (m_fHeight < m_fTargetHeight)
        m_fHeight = min(m_fHeight + m_fHeightChangeSpeed * fTimeDelta, m_fTargetHeight);
    else if (m_fHeight > m_fTargetHeight)
        m_fHeight = max(m_fHeight - m_fHeightChangeSpeed * fTimeDelta, m_fTargetHeight);

    vEye.y = vAt.y + m_fHeight;
}

float CCutSceneCamera::Get_AnimationProgress() const
{
    if (m_AllCutScenes.empty() || m_iCurrentCutSceneIdx >= m_AllCutScenes.size())
        return 0.0f;

    const vector<CameraKeyFrame>& currentCutScene = m_AllCutScenes[m_iCurrentCutSceneIdx];

    if (currentCutScene.empty())
        return 0.0f;

    if (m_bAnimationFinished)
        return 1.0f;

    if (m_iCurrentKeyFrame >= currentCutScene.size() - 1)
        return 1.0f;

    float totalDuration = currentCutScene.back().fTime - currentCutScene.front().fTime;
    float currentTime = currentCutScene[m_iCurrentKeyFrame].fTime + m_fKeyFrameTime;

    return (currentTime - currentCutScene.front().fTime) / totalDuration;
}

const vector<CCutSceneCamera::CameraKeyFrame>& CCutSceneCamera::Get_CutScene(_uint iIndex) const
{
    if (iIndex >= m_AllCutScenes.size())
    {
        // 잘못된 인덱스에 대한 예외 처리
        static vector<CameraKeyFrame> emptyKeyFrames;
        return emptyKeyFrames;
    }
    return m_AllCutScenes[iIndex];
}

void CCutSceneCamera::Set_CutScene(_uint iIndex, const vector<CameraKeyFrame>& keyFrames)
{
    if (iIndex >= m_AllCutScenes.size())
    {
        // 잘못된 인덱스에 대한 예외 처리
        return;
    }
    m_AllCutScenes[iIndex] = keyFrames;
}

void CCutSceneCamera::Clear_CutScenes()
{
    m_AllCutScenes.clear();
    m_iCurrentCutSceneIdx = 0;
    m_iCurrentKeyFrame = 0;
    m_fKeyFrameTime = 0.0f;
    m_bAnimationFinished = true;
    m_bPaused = true;
}

void CCutSceneCamera::Init_AndrasQTECutScene()
{
    m_bPaused = false;
    vector<CameraKeyFrame> andrasQTECutScene;
    float currentTime = 0.0f;

    m_iCurrentCutSceneIdx = SCENE_ANDRAS_PROJECTILE;
    LEVEL level = (LEVEL)m_pGameInstance->Get_CurrentLevel();
    m_pPlayerMatrix = std::shared_ptr<_float4x4>(dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(level, TEXT("Layer_Player"), TEXT("Com_Transform"), 0))->Get_WorldFloat4x4Ref(), [](const _float4x4*) {});
    m_pAndrasMatrix = std::shared_ptr<_float4x4>(dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(level, TEXT("Layer_Monster"), TEXT("Com_Transform"), 0))->Get_WorldFloat4x4Ref(), [](const _float4x4*) {});
    m_pProjectileMatrix = std::shared_ptr<_float4x4>(dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(level, TEXT("Layer_QTESword"), TEXT("Com_Transform"), 0))->Get_WorldFloat4x4Ref(), [](const _float4x4*) {});

    // 1. 보스 정면 클로즈업
    {
        CameraKeyFrame keyFrame;
        keyFrame.fTime = currentTime;
        _vector bossPosition = { m_pAndrasMatrix->_41, m_pAndrasMatrix->_42 + 1.5f, m_pAndrasMatrix->_43, 1.f };
        _vector playerPosition = { m_pPlayerMatrix->_41, m_pPlayerMatrix->_42, m_pPlayerMatrix->_43, 1.f };
        _vector toPlayer = XMVector3Normalize(playerPosition - bossPosition);

        _vector cameraPosition = bossPosition + toPlayer * 5.0f;
        cameraPosition = XMVectorSetY(cameraPosition, XMVectorGetY(bossPosition) + 1.7f);

        _vector lookAt = bossPosition;
        _matrix viewMatrix = SafeLookAtLH(cameraPosition, lookAt, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        XMStoreFloat4x4(&keyFrame.matWorld, XMMatrixInverse(nullptr, viewMatrix));
        keyFrame.fFovy = XMConvertToRadians(40.0f);
        keyFrame.speedChanges.emplace_back(0.165f, 2.1f, 0.3f);
        keyFrame.speedChanges.emplace_back(0.741f, 1.f, 0.3f);
        andrasQTECutScene.push_back(keyFrame);
        currentTime += 0.8f;
    }

    // 2. 클로즈업 유지
    {
        CameraKeyFrame keyFrame = andrasQTECutScene.back();
        keyFrame.fTime = currentTime;
        keyFrame.fFovy = XMConvertToRadians(80.0f);

        keyFrame.speedChanges.clear();
        keyFrame.speedChanges.emplace_back(0.165f, 2.1f, 0.3f);
        keyFrame.speedChanges.emplace_back(0.741f, 1.f, 0.3f);
        andrasQTECutScene.push_back(keyFrame);
        currentTime += 1.5f;
    }

    // 3. 투사체로 전환 시작
    {
        CameraKeyFrame keyFrame;
        keyFrame.fTime = currentTime;
        _vector projectilePosition = { m_pProjectileMatrix->_41, m_pProjectileMatrix->_42, m_pProjectileMatrix->_43, 1.f };
        _vector bossPosition = { m_pAndrasMatrix->_41, m_pAndrasMatrix->_42 + 3.0f, m_pAndrasMatrix->_43, 1.f }; // y 위치를 3으로 조정
        _vector playerPosition = { m_pPlayerMatrix->_41, m_pPlayerMatrix->_42, m_pPlayerMatrix->_43, 1.f };

        _vector midPoint = (projectilePosition + bossPosition) * 0.5f;
        _vector toProjectile = XMVector3Normalize(projectilePosition - bossPosition);
        _vector toPlayer = XMVector3Normalize(playerPosition - bossPosition);

        // 보스와 플레이어 사이의 중간 지점에서 약간 위쪽으로 카메라 위치 조정
        _vector cameraPosition = midPoint + XMVector3Cross(toProjectile, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) * 3.0f + XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);
        _vector lookAt = midPoint + toProjectile * 2.0f; // 투사체 방향으로 약간 앞을 바라보도록

        _matrix viewMatrix = SafeLookAtLH(cameraPosition, lookAt, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        XMStoreFloat4x4(&keyFrame.matWorld, XMMatrixInverse(nullptr, viewMatrix));
        keyFrame.fFovy = XMConvertToRadians(70.0f); // FOV를 좀 더 낮게 조정
        keyFrame.speedChanges.emplace_back(0.165f, 4.f, 0.3f);
        keyFrame.speedChanges.emplace_back(0.741f, 1.f, 0.3f);
        andrasQTECutScene.push_back(keyFrame);
        currentTime += 0.6f;
    }

    // 4. 투사체 추적 (플레이어를 점진적으로 화면에 포함)
    {
        CameraKeyFrame keyFrame;
        keyFrame.fTime = currentTime;
        keyFrame.bTrackProjectile = true;

        _vector vProjectilePos = XMLoadFloat4((_float4*)&m_pProjectileMatrix->_41);
        _vector vPlayerPos = XMLoadFloat4((_float4*)&m_pPlayerMatrix->_41);
        _vector vMidPoint = (vProjectilePos + vPlayerPos) * 0.5f;
        _vector vProjectileDir = XMVector3Normalize(vProjectilePos - vPlayerPos);

        _vector vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        _vector vRight = XMVector3Cross(vProjectileDir, vUp);

        // 카메라 위치를 투사체 뒤쪽으로 조정
        _vector vCameraOffset = -vProjectileDir * 4.0f + vUp * 1.5f + vRight * 1.0f;
        XMStoreFloat3(&keyFrame.offset, vCameraOffset);

        // 시선은 투사체 앞쪽을 향하도록
        _vector vLookOffset = vProjectileDir * 2.0f;
        XMStoreFloat3(&keyFrame.lookOffset, vLookOffset);

        keyFrame.fFovy = XMConvertToRadians(80.0f); // FOV를 조금 더 넓게 조정
        keyFrame.speedChanges.emplace_back(0.3f, 0.24f, 0.3f);
        keyFrame.speedChanges.emplace_back(0.887f, 1.f, 0.3f);
        andrasQTECutScene.push_back(keyFrame);
        currentTime += 1.f;

        // 중간 단계 추가 (플레이어 쪽으로 카메라 점진적 이동)
        keyFrame.fTime = currentTime;
        vCameraOffset = -vProjectileDir * 3.0f + vUp * 2.0f + vRight * 0.5f;
        XMStoreFloat3(&keyFrame.offset, vCameraOffset);
        vLookOffset = vProjectileDir * 1.5f + vUp * 0.5f;
        XMStoreFloat3(&keyFrame.lookOffset, vLookOffset);
        keyFrame.fFovy = XMConvertToRadians(85.0f);
        andrasQTECutScene.push_back(keyFrame);
        currentTime += 2.0f;
    }

    // 5. 플레이어의 뒤에서 투사체와 플레이어를 동시에 화면에 보이게 함w a
    {
        CameraKeyFrame keyFrame;
        keyFrame.fTime = currentTime;
        _vector playerPosition = { m_pPlayerMatrix->_41, m_pPlayerMatrix->_42, m_pPlayerMatrix->_43, 1.f };
        _vector projectilePosition = { m_pProjectileMatrix->_41, m_pProjectileMatrix->_42, m_pProjectileMatrix->_43, 1.f };
        _vector direction = XMVector3Normalize(projectilePosition - playerPosition);

        _vector cameraPosition = playerPosition - direction * 7.0f + XMVectorSet(0.0f, 5.0f, 0.0f, 0.0f);
        _vector lookAt = (playerPosition + projectilePosition) * 0.5f + XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

        _matrix viewMatrix = SafeLookAtLH(cameraPosition, lookAt, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        XMStoreFloat4x4(&keyFrame.matWorld, XMMatrixInverse(nullptr, viewMatrix));
        keyFrame.fFovy = XMConvertToRadians(30.0f);
        keyFrame.speedChanges.emplace_back(0.165f, 2.1f, 0.3f);
        keyFrame.speedChanges.emplace_back(0.741f, 1.f, 0.3f);

        andrasQTECutScene.push_back(keyFrame);
        currentTime += 5.f;
    }

    m_AllCutScenes[SCENE_ANDRAS_PROJECTILE] = andrasQTECutScene;
    Set_CutSceneIdx(SCENE_ANDRAS_PROJECTILE);
}

_matrix CCutSceneCamera::CalculateCameraMatrixFromProjectile(const _float4x4& fromMatrix, const _float4x4& toMatrix, const _float3& offset, const _float3& lookOffset)
{
    _vector vFrom = { fromMatrix._41, fromMatrix._42, fromMatrix._43, 1.f };  // 투사체 위치
    _vector vTo = { toMatrix._41, toMatrix._42, toMatrix._43, 1.f };  // 플레이어 위치
    _vector vDirection = XMVector3Normalize(vTo - vFrom);

    // vDirection이 0 벡터인 경우 처리
    if (XMVector3Equal(vDirection, XMVectorZero()))
    {
        vDirection = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vDirection));

    // vRight가 0 벡터인 경우 처리
    if (XMVector3Equal(vRight, XMVectorZero()))
    {
        vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(1.f, 0.f, 0.f, 0.f), vDirection));
    }

    _vector vUp = XMVector3Cross(vDirection, vRight);

    // vUp이 0 벡터인 경우 처리
    if (XMVector3Equal(vUp, XMVectorZero()))
    {
        vUp = vWorldUp;
        vDirection = XMVector3Normalize(XMVector3Cross(vRight, vUp));
    }

    _vector vOffset = XMLoadFloat3(&offset);
    _vector vLookOffset = XMLoadFloat3(&lookOffset);

    _vector vCameraPos = vFrom + vRight * XMVectorGetX(vOffset) + vUp * XMVectorGetY(vOffset) + vDirection * XMVectorGetZ(vOffset);
    _vector vLookAt = vFrom + vRight * XMVectorGetX(vLookOffset) + vUp * XMVectorGetY(vLookOffset) + vDirection * XMVectorGetZ(vLookOffset);

    // 카메라 위치와 시선 방향이 같아지는 문제 해결
    _vector vCameraToLook = vLookAt - vCameraPos;
    if (XMVector3LengthSq(vCameraToLook).m128_f32[0] < 0.0001f)  // 거의 0에 가까운 경우
    {
        // 카메라를 약간 뒤로 이동
        vCameraPos = vLookAt - vDirection * 0.1f;  // 0.1f는 최소 거리, 필요에 따라 조정 가능
    }

    _matrix matView = SafeLookAtLH(vCameraPos, vLookAt, vUp);
    XMVECTOR det;
    _matrix matInvView = XMMatrixInverse(&det, matView);

    if (XMVectorGetX(det) == 0.0f)
    {
        // 오류 처리: 기본 뷰 행렬 반환
        return XMMatrixIdentity();
    }

    return matInvView;
}

_matrix CCutSceneCamera::SafeLookAtLH(_vector cameraPosition, _vector lookAt, _vector upDirection)
{
    // 카메라 위치와 lookAt 지점 사이의 거리 계산
    _vector direction = XMVectorSubtract(lookAt, cameraPosition);
    float distance = XMVectorGetX(XMVector3Length(direction));

    // 최소 허용 거리 (이 값은 필요에 따라 조정 가능)
    const float MIN_DISTANCE = 0.5f;

    if (distance < MIN_DISTANCE)
    {
        // 카메라 위치와 lookAt 지점이 너무 가까운 경우
        // lookAt 지점을 카메라 전방으로 약간 이동
        _vector forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        lookAt = XMVectorAdd(cameraPosition, XMVectorScale(forward, MIN_DISTANCE));
    }

    // 수정된 lookAt 지점으로 뷰 행렬 생성
    return XMMatrixLookAtLH(cameraPosition, lookAt, upDirection);
}

void CCutSceneCamera::EndCutScene()
{
    m_bAnimationFinished = true;
    m_bPaused = true;

    // 컷신 종료 시 필요한 추가 처리
    CUI_Manager::GetInstance()->Setting_Cinematic(true);

    // 메인 카메라로 전환
    m_pGameInstance->Set_MainCamera(CAM_THIRDPERSON);

    // 필요한 경우 트랜지션 카메라 설정
    CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};
    pTCDesc.fFovy = XMConvertToRadians(60.f);
    pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    pTCDesc.fNear = 0.1f;
    pTCDesc.fFar = 3000.f;
    pTCDesc.fSpeedPerSec = 40.f;
    pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pTCDesc.iStartCam = CAM_CUTSCENE;
    pTCDesc.iEndCam = CAM_THIRDPERSON;
    pTCDesc.fTransitionTime = 1.f;

    if (FAILED(m_pGameInstance->Add_Camera(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
    {
        MSG_BOX("Failed to add transition camera");
    }
}

void CCutSceneCamera::StartIntenseShaking(float duration, float strength)
{
    m_bIntenseShaking = true;
    m_fIntenseShakeDuration = duration;
    m_fIntenseShakeTimer = 0.0f;
    m_fIntenseShakeStrength = strength;
}

CCutSceneCamera* CCutSceneCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCutSceneCamera* pInstance = new CCutSceneCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CCutSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCutSceneCamera::Clone(void* pArg)
{
    CCutSceneCamera* pInstance = new CCutSceneCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CCutSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCutSceneCamera::Free()
{
    Safe_Release(m_pPlayerTrans);

    __super::Free();
}

void CCutSceneCamera::Shaking()
{
    _vector vLook;
    _vector vRight;
    _vector vUp;

    vLook = XMVector3Normalize(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye));
    vRight = XMVector3Cross(vLook, m_pTransformCom->Get_State(CTransform::STATE_UP));
    vUp = XMVector3Cross(vLook, vRight);

    // -1.5f ~ 1.5f 사이의값 구함
    float offsetY = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
    _vector eye = XMLoadFloat4(&vEye);
    _vector at = XMLoadFloat4(&vAt);
    eye += vUp * offsetY;
    at += vUp * offsetY;

    if (!m_bLockWidth)
    {
        float offsetX = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
        eye += vRight * offsetX;
        at += vRight * offsetX;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, eye);
    m_pTransformCom->LookAt(at);

}


void CCutSceneCamera::Key_Input(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing(DIK_W))
        fDistance -= 10.f * fTimeDelta;
    else if (m_pGameInstance->Key_Pressing(DIK_S))
        fDistance += 10.f * fTimeDelta;


    if (m_pGameInstance->Key_Pressing(DIK_O))
    {
        m_fAngle += 5.f;
    }


    if (m_pGameInstance->Key_Pressing(DIK_P))
    {
        m_fAngle -= 5.f;
    }

    if (m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_bPaused = !m_bPaused;
    }

    /*if (m_pGameInstance->Key_Down(DIK_UP))
{
    m_fDebugRevolveTime += 0.5f;
}
else if (m_pGameInstance->Key_Down(DIK_DOWN))
{
    m_fDebugRevolveTime -= 0.5f;
}*/
////줌인
//if (m_pGameInstance->Key_Down(DIK_Z))
//    m_bZoomIn = true;
//else
//    m_bZoomIn = false;

////공전
//if (m_pGameInstance->Key_Down(DIK_X))
//    m_bRevolution360 = true;
//else
//    m_bRevolution360 = false;

////각도
//if (m_pGameInstance->Key_Down(DIK_C))
//    m_bTiltAdjust = true;
//else
//    m_bTiltAdjust = false;

////높이체인지
//if (m_pGameInstance->Key_Down(DIK_V))
//    m_bHeightChange = true;
//else
//    m_bHeightChange = false;

}
