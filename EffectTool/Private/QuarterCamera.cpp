
#include "QuarterCamera.h"
#include "GameInstance.h"

CQuarterCamera::CQuarterCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera{ pDevice, pContext }
{
}

CQuarterCamera::CQuarterCamera(const CQuarterCamera & rhs)
	: CCamera{ rhs }
{
}

HRESULT CQuarterCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuarterCamera::Initialize(void * pArg)
{
	FREE_CAMERA_DESC*		pDesc = (FREE_CAMERA_DESC*)pArg;

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&vEye, sizeof(_float4));
	ZeroMemory(&vAt, sizeof(_float4));
	ZeroMemory(&vDir, sizeof(_float4));

	fDistance = 6.f;
	m_fHeight = 4.f;
	m_fAngle = 180.f;
	fSpeed = 12.f;

	return S_OK;
}

void CQuarterCamera::Priority_Tick(_float fTimeDelta)
{
}

void CQuarterCamera::Tick(_float fTimeDelta)
{
	if (!m_FreeCamera)
	{
		if (m_pTarget == nullptr)
			return;

		if (!m_isTargetChanging)
		{
			_vector vTarget;
			CTransform* TargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
			vTarget = TargetTransform->Get_State(CTransform::STATE_POSITION);
			XMStoreFloat4(&vAt, vTarget); //vTarget이 플레이어의 위치벡터. vAt은 플레이어를 바라본다.

			_vector CurAt = XMLoadFloat4(&vAt);
			vEye.x = vAt.x + cosf(m_fAngle) * fDistance; //카메라의 각도와 위치 조정
			vEye.y = vAt.y + fDistance;
			vEye.z = vAt.z + sinf(m_fAngle) * fDistance;
			vEye.w = 1.f;

			_vector CurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector Dst = XMLoadFloat4(&vEye);


			_vector Lerp = XMVectorLerp(CurPos, Dst, 0.5f);
			_vector LerpAt = XMVectorLerp(CurAt, XMLoadFloat4(&vAt), 0.5f);


			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp);
			m_pTransformCom->LookAt(LerpAt);


		}
		else
		{
			_vector vTarget;
			CTransform* TargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
			vTarget = TargetTransform->Get_State(CTransform::STATE_POSITION);
			XMStoreFloat4(&vAt, vTarget); //vTarget이 플레이어의 위치벡터. vAt은 플레이어를 바라본다.

			_vector CurAt = XMLoadFloat4(&vAt);
			vEye.x = vAt.x + cosf(m_fAngle) * fDistance; //카메라의 각도와 위치 조정
			vEye.y = vAt.y + fDistance;
			vEye.z = vAt.z + sinf(m_fAngle) * fDistance;
			vEye.w = 1.f;

			_vector CurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector Dst = XMLoadFloat4(&vEye);


			_vector Lerp = XMVectorLerp(CurPos, Dst, fTimeDelta);
			_vector LerpAt = XMVectorLerp(CurAt, XMLoadFloat4(&vAt), fTimeDelta);

			if (XMVectorGetX(XMVector3Length(Dst - Lerp)) < 0.01f)
				m_isTargetChanging = false;



			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp);
			m_pTransformCom->LookAt(LerpAt);
		}
		if (m_fShakeTime > m_fShakeAccTime)
		{
			m_fShakeAccTime += fTimeDelta;
			Shaking();
		}

		Key_Input(fTimeDelta);
	}
	else
	{
		Free_Key_Input(fTimeDelta);
	}
	__super::Tick(fTimeDelta);
}

void CQuarterCamera::Late_Tick(_float fTimeDelta)
{
	
}

HRESULT CQuarterCamera::Render()
{
	return S_OK;
}

void CQuarterCamera::Change_Target(CGameObject* m_Target)
{
	m_isTargetChanging = true;
	m_pTarget = m_Target;
}

void CQuarterCamera::Shaking()
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

void CQuarterCamera::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->CheckMouseWheel() == WHEEL_UP)
		fDistance -= 10.f * fTimeDelta;
	else if (m_pGameInstance->CheckMouseWheel() == WHEEL_DOWN)
		fDistance += 10.f * fTimeDelta;
	

	if (GetAsyncKeyState('O'))
	{
		m_fAngle += fTimeDelta;
	}


	if (GetAsyncKeyState('P'))
	{
		m_fAngle -= fTimeDelta;
	}

}

void CQuarterCamera::Free_Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_TAB))
		m_bStop = !m_bStop;

	if (!m_bStop)
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
			m_pTransformCom->Go_Left(fTimeDelta);
		if (m_pGameInstance->Key_Pressing(DIK_D))
			m_pTransformCom->Go_Right(fTimeDelta);
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_pTransformCom->Go_Straight(fTimeDelta);
		if (m_pGameInstance->Key_Pressing(DIK_S))
			m_pTransformCom->Go_Backward(fTimeDelta);

		_long		MouseMove = { 0 };
		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fSensor * MouseMove);
		}

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * m_fSensor * MouseMove);
		}
	}
}

void CQuarterCamera::Change_Camera()
{
	m_FreeCamera = !m_FreeCamera;
}





CQuarterCamera * CQuarterCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CQuarterCamera*		pInstance = new CQuarterCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CQuarterCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQuarterCamera::Clone(void * pArg)
{
	CQuarterCamera*		pInstance = new CQuarterCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CQuarterCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuarterCamera::Free()
{
	__super::Free();

}
