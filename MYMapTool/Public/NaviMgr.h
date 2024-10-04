#pragma once

#include "Base.h"
#include "MYMapTool_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CCell;
class CNavigation; // 
END

// Navigation의 위치 정보를 파일에 저장하는 클래스(싱글톤이어야 하나?)
// Terrain에서 마우스피킹이 되면 해당 클래스의 함수를 호출하여 해당 위치가 현재 몇 번째로 선택된 녀석이며~ 이런 것을 계산? 하여 vector에 할당?

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
	// 위치를 받아서 vector에 저장
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
	vector<class CCell*>	m_vecCell;		// 실제로 파일화 할 Navi 데이터

public:
	virtual void	Free() override;
};

END