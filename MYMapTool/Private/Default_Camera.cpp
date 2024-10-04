#include "Default_Camera.h"

#include "GameInstance.h"

CDefault_Camera::CDefault_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{pDevice, pContext}
{
}

CDefault_Camera::CDefault_Camera(const CDefault_Camera& rhs)
	: CCamera{rhs}
{
}

HRESULT CDefault_Camera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDefault_Camera::Initialize(void* pArg)
{
	DEFAULTCAM_DESC* pDesc = (DEFAULTCAM_DESC*)pArg;

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDefault_Camera::Priority_Tick(_float fTimeDelta)
{
}

void CDefault_Camera::Tick(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	// 마우스
	if (!m_bFix)
	{
		Mouse_Move(fTimeDelta);
		Mouse_Fix();
	}

	__super::Tick(fTimeDelta);
}

void CDefault_Camera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CDefault_Camera::Render()
{
	return S_OK;
}

void CDefault_Camera::Key_Input(_float fTimeDelta)
{
	// W, S, A, D
	if (m_pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta * m_fCamSpeed);
	if (m_pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta * m_fCamSpeed);
	if (m_pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta * m_fCamSpeed);
	if (m_pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta * m_fCamSpeed);

	
	//~키 누르면 캠 고정
	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		m_bFix = !m_bFix;
		m_pGameInstance->Set_MainCamera(0);
	}

	////~키 누르면 캠 고정
	//if (m_pGameInstance->Key_Down(DIK_1))
	//{
	//	m_pGameInstance->Set_MainCamera(1);
	//}

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
}

void CDefault_Camera::Mouse_Fix()
{
	POINT	pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void CDefault_Camera::Mouse_Move(_float fTimeDelta)
{
	_long MouseMove = { 0 };

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fSensor * MouseMove);
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * m_fSensor * MouseMove);

}

CDefault_Camera* CDefault_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDefault_Camera* pInstance = new CDefault_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDefault_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefault_Camera::Clone(void* pArg)
{
	CDefault_Camera* pInstance = new CDefault_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDefault_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefault_Camera::Free()
{
	__super::Free();
}
