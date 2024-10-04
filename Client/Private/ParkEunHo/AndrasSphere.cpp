#include "AndrasSphere.h"
#include "GameInstance.h"

CAndrasSphere::CAndrasSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CAndrasSphere::CAndrasSphere(const CAndrasSphere& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CAndrasSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndrasSphere::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<ANDRASSPHERE>(*((ANDRASSPHERE*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	m_pTransformCom->Set_Scale(m_OwnDesc->vSize.x, m_OwnDesc->vSize.y, m_OwnDesc->vSize.z);
	return S_OK;
}

void CAndrasSphere::Priority_Tick(_float fTimeDelta)
{
}

void CAndrasSphere::Tick(_float fTimeDelta)
{

	if (m_fLifeTimeRatio > m_OwnDesc->fThreadRatio.x && m_fLifeTimeRatio < m_OwnDesc->fThreadRatio.y)
		m_fCurLifeTime += fTimeDelta * m_OwnDesc->fSlowStrength;
	else
		m_fCurLifeTime += fTimeDelta;

	m_UvTime += fTimeDelta;

	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));

	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	_vector OrgSize = XMLoadFloat3(&m_OwnDesc->vSize);
	_vector EndSize = XMLoadFloat3(&m_OwnDesc->vEndSized);
	_vector ResultSize = XMVectorLerp(OrgSize, EndSize, m_fLifeTimeRatio);
	m_pTransformCom->Set_Scale(ResultSize.m128_f32[0], ResultSize.m128_f32[1], ResultSize.m128_f32[2]);
}

void CAndrasSphere::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CAndrasSphere::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(38);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CAndrasSphere::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(39);
		m_pModelCom->Render(i);
	}
	return S_OK;
}


HRESULT CAndrasSphere::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Sphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAndrasSphere::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_UvTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OpacityPower", &m_OwnDesc->OppacityPower, sizeof(_float))))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CAndrasSphere::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_UvTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OpacityPower", &m_OwnDesc->OppacityPower, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CAndrasSphere* CAndrasSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndrasSphere* pInstance = new CAndrasSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAndrasSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAndrasSphere::Clone(void* pArg)
{
	CAndrasSphere* pInstance = new CAndrasSphere(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAndrasSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAndrasSphere::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
