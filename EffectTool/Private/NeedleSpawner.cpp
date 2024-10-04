#include "NeedleSpawner.h"
#include "GameInstance.h"

CNeedleSpawner::CNeedleSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CNeedleSpawner::CNeedleSpawner(const CNeedleSpawner& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CNeedleSpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNeedleSpawner::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<NEEDLESPAWNER>(*((NEEDLESPAWNER*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));
	m_pTransformCom->Set_Scale(m_OwnDesc->vMinSize.x, m_OwnDesc->vMinSize.y, m_OwnDesc->vMinSize.z);
	return S_OK;
}

void CNeedleSpawner::Priority_Tick(_float fTimeDelta)
{

}

void CNeedleSpawner::Tick(_float fTimeDelta)
{

	if (m_fLifeTimeRatio > m_OwnDesc->fThreadRatio.x && m_fLifeTimeRatio < m_OwnDesc->fThreadRatio.y)
		m_fCurLifeTime += fTimeDelta * m_OwnDesc->fSlowStrength;
	else
		m_fCurLifeTime += fTimeDelta;


	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));

	if (m_fLifeTimeRatio > m_OwnDesc->SpawnTiming && !Spawned)
	{
		Spawned = true;
		XMStoreFloat4(&m_OwnDesc->ChildDesc.vStartPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Hedgehog"), &m_OwnDesc->ChildDesc);
	}

	_vector vStartSize = XMLoadFloat3(&m_OwnDesc->vMinSize);
	_vector vMaxSize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
	_vector vResultSize = XMVectorLerp(vStartSize, vMaxSize, m_fLifeTimeRatio);
	m_pTransformCom->Set_Scale(XMVectorGetX(vResultSize), XMVectorGetY(vResultSize), XMVectorGetZ(vResultSize));
}

void CNeedleSpawner::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CNeedleSpawner::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(20);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CNeedleSpawner::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(21);
		m_pModelCom->Render(i);
	}
	return S_OK;
}


HRESULT CNeedleSpawner::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NeedleSpawner"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CNeedleSpawner::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;


	return S_OK;
}

HRESULT CNeedleSpawner::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->BloomColor, sizeof(_float3))))
		return E_FAIL;


	return S_OK;
}

CNeedleSpawner* CNeedleSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNeedleSpawner* pInstance = new CNeedleSpawner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNeedleSpawner");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNeedleSpawner::Clone(void* pArg)
{
	CNeedleSpawner* pInstance = new CNeedleSpawner(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNeedleSpawner");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNeedleSpawner::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
