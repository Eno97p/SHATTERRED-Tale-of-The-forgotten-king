#include "Weapon_Arrow_LGGun.h"

#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"
#include "Legionnaire_Gun.h"

CWeapon_Arrow_LGGun::CWeapon_Arrow_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Arrow_LGGun::CWeapon_Arrow_LGGun(const CWeapon_Arrow_LGGun& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CWeapon_Arrow_LGGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Arrow_LGGun::Initialize(void* pArg)
{
	ARROW_DESC* pDesc = (ARROW_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pParent = pDesc->pParent;
	Safe_AddRef(m_pParent);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, pDesc->vDir);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));


	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

void CWeapon_Arrow_LGGun::Priority_Tick(_float fTimeDelta)
{
}

void CWeapon_Arrow_LGGun::Tick(_float fTimeDelta)
{
	fMaxLifeTime -= fTimeDelta;
	if (fMaxLifeTime < 0.f)
		m_pGameInstance->Erase(this);

	m_pTransformCom->Go_Straight(fTimeDelta);
	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsParried)
	{
		m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
		if (m_eColltype == CCollider::COLL_START)
		{
			if (m_pPlayer->Get_Parry())
			{
				
				EFFECTMGR->Generate_Particle(4, fPos, nullptr, XMVectorZero(), 0.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				EFFECTMGR->Generate_Particle(5, fPos);
				m_pPlayer->Parry_Succeed();
				m_bIsParried = true;
				m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
			}
			else
			{
				m_pPlayer->PlayerHit(10);
				m_pGameInstance->Erase(this);
			}
		}
	}
	else
	{
		m_eColltype = m_pColliderCom->Intersect(m_pParent->Get_Collider());
		if (m_eColltype == CCollider::COLL_START)
		{
			m_pParent->Add_Hp(-10);
			m_pGameInstance->Erase(this);
		}
	}

	fAccParticle -= fTimeDelta;
	if (fAccParticle < 0.f && m_bIsParried)
	{
		
		EFFECTMGR->Generate_Particle(3, fPos, nullptr,XMVectorZero(),0.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		fAccParticle = RandomFloat(0.1f, 0.2f);
	}
	_float4 ParticlePosition;
	XMStoreFloat4(&ParticlePosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	EFFECTMGR->Generate_Particle(19, ParticlePosition, nullptr, m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 90.f);

}

void CWeapon_Arrow_LGGun::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CWeapon_Arrow_LGGun::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CWeapon_Arrow_LGGun::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float4 fPos = m_pGameInstance->Get_PlayerPos();

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(fPos.x, fPos.y + 10.f, fPos.z - 10.f, 1.f), XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CWeapon_Arrow_LGGun::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vExtents.z);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CWeapon_Arrow_LGGun::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
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
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CWeapon_Arrow_LGGun* CWeapon_Arrow_LGGun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Arrow_LGGun* pInstance = new CWeapon_Arrow_LGGun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CWeapon_Arrow_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Arrow_LGGun::Clone(void* pArg)
{
	CWeapon_Arrow_LGGun* pInstance = new CWeapon_Arrow_LGGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CWeapon_Arrow_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Arrow_LGGun::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
	Safe_Release(m_pParent);
}
