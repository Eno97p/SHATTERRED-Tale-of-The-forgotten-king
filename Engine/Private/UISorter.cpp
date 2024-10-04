#include "UISorter.h"
#include "GameInstance.h"
#include "GameObject.h"

CUISorter::CUISorter()
{

}

HRESULT CUISorter::Initialize()
{
    return S_OK;
}

void CUISorter::Sorting()
{
	for (int i = 0; i < UISORT_PRIORITY::SORT_END; ++i)
	{
		for (auto& iter : m_VecUI[i])
		{
			if (iter == nullptr)
				continue;
			CGameInstance::GetInstance()->Add_RenderObject(CRenderer::RENDER_UI, iter);
		}
	}
	Clear();
}

HRESULT CUISorter::Add_UI(CGameObject* ui, UISORT_PRIORITY type)
{
	Safe_AddRef(ui);
	m_VecUI[type].emplace_back(ui);
	return S_OK;
}

void CUISorter::Clear()
{
	for (int i = 0; i < SORT_END; ++i)
	{
		for (auto& iter : m_VecUI[i])
		{
			Safe_Release(iter);
		}
		m_VecUI[i].clear();
	}
}

CUISorter* CUISorter::Create()
{
	CUISorter* pInstance = new CUISorter();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : UISorter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUISorter::Free()
{

}
