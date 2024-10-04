#include "UIGroup_BuffTimer.h"

#include "GameInstance.h"
#include "UI_BuffTimer.h"

CUIGroup_BuffTimer::CUIGroup_BuffTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_BuffTimer::CUIGroup_BuffTimer(const CUIGroup_BuffTimer& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_BuffTimer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_BuffTimer::Initialize(void* pArg)
{
	UIGROUP_BUFFTIMER_DESC* pDesc = static_cast<UIGROUP_BUFFTIMER_DESC*>(pArg);

	m_iIndex = pDesc->iBuffTimerIdx;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(Create_BuffTimer())) // Test�� (���� �������� �����ǵ��� ������ ����. ��ġ Ȯ�� ���� �׽�Ʈ������ ȣ��)
	//	return E_FAIL;

	//m_isRend = true; // �׻� �׷������� �ϱ� (��Ұ� ������ �˾Ƽ� �� �׷��� ��) >> �� �� ��ü�� �ʿ� ���� ������?

	return S_OK;
}

void CUIGroup_BuffTimer::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_BuffTimer::Tick(_float fTimeDelta)
{
	Check_BuffEnd();

	for (auto& pUI : m_vecUI)
		pUI->Tick(fTimeDelta);
}

void CUIGroup_BuffTimer::Late_Tick(_float fTimeDelta)
{
	for (auto& pUI : m_vecUI)
		pUI->Late_Tick(fTimeDelta);
}

HRESULT CUIGroup_BuffTimer::Render()
{
	return S_OK;
}

HRESULT CUIGroup_BuffTimer::Create_BuffTimer(wstring wstrTextureName)
{
	vector<CUI_BuffTimer*>::iterator timer = m_vecUI.begin();
	for (size_t i = 0; i < m_vecUI.size(); ++i)
	{
		if ((*timer)->Get_TextureName() == wstrTextureName) // �ߺ��Ǵ� ���� �ִٸ�
		{
			(*timer)->Reset_BuffTime(); // Ÿ�̸Ӹ� �ʱ�ȭ
			return S_OK;
		}
		else
			++timer;
	}

	CUI_BuffTimer::UI_BUFFTIMER_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.iBuffTimerIdx = m_vecUI.size();
	pDesc.wstrTextureName = wstrTextureName;
	m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer"), &pDesc)));

	return S_OK;
}

void CUIGroup_BuffTimer::Check_BuffEnd()
{
	// BuffEnd�� �� �༮���� �������־�� ��
	vector<CUI_BuffTimer*>::iterator timer = m_vecUI.begin();
	for (size_t i = 0; i < m_vecUI.size(); ++i)
	{
		if ((*timer)->Get_isBuffEnd())
		{
			Safe_Release(*timer);
			timer = m_vecUI.erase(timer);

			while (true)
			{
				if (timer == m_vecUI.end())
					return;

				(*timer)->Set_Index(i);

				++i;
				++timer;
			}
		}
		else
			++timer;
	}
}

CUIGroup_BuffTimer* CUIGroup_BuffTimer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_BuffTimer* pInstance = new CUIGroup_BuffTimer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_BuffTimer");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_BuffTimer::Clone(void* pArg)
{
	CUIGroup_BuffTimer* pInstance = new CUIGroup_BuffTimer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_BuffTimer");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_BuffTimer::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
