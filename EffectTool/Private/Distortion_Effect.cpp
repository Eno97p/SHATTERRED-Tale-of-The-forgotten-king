
#include "Distortion_Effect.h"
#include "GameInstance.h"

CDistortionEffect::CDistortionEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBlendObject(pDevice, pContext)
{
}

CDistortionEffect::CDistortionEffect(const CDistortionEffect & rhs)
	: CBlendObject(rhs)
{
}

HRESULT CDistortionEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDistortionEffect::Initialize(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	m_OwnDesc = make_shared<DISTORTIONEFFECT>(*((DISTORTIONEFFECT*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartpos));
	m_pTransformCom->Set_Scale(m_OwnDesc->vStartScale.x, m_OwnDesc->vStartScale.y, 0.f);

	return S_OK;
}

void CDistortionEffect::Priority_Tick(_float fTimeDelta)
{
	
}

void CDistortionEffect::Tick(_float fTimeDelta)
{
	m_fLifetime += fTimeDelta;
	if (m_fLifetime > m_OwnDesc->fLifeTime)
	{
		m_fLifetime = m_OwnDesc->fLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fRatio = clamp(m_fLifetime / m_OwnDesc->fLifeTime , 0.f,1.f);

	if(m_OwnDesc->bFuncType)	//커지기
	{
		m_OwnDesc->vStartScale.y += m_OwnDesc->fSpeed * fTimeDelta;
		m_OwnDesc->vStartScale.x += m_OwnDesc->fSpeed * fTimeDelta;
	}
	else //작아지기
	{
		m_OwnDesc->vStartScale.y -= m_OwnDesc->fSpeed * fTimeDelta;
		m_OwnDesc->vStartScale.x -= m_OwnDesc->fSpeed * fTimeDelta;

		if (m_OwnDesc->vStartScale.y < 0.f)
			m_OwnDesc->vStartScale.y = 0.f;
		if (m_OwnDesc->vStartScale.x < 0.f)
			m_OwnDesc->vStartScale.x = 0.f;
	}
}

void CDistortionEffect::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->BillBoard();
	m_pTransformCom->Set_Scale(m_OwnDesc->vStartScale.x, m_OwnDesc->vStartScale.y, 0.f);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CDistortionEffect::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}



HRESULT CDistortionEffect::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevel(), m_OwnDesc->Texture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_DistortionEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Desolve"), reinterpret_cast<CComponent**>(&m_DesolveTexture))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDistortionEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_DesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->iDesolveNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &m_OwnDesc->bDisolve, sizeof(_bool))))
		return E_FAIL;
	return S_OK;
}

CDistortionEffect * CDistortionEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDistortionEffect*		pInstance = new CDistortionEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDistortionEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDistortionEffect::Clone(void * pArg)
{
	CDistortionEffect*		pInstance = new CDistortionEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDistortionEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDistortionEffect::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_DesolveTexture);
}
