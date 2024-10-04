#include "CUserErrorCallBack.h"



CUserErrorCallBack::CUserErrorCallBack()
{
}

CUserErrorCallBack::~CUserErrorCallBack()
{
}

void CUserErrorCallBack::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
{


	std::wstring wideMessage = std::wstring(message, message + strlen(message));
	std::wstring errorType;

	switch (code)
	{
	case PxErrorCode::eDEBUG_INFO:
	{
		errorType = L"DEBUG_INFO";
	}
	break;
	case PxErrorCode::eDEBUG_WARNING:
	{
		
		errorType = L"DEBUG_WARNING";
	}
	break;
	case PxErrorCode::eINVALID_PARAMETER:
	{
		
		errorType = L"!!!!!Very Danger!!!!! INVALID_PARAMETER";
	}
	break;
	case PxErrorCode::eINVALID_OPERATION:
	{
		
		errorType = L"!!!!!Very Danger!!!!! INVALID_OPERATION";
	}
	break;
	case PxErrorCode::eOUT_OF_MEMORY:
	{
		
		errorType = L"OUT_OF_MEMORY";
	}
	break;
	case PxErrorCode::eINTERNAL_ERROR:
	{
		
		errorType= L"!!!!!Very Danger!!!!! INTERNAL_ERROR";
	}
	break;
	case PxErrorCode::eABORT:
	{
		
		errorType = L"!!!!!Very Danger!!!!!  ABORT";
	}
	break;
	case PxErrorCode::ePERF_WARNING:
	{
		

		errorType = L"PERF_WARNING";
	}
	break;
	case PxErrorCode::eMASK_ALL:
	{
	
		errorType= L"MASK_ALL";
	}
	break;
	default:
		break;
	}

	std::wstring wideFile = std::wstring(file, file + strlen(file));

	std::wstring wideLine = std::to_wstring(line);

	// 최종 출력 문자열 생성
	std::wstring output = errorType + L": " + wideMessage + L"\n"
		L"File: " + wideFile + L"\n"
		L"Line: " + wideLine + L"\n";


	//std::wstring output = errorType + L": " + wideMessage + L"\n";
	OutputDebugString(output.c_str());

}
