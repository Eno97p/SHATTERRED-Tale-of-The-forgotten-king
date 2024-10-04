#include "BackGround_Moon.h"
#include "GameInstance.h"

CBackGround_Moon::CBackGround_Moon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMap_Element(pDevice, pContext)
{
}

CBackGround_Moon::CBackGround_Moon(const CBackGround_Moon& rhs)
	:CMap_Element(rhs)
{
}

HRESULT CBackGround_Moon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround_Moon::Initialize(void* pArg)
{

	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;
	if (pArg != nullptr) 
	{
	}




	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1000.f, 1000.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1695.958f, 6004.3f, 7175.976, 1.f));

	CARD_DESC* cardDesc = (CARD_DESC*)pArg;

	m_iTexNum = 1;
	return S_OK;
}

void CBackGround_Moon::Priority_Tick(_float fTimeDelta)
{
}

void CBackGround_Moon::Tick(_float fTimeDelta)
{
	m_pTransformCom->BillBoard();

}

void CBackGround_Moon::Late_Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_OwnDesc->vStartScale.y))
	//{
	//	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//	m_pTransformCom->BillBoard();
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	//}
}


HRESULT CBackGround_Moon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

	

		if (FAILED(m_pShaderCom->Bind_RawValue("g_TextureNum", &m_iTexNum, sizeof(_uint))))
			return E_FAIL;

		_bool t = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDiffuse", &t, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pMoonTex->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;

		m_pShaderCom->Begin(8); //COLOR

		m_pModelCom->Render(i);
	}
}

HRESULT CBackGround_Moon::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();



		if (FAILED(m_pShaderCom->Bind_RawValue("g_TextureNum", &m_iTexNum, sizeof(_uint))))
			return E_FAIL;

		_bool t = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDiffuse", &t, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pMoonTex->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;

		m_pShaderCom->Begin(8); //COLOR

		m_pModelCom->Render(i);
	}
}


HRESULT CBackGround_Moon::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mountain_Card"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	
	
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Moon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMoonTex))))
		return E_FAIL;


	return S_OK;
}

HRESULT CBackGround_Moon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}


CBackGround_Moon* CBackGround_Moon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround_Moon* pInstance = new CBackGround_Moon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBackGround_Moon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBackGround_Moon::Clone(void* pArg)
{
	CBackGround_Moon* pInstance = new CBackGround_Moon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBackGround_Moon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround_Moon::Free()
{
	__super::Free();

	Safe_Release(m_pMoonTex);

}
