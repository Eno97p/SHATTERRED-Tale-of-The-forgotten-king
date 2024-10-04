#pragma once
#include<windows.h>
#include <process.h>
#include <vector>
#include<unordered_set>
#include<set>
namespace Client
{
#define MAX_THREAD 4
#define USED_THREAD_COUNT 3
	static std::unordered_set<const wchar_t*> g__Exit_Delete_FileList ;		//프로그램이 종료되면 삭제할 파일들

	static bool g_IsThreadFinish[USED_THREAD_COUNT];
	//Level이 추가되면 아래 문자열 순서 맞춰서 추가해주기
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_ACKBAR, LEVEL_JUGGLAS, LEVEL_ANDRASARENA, LEVEL_GRASSLAND, LEVEL_END };

	constexpr const char* LevelNames[] = {
		"Static",
		"Loading",
		"Logo",
		"GamePlay",
		"Ackbar",
		"Jugglas",
		"AndrasArena",
		"GrassLand",
		"End"
	};




	enum CAMERA_INDEX { CAM_FREE = 0, CAM_THIRDPERSON, CAM_CUTSCENE, CAM_SIDEVIEW, CAM_TRANSITION, CAM_END };

	enum FRUSTUM { FRUSTUM_NEAR, FRUSTUM_MIDDLE, FRUSTUM_FAR, FRUSTUM_END };

#define EFFECTMGR 						CEffectManager::GetInstance()
	//아래 enum, 건들지 말것 - 박은호
#pragma region EFFECT_ENUM
	enum EFFECTTYPE {
		SPREAD, DROP, GROWOUT, SPREAD_SIZEUP,
		SPREAD_NONROTATION, TORNADO, SPREAD_SPEED_DOWN,
		SLASH_EFFECT, SPREAD_SPEED_DOWN_SIZE_UP, GATHER, EXTINCTION,
		GROWOUTY, GROWOUT_SPEEDDOWN, LEAF_FALL, SPIRAL_EXTINCTION, SPIRAL_SPERAD, LENZ_FLARE, BLOW, UP_TO_STOP, ONLY_UP, EFFECT_END
	};
	enum PARTICLETYPE { PART_POINT, PART_MESH, PART_RECT, PART_END };
	enum EFFECTMODELTYPE { CUBE, CIRCLE, SLASH, LEAF0, LEAF1, GRASS, ROCK0, ROCK1, BLADE_SLASH, BLADE_SLASH_LONG, NEEDLE, BUBBLE, TYPE_END };
	enum TRAILFUNCTYPE { TRAIL_EXTINCT, TRAIL_ETERNAL, TRAIL_CATMULROM, TRAIL_END };
	enum TRAIL_USAGE { USAGE_SWORD, USAGE_EYE, USAGE_END };
#pragma endregion EFFECT_ENUM



	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
	float fSlowValue = 1.f;


}

#include "Client_Function.hpp"
#include "Client_Struct.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;


using namespace Client;
