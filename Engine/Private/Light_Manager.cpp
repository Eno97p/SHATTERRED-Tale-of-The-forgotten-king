#include "..\Public\Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	if (iIndex >= m_Lights.size())
	{
		MSG_BOX("잘못된 Light 접근인데용");
		return m_Lights.back()->Get_LightDesc();
	}
	auto iter = m_Lights.begin();
	std::advance(iter, iIndex);
	return (*iter)->Get_LightDesc();
}


void CLight_Manager::LightOff(_uint iIndex)
{
	if (iIndex >= m_Lights.size())
	{
		MSG_BOX("잘못된 Light 접근입니다");
		return;
	}

	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	(*iter)->LightOff();

	return;
}

void CLight_Manager::LightOn(_uint iIndex)
{
	if (iIndex >= m_Lights.size())
	{
		MSG_BOX("잘못된 Light 접근입니다");
		return;
	}

	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	(*iter)->LightOn();

	return;
}

void CLight_Manager::LightOff_All()
{
	if (!m_Lights.empty())
	{
		for (auto& light : m_Lights)
		{
			light->LightOff();
			Safe_Release(light);
		}
		m_Lights.clear();

	}
	else
	{

		MSG_BOX("빛이 없습니다");
	}
	
	return;

}

HRESULT CLight_Manager::Light_Clear()
{
	if (!m_Lights.empty())
	{
		for (auto& light : m_Lights)
		{
			light->LightOff();
			Safe_Release(light);
		}
		m_Lights.clear();

	}


	return S_OK;

}

void CLight_Manager::Update_LightPos(_uint iIndex, _vector LightPos)
{
	if (iIndex >= m_Lights.size())
		return;

	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	(*iter)->Set_LightPos(LightPos);

	return;
}


void CLight_Manager::Edit_Light(_uint iIndex, LIGHT_DESC* desc)
{
	if (iIndex >= m_Lights.size())
		return;

	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	(*iter)->Set_LightDesc(*desc);

	return;
}

HRESULT CLight_Manager::Initialize()
{

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
	CLight* pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace_back(pLight);

	pLight->LightOn();

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer);

	return S_OK;
}

CLight_Manager* CLight_Manager::Create()
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLight_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
