#include "FirePillarEffect.h"
#include "GameInstance.h"
#include "Player.h"

CFirePillarEffect::CFirePillarEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CFirePillarEffect::CFirePillarEffect(const CFirePillarEffect& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CFirePillarEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFirePillarEffect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<FIREPILLAREFFECTDESC>(*((FIREPILLAREFFECTDESC*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	switch (m_OwnDesc->NumModels)
	{
	case F_1:
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Juggulus_Flame.ogg"), SOUND_EFFECT);
		m_ModelProtoName = TEXT("Prototype_Component_Model_FirePillar1");
		break;
	case F_2:
		m_ModelProtoName = TEXT("Prototype_Component_Model_FirePillar2");
		break;
	case F_4:
		m_ModelProtoName = TEXT("Prototype_Component_Model_FirePillar4");
		break;
	default:
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_CurrentSize = { 0.f,0.f,0.f };

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	return S_OK;
}

void CFirePillarEffect::Priority_Tick(_float fTimeDelta)
{

}

void CFirePillarEffect::Tick(_float fTimeDelta)
{
	m_fCurLifeTime += fTimeDelta;
	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));


	_vector CurSize = XMLoadFloat3(&m_CurrentSize);

	if (!SizeUpDone[0] && !SizeUpDone[1])
	{
		CurSize = XMVectorLerp(CurSize, XMLoadFloat3(&m_OwnDesc->vMaxSize) * 2, 0.5f);
		XMStoreFloat3(&m_CurrentSize, CurSize);
		if (((m_OwnDesc->vMaxSize.x * 2.f) - m_CurrentSize.x) < 0.1f)
			SizeUpDone[0] = true;

	}
	else if(SizeUpDone[0]&& !SizeUpDone[1])
	{
		CurSize = XMVectorLerp(CurSize, XMLoadFloat3(&m_OwnDesc->vMaxSize), 0.2f);
		XMStoreFloat3(&m_CurrentSize, CurSize);
		if ((m_CurrentSize.x - (m_OwnDesc->vMaxSize.x)) < 0.1f)
			SizeUpDone[1] = true;
	}
	else
	{
		if (!SizeDirectionChange)
		{
			m_CurrentSize.x -= m_OwnDesc->fGrowSpeed * fTimeDelta;
			m_CurrentSize.z -= m_OwnDesc->fGrowSpeed * fTimeDelta;
		}
		else
		{
			m_CurrentSize.x += m_OwnDesc->fGrowSpeed * fTimeDelta;
			m_CurrentSize.z += m_OwnDesc->fGrowSpeed * fTimeDelta;
		}
	}

	m_Interval -= fTimeDelta;
	if (m_Interval < 0.f)
	{
		m_Interval = m_OwnDesc->SizeInterval;
		SizeDirectionChange = !SizeDirectionChange;
	}

	m_pTransformCom->Set_Scale(m_CurrentSize.x, m_CurrentSize.y, m_CurrentSize.z);
}

void CFirePillarEffect::Late_Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	// 플레이어 충돌처리
	if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START && m_OwnDesc->NumModels == F_1)
	{
		m_pPlayer->PlayerHit(10);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	if(m_OwnDesc->IsDistortion)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CFirePillarEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (m_OwnDesc->NumModels != F_4)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}
		m_pShaderCom->Begin(6);

		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CFirePillarEffect::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (m_OwnDesc->NumModels != F_4)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(12);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CFirePillarEffect::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (m_OwnDesc->NumModels != F_4)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(6);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CFirePillarEffect::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(1.f, 10.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_ModelProtoName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Desolve"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFirePillarEffect::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->NumDesolve)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_fCurLifeTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RadialStrength", &m_OwnDesc->RadicalStrength, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OpacityPower", &m_OwnDesc->OpacityPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color2", &m_OwnDesc->fColor2, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Opacity", &m_OwnDesc->Opacity, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFirePillarEffect::Bind_BloomResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->NumDesolve)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_fCurLifeTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RadialStrength", &m_OwnDesc->RadicalStrength, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomPower", &m_OwnDesc->fBloomPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OpacityPower", &m_OwnDesc->OpacityPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color2", &m_OwnDesc->fColor2, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Opacity", &m_OwnDesc->Opacity, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CFirePillarEffect* CFirePillarEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFirePillarEffect* pInstance = new CFirePillarEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFirePillarEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFirePillarEffect::Clone(void* pArg)
{
	CFirePillarEffect* pInstance = new CFirePillarEffect(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFirePillarEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFirePillarEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}
