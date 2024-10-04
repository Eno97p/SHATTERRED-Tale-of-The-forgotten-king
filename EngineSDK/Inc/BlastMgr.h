#pragma once
#include "Base.h"

BEGIN(Engine)
class CBlastMgr final: public CBase
{
private:
	CBlastMgr();
	virtual ~CBlastMgr() = default;

private:
	HRESULT Initialize();


private:
	TkFramework* m_pFramework = { nullptr };

public:
	static CBlastMgr* Create();
	virtual void Free() override;
};

END