#pragma once

#include "Base.h"

/* ���� �Ҵ�Ǿ��ִ� �������� ����ִ´�. */
/* ����ִ� ������ �ݺ����� ������ �����Ѵ�. */
/* ������ ��ü �۾��� �����Ѵ�. */
/* ���� ��ü ��, ���� ������ �ڿ��帪 Ŭ�����Ѵ�. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	/* ���ο� ������ ��ü�Ѵ�. */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

	void Set_NextLevelIndex(_uint iNext) {
		m_iNextLevelIndex = iNext; }
	_uint Get_CurrentLevel() { 
		return m_iNextLevelIndex; }
	_uint Get_CurrentLevelIndex() {
		return m_iLevelIndex; }
	_uint Get_PrevLevelIndex() { 
		return m_iPrevLevelIndex; }

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iLevelIndex = { 0 };
	_uint					m_iNextLevelIndex = { 0 };
	_uint					m_iPrevLevelIndex = { 0 };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END