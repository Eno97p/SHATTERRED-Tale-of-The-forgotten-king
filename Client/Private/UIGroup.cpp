#include "UIGroup.h"

#include "GameInstance.h"

CUIGroup::CUIGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CUIGroup::CUIGroup(const CUIGroup& rhs)
	: CGameObject{rhs}
{
}

HRESULT CUIGroup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup::Initialize(void* pArg)
{
	UIGROUP_DESC* pDesc = static_cast<UIGROUP_DESC*>(pArg);

	m_eLevel = pDesc->eLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIGroup::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup::Tick(_float fTimeDelta)
{
}

void CUIGroup::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUIGroup::Render()
{
	return S_OK;
}

void CUIGroup::Free()
{
	__super::Free();
}
