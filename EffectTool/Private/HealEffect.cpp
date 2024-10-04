#include "HealEffect.h"
#include "GameInstance.h"
CHealEffect::CHealEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CHealEffect::CHealEffect(const CHealEffect& rhs)
	:CGameObject(rhs)
{
}

HRESULT CHealEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHealEffect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<HEALEFFECT>(*((HEALEFFECT*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMat);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	if (FAILED(Add_Child_Effects()))
		return E_FAIL;

	return S_OK;
}

void CHealEffect::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Priority_Tick(fTimeDelta);
}

void CHealEffect::Tick(_float fTimeDelta)
{
	m_OwnDesc->fLifeTime -= fTimeDelta;
	if (m_OwnDesc->fLifeTime < 0.f)
		m_pGameInstance->Erase(this);

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMat);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);

	for (auto& iter : m_EffectClasses)
		iter->Tick(fTimeDelta);
}

void CHealEffect::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CHealEffect::Add_Components()
{
	return S_OK;
}

HRESULT CHealEffect::Add_Child_Effects()
{
	m_OwnDesc->RibbonDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->LineDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->SpiralDesc.ParentMatrix = m_OwnDesc->ParentMat;
	m_OwnDesc->SpiralDesc.fMaxLifeTime = m_OwnDesc->RibbonDesc.fMaxLifeTime;
	m_OwnDesc->LineDesc.fMaxLifeTime = m_OwnDesc->RibbonDesc.fMaxLifeTime;

	CGameObject* StockValue = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HealEffect_Ribbon"), &m_OwnDesc->RibbonDesc);
	m_EffectClasses.emplace_back(StockValue);
	
	StockValue = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HealEffect_Spiral"), &m_OwnDesc->SpiralDesc);
	m_EffectClasses.emplace_back(StockValue);

	StockValue = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HealEffect_Line"), &m_OwnDesc->LineDesc);
	m_EffectClasses.emplace_back(StockValue);
	return S_OK;
}

CHealEffect* CHealEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHealEffect* pInstance = new CHealEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHealEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHealEffect::Clone(void* pArg)
{
	CHealEffect* pInstance = new CHealEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHealEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHealEffect::Free()
{
	__super::Free();
	for (auto& iter : m_EffectClasses)
		Safe_Release(iter);

}
