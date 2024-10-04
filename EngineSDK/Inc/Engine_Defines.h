#pragma once

#pragma warning (disable : 4251)

#include <vld.h>
#include<malloc.h>
#include <d3d11_4.h>


#ifndef __ID3D11Multithread_INTERFACE_DEFINED__
#define __ID3D11Multithread_INTERFACE_DEFINED__
#endif

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>



// For. NVIDIA HBAO+
#include"GFSDK_ShadowLib.h"
#include"GFSDK_ShadowLib_Common.h"
#include"GFSDK_SSAO.h"




#include"PxPhysicsAPI.h"

#pragma region Lowlevel_Blast
#include"NvBlast.h"
#pragma endregion Lowlevel_Blast

#pragma region ToolKit_Blast
#include"NvBlastTk.h"
#pragma endregion ToolKit_Blast

#pragma region Global_Blast
#include"NvBlastGlobals.h"
#pragma endregion Global_Blast

#pragma region Ext_Blast
#include"NvBlastExtAssetUtils.h"
#include"NvBlastExtAuthoring.h"
#include"NvBlastExtAuthoringBondGenerator.h"
#include"NvBlastExtAuthoringBooleanTool.h"
#include"NvBlastExtAuthoringCutout.h"
#include"NvBlastExtAuthoringFractureTool.h"
#include"NvBlastExtAuthoringMeshCleaner.h"
#include"NvBlastExtAuthoringAccelerator.h"
#include"NvBlastExtAuthoringConvexMeshBuilder.h"
#include"NvBlastExtAuthoringMesh.h"
#include"NvBlastExtAuthoringPatternGenerator.h"
#include"NvBlastExtAuthoringTypes.h"
#include"NvBlastExtLlSerialization.h"
#include"NvBlastExtSerialization.h"
#include"NvBlastExtTkSerialization.h"
#include"NvBlastExtDamageShaders.h"
#include"NvBlastExtStressSolver.h"

#pragma endregion Ext_Blast



#include <random>

#include "fx11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\Keyboard.h"
#include "DirectXTK\Mouse.h"
#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
#include "DirectXTK\Effects.h"



#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tchar.h>
#include<filesystem>
using namespace DirectX;
using namespace physx;
using namespace physx::vehicle2;
using namespace Nv;
using namespace Nv::Blast;


#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <ctime>
#include <random>
#include <cmath>
#include <memory>
#include <filesystem>

#include <queue>
#include <deque>
#include <stack>
#include<cstring>
#include<cassert>
#include<locale>
#include<mutex>
#include<thread>
#include<future>
#include<any>
#include<optional>

#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")
#include <io.h>




namespace Engine
{
	enum LEVEL_ENGINE { ENGINE_STATIC, ENGINE_LOADING, ENGINE_LOGO, ENGINE_GAMEPLAY, ENGINE_ACKBAR, ENGINE_JUGGLAS, ENGINE_ANDRASARENA, ENGINE_GRASSLAND, ENGINE_END };
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
	enum NodeStates { SUCCESS, FAILURE, RUNNING, COOLING, NONE };		//비헤이비어 노드 반환값
	enum Decorator { CoolDown, Condition, Deco_End };								//베헤비어 트리 데코레이터
	enum MOUSEWHEELSTATE { WHEEL_UP, WHEEL_DOWN, WHEEL_NONE };
	enum UISORT_PRIORITY { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH, EIGHT, NINETH, TENTH,
		ELEVENTH, TWELFTH, THIRTEENTH, FOURTEENTH, FIFTEENTH, SIXTEENTH, SEVENTEENTH,  SORT_END }; //UI정렬

	enum CHANNELID { SOUND_EFFECT, SOUND_SAVEPOINT, SOUND_HOVERBOARD, SOUND_HOVERBOARD_DASH, SOUND_PLAYER, SOUND_MONSTER, SOUND_MONSTER05, SOUND_BOSS, SOUND_BGM, SOUND_SUBBGM, SOUND_UI, MAXCHANNEL };

	enum FRUSTUM { FRUSTUM_NEAR, FRUSTUM_MIDDLE, FRUSTUM_FAR, FRUSTUM_END };

	enum class eEVENT_TYPE
	{
		CREATE_OBJECT,		// CreateObject
		DELETE_OBJECT,		// DeleteObject
		SCENE_CHANGE,		// SceneChange
		NONE
	};


	enum CollisionGropuID
	{

		GROUP_PLAYER = 1 << 0,
		GROUP_WEAPON = 1 << 1,
		GROUP_ENVIRONMENT = 1 << 2,
		GROUP_ENEMY = 1 << 3,
		GROUP_NONCOLLIDE = 1 << 4
	};
	enum RENDER_THREAD_EVENT_TYPE
	{
		RENDER_THREAD_EVENT_TYPE_PROCESS,
		RENDER_THREAD_EVENT_TYPE_DESTROY,
		RENDER_THREAD_EVENT_TYPE_COUNT
	};


	
}

#include "Engine_Typedef.h"
#include "Engine_Function.hpp"
#include"Engine_Functor.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Functor.h"



#define WORLD_CMETER 0.01f
#define WORLD_METER  1.0f
#define WORLD_SPEED 1.0f



//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef DBG_NEW 
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//#endif
//#endif // _DEBUG


using namespace Engine;
using namespace std;

// 사용자 정의 Vehicle 네임스페이스
using namespace Vehicle;
