#include "BackGround_Card.h"
#include "GameInstance.h"

CBackGround_Card::CBackGround_Card(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMap_Element(pDevice, pContext)
{
}

CBackGround_Card::CBackGround_Card(const CBackGround_Card& rhs)
	:CMap_Element(rhs)
{
}

HRESULT CBackGround_Card::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround_Card::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;
	if (pArg != nullptr) 
	{
		CARD_DESC* cardDesc = (CARD_DESC*)pArg;
		m_iTexNum = cardDesc->iTexNum;
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&cardDesc->mWorldMatrix));
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackGround_Card::Priority_Tick(_float fTimeDelta)
{
}

void CBackGround_Card::Tick(_float fTimeDelta)
{
	m_pTransformCom->BillBoard_Y();

}

void CBackGround_Card::Late_Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_OwnDesc->vStartScale.y))
	//{
	//	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//	m_pTransformCom->BillBoard();
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	//	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	//}
}


HRESULT CBackGround_Card::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_TextureNum", &m_iTexNum, sizeof(_uint))))
			return E_FAIL;

		m_pShaderCom->Begin(7); //COLOR

		m_pModelCom->Render(i);
	}
}


HRESULT CBackGround_Card::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mountain_Card"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CBackGround_Card::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_MainCamera()->Get_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}


CBackGround_Card* CBackGround_Card::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround_Card* pInstance = new CBackGround_Card(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBackGround_Card");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBackGround_Card::Clone(void* pArg)
{
	CBackGround_Card* pInstance = new CBackGround_Card(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBackGround_Card");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround_Card::Free()
{
	__super::Free();

}
