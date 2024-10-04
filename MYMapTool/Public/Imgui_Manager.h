#pragma once

#include "Base.h"
#include "MYMapTool_Defines.h"
#include "..\Public\imgui.h"
#include "GameInstance.h"
#include "CutSceneCamera.h"

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

private:
	enum LIST_LAYER { LAYER_MONSTER, LAYER_PASSIVE_ELEMENT, LAYER_ACTIVE_ELEMENT, LAYER_TRIGGER, LAYER_ENVEFFECTS,
		LAYER_TRAP , LAYER_DECAL, LAYER_VEGETATION, LAYER_END };

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	_int	Get_ObjIdx() { return m_iObjIdx; }
	_int	Get_StageIdx() { return m_iStageIdx; }
	_int	Get_LayerIdx() { return m_iLayerIdx; }
	_int	Get_CellIdx() { return m_iCreateCellIdx; }

	_bool	Get_IsSave() { return m_IsSave; }
	void	Set_DontSave() { m_IsSave = false; }

	_bool	Get_IsPhysXSave() { return m_IsPhysXSave; }
	void	Set_DontPhysXSave() { m_IsPhysXSave = false; }

	_bool	Get_IsPhysXLoad() { return m_IsPhysXLoad; }
	void	Set_DontPhysXLoad() { m_IsPhysXLoad = false; }

	_bool	Get_IsDecalSave() { return m_IsDecalSave; }
	void	Set_DontDecalSave() { m_IsDecalSave = false; }

	_bool	Get_IsDecalLoad() { return m_IsDecalLoad; }
	void	Set_DontDecalLoad() { m_IsDecalLoad = false; }

	_bool	Get_IsLoad() { return m_IsLoad; }
	void	Set_DontLoad() { m_IsLoad = false; }

	_bool	Get_IsEffectsSave() { return m_IsEffectsSave; }
	void	Set_DontEffectsSave() { m_IsEffectsSave = false; }

	_bool	Get_IsEffectsLoad() { return m_IsEffectsLoad; }
	void	Set_DontEffectsLoad() { m_IsEffectsLoad = false; }

	_bool	Get_IsReLoad() { return m_IsTerrainReLoad; }
	void	Set_DontReLoad() { m_IsTerrainReLoad = false; }

	_int	Get_TerrainX() { return m_iTerrainX; }
	_int	Get_TerrainZ() { return m_iTerrainZ; }

	_float Get_TrapTimeOffset() { return m_fTrapTimeOffset; }
	_bool isAnimModel() { return m_bIsAnimModel; }
	_bool isDecoObject() { return m_bIsDecoObject; }


	_float3 Get_GlobalWindDir() { return m_GlobalWindDir; }
	_float Get_GlobalWindStrenth() { return m_fGlobalWindStrength; }


	_float3 Get_GrassTopCol() { return m_TopCol; }
	_float3 Get_GrassBotCol() { return m_BotCol; }

	_float Get_BillboardFactor() { return m_fBillboardFactor; }
	_float Get_ElasticityFactor() { return m_fElasticityFactor; }

	_float Get_PlaneOffset() const { return m_fPlaneOffset; }
	_float Get_PlaneVertOffset() const { return m_fPlaneVertOffset; }
	_float Get_LODDistance1() const { return m_fLODDistance1; }
	_float Get_LODDistance2() const { return m_fLODDistance2; }
	_float Get_GrassAmplitude() const { return m_fGrassAmplitude; }
	_float Get_GrassFrequency() const { return m_fGrassFrequency; }
	_uint Get_LODPlaneCount1() const { return m_iLODPlaneCount1; }
	_uint Get_LODPlaneCount2() const { return m_iLODPlaneCount2; }
	_uint Get_LODPlaneCount3() const { return m_iLODPlaneCount3; }
	_float3 Get_LeafCol() { return m_LeafCol; }
	_bool Get_TreeBloom() { return m_bTreeBloom; }
	_int Get_GrassCount() { return m_iGrassCount; }

	_uint Get_SkyIdx() { return m_iSkyBoxIdx; }

	_float4 Get_GlobalColorPicker() { return m_GlobalColor; }
	void	Add_vecCreateObj(_char* pName) { m_vecCreateObj.emplace_back(pName); }
	void	Add_vecCreateCell(_int iIndex)
	{
		_char szName[MAX_PATH] = "";
		sprintf(szName, "%d", iIndex);
		m_vecCreateCell.emplace_back(szName);
	}
	void	Add_vecCreateCellSize();

	void	Clear_vecCell();


public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

	void	EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);
	void Add_ToolObj(_vector vPosition, _vector vColor);

private:
	void Light_Editor();
	ImTextureID DirectXTextureToImTextureID(_uint iIdx);

	HRESULT Create3DNoiseTexture(UINT width, UINT height, UINT depth, DXGI_FORMAT format);
	void GenerateNoiseData(BYTE* data, UINT width, UINT height, UINT depth);


public:
	void Save_Lights();
	void Load_Lights();
	void Reset_Lights();

private:
	void Shadow_Editor();

	void Camera_Editor();
	void Save_CameraKeyFrames();
	void Load_CameraKeyFrames();

	void Battle_Camera_Editor();

	void Terrain_Editor();
	void GlobalWind_Editor();
	void Fog_Editor();
	void Cloud_Editor();
	void Water_Editor();
	void HBAO_Editor();


private:
	wstring m_LightsDataPath = L""; //Lights 저장 경로
	WCHAR m_filePath[MAX_PATH];


private:
	_bool	m_bShowWindow;
	_bool	m_bShowAnotherWindow;
	_bool	m_bLightWindow = false;
	_bool	m_bShadowWindow = false;
	_bool	m_bCameraWindow = false;
	_bool	m_bBattleCameraWindow = false;
	_bool	m_bTerrainWindow = false;
	_bool	m_bGlobalWindWindow = false;
	_bool	m_bFogWindow = false;
	_bool	m_bCloudWindow = false;
	_bool	m_bWaterWindow = false;
	_bool	m_bHBAOWindow = false;
	
	_bool	m_bShowDecalTextureWindow = false;

	_bool	m_IsSave = { false };
	_bool	m_IsLoad = { false };

	_bool	m_IsPhysXSave = { false };
	_bool	m_IsPhysXLoad = { false };

	_bool	m_IsEffectsSave = { false };
	_bool	m_IsEffectsLoad = { false };

	_bool	m_IsDecalSave = { false };
	_bool	m_IsDecalLoad = { false };

	_bool	m_IsTerrainReLoad = { false };

	bool m_bEditingCutScene = false;
	_uint m_iEditingCutSceneIdx = 0;

private:
	_bool m_bAddToolObj = false;
	_bool g_bIsHovered = false;
	_bool m_bIsAnimModel = false;
	_bool m_bIsDecoObject = false;

	_float4 m_ClickedMousePos = { 0.f, 0.f, 0.f, 1.f };
	_int	m_selectedOption = 0; // 선택된 옵션을 저장할 변수

	_bool m_bTreeBloom = false;
private:

	// Imguizmo
	_bool	useWindow = { false };
	_int	gizmoCount = 1;

	_int		m_iObjIdx; // Obj Index
	_int		m_iStageIdx; // 스테이지 Index
	_int		m_iLayerIdx; // Layer Index
	_int		m_iCreateObjIdx = { 0 }; // 생성된 Obj Index
	_int		m_iCreateCellIdx = { 0 }; // 생성된 Cell Index

	_int		m_iTerrainX = { 512 };
	_int		m_iTerrainZ = { 512 };

	vector<_char*>	m_vecCreateObj;			// 생성된 ToolObj 벡터
	vector<_char*>	m_vecCreateCell;		// 생성된 Cell 벡터

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };


	CTexture* m_pDecalTexs = { nullptr };
private:
	_float m_fTrapTimeOffset = 0.f;
private:
	// Imgui_Manager 클래스에 멤버 변수 추가
	XMFLOAT4 m_DirectionalLightDirection{ 0.f, -1.f, 0.0f, 0.0f };
	XMFLOAT4 m_SpotlightDirection{ 0.f, -1.f, 0.0f, 0.0f };

private:
	_float3				m_TopCol = { 0.f, 1.f, 0.f };
	_float3				m_BotCol = { 0.f, 0.f, 0.f };
	_float				m_fPlaneOffset = { 0.f};
	_float				m_fPlaneVertOffset = { 0.f};
	_float				m_fLODDistance1 = { 0.f};
	_float				m_fLODDistance2 = { 0.f};
	_float				m_fGrassAmplitude = { 0.f};
	_float				m_fGrassFrequency = { 0.f};
	_uint			 m_iLODPlaneCount1 = 0;
	_uint			 m_iLODPlaneCount2 = 0;
	_uint			 m_iLODPlaneCount3 = 0;

	_int				m_iGrassCount = { 0 };

	_float				m_fBillboardFactor = { 0.f };
	_float				m_fElasticityFactor = { 0.f};
	_float3				m_LeafCol = { 0.f, 0.f, 0.f };

	_float3 m_GlobalWindDir = _float3(1.0f, 0.0f, 0.5f);
	_float m_fGlobalWindStrength = 1.f;
	_float m_fGlobalWindFrequency = 1.f;

	_int m_iSkyBoxIdx = 0;

//FOR CINE CAMERA TOOL
private:
	vector<CCamera::CameraKeyFrame>	m_vCameraKeyFrames;
	_float m_fKeyFrameTime = 0.f;
	_int m_iSelectedKeyFrame = 0;

	vector<string> m_keyFrameNames;


private:
	_float4 m_GlobalColor = { 1.f, 1.f, 1.f, 1.f };

	private:
		ID3D11ShaderResourceView* m_pNoiseTextureView = { nullptr };
private:
	void	Setting_ObjListBox(_int iLayerIdx);



	void	Setting_CreateObj_ListBox();
	void	Delete_Obj();
	//Utility  유틸리티 함수들
	_float3 hash33(_float x, _float y, _float z)
	{
		int q[3];
		q[0] = (int)(x * 1597334673U);
		q[1] = (int)(y * 3812015801U);
		q[2] = (int)(z * 2798796415U);

		q[0] = (q[0] ^ q[1] ^ q[2]) * 1597334673U;
		q[1] = (q[1] ^ q[2] ^ q[0]) * 3812015801U;
		q[2] = (q[2] ^ q[0] ^ q[1]) * 2798796415U;

		_float3 r;
		r.x = (_float)q[0] / 0xffffffffU * 2.0f - 1.0f;
		r.y = (_float)q[1] / 0xffffffffU * 2.0f - 1.0f;
		r.z = (_float)q[2] / 0xffffffffU * 2.0f - 1.0f;

		return r;
	}

	_float remap(_float x, _float a, _float b, _float c, _float d)
	{
		return (((x - a) / (b - a)) * (d - c)) + c;
	}

	_float lerp(_float a, _float b, _float t)
	{
		return a + t * (b - a);
	}

	_float dot(_float x1, _float y1, _float z1, _float x2, _float y2, _float z2)
	{
		return x1 * x2 + y1 * y2 + z1 * z2;
	}

	_float gradientNoise(_float x, _float y, _float z, _float freq)
	{
		_float px = std::floor(x);
		_float py = std::floor(y);
		_float pz = std::floor(z);
		_float wx = x - px;
		_float wy = y - py;
		_float wz = z - pz;

		_float ux = wx * wx * wx * (wx * (wx * 6.0f - 15.0f) + 10.0f);
		_float uy = wy * wy * wy * (wy * (wy * 6.0f - 15.0f) + 10.0f);
		_float uz = wz * wz * wz * (wz * (wz * 6.0f - 15.0f) + 10.0f);

		_float3 ga = hash33(fmodf(px + 0.0f, freq), fmodf(py + 0.0f, freq), fmodf(pz + 0.0f, freq));
		_float3 gb = hash33(fmodf(px + 1.0f, freq), fmodf(py + 0.0f, freq), fmodf(pz + 0.0f, freq));
		_float3 gc = hash33(fmodf(px + 0.0f, freq), fmodf(py + 1.0f, freq), fmodf(pz + 0.0f, freq));
		_float3 gd = hash33(fmodf(px + 1.0f, freq), fmodf(py + 1.0f, freq), fmodf(pz + 0.0f, freq));
		_float3 ge = hash33(fmodf(px + 0.0f, freq), fmodf(py + 0.0f, freq), fmodf(pz + 1.0f, freq));
		_float3 gf = hash33(fmodf(px + 1.0f, freq), fmodf(py + 0.0f, freq), fmodf(pz + 1.0f, freq));
		_float3 gg = hash33(fmodf(px + 0.0f, freq), fmodf(py + 1.0f, freq), fmodf(pz + 1.0f, freq));
		_float3 gh = hash33(fmodf(px + 1.0f, freq), fmodf(py + 1.0f, freq), fmodf(pz + 1.0f, freq));

		_float va = dot(ga.x, ga.y, ga.z, wx - 0.0f, wy - 0.0f, wz - 0.0f);
		_float vb = dot(gb.x, gb.y, gb.z, wx - 1.0f, wy - 0.0f, wz - 0.0f);
		_float vc = dot(gc.x, gc.y, gc.z, wx - 0.0f, wy - 1.0f, wz - 0.0f);
		_float vd = dot(gd.x, gd.y, gd.z, wx - 1.0f, wy - 1.0f, wz - 0.0f);
		_float ve = dot(ge.x, ge.y, ge.z, wx - 0.0f, wy - 0.0f, wz - 1.0f);
		_float vf = dot(gf.x, gf.y, gf.z, wx - 1.0f, wy - 0.0f, wz - 1.0f);
		_float vg = dot(gg.x, gg.y, gg.z, wx - 0.0f, wy - 1.0f, wz - 1.0f);
		_float vh = dot(gh.x, gh.y, gh.z, wx - 1.0f, wy - 1.0f, wz - 1.0f);

		return va +
			ux * (vb - va) +
			uy * (vc - va) +
			uz * (ve - va) +
			ux * uy * (va - vb - vc + vd) +
			uy * uz * (va - vc - ve + vg) +
			uz * ux * (va - vb - ve + vf) +
			ux * uy * uz * (-va + vb + vc - vd + ve - vf - vg + vh);
	}


	// XMFLOAT3에 대한 연산 함수들
	XMFLOAT3 Add(const XMFLOAT3& a, const XMFLOAT3& b)
	{
		return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	XMFLOAT3 Multiply(const XMFLOAT3& a, float scalar)
	{
		return XMFLOAT3(a.x * scalar, a.y * scalar, a.z * scalar);
	}

	XMFLOAT3 Subtract(const XMFLOAT3& a, const XMFLOAT3& b)
	{
		return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	// floor 함수 (XMFLOAT3 버전)
	XMFLOAT3 Floor(const XMFLOAT3& v)
	{
		return XMFLOAT3(std::floor(v.x), std::floor(v.y), std::floor(v.z));
	}

	// frac 함수 (XMFLOAT3 버전)
	XMFLOAT3 Frac(const XMFLOAT3& v)
	{
		return XMFLOAT3(v.x - std::floor(v.x), v.y - std::floor(v.y), v.z - std::floor(v.z));
	}

	// dot 함수 (XMFLOAT3 버전)
	float Dot(const XMFLOAT3& a, const XMFLOAT3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	_float worleyNoise(_float x, _float y, _float z, _float freq, _float jitter)
	{
		XMFLOAT3 pos(x * freq, y * freq, z * freq);
		XMFLOAT3 id = Floor(pos);
		XMFLOAT3 localPos = Frac(pos);

		_float minDist = 10000.0f;
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				for (int k = -1; k <= 1; k++)
				{
					XMFLOAT3 offset2 = { _float(i), _float(j), _float(k)};
					XMFLOAT3 cellId = Add(id , offset2);
					XMFLOAT3 h = hash33(cellId.x, cellId.y, cellId.z);
					h = Add(Multiply(h, jitter), offset2);
					XMFLOAT3 d = Subtract(localPos, h);
					_float dist = Dot(d, d);
					minDist = std::min(minDist, dist);
				}
			}
		}

		return 1.0f - std::sqrt(minDist);
	}

	_float saturate(_float x)
	{
		return std::max(0.0f, std::min(1.0f, x));
	}

	float improvedPerlinNoise(float x, float y, float z, float freq)
	{
		XMFLOAT3 p(x * freq, y * freq, z * freq);
		XMFLOAT3 pi = Floor(p);
		XMFLOAT3 pf = Frac(p);

		XMFLOAT3 pf_min1 = Subtract(pf, XMFLOAT3(1.0f, 1.0f, 1.0f));

		float n000 = Dot(hash33(pi.x, pi.y, pi.z), pf);
		float n100 = Dot(hash33(pi.x + 1, pi.y, pi.z), XMFLOAT3(pf.x - 1, pf.y, pf.z));
		float n010 = Dot(hash33(pi.x, pi.y + 1, pi.z), XMFLOAT3(pf.x, pf.y - 1, pf.z));
		float n110 = Dot(hash33(pi.x + 1, pi.y + 1, pi.z), XMFLOAT3(pf.x - 1, pf.y - 1, pf.z));
		float n001 = Dot(hash33(pi.x, pi.y, pi.z + 1), XMFLOAT3(pf.x, pf.y, pf.z - 1));
		float n101 = Dot(hash33(pi.x + 1, pi.y, pi.z + 1), XMFLOAT3(pf.x - 1, pf.y, pf.z - 1));
		float n011 = Dot(hash33(pi.x, pi.y + 1, pi.z + 1), XMFLOAT3(pf.x, pf.y - 1, pf.z - 1));
		float n111 = Dot(hash33(pi.x + 1, pi.y + 1, pi.z + 1), pf_min1);

		XMFLOAT3 u = XMFLOAT3(
			pf.x * pf.x * (3 - 2 * pf.x),
			pf.y * pf.y * (3 - 2 * pf.y),
			pf.z * pf.z * (3 - 2 * pf.z)
		);

		return lerp(
			lerp(lerp(n000, n100, u.x), lerp(n010, n110, u.x), u.y),
			lerp(lerp(n001, n101, u.x), lerp(n011, n111, u.x), u.y),
			u.z
		);
	}

	// 개선된 워리 노이즈 함수
	float improvedWorleyNoise(float x, float y, float z, float freq, float jitter)
	{
		XMFLOAT3 pos(x * freq, y * freq, z * freq);
		XMFLOAT3 id = Floor(pos);
		XMFLOAT3 localPos = Frac(pos);

		float minDist = 10000.0f;
		for (int offsetZ = -1; offsetZ <= 1; offsetZ++)
		{
			for (int offsetY = -1; offsetY <= 1; offsetY++)
			{
				for (int offsetX = -1; offsetX <= 1; offsetX++)
				{
					XMFLOAT3 offset = { _float(offsetX), _float(offsetY), _float(offsetZ) };
					XMFLOAT3 cellId = Add(id, offset);
					XMFLOAT3 cellPos = Add(hash33(cellId.x, cellId.y, cellId.z), offset);
					cellPos = Add(Multiply(cellPos, jitter), offset);

					XMFLOAT3 diff = Subtract(cellPos, localPos);
					float dist = Dot(diff, diff);  // 거리의 제곱을 사용
					minDist = min(minDist, dist);
				}
			}
		}

		return sqrt(minDist);  // 최종 결과에서만 제곱근을 적용
	}

public:
	

public:

public:
	virtual void Free() override;
};

