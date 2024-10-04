#include "UI_Interaction.h"

#include "GameInstance.h"
#include "CMouse.h"

CUI_Interaction::CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_Interaction::CUI_Interaction(const CUI_Interaction& rhs)
	: CUI{rhs}
{
}

_bool CUI_Interaction::IsCollisionRect(RECT* SrcRect)
{
	RECT rt = {};
	return IntersectRect(&rt, &m_CollisionRect, SrcRect);
}

HRESULT CUI_Interaction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Interaction::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pMouse = static_cast<CMouse*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Mouse")));

	return S_OK;
}

void CUI_Interaction::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Interaction::Tick(_float fTimeDelta)
{
	m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.3f),
						LONG(m_fY - m_fSizeY * 0.3f),
						LONG(m_fX + m_fSizeX * 0.3f),
						LONG(m_fY + m_fSizeY * 0.3f) };
}

void CUI_Interaction::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUI_Interaction::Render()
{
	return S_OK;
}

void CUI_Interaction::Free()
{
	__super::Free();

	m_pMouse = nullptr;
}
