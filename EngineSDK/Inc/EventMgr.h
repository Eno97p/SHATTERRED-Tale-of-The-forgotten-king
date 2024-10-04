#pragma once
#include "Base.h"

BEGIN(Engine)

class CEventMgr :  public CBase
{
public:
	HRESULT EventUpdate();
public:
	void AddEvent(const tEvent& _eve) { mVecEvent.push_back(_eve); }

private:
	HRESULT Initialize(class CObject_Manager* manager);
	CEventMgr();
	virtual ~CEventMgr() = default;;
private:
	vector<tEvent> mVecEvent;
	vector<class CGameObject*> mVecDeadObj;
private:
	void excute(const tEvent& _eve);
private:
	class CObject_Manager* m_ObjManager = nullptr;
public:
	static CEventMgr* Create(class CObject_Manager* manager);
	virtual void		Free();

};

END