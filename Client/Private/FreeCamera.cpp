#include "stdafx.h"
#include "..\Public\FreeCamera.h"

#include "GameInstance.h"

CFreeCamera::CFreeCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera{ pDevice, pContext }
{
}

CFreeCamera::CFreeCamera(const CFreeCamera & rhs)
	: CCamera{ rhs }
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFreeCamera::Initialize(void * pArg)
{
	FREE_CAMERA_DESC*		pDesc = (FREE_CAMERA_DESC*)pArg;

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CFreeCamera::Priority_Tick(_float fTimeDelta)
{
}

void CFreeCamera::Tick(_float fTimeDelta)
{
	if (GetKeyState(VK_LEFT) & 0x8000)
		m_pTransformCom->Go_Left(fTimeDelta * m_fCamSpeed);
	if (GetKeyState(VK_RIGHT) & 0x8000)
		m_pTransformCom->Go_Right(fTimeDelta * m_fCamSpeed);
	if (GetKeyState(VK_UP) & 0x8000)
		m_pTransformCom->Go_Straight(fTimeDelta * m_fCamSpeed);
	if (GetKeyState(VK_DOWN) & 0x8000)
		m_pTransformCom->Go_Backward(fTimeDelta * m_fCamSpeed);

	_long		MouseMove = { 0 };

	if (KEY_TAP(DIK_TAB))
	{
		if (m_bIsNoMove == false)
		{
			m_bIsNoMove = true;
		}
		else
			m_bIsNoMove = false;
	}

	if (m_bIsNoMove)
	{

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fSensor * MouseMove);
		}

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * m_fSensor * MouseMove);
		}


	}

	//Up, Down으로 캠 스피드 조정
	if (m_pGameInstance->CheckMouseWheel() == WHEEL_UP)
	{
		m_fCamSpeed += 0.1f;
	}
	if (m_pGameInstance->CheckMouseWheel() == WHEEL_DOWN)
	{

		m_fCamSpeed -= 0.1f;

		if (m_fCamSpeed < 0.f)
		{
			m_fCamSpeed = 0.f;
		}

	}
	__super::Tick(fTimeDelta);
}

void CFreeCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CFreeCamera::Render()
{
	return S_OK;
}

CFreeCamera * CFreeCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFreeCamera*		pInstance = new CFreeCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFreeCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFreeCamera::Clone(void * pArg)
{
	CFreeCamera*		pInstance = new CFreeCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFreeCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFreeCamera::Free()
{
	__super::Free();

}
