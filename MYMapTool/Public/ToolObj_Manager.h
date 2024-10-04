#pragma once

#include "Base.h"
#include "MYMapTool_Defines.h"
#include "ToolObj.h"

// ToolObj�� ����, ����, ������ ���
// ���ڷ� ���� Index�� �б� ó�� �Ͽ� ���ǿ� �´� Tool Obj ����

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
	const _float4x4*			Get_Obj_WorldMatrix(_int iIndex);	// �ش� Index Obj�� WorldMatrix�� ��ȯ�ϴ� �Լ�

public:
	HRESULT	Add_CloneObj(_int iLayerIdx, _int iSelectIdx, _vector vPosition, _vector vColor); // Tool���� Obj ����

	HRESULT	Delete_Obj(_int iSelectIdx);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<CToolObj*>	m_ToolObjs;
	vector<wstring>		m_Monsters;		// Monster
	vector<wstring>		m_PassiveElements;		// Map Element(�� ������)
	vector<wstring>		m_ActiveElements;			// Map Obj

public:
	void	Initialize();

	void		Setting_Desc(_int iLayerIdx, _int iSelectIdx, CToolObj::TOOLOBJ_DESC& pDesc);
	const char* Setting_ObjName(_int iLayerIdx, _int iSelectIdx);

public:
	virtual void	Free() override;
};

END