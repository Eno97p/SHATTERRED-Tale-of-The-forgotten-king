#include "stdafx.h"
#include "..\Public\Distortion.h"

#include "GameInstance.h"

CDistortion::CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CDistortion::CDistortion(const CDistortion& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CDistortion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDistortion::Initialize(void* pArg)
{
	CLandObject::LANDOBJ_DESC* pDesc = (CLandObject::LANDOBJ_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.f, 15.f, 30.f, 1.f));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	return S_OK;
}

void CDistortion::Priority_Tick(_float fTimeDelta)
{
}

void CDistortion::Tick(_float fTimeDelta)
{
	_float3 fScale = m_pTransformCom->Get_Scaled();
	m_pTransformCom->Set_Scale(fScale.x + 0.4f, fScale.y + 0.4f, fScale.z);

	if(fScale.x > 1000.f) m_pGameInstance->Erase(this);
}

void CDistortion::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CDistortion::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CDistortion::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Distortion"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDistortion::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	return S_OK;
}

CDistortion* CDistortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDistortion* pInstance = new CDistortion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDistortion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDistortion::Clone(void* pArg)
{
	CDistortion* pInstance = new CDistortion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDistortion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDistortion::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}