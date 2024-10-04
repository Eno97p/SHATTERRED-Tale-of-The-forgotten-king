#include "Hedgehog.h"
#include "GameInstance.h"
CHedgehog::CHedgehog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CHedgehog::CHedgehog(const CHedgehog& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CHedgehog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHedgehog::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	
	m_OwnDesc = make_unique<HEDGEHOG>(*((HEDGEHOG*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMMATRIX worldmat = XMMatrixIdentity();
	worldmat = XMMatrixScaling(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, m_OwnDesc->vSize.z);
	worldmat.r[3] = XMLoadFloat4(&m_OwnDesc->vStartPos);
	worldmat *= XMMatrixRotationY(XMConvertToRadians(RandomFloat(0.f, 360.f)));
	vStart = worldmat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), worldmat);
	m_pTransformCom->Set_WorldMatrix(worldmat);

	return S_OK;
}

void CHedgehog::Priority_Tick(_float fTimeDelta)
{
}

void CHedgehog::Tick(_float fTimeDelta)
{
	m_fCurLifeTime += fTimeDelta;

	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));

	if (m_fLifeTimeRatio < m_OwnDesc->fThreadRatio.x)
	{
		float t = m_fLifeTimeRatio / m_OwnDesc->fThreadRatio.x;
		_vector DestPos = XMLoadFloat4(&m_OwnDesc->vStartPos);
		_vector ResultPos = XMVectorLerp(vStart, DestPos, t);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, ResultPos);
	}
	else if (m_fLifeTimeRatio > m_OwnDesc->fThreadRatio.y)
	{
		float dissolveProgress = (m_fLifeTimeRatio - m_OwnDesc->fThreadRatio.y) / (1.0f - m_fLifeTimeRatio);

		_vector vStartScale = XMLoadFloat3(&m_OwnDesc->vSize);
		_vector vMaxScale = XMLoadFloat3(&m_OwnDesc->vMaxSize);
		_vector vResult = XMVectorLerp(vStartScale, vMaxScale, dissolveProgress);
		m_pTransformCom->Set_Scale(XMVectorGetX(vResult), XMVectorGetY(vResult), XMVectorGetZ(vResult));
	}
}

void CHedgehog::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CHedgehog::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(29);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CHedgehog::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(30);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CHedgehog::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hedgehog"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_DesolveTexture"), reinterpret_cast<CComponent**>(&m_pDesolveTexture))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHedgehog::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_FrameRatio", &m_OwnDesc->fThreadRatio.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->NumDesolve)))
		return E_FAIL;




	return S_OK;
}

HRESULT CHedgehog::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->BloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_FrameRatio", &m_OwnDesc->fThreadRatio.y, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->NumDesolve)))
		return E_FAIL;
	return S_OK;
}


CHedgehog* CHedgehog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHedgehog* pInstance = new CHedgehog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHedgehog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHedgehog::Clone(void* pArg)
{
	CHedgehog* pInstance = new CHedgehog(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHedgehog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHedgehog::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDesolveTexture);
}
