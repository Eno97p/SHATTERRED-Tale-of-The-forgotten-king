#include "ToolPartObj.h"

#include "GameInstance.h"
#include "ToolObj_Manager.h"

CToolPartObj::CToolPartObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CToolPartObj::CToolPartObj(const CToolPartObj& rhs)
	: CGameObject{ rhs }
{
}

void CToolPartObj::Set_Radian(_float fRight, _float fLook, _float fUp)
{
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(fRight));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(fLook));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(fUp));
}

HRESULT CToolPartObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolPartObj::Initialize(void* pArg)
{
	PARTOBJ_DESC* pPartObjDesc = (PARTOBJ_DESC*)pArg;

	_tchar wstrModelName[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, pPartObjDesc->strModelName.c_str(), strlen(pPartObjDesc->strModelName.c_str()), wstrModelName, MAX_PATH);

	m_iSocketBoneIdx = pPartObjDesc->iBoneIdx;
	m_fRightRadian = pPartObjDesc->fRightRadian;
	m_fLookRadian = pPartObjDesc->fLookRadian;
	m_fUpRadian = pPartObjDesc->fUpRadian;
	m_vPos = pPartObjDesc->vPos;
	m_wstrModelName = wstrModelName;
	m_pParentMatrix = pPartObjDesc->pParentMatrix;
	m_pSocketMatrix = pPartObjDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fRightRadian));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_fLookRadian));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_fUpRadian));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);
	//m_vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	// 여기서 Imgui에 넣어주기
	CToolObj_Manager::GetInstance()->Get_ToolPartObjs().emplace_back(this);

	return S_OK;
}

void CToolPartObj::Priority_Tick(_float fTimeDelta)
{
}

void CToolPartObj::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

}

void CToolPartObj::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CToolPartObj::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CToolPartObj::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolPartObj::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CToolPartObj* CToolPartObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolPartObj* pInstance = new CToolPartObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CToolPartObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolPartObj::Clone(void* pArg)
{
	CToolPartObj* pInstance = new CToolPartObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CToolPartObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolPartObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
