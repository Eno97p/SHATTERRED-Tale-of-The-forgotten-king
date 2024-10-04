#include "BackGround_Moon.h"
#include "GameInstance.h"
#include "ToolObj_Manager.h"

CBackGround_Moon::CBackGround_Moon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CToolObj(pDevice, pContext)
{
}

CBackGround_Moon::CBackGround_Moon(const CBackGround_Moon& rhs)
	:CToolObj(rhs)
{
}

HRESULT CBackGround_Moon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround_Moon::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;
	if (pArg != nullptr) 
	{
		strcpy_s(m_szName, "Prototype_GameObject_BackGround_Moon");
		strcpy_s(m_szLayer, "Layer_BGCards");
		strcpy_s(m_szModelName, "Prototype_Component_Model_BGMoon");

		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;

		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		//_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	}




	if (FAILED(Add_Components()))
		return E_FAIL;
	CARD_DESC* cardDesc = (CARD_DESC*)pArg;

	m_iTexNum = cardDesc->iTexNum;
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

		m_pShaderCom->Begin(11); //COLOR

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

		if (FAILED(m_pMoonTex->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 2)))
			return E_FAIL;

		m_pShaderCom->Begin(11); //COLOR

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
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
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

}
