#pragma once

#include <process.h>
//#include "vld.h"
#include "DirectXTex.h"
#pragma comment(lib, "DirectXTex.lib")


namespace MYMapTool
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	enum STAGE_NAME { STAGE_HOME, STAGE_ONE, STAGE_TWO, STAGE_THREE, STAGE_BOSS, STAGE_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE	g_hInst;
extern HWND g_hWnd;

using namespace MYMapTool;