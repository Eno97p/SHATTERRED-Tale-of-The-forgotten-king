#include "CutSceneCamera.h"
#include "GameInstance.h"

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

    //__super::Activate();

    return S_OK;
}

void CCutSceneCamera::Priority_Tick(_float fTimeDelta)
{

}
void CCutSceneCamera::Tick(_float fTimeDelta)
{
    if (m_AllCutScenes.empty() || m_iCurrentCutSceneIdx >= m_AllCutScenes.size())
        return;

    vector<CameraKeyFrame>& currentCutScene = m_AllCutScenes[m_iCurrentCutSceneIdx];

    if (currentCutScene.empty())
        return;

    if (m_bAnimationFinished || m_bPaused)
    {
        m_pGameInstance->Set_MainCamera(0);
        return;
    }

    if (!m_bPaused)
    {
        if (m_iCurrentKeyFrame + 1 < currentCutScene.size())
        {
            CameraKeyFrame& currentKeyFrame = currentCutScene[m_iCurrentKeyFrame];
            CameraKeyFrame& nextKeyFrame = currentCutScene[m_iCurrentKeyFrame + 1];
            float t = m_fKeyFrameTime / (nextKeyFrame.fTime - currentKeyFrame.fTime);

            // ���� Ű������ ���������� �ӵ� ��ȭ Ȯ�� �� ����
            float speedMultiplier = 1.0f;
            float prevSpeedMultiplier = 1.0f;

            for (const auto& [changeTime, changeSpeed, smoothOffset] : currentKeyFrame.speedChanges)
            {
                if (t >= changeTime - smoothOffset && t < changeTime + smoothOffset)
                {
                    // �ӵ� ��ȭ ���������� ���� �ӵ��� ���� �ӵ��� ����
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

            m_fKeyFrameTime += fTimeDelta * speedMultiplier;

            // ���� ���� ��İ� ��ǥ ���� ��� ��������
            _matrix matCurrent = XMLoadFloat4x4(&currentKeyFrame.matWorld);
            _matrix matTarget = XMLoadFloat4x4(&nextKeyFrame.matWorld);

            if (t >= 1.0f)
            {
                m_iCurrentKeyFrame++;
                m_fKeyFrameTime = 0.0f;
                m_pTransformCom->Set_WorldMatrix(matTarget);
            }
            else
            {
                // ��� ���� ���� ����
                _matrix matLerpedWorld = XMMatrixSlerp(matCurrent, matTarget, t);
                // ������ ���� ��� ����
                m_pTransformCom->Set_WorldMatrix(matLerpedWorld);
            }

            // ������ �þ߰�, �����, ����� ����
            _float fLerpedFovy = XMConvertToRadians(XMConvertToDegrees(currentKeyFrame.fFovy) + (XMConvertToDegrees(nextKeyFrame.fFovy) - XMConvertToDegrees(currentKeyFrame.fFovy)) * t);
            _float fLerpedNear = currentKeyFrame.fNear + (nextKeyFrame.fNear - currentKeyFrame.fNear) * t;
            _float fLerpedFar = currentKeyFrame.fFar + (nextKeyFrame.fFar - currentKeyFrame.fFar) * t;
            Set_Fovy(fLerpedFovy);
            // Set_Near(fLerpedNear);
            // Set_Far(fLerpedFar);
        }
        else
        {
            m_bAnimationFinished = true;
        }
    }

    __super::Tick(fTimeDelta);
}
void CCutSceneCamera::Late_Tick(_float fTimeDelta)
{
    Key_Input(fTimeDelta);
}

HRESULT CCutSceneCamera::Render()
{
    return S_OK;
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

void CCutSceneCamera::Pop_CutScene(_uint iIndex)
{
    if (iIndex >= m_AllCutScenes.size())
    {
        // �߸��� �ε����� ���� ���� ó��
        MSG_BOX("Invalid CutScene index in Pop_CutScene");
        return;
    }

    // �ش� �ε����� �ƾ� ����
    m_AllCutScenes.erase(m_AllCutScenes.begin() + iIndex);

    // ���� ��� ���� �ƾ��� ���ŵ� ��� ó��
    if (m_iCurrentCutSceneIdx == iIndex)
    {
        // ���� ��� ���� �ƾ��� ������ �ƾ��̾��ٸ�
        if (m_iCurrentCutSceneIdx >= m_AllCutScenes.size())
        {
            m_iCurrentCutSceneIdx = 0;  // ù ��° �ƾ����� ����
            m_iCurrentKeyFrame = 0;
            m_fKeyFrameTime = 0.0f;
            m_bAnimationFinished = true;
            m_bPaused = true;
        }
        // �׷��� �ʴٸ� ���� �ƾ����� �̵�
        else
        {
            m_iCurrentKeyFrame = 0;
            m_fKeyFrameTime = 0.0f;
            m_bAnimationFinished = false;
            m_bPaused = false;
        }
    }
    // ���ŵ� �ƾ��� ���� ��� ���� �ƾ� ������ �־��ٸ� �ε��� ����
    else if (m_iCurrentCutSceneIdx > iIndex)
    {
        m_iCurrentCutSceneIdx--;
    }

    // ��� �ƾ��� ���ŵ� ���
    if (m_AllCutScenes.empty())
    {
        Clear_CutScenes();
    }
}
const vector<CCutSceneCamera::CameraKeyFrame>& CCutSceneCamera::Get_CutScene(_uint iIndex) const
{
    if (iIndex >= m_AllCutScenes.size())
    {
        // �߸��� �ε����� ���� ���� ó��
        static vector<CameraKeyFrame> emptyKeyFrames;
        return emptyKeyFrames;
    }
    return m_AllCutScenes[iIndex];
}

void CCutSceneCamera::Set_CutScene(_uint iIndex, const vector<CameraKeyFrame>& keyFrames)
{
    if (iIndex >= m_AllCutScenes.size())
    {
        // �߸��� �ε����� ���� ���� ó��
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

    // -1.5f ~ 1.5f �����ǰ� ����
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
    ////����
    //if (m_pGameInstance->Key_Down(DIK_Z))
    //    m_bZoomIn = true;
    //else
    //    m_bZoomIn = false;

    ////����
    //if (m_pGameInstance->Key_Down(DIK_X))
    //    m_bRevolution360 = true;
    //else
    //    m_bRevolution360 = false;

    ////����
    //if (m_pGameInstance->Key_Down(DIK_C))
    //    m_bTiltAdjust = true;
    //else
    //    m_bTiltAdjust = false;

    ////����ü����
    //if (m_pGameInstance->Key_Down(DIK_V))
    //    m_bHeightChange = true;
    //else
    //    m_bHeightChange = false;

}
