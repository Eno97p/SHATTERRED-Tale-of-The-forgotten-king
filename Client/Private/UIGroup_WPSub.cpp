#include "UIGroup_WPSub.h"

#include "GameInstance.h"
#include "UI.h"

CUIGroup_WPSub::CUIGroup_WPSub(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIGroup{ pDevice, pContext }
{
}

CUIGroup_WPSub::CUIGroup_WPSub(const CUIGroup_WPSub& rhs)
    : CUIGroup{ rhs }
{
}

HRESULT CUIGroup_WPSub::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIGroup_WPSub::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUIGroup_WPSub::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_WPSub::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			if (!m_isRenderOnAnim && !(pUI->Get_RenderOnAnim()))
			{
				pUI->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pUI->Get_RenderOnAnim())
			{
				pUI->Resset_Animation(false);
			}

			pUI->Tick(fTimeDelta);

			isRender_End = pUI->isRender_End();
		}
		if (isRender_End)
			m_isRend = false;
	}
}

void CUIGroup_WPSub::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_WPSub::Render()
{
	return S_OK;
}

HRESULT CUIGroup_WPSub::Create_UI()
{
    return S_OK;
}

CUIGroup_WPSub* CUIGroup_WPSub::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_WPSub* pInstance = new CUIGroup_WPSub(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_WPSub");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_WPSub::Clone(void* pArg)
{
	CUIGroup_WPSub* pInstance = new CUIGroup_WPSub(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_WPSub");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_WPSub::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
