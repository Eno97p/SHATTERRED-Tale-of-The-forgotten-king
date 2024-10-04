#include "Rock_Ground.h"
#include "GameInstance.h"

CRock_Ground::CRock_Ground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CRock_Ground::CRock_Ground(const CRock_Ground& rhs)
	:CGameObject(rhs)
{
}

HRESULT CRock_Ground::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRock_Ground::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<ROCK_GROUND>(*((ROCK_GROUND*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	m_pTransformCom->Set_Scale(m_OwnDesc->vMinSize.x, m_OwnDesc->vMinSize.y, m_OwnDesc->vMinSize.z);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(RandomFloat(0.f, 360.f)));
	m_CurSize = m_OwnDesc->vMinSize;
	return S_OK;
}

void CRock_Ground::Priority_Tick(_float fTimeDelta)
{
}

void CRock_Ground::Tick(_float fTimeDelta)
{

	m_fCurLifeTime += fTimeDelta;
	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));


	XMVECTOR vMinSize = XMLoadFloat3(&m_OwnDesc->vMinSize);
	XMVECTOR vMaxSize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
	XMVECTOR vCurSize;
	if (m_fLifeTimeRatio < m_OwnDesc->MinRatio)
	{
		float t = m_fLifeTimeRatio / m_OwnDesc->MinRatio;
		vCurSize = XMVectorLerp(vMinSize, vMaxSize, t);
	}
	else if (m_fLifeTimeRatio >= m_OwnDesc->MaxRatio)
	{
		_vector vCurpos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vCurpos += XMVectorSet(0.f, -1.f, 0.f, 0.f) * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurpos);
		vCurSize = vMaxSize;
	}
	else
	{
		vCurSize = vMaxSize;
	}
	XMStoreFloat3(&m_CurSize, vCurSize);
	m_pTransformCom->Set_Scale(m_CurSize.x, m_CurSize.y, m_CurSize.z);
}

void CRock_Ground::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
}

HRESULT CRock_Ground::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}





HRESULT CRock_Ground::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock_Ground"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRock_Ground::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러

	return S_OK;
}


CRock_Ground* CRock_Ground::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRock_Ground* pInstance = new CRock_Ground(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CRock_Ground");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CRock_Ground::Clone(void* pArg)
{
	CRock_Ground* pInstance = new CRock_Ground(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRock_Ground");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRock_Ground::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
