#pragma once
#include"Base.h"

BEGIN(Engine)
class CCCTFilterCallBack :
	public PxControllerFilterCallback
	,public CBase
{
public:
	using FilterCallBack = std::function<bool(const PxController& a, const PxController& b)>;
private:
	CCCTFilterCallBack();
	virtual ~CCCTFilterCallBack() = default;

private:
	HRESULT Initialize_Prototype(const FilterCallBack& callback);




private:
	FilterCallBack m_FilterCallBack;

public:
	virtual bool filter(const PxController& a, const PxController& b) override;


public:
	static CCCTFilterCallBack* Create(const FilterCallBack& callback);
	virtual void Free() override;
};

END