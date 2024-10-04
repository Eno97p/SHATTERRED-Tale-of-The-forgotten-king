#pragma once

#include "Base.h"
#include "MYMapTool_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CCell;
class CNavigation; // 
END

// Navigation�� ��ġ ������ ���Ͽ� �����ϴ� Ŭ����(�̱����̾�� �ϳ�?)
// Terrain���� ���콺��ŷ�� �Ǹ� �ش� Ŭ������ �Լ��� ȣ���Ͽ� �ش� ��ġ�� ���� �� ��°�� ���õ� �༮�̸�~ �̷� ���� ���? �Ͽ� vector�� �Ҵ�?

BEGIN(MYMapTool)

class CNaviMgr final : public CBase
{
	DECLARE_SINGLETON(CNaviMgr)

public:
	enum STAGE_NAME { STAGE_HOME, STAGE_ONE, STAGE_TWO, STAGE_THREE, STAGE_BOSS, STAGE_END };

private:
	CNaviMgr();
	virtual ~CNaviMgr() = default;

public:
	HRESULT	Load_NaviData(CNavigation* pNavigationCom);

public:
	// ��ġ�� �޾Ƽ� vector�� ����
	void	Input_NaviData(CNavigation* pNavigationCom, _vector vPos);
	//void	Check_Around(_float3& vPos);
	void	Check_Around(_float3& vPos);
	//_float	Calculate_Distance(_float3 vNewPos, _float3 vOriginPos);
	_float	Calculate_Distance(_float3 vNewPos, _vector vOriginPos);

	void	Update_NaviName();
	HRESULT	Save_NaviData();
	void	Check_Cell(_int iCellIndex);
	void	Delete_Cell(_int iCellIndex);
	void	Clear_Cell();
	
	void	Update_Cell();

private:
	_uint			m_iCount = { 0 };

	_float3					m_NaviList[3];
	vector<class CCell*>	m_vecCell;		// ������ ����ȭ �� Navi ������

public:
	virtual void	Free() override;
};

END