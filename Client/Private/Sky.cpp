#include "stdafx.h"
#include "Sky.h"
#include "GameInstance.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iSkyTex = m_pGameInstance->Get_CurrentLevel() - LEVEL_GAMEPLAY;

	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_ANDRASARENA)
	{
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(60.f));
	}

	XMStoreFloat4(&m_vFogColor, m_pGameInstance->Get_FogColor());

	return S_OK;
}

void CSky::Priority_Tick(_float fTimeDelta)
{

}

void CSky::Tick(_float fTimeDelta)
{
	//fTime += fTimeDelta;
}

void CSky::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance->Get_CamPosition());

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSky::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iSkyTex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_FogColor", &m_vFogColor, sizeof(_float4))))
		return E_FAIL;



	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
	//		return E_FAIL;
	//	m_pShaderCom->Begin(0);
	//	m_pModelCom->Render(i);
	//}

	return S_OK;

}

HRESULT CSky::Render(ID3D11DeviceContext* pDeferredContext)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_FogColor", &m_vFogColor, sizeof(_float4))))

		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iSkyTex)))
		return E_FAIL;


	m_pShaderCom->Begin(0, pDeferredContext);

	m_pVIBufferCom->Bind_Buffers(pDeferredContext);

	m_pVIBufferCom->Render(pDeferredContext);



	return S_OK;
}

HRESULT CSky::Add_Components()
{
	/* For.Com_VIBuffer */
 	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	//FOR SKYSPHERE@@
	//FOR SKYSPHERE@@
	//FOR SKYSPHERE@@
	//FOR SKYSPHERE@@
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SkySphere"),	//¸ðµ¨ Ãß°¡
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	/* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
{

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &fTime, sizeof(_float))))
	//	return E_FAIL;

	return S_OK;

}

CSky* CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSky* pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}


