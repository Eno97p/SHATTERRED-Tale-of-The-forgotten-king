#pragma once
#include"Base.h"

BEGIN(Engine)
class CCCTFilerCallBack :
	public PxControllerFilterCallback
	,public CBase
{
public:
	using FilterCallBack = std::function<bool(const PxController& a, const PxController& b)>;
private:
	CCCTFilerCallBack();
	virtual ~CCCTFilerCallBack() = default;

private:
	HRESULT Initialize_Prototype(const FilterCallBack& callback);




private:
	FilterCallBack m_FilterCallBack;

public:
	virtual bool filter(const PxController& a, const PxController& b) override;


public:
	static CCCTFilerCallBack* Create(const FilterCallBack& callback);
	virtual void Free() override;
};

END