#include "TargetLock.h"

#include "GameInstance.h"

CTargetLock::CTargetLock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTargetLock::CTargetLock(const CTargetLock& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CTargetLock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTargetLock::Initialize(void* pArg)
{
	TARGETLOCK_DESC* pDesc = static_cast<TARGETLOCK_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;
	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vOffsetPos);
	m_pTransformCom->Set_Scale(pDesc->fScale, pDesc->fScale, pDesc->fScale);

	return S_OK;
}

void CTargetLock::Priority_Tick(_float fTimeDelta)
{
}

void CTargetLock::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CTargetLock::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CTargetLock::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(6);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CTargetLock::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(6);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CTargetLock::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTargetLock::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4 fColor = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &fColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CTargetLock* CTargetLock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTargetLock* pInstance = new CTargetLock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTargetLock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTargetLock::Clone(void* pArg)
{
	CTargetLock* pInstance = new CTargetLock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTargetLock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTargetLock::Free()
{
	__super::Free();

	//Safe_Delete(m_pParentMatrix);
	//Safe_Delete(m_pSocketMatrix);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
