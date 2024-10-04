#include "Heal_Line.h"
#include "GameInstance.h"

CHeal_Line::CHeal_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CHeal_Line::CHeal_Line(const CHeal_Line& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CHeal_Line::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHeal_Line::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<HEAL_LINE>(*((HEAL_LINE*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	m_pTransformCom->Set_Scale(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, m_OwnDesc->vSize.z);
	return S_OK;
}

void CHeal_Line::Priority_Tick(_float fTimeDelta)
{
	if (EffectDead)
		return;
}

void CHeal_Line::Tick(_float fTimeDelta)
{
	if (EffectDead)
		return;

	m_fCurLifeTime += fTimeDelta;
	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		EffectDead = true;
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));


	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ParentMat.r[3]);
}

void CHeal_Line::Late_Tick(_float fTimeDelta)
{
	if (EffectDead)
		return;
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CHeal_Line::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(18);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CHeal_Line::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(19);
		m_pModelCom->Render(i);
	}
	return S_OK;
}


HRESULT CHeal_Line::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SmoothLine"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHeal_Line::Bind_ShaderResources()
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

HRESULT CHeal_Line::Bind_BloomResources()
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

CHeal_Line* CHeal_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHeal_Line* pInstance = new CHeal_Line(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHeal_Line");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHeal_Line::Clone(void* pArg)
{
	CHeal_Line* pInstance = new CHeal_Line(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHeal_Line");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHeal_Line::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
