#include "Meteor.h"
#include "GameInstance.h"
#include "EffectManager.h"
#include "ThirdPersonCamera.h"
#include "Player.h"

CMeteor::CMeteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CWeapon(pDevice, pContext)
{
}

CMeteor::CMeteor(const CMeteor& rhs)
	:CWeapon(rhs)
{
}

HRESULT CMeteor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeteor::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<METEOR_DESC>(*((METEOR_DESC*)pArg));
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	m_CurLifeTime = m_OwnDesc->fLifeTime;
	_vector vDestPos = XMLoadFloat4(&m_OwnDesc->vTargetPos);

	_matrix Mat = XMMatrixIdentity();
	Mat.r[3] = vDestPos;
	//Mat *= XMMatrixRotationY(XMConvertToRadians(RandomFloat(0.f, 360.f)));
	m_vStartPos = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->fStartOffset), Mat);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPos);

	m_pTransformCom->LookAt(vDestPos);


	if (FAILED(Add_Child_Effects()))
		return E_FAIL;

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pGameInstance->Disable_Echo();
	m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_Meteor.ogg"), SOUND_EFFECT);

	return S_OK;
}

void CMeteor::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Priority_Tick(fTimeDelta);
}

void CMeteor::Tick(_float fTimeDelta)
{
	m_CurLifeTime -= fTimeDelta;
	m_fWindSpawn -= fTimeDelta;

	CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
	pThirdPersonCamera->Zoom(90.f, 0.3f, 2.f);

	if (m_CurLifeTime < 0.f)
	{
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

		if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
		{
			m_pPlayer->PlayerHit(10);
		}

		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Malkhel_MeteorExplode.ogg"), SOUND_EFFECT, 0.f, 0.5f);
		EFFECTMGR->Generate_Particle(65, m_OwnDesc->vTargetPos);
		_vector vLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f);
		EFFECTMGR->Generate_Particle(66, m_OwnDesc->vTargetPos, nullptr, XMVectorZero(), 0.f, vLook);
		EFFECTMGR->Generate_Particle(67, m_OwnDesc->vTargetPos);
		EFFECTMGR->Generate_Particle(72, m_OwnDesc->vTargetPos);
		EFFECTMGR->Generate_Particle(73, m_OwnDesc->vTargetPos);
		EFFECTMGR->Generate_Particle(73, m_OwnDesc->vTargetPos, nullptr, XMVectorSet(0.f,1.f,0.f,0.f), 90.f);
		_float4 vShockPos = m_OwnDesc->vTargetPos;
		vShockPos.y += 1.f;
		EFFECTMGR->Generate_Particle(70, vShockPos);

		CThirdPersonCamera* pThirdPersonCamera = dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON]);
		pThirdPersonCamera->Shake_Camera(0.548f, 0.048f, 0.058f, 65.949f);

		m_pGameInstance->Erase(this);
	}

	m_LifeRatio = m_CurLifeTime / m_OwnDesc->fLifeTime;

	if (m_fWindSpawn < 0.f)
	{
		m_fWindSpawn = m_OwnDesc->fWindInterval;
		Generate_Wind();
	}
	
	_vector vDestPos = XMLoadFloat4(&m_OwnDesc->vTargetPos);
	_vector vFinalPos = XMVectorLerp(m_vStartPos, vDestPos, 1.f-m_LifeRatio);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFinalPos);

	for (auto& iter : m_EffectClasses)
		iter->Tick(fTimeDelta);
}

void CMeteor::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CMeteor::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeteor::Add_Child_Effects()
{
	m_OwnDesc->CoreDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->WindDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

	_matrix Targetmat = XMMatrixIdentity();
	Targetmat.r[3] = XMLoadFloat4(&m_OwnDesc->vTargetPos);
	_float4 vParticlePos = m_OwnDesc->vTargetPos;
	_float4x4 CylMat;
	XMStoreFloat4x4(&CylMat, Targetmat);
	m_OwnDesc->CylinderDesc.ParentMatrix = &CylMat;

	vParticlePos.y += 1.5f;
	EFFECTMGR->Generate_Particle(71, vParticlePos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
	vParticlePos.y += 1.5f;
	EFFECTMGR->Generate_Particle(71, vParticlePos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), -90.f);

	CGameObject* Core =  m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Meteor_Core"), &m_OwnDesc->CoreDesc);
	m_EffectClasses.emplace_back(Core);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_DefaultCylinder"), &m_OwnDesc->CylinderDesc);

	return S_OK;
}

HRESULT CMeteor::Generate_Wind()
{
	if (m_OwnDesc->fLifeTime < m_OwnDesc->WindDesc.fMaxLifeTime)
		return S_OK;

	CGameObject* Core = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Meteor_Wind"), &m_OwnDesc->WindDesc);
	m_EffectClasses.emplace_back(Core);

	_float4 ParticlePos;
	XMStoreFloat4(&ParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	EFFECTMGR->Generate_Particle(61, ParticlePos, nullptr, XMVectorZero(), 0.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	EFFECTMGR->Generate_Particle(62, ParticlePos, nullptr, XMVectorZero(), 0.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	EFFECTMGR->Generate_Particle(63, ParticlePos, nullptr, XMVectorZero(), 0.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	return S_OK;
}


void CMeteor::Generate_Final_Particles()
{
}

CMeteor* CMeteor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMeteor* pInstance = new CMeteor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMeteor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMeteor::Clone(void* pArg)
{
	CMeteor* pInstance = new CMeteor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMeteor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeteor::Free()
{
	__super::Free();
	for (auto& iter : m_EffectClasses)
		Safe_Release(iter);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}
