#include "stdafx.h"
#include "..\Public\Map_Element.h"

#include "GameInstance.h"


CMap_Element::CMap_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CMap_Element::CMap_Element(const CMap_Element& rhs)
	: CBlendObject{ rhs }
{
}

HRESULT CMap_Element::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMap_Element::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(Add_Components((MAP_ELEMENT_DESC*)(pArg))))
	//	return E_FAIL;

	/*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 3.f, 5.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);*/

	return S_OK;
}

void CMap_Element::Priority_Tick(_float fTimeDelta)
{
}

void CMap_Element::Tick(_float fTimeDelta)
{

}

void CMap_Element::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMap_Element::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;



	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
				return E_FAIL;
		}
		else if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);

		m_pShaderCom->Unbind_SRVs();
	}


	return S_OK;
}

HRESULT CMap_Element::Add_Components(MAP_ELEMENT_DESC* desc)
{
	//_int iLevelIndex = m_pGameInstance->Get_

	//LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
	//LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
	//LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
	//LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
	//LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;








	return S_OK;
}

HRESULT CMap_Element::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CMap_Element* CMap_Element::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMap_Element* pInstance = new CMap_Element(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMap_Element");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMap_Element::Clone(void* pArg)
{
	CMap_Element* pInstance = new CMap_Element(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMap_Element");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap_Element::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
