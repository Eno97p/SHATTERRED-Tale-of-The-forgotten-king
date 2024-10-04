#pragma once
#include"Base.h"



class CUserErrorCallBack : public PxErrorCallback
{
public:
	CUserErrorCallBack();
	virtual ~CUserErrorCallBack();

	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line);






};

