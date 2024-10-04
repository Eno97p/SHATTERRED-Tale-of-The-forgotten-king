#include "TornadoWind.h"
#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"

CTornado_Wind::CTornado_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CWeapon(pDevice, pContext)
{
}

CTornado_Wind::CTornado_Wind(const CTornado_Wind& rhs)
	:CWeapon(rhs)
{
}

HRESULT CTornado_Wind::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTornado_Wind::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<TORNADO_WIND>(*((TORNADO_WIND*)pArg));
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(m_OwnDesc->fRotationSpeed);
	desc.fSpeedPerSec = 0.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	switch (m_OwnDesc->NumModels)
	{
	case Tor_1:
		m_ModelProtoName = TEXT("Prototype_Component_Model_TornadoWind");
		break;
	case Tor_2:
		m_ModelProtoName = TEXT("Prototype_Component_Model_TornadoWind2");
		break;
	case Tor_3:
		m_ModelProtoName = TEXT("Prototype_Component_Model_TornadoWind3");
		break;
	default:
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;
	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_CurrentSize = { 0.f,0.f,0.f };
	return S_OK;
}

void CTornado_Wind::Priority_Tick(_float fTimeDelta)
{
	if (EffectDead)
		return;
}

void CTornado_Wind::Tick(_float fTimeDelta)
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

	if (m_fLifeTimeRatio < 0.5f)
	{
		_vector CurSize = XMLoadFloat3(&m_CurrentSize);
		_vector LerpSize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
		CurSize = XMVectorLerp(CurSize, LerpSize, fTimeDelta * m_OwnDesc->fGrowSpeed);
		XMStoreFloat3(&m_CurrentSize, CurSize);
	}

	m_pTransformCom->Set_Scale(m_CurrentSize.x, m_CurrentSize.y, m_CurrentSize.z);
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	_matrix ParentMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	_vector vFinalPos = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), ParentMat);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFinalPos);
}

void CTornado_Wind::Late_Tick(_float fTimeDelta)
{
	if (EffectDead)
		return;

	m_fHitCoolTime -= fTimeDelta;
	if (!m_bIsActive)
	{
		m_bIsActive = true;
	}
	if (m_fHitCoolTime < 0.f)
	{
		m_bIsActive = false;
		m_fHitCoolTime = HITCOOLTIME;
	}

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	// 토네이도와 플레이어 충돌처리
	if (m_bIsActive)
	{
		if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
		{
			m_pPlayer->PlayerHit(5);
		}
	}
	else
	{
		m_pColliderCom->Reset();
	}

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	if (m_OwnDesc->IsDistortion)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);

#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
#endif
}

HRESULT CTornado_Wind::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
			return E_FAIL;

		m_pShaderCom->Begin(6);

		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CTornado_Wind::Render_Bloom()
{
	if (FAILED(Bind_BloomResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
			return E_FAIL;

		m_pShaderCom->Begin(12);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CTornado_Wind::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTex", i, aiTextureType_OPACITY)))
			return E_FAIL;

		m_pShaderCom->Begin(6);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CTornado_Wind::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(1.f, 2.f, 1.f);
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

HRESULT CTornado_Wind::Bind_ShaderResources()
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

HRESULT CTornado_Wind::Bind_BloomResources()
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

CTornado_Wind* CTornado_Wind::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTornado_Wind* pInstance = new CTornado_Wind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTornado_Wind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTornado_Wind::Clone(void* pArg)
{
	CTornado_Wind* pInstance = new CTornado_Wind(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTornado_Wind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTornado_Wind::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}
