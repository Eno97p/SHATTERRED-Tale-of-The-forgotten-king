#pragma once

#include "Client_Defines.h"
#include"Engine_Defines.h"
#include "GameObject.h"


namespace Client
{

	struct _tagMonsterInit_Property
	{
		wstring strMonsterTag = TEXT("");
		_float4 vPos = { 0.f,0.f,0.f,0.f };
		size_t uArgDescSize = 0;
		void* pArgDesc = nullptr;
	};

	struct _tagTriggrrInit_Property
	{
		_float4x4 mWorldMatrix = {};
		wstring strMonsterTag = TEXT("");
		_uint iTriggerType = 0;
	};

	struct _tagBlastWallInit_Property
	{
		_float4x4 mWorldMatrix = {};
		wstring strMonsterTag = TEXT("");


	};


}