#pragma once
#include "Base.h"

BEGIN(Engine)
class CUISorter final : public CBase
{
private:
	CUISorter();
	virtual ~CUISorter() = default;
public:
	void Sorting();
	HRESULT Add_UI(class CGameObject* ui, UISORT_PRIORITY type);
private:
	void Clear();
	HRESULT Initialize();
private:

	vector<class CGameObject*>  m_VecUI[UISORT_PRIORITY::SORT_END];
public:
	static CUISorter* Create();
	virtual void Free() override;
};

END