#include "Electronic.h"
#include "GameInstance.h"
#include "EffectManager.h"
#include "Player.h"

CElectronic::CElectronic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CElectronic::CElectronic(const CElectronic& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CElectronic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectronic::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	OwnDesc = make_shared<ELECTRONICDESC>(*((ELECTRONICDESC*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	


	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&OwnDesc->vStartPos));
	_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4());
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&OwnDesc->vOffset), WorldMat);
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	m_pTransformCom->Set_Scale(OwnDesc->vSize.x, OwnDesc->vSize.y, OwnDesc->vSize.z);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(RandomFloat(0.f, 360.f)));


	if (OwnDesc->fBloomPower > 1.f)
		OwnDesc->fBloomPower = 1.f;
	_float4 ParticlePos;
	XMStoreFloat4(&ParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	EFFECTMGR->Generate_Particle(OwnDesc->ParticleIndex, ParticlePos);

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pGameInstance->Disable_Echo();
	m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_Lightning.ogg"), SOUND_EFFECT);

	return S_OK;
}

void CElectronic::Priority_Tick(_float fTimeDelta)
{
}

void CElectronic::Tick(_float fTimeDelta)
{
	m_fCurLifeTime += fTimeDelta;
	if (m_fCurLifeTime >= OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fRatio = m_fCurLifeTime / OwnDesc->fMaxLifeTime;
	m_fRatio = max(0.f, min(m_fRatio, 1.f));

}

void CElectronic::Late_Tick(_float fTimeDelta)
{
	//_float4 fPos;
	//XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	//if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
	//{
	//	m_pPlayer->PlayerHit(10);
	//}

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CElectronic::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(8);
		m_pModelCom->Render(i);

	}
//#pragma region 모션블러
//	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
//	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
//#pragma endregion 모션블러
	return S_OK;
	return S_OK;
}

HRESULT CElectronic::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(9);

		m_pModelCom->Render(i);

	}
	return S_OK;
}



HRESULT CElectronic::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PerlinNoise"),
		TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseTex))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_InstanceLightning"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CElectronic::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
//#pragma region 모션블러
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
//		return E_FAIL;
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
//		return E_FAIL;
//	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
//	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
//		return E_FAIL;
//#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNoiseTex->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_fCurLifeTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &OwnDesc->vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CElectronic::Bind_BloomResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNoiseTex->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_fCurLifeTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &OwnDesc->vBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}



CElectronic* CElectronic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElectronic* pInstance = new CElectronic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CElectronic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElectronic::Clone(void* pArg)
{
	CElectronic* pInstance = new CElectronic(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CElectronic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CElectronic::Free()
{
	__super::Free();
	Safe_Release(m_pNoiseTex);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}
