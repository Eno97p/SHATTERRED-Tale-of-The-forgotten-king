#include "EventMgr.h"
#include "GameObject.h"
#include "Object_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CEventMgr::CEventMgr()
{

}

HRESULT CEventMgr::Initialize(CObject_Manager* manager)
{
	m_ObjManager = manager;
	Safe_AddRef(m_ObjManager);
	return S_OK;
}
HRESULT CEventMgr::EventUpdate()
{
	
	m_ObjManager->Delete_Event(mVecDeadObj);
	mVecDeadObj.clear();

	for (size_t i = 0; i < mVecEvent.size(); ++i)
	{
		excute(mVecEvent[i]);
	}
	mVecEvent.clear();

    return S_OK;
}


void CEventMgr::excute(const tEvent& _eve)
{
	_eve.eEven;
	switch (_eve.eEven)
	{
	case eEVENT_TYPE::CREATE_OBJECT:
	{
		//evn.eEven = eEVENT_TYPE::CREATE_OBJECT;
		//evn.lParam = (DWORD_PTR)Level;
		//evn.wParam = (DWORD_PTR)Layer;
		//evn.pParam = (DWORD_PTR)pObj;

		_uint	Level = (_uint)_eve.lParam;
		const wchar_t* LayerName = (const wchar_t*)_eve.wParam;
		CGameObject* pObj = (CGameObject*)_eve.pParam; //CGameObject
		m_ObjManager->Add_Object(Level, LayerName, pObj);

	}
	break;
	case eEVENT_TYPE::DELETE_OBJECT:
	{
		// lParam : object Adress
		// Object 를 Dead 상태로 변경
		// 삭제예정 오브젝트들을 모아둔다.
		CGameObject* pDeadObj = (CGameObject*)_eve.lParam;
		pDeadObj->Set_Dead();
		mVecDeadObj.push_back(pDeadObj);
	}
	break;
	case eEVENT_TYPE::SCENE_CHANGE:
	{
		mVecDeadObj.clear();
		_uint	LevelIdx = (_uint)_eve.lParam;
		CLevel*	nextLevel = (CLevel*)_eve.eParam;
		CGameInstance::GetInstance()->Open_Level(LevelIdx, nextLevel);
	}
		break;
	}
	


}

CEventMgr* CEventMgr::Create(CObject_Manager* manager)
{
	CEventMgr* pInstance = new CEventMgr();

	if (FAILED(pInstance->Initialize(manager)))
	{
		MSG_BOX("Failed To Created : CEventMgr");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEventMgr::Free()
{
	Safe_Release(m_ObjManager);
}
