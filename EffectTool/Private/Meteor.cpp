#include "Meteor.h"
#include "GameInstance.h"
#include "Player.h"
CMeteor::CMeteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CMeteor::CMeteor(const CMeteor& rhs)
	:CGameObject(rhs)
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
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	m_CurLifeTime = m_OwnDesc->fLifeTime;
	_vector vDestPos = XMLoadFloat4(&m_OwnDesc->vTargetPos);

	_matrix Mat = XMMatrixIdentity();
	Mat.r[3] = vDestPos;
	Mat *= XMMatrixRotationY(XMConvertToRadians(RandomFloat(0.f, 360.f)));
	m_vStartPos = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->fStartOffset), Mat);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPos);

	m_pTransformCom->LookAt(vDestPos);


	if (FAILED(Add_Child_Effects()))
		return E_FAIL;

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
	if (m_CurLifeTime < 0.f)
	{
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
	return S_OK;
}

HRESULT CMeteor::Add_Child_Effects()
{
	m_OwnDesc->CoreDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->WindDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

	_matrix Targetmat = XMMatrixIdentity();
	Targetmat.r[3] = XMLoadFloat4(&m_OwnDesc->vTargetPos);
	_float4x4 CylMat;
	XMStoreFloat4x4(&CylMat, Targetmat);
	m_OwnDesc->CylinderDesc.ParentMatrix = &CylMat;

	CGameObject* Core =  m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Meteor_Core"), &m_OwnDesc->CoreDesc);
	m_EffectClasses.emplace_back(Core);

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Meteor"),
		TEXT("Prototype_GameObject_DefaultCylinder"), &m_OwnDesc->CylinderDesc);

	return S_OK;
}

HRESULT CMeteor::Generate_Wind()
{
	if (m_OwnDesc->fLifeTime < m_OwnDesc->WindDesc.fMaxLifeTime)
		return S_OK;

	CGameObject* Core = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Meteor_Wind"), &m_OwnDesc->WindDesc);
	m_EffectClasses.emplace_back(Core);

	return S_OK;
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
}
