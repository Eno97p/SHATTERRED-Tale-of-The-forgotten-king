#include "Meteor_Core.h"
#include "GameInstance.h"

CMeteor_Core::CMeteor_Core(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CMeteor_Core::CMeteor_Core(const CMeteor_Core& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMeteor_Core::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeteor_Core::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<METEOR_CORE>(*((METEOR_CORE*)pArg));

	GAMEOBJECT_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(m_OwnDesc->fRotationSpeed);
	desc.fSpeedPerSec = 1.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	m_pTransformCom->Set_Scale(m_OwnDesc->vMaxSize.x, m_OwnDesc->vMaxSize.y, m_OwnDesc->vMaxSize.z);

	return S_OK;
}

void CMeteor_Core::Priority_Tick(_float fTimeDelta)
{
	if (EffectDead)
		return;
}

void CMeteor_Core::Tick(_float fTimeDelta)
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

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);
}

void CMeteor_Core::Late_Tick(_float fTimeDelta)
{
	if (EffectDead)
		return;
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
}

HRESULT CMeteor_Core::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
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





HRESULT CMeteor_Core::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Meteor_Core"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeteor_Core::Bind_ShaderResources()
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


CMeteor_Core* CMeteor_Core::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMeteor_Core* pInstance = new CMeteor_Core(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMeteor_Core");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMeteor_Core::Clone(void* pArg)
{
	CMeteor_Core* pInstance = new CMeteor_Core(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMeteor_Core");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeteor_Core::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
