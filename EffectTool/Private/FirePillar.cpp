#include "FirePillar.h"
#include "GameInstance.h"
CFirePillar::CFirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CFirePillar::CFirePillar(const CFirePillar& rhs)
	:CGameObject(rhs)
{
}

HRESULT CFirePillar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFirePillar::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_OwnDesc = make_shared<FIREPILLAR>(*((FIREPILLAR*)pArg));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));
	if (FAILED(Add_Components()))
		return E_FAIL;

	//if (FAILED(Add_Child_Effects()))
	//	return E_FAIL;

	m_OwnDesc->Charge.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CGameObject* Ribbon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_FirePillar_Charge"), &m_OwnDesc->Charge);
	m_ChildEffects.emplace_back(Ribbon);
	return S_OK;
}

void CFirePillar::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_ChildEffects)
		iter->Priority_Tick(fTimeDelta);
}

void CFirePillar::Tick(_float fTimeDelta)
{
	for (auto& iter : m_ChildEffects)
		iter->Tick(fTimeDelta);

	m_OwnDesc->fLifeTime -= fTimeDelta;
	if (m_OwnDesc->fLifeTime < 0.f)
	{
		m_pGameInstance->Erase(this);
	}

	if (!m_bEffectOn && static_cast<CFirePillar_Charge*>(m_ChildEffects[0])->Get_Effect_Dead())
	{
		Add_Child_Effects();
		m_bEffectOn = true;
	}

}

void CFirePillar::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_ChildEffects)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CFirePillar::Add_Components()
{
	return S_OK;
}

HRESULT CFirePillar::Add_Child_Effects()
{
	m_OwnDesc->pillar1.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->pillar2.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->pillar4.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->Bottom.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->Ground.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

	m_OwnDesc->pillar1.NumModels = CFirePillarEffect::FIREPILLARMODELNUM::F_1;
	m_OwnDesc->pillar2.NumModels = CFirePillarEffect::FIREPILLARMODELNUM::F_2;
	m_OwnDesc->pillar4.NumModels = CFirePillarEffect::FIREPILLARMODELNUM::F_4;

	m_OwnDesc->pillar1.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->pillar2.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->pillar4.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->Bottom.fMaxLifeTime = m_OwnDesc->fLifeTime;


	m_OwnDesc->pillar1.SizeInterval = m_OwnDesc->Interval;
	m_OwnDesc->pillar2.SizeInterval = m_OwnDesc->Interval;
	m_OwnDesc->pillar4.SizeInterval = m_OwnDesc->Interval;


	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FirePillar"),
		TEXT("Prototype_GameObject_FirePillar_Effect"), &m_OwnDesc->pillar1);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FirePillar"),
		TEXT("Prototype_GameObject_FirePillar_Effect"), &m_OwnDesc->pillar2);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FirePillar"),
		TEXT("Prototype_GameObject_FirePillar_Effect"), &m_OwnDesc->pillar4);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FirePillar"),
		TEXT("Prototype_GameObject_FirePillar_Bottom"), &m_OwnDesc->Bottom);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FirePillar"),
		TEXT("Prototype_GameObject_Rock_Ground"), &m_OwnDesc->Ground);

	return S_OK;
}

CFirePillar* CFirePillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFirePillar* pInstance = new CFirePillar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFirePillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFirePillar::Clone(void* pArg)
{
	CFirePillar* pInstance = new CFirePillar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFirePillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFirePillar::Free()
{
	__super::Free();
	for (auto& iter : m_ChildEffects)
		Safe_Release(iter);
}
