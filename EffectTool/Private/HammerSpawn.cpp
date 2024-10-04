#include "HammerSpawn.h"
#include "GameInstance.h"
CHammerSpawn::CHammerSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CHammerSpawn::CHammerSpawn(const CHammerSpawn& rhs)
	:CGameObject(rhs)
{
}

HRESULT CHammerSpawn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHammerSpawn::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<HAMMERSPAWN>(*((HAMMERSPAWN*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));

	if (FAILED(Add_Child_Effects()))
		return E_FAIL;

	return S_OK;
}

void CHammerSpawn::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Priority_Tick(fTimeDelta);
}

void CHammerSpawn::Tick(_float fTimeDelta)
{
	m_OwnDesc->fLifeTime -= fTimeDelta;
	if (m_OwnDesc->fLifeTime < 0.f)
		m_pGameInstance->Erase(this);


	for (auto& iter : m_EffectClasses)
		iter->Tick(fTimeDelta);
}

void CHammerSpawn::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CHammerSpawn::Add_Components()
{
	return S_OK;
}

HRESULT CHammerSpawn::Add_Child_Effects()
{
	m_OwnDesc->RibbonDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->VaneDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CGameObject* StockValue = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HealEffect_Ribbon"), &m_OwnDesc->RibbonDesc);
	m_EffectClasses.emplace_back(StockValue);

	CGameObject* Vane = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Vane"), &m_OwnDesc->VaneDesc);
	m_EffectClasses.emplace_back(Vane);

	return S_OK;
}

CHammerSpawn* CHammerSpawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHammerSpawn* pInstance = new CHammerSpawn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHammerSpawn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHammerSpawn::Clone(void* pArg)
{
	CHammerSpawn* pInstance = new CHammerSpawn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHammerSpawn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHammerSpawn::Free()
{
	__super::Free();
	for (auto& iter : m_EffectClasses)
		Safe_Release(iter);
}
