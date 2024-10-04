#include "UI_UpCompleted_Circle.h"

#include "GameInstance.h"

CUI_UpCompleted_Circle::CUI_UpCompleted_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpCompleted_Circle::CUI_UpCompleted_Circle(const CUI_UpCompleted_Circle& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpCompleted_Circle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpCompleted_Circle::Initialize(void* pArg)
{
	UI_CIRCLE_DESC* pDesc = static_cast<UI_CIRCLE_DESC*>(pArg);

	m_eCircleType = pDesc->eCircleType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_UpCompleted_Circle::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpCompleted_Circle::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Turn_Animation(fTimeDelta);
}

void CUI_UpCompleted_Circle::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, THIRTEENTH);
}

HRESULT CUI_UpCompleted_Circle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_UpCompleted_Circle::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (CIRCLE_ONE == m_eCircleType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_UpCompleted_Circle_1"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	else if (CIRCLE_TWO == m_eCircleType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_UpCompleted_Circle_2"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_UpCompleted_Circle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_float4x4 rotationMatrix;
	XMStoreFloat4x4(&rotationMatrix, m_RotationMatrix);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_RotationMatrix", &rotationMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;*/

	return S_OK;
}

void CUI_UpCompleted_Circle::Turn_Animation(_float fTimeDelta)
{
	if (CIRCLE_ONE == m_eCircleType)
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(-5.f);
	else
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(-20.f);

	if (m_fRotationAngle > XM_2PI)
		m_fRotationAngle -= XM_2PI;

	m_RotationMatrix = XMMatrixRotationZ(m_fRotationAngle); // XMMatrix
}

CUI_UpCompleted_Circle* CUI_UpCompleted_Circle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpCompleted_Circle* pInstance = new CUI_UpCompleted_Circle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpCompleted_Circle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpCompleted_Circle::Clone(void* pArg)
{
	CUI_UpCompleted_Circle* pInstance = new CUI_UpCompleted_Circle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpCompleted_Circle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpCompleted_Circle::Free()
{
	__super::Free();
}
