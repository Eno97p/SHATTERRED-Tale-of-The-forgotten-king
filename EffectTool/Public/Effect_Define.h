#pragma once
#include <Windows.h>
#include <process.h>
#include <wincodec.h>
#include "DirectXTex.h"
#include <string>
#include <istream>

using namespace std;
//#include <vld.h>
#pragma comment(lib, "DirectXTex.lib")
namespace Effect
{


	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	enum EFFECTTYPE { SPREAD, DROP, GROWOUT, SPREAD_SIZEUP,
		SPREAD_NONROTATION, TORNADO, SPREAD_SPEED_DOWN,
		SLASH_EFFECT, SPREAD_SPEED_DOWN_SIZE_UP, GATHER, EXTINCTION,
		GROWOUTY , GROWOUT_SPEEDDOWN , LEAF_FALL, SPIRAL_EXTINCTION, SPIRAL_SPERAD,LENZ_FLARE,BLOW, UP_TO_STOP,ONLY_UP, EFFECT_END };
	enum EFFECTMODELTYPE { CUBE, CIRCLE, SLASH, LEAF0, LEAF1, GRASS,ROCK0,ROCK1, BLADE_SLASH, BLADE_SLASH_LONG,NEEDLE, BUBBLE, TYPE_END };
	enum PARTICLETYPE { PART_POINT, PART_MESH, PART_RECT, PART_END};
	enum TRAILFUNCTYPE { TRAIL_EXTINCT, TRAIL_ETERNAL,TRAIL_CATMULROM, TRAIL_END};
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
using namespace Effect;
