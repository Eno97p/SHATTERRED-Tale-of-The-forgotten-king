#pragma once

#include "Base.h"
#include "AnimTool_Defines.h"
#include "ToolObj.h"
#include "ToolPartObj.h"

// ToolObj의 생성, 관리, 저장의 기능
// 인자로 받은 Index로 분기 처리 하여 조건에 맞는 Tool Obj 생성

BEGIN(Engine)
class CGameInstance;
END

BEGIN(AnimTool)

class CToolObj_Manager final : public CBase
{
	DECLARE_SINGLETON(CToolObj_Manager)

private:
	CToolObj_Manager();
	virtual ~CToolObj_Manager() = default;

public:
	vector<CToolObj*>& Get_ToolObjs() { return m_ToolObjs; }
	vector<CToolPartObj*>& Get_ToolPartObjs() { return m_ToolPartObjs; }
	const _float4x4* Get_Obj_WorldMatrix(_int iIndex);	// 해당 Index Obj의 WorldMatrix를 반환하는 함수

	_uint	Get_CurKeyFrame(_uint iAnimIdx, _uint iChannelIdx);
	_double Get_Duration(_uint iAnimIdx);
	const _double* Get_CurrentPos(_uint iAnimIdx);

	_bool	Get_isObjEmpty() {
		if (m_ToolObjs.empty())
			return true;
		else
			return false;
	}

	// PartObj


public:
	HRESULT	Add_CloneObj(_int iLayerIdx, _int iSelectIdx, _vector vPosition); // Tool에서 Obj 생성
	HRESULT	Add_AnimModel(_int iSelectIdx);
	HRESULT	Add_PartObj(_int iSelectIdx, _int iBoneIdx);

	HRESULT	Delete_Obj(_int iSelectIdx);
	HRESULT	Delete_AnimModel();
	HRESULT	Delete_PartObj(_int iSelectIdx);

	void	Setting_ChannelList();

	void	Change_AnimState(_uint iAnimState);
	void	Set_Animation(_uint iAnimIdx, _bool isLoop);
	void	Setting_KeyFrameSliders(_uint iAnimIdx, _uint iChannelIdx);

	const _tchar*	Setting_FileName();
	string			Setting_PartObjName(_int iSelectIdx);

	HRESULT	Save_Data();
	HRESULT	Load_Data();

	void	Setting_AddPartObj(const _char* szName);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<CToolObj*>		m_ToolObjs;
	vector<CToolPartObj*>	m_ToolPartObjs;

	vector<wstring>		m_MonsterName;		// Monster
	vector<wstring>		m_MapElement;		// Map Element(맵 조형물)
	vector<wstring>		m_MapObj;			// Map Obj

private:
	void	Initialize();

	void		Setting_Desc(_int iLayerIdx, _int iSelectIdx, CToolObj::TOOLOBJ_DESC& pDesc);
	const char* Setting_ObjName(/*_int iLayerIdx, */_int iSelectIdx);

public:
	virtual void	Free() override;
};

END