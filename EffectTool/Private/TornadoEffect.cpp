#include "TornadoEffect.h"
#include "GameInstance.h"
CTornadoEffect::CTornadoEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CTornadoEffect::CTornadoEffect(const CTornadoEffect& rhs)
    :CGameObject(rhs)
{
}

HRESULT CTornadoEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTornadoEffect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<TORNADODESC>(*((TORNADODESC*)pArg));

	CGameObject::GAMEOBJECT_DESC GDesc{};
	GDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GDesc.fSpeedPerSec = m_OwnDesc->fSpeed;
	if (FAILED(__super::Initialize(&GDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	RootInterval = m_OwnDesc->fRootInterval;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));
	if (FAILED(Add_Child_Effects()))
		return E_FAIL;
    return S_OK;
}

void CTornadoEffect::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Priority_Tick(fTimeDelta);
}

void CTornadoEffect::Tick(_float fTimeDelta)
{
	if (m_OwnDesc->pTarget == nullptr)
		return;
	m_OwnDesc->fLifeTime -= fTimeDelta;
	if (m_OwnDesc->fLifeTime < 0.f)
	{
		m_OwnDesc->fLifeTime = 0.f;
		m_pGameInstance->Erase(this);
	}

	RootInterval -= fTimeDelta;
	if (RootInterval < 0.f)
	{
		RootInterval = m_OwnDesc->fRootInterval;
		Generate_RootWind();
	}


	CTransform* TargetTransform = static_cast<CTransform*>(m_OwnDesc->pTarget->Get_Component(TEXT("Com_Transform")));
	m_pTransformCom->TurnToTarget(fTimeDelta, TargetTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Go_Straight(fTimeDelta);

	for (auto& iter : m_EffectClasses)
		iter->Tick(fTimeDelta);

}

void CTornadoEffect::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CTornadoEffect::Add_Components()
{
    return S_OK;
}

HRESULT CTornadoEffect::Add_Child_Effects()
{
	m_OwnDesc->WindDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->WindDesc2.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->WindDesc3.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->RingDesc1.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->RingDesc2.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

	m_OwnDesc->WindDesc.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->WindDesc2.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->WindDesc3.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->RingDesc1.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->RingDesc2.fMaxLifeTime = m_OwnDesc->fLifeTime;

	m_OwnDesc->WindDesc.NumModels = 0;
	m_OwnDesc->WindDesc2.NumModels = 1;
	m_OwnDesc->WindDesc3.NumModels = 2;

	CGameObject* Wind = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Tornado_Wind"), &m_OwnDesc->WindDesc);
	m_EffectClasses.emplace_back(Wind);

	Wind = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Tornado_Wind"), &m_OwnDesc->WindDesc2);
	m_EffectClasses.emplace_back(Wind);

	Wind = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Tornado_Wind"), &m_OwnDesc->WindDesc3);
	m_EffectClasses.emplace_back(Wind);

	CGameObject* Ring = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Tornado_Ring"), &m_OwnDesc->RingDesc1);
	m_EffectClasses.emplace_back(Ring);

	m_OwnDesc->RingDesc2.StartWithBody = true;
	Ring = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Tornado_Ring"), &m_OwnDesc->RingDesc2);
	m_EffectClasses.emplace_back(Ring);


	return S_OK;
}

void CTornadoEffect::Generate_RootWind()
{
	if (m_OwnDesc->fLifeTime < m_OwnDesc->RootDesc.fMaxLifeTime)
		return;
	m_OwnDesc->RootDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CGameObject* Root = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Tornado_Root"), &m_OwnDesc->RootDesc);
	m_EffectClasses.emplace_back(Root);
}

CTornadoEffect* CTornadoEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTornadoEffect* pInstance = new CTornadoEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTornadoEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTornadoEffect::Clone(void* pArg)
{
	CTornadoEffect* pInstance = new CTornadoEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTornadoEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTornadoEffect::Free()
{
    __super::Free();
	for (auto& iter : m_EffectClasses)
		Safe_Release(iter);
	m_EffectClasses.clear();

}
