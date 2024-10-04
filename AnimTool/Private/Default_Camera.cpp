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

	// ¸¶¿ì½º
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
	if (m_pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
		m_pTransformCom->Go_Backward(fTimeDelta);
	if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
		m_pTransformCom->Go_Left(fTimeDelta);
	if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
		m_pTransformCom->Go_Right(fTimeDelta);

	if (m_pGameInstance->Get_DIKeyState(DIK_GRAVE) & 0x80)
	{
		if (m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
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
