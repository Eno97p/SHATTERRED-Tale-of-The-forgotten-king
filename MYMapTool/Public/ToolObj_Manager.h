#pragma once

#include "Base.h"
#include "MYMapTool_Defines.h"
#include "ToolObj.h"

// ToolObj의 생성, 관리, 저장의 기능
// 인자로 받은 Index로 분기 처리 하여 조건에 맞는 Tool Obj 생성

BEGIN(Engine)
class CGameInstance;
END

BEGIN(MYMapTool)

class CToolObj_Manager final : public CBase
{
	DECLARE_SINGLETON(CToolObj_Manager)

private:
	CToolObj_Manager();
	virtual ~CToolObj_Manager() = default;

public:
	vector<CToolObj*>&			Get_ToolObjs() { return m_ToolObjs; }
	const _float4x4*			Get_Obj_WorldMatrix(_int iIndex);	// 해당 Index Obj의 WorldMatrix를 반환하는 함수

public:
	HRESULT	Add_CloneObj(_int iLayerIdx, _int iSelectIdx, _vector vPosition, _vector vColor); // Tool에서 Obj 생성

	HRESULT	Delete_Obj(_int iSelectIdx);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<CToolObj*>	m_ToolObjs;
	vector<wstring>		m_Monsters;		// Monster
	vector<wstring>		m_PassiveElements;		// Map Element(맵 조형물)
	vector<wstring>		m_ActiveElements;			// Map Obj

public:
	void	Initialize();

	void		Setting_Desc(_int iLayerIdx, _int iSelectIdx, CToolObj::TOOLOBJ_DESC& pDesc);
	const char* Setting_ObjName(_int iLayerIdx, _int iSelectIdx);

public:
	virtual void	Free() override;
};

END