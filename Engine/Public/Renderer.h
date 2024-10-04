#pragma once

#include "Base.h"

/* 실제 객체들을 그리는 순서를 관리해야한다. */
/* 대부분의 객체들은 깊이 테스트를 통해서 알아서 차폐관리가 되고 있다. */
/* 깊이버퍼의 사용을 통해 알아서 깊이관리가된다 굉장히 큰 장점이긴한데. */
/* 이로 인해서 생기는 부작용이 존재하낟. */
/* 부작용 1. 스카이박스. : 스카이박스는 실제로 작게 그리낟.  */
/* 하늘 : 무조건 다른 객체들에게 덮여야한다(가장먼저그린다) + 하늘의 깊이는 다른객체들과 비교연산이 일어나서는 안된다.(하늘의 깊이를 깊이버퍼에 기록하지 않는다.) */
/* 부작용 2. 알파블렌드 :   */
/* 블렌드 : 내가 그릴려고하는 색과 이전에 그려져있던 색을 섞는다. */
/* 부작용 3. UI.  */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOWOBJ, RENDER_NONBLEND, RENDER_DECAL, RENDER_NONDECAL, RENDER_NONLIGHT, RENDER_BLEND, RENDER_MIRROR, RENDER_REFLECTION, RENDER_BLUR, RENDER_BLOOM, RENDER_DISTORTION, RENDER_UI, RENDER_END };

	typedef struct FOG_DESC
	{
		_float4 vFogColor;
		_float4 vFogColor2;  // 두 번째 안개 색상
		_float fFogRange;
		_float fFogHeightFalloff;
		_float fFogGlobalDensity;
		_float fFogTimeOffset;
		_float fFogTimeOffset2;  // 두 번째 안개 시간 오프셋
		_float fNoiseIntensity;
		_float fNoiseIntensity2;  // 두 번째 안개 노이즈 강도
		_float fNoiseSize;
		_float fNoiseSize2;  // 두 번째 안개 노이즈 크기
		_float fFogBlendFactor;  // 안개 블렌딩 팩터
	}FOG_DESC;

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

	void Clear();
	void Draw();
	void Set_Mirror(_float fMirror) { m_fMirror = fMirror; }
	void Set_BRIS(_float fBRIS) { m_fBRIS = fBRIS; }
	HRESULT SaveRenderTargetToDDS(ID3D11RenderTargetView* pRenderTargetView, const WCHAR* filePath);

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	list<class CGameObject*>			m_RenderGroup[RENDER_END];

private:
	class CShader* m_pShader = { nullptr };
	//class CComputeShader_Texture* m_pBloomComputeShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	_float4x4 m_prevWorldMatrix = {};
	_float4x4 m_prevViewMatrix = {};
	_float4x4 m_newWorldMatrix = {};
	_float4x4 m_newViewMatrix = {};

	_float4x4							m_WorldMatrix = {};
	_float4x4							m_ViewMatrix = {};
	_float4x4							m_ProjMatrix = {};

	ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };
	ID3D11DepthStencilView* m_pReflectionDepthStencilView = { nullptr };
	_float	m_fTime = 0.f;
	_float m_fValue = 2.2f;

#pragma region MINYOUNG
	//이민영 추가 240621 1423PM
public:
	void Set_ShadowEyeFocus(_vector vEye, _vector vFocus, _float fThreshold)
	{
		m_vShadowEye = vEye;
		m_vShadowFocus = vFocus;
		m_fShadowThreshold = fThreshold;
	}
	_vector Get_FogColor() { return  XMLoadFloat4(&m_vFogColor); };
	_vector Get_ShadowEye() { return m_vShadowEye; };
	_vector Get_ShadowFocus() { return m_vShadowFocus; };

	ID3D11Texture2D* Get_PrevDepthTex() { return m_pPrevDepthTexture; }
	_float Sample_HZB(_float2 uv, UINT mipLevel);

	void Set_FogOption(const FOG_DESC& fogDesc)
	{
		m_vFogColor = fogDesc.vFogColor;
		m_fFogRange = fogDesc.fFogRange;
		m_fFogHeightFalloff = fogDesc.fFogHeightFalloff;
		m_fFogGlobalDensity = fogDesc.fFogGlobalDensity;
		m_fFogTimeOffset = fogDesc.fFogTimeOffset;
		m_fNoiseIntensity = fogDesc.fNoiseIntensity;
		m_fNoiseSize = fogDesc.fNoiseSize;
		m_vFogColor2 = fogDesc.vFogColor2;
		m_fFogTimeOffset2 = fogDesc.fFogTimeOffset2;
		m_fNoiseIntensity2 = fogDesc.fNoiseIntensity2;
		m_fNoiseSize2 = fogDesc.fNoiseSize2;
		m_fFogBlendFactor = fogDesc.fFogBlendFactor;
	}

	void Set_ReflectionWave(_float strength, _float frequency, _float fWaveTimeOffset, _float fresnelPower, _uint CausticIdx)
	{
		m_fWaveStrength = strength;
		m_fWaveFrequency = frequency;
		m_fWaveTimeOffset = fWaveTimeOffset;
		m_fFresnelPower = fresnelPower;
		m_iCausticIdx = CausticIdx;
	}

public:
	void Set_HBAOParams(GFSDK_SSAO_Parameters params);
	//이민영 추가 240711 2002PM
private:
	ID3D11Texture2D* m_pPrevDepthTexture = nullptr;
	ID3D11ShaderResourceView* m_pPrevDepthSRV = { nullptr };
	class CComputeShader_Texture* m_pHZBComputeShader = { nullptr };

	static const UINT MAX_MIP_LEVELS = 16;  // 예상되는 최대 밉맵 레벨 수
	ID3D11ShaderResourceView* m_pHZBSRV[MAX_MIP_LEVELS] = { nullptr };
	ID3D11UnorderedAccessView* m_pHZBUAV[MAX_MIP_LEVELS] = { nullptr };
	ID3D11Texture2D* m_pHZBTexture[MAX_MIP_LEVELS] = { nullptr };


	UINT m_HZBMipLevels = 0;

#pragma endregion



#pragma region PARKJAEWON
public:
	void ProcessByThread(DWORD dwThreadIndex);
	void ProcessRenderQueue(DWORD dwThreadIndex, ID3D11DeviceContext* pDeferredContext, RENDERGROUP eRenderGroup, const wchar_t* mrtTarget);
private:
	HRESULT InitRenderThreadPool(DWORD dwThreadCount);
	void ClearRenderThreadPool();
public:
	struct RENDER_THREAD_DESC
	{
		HANDLE hEventList[RENDER_THREAD_EVENT_TYPE_COUNT];
		CRenderer* pRenderer;
		DWORD dwThreadIndex;
		HANDLE hThread;
	};
private:
	RENDER_THREAD_DESC* m_pThreadDescList = nullptr;
	HANDLE m_hCompleteEvent = nullptr;
	LONG m_lActiveThreadCount = 0;
	DWORD m_dwThreadCount = 0;
	vector<ID3D11DeviceContext*> m_DeferredContexts;
	vector<ID3D11CommandList*> m_CommandLists;
	const UINT MAX_OBJECTS_PER_COMMANDLIST = 400;
#pragma endregion

private:
	class CTexture* m_pMaskTex = { nullptr };
	//LUT TEXTURE
	class CTexture* m_pLUTTex = { nullptr };
	class CTexture* m_pDistortionTex = { nullptr };
	class CTexture* m_pShadowTex = { nullptr };
	_float								m_fLUTOffset = 0.f;
	vector<_matrix> m_vDecalMatricesInv;
	class CTexture* m_pDecalTex = { nullptr };
	_float m_fBRIS = 0.f;
	_float m_fMirror = 0.f;


private:		//For. NVIDIA_HBAO
	GFSDK_ShadowLib_Context* m_pShadowLibContext = nullptr;
	GFSDK_ShadowLib_Buffer*	m_pShadowBuffer = {};
	GFSDK_ShadowLib_Map*	m_pShadowMap = nullptr;
	ID3D11ShaderResourceView* m_pHBAOShadowSRV = nullptr;
	GFSDK_SSAO_Context_D3D11* m_pSSAOContext = nullptr;

	GFSDK_SSAO_Parameters m_Params;

	//Shadow
private:
	_vector								m_vShadowEye = XMVectorSet(0.f, 10.f, -10.f, 1.f);
	_vector								m_vShadowFocus = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_float								m_fShadowThreshold = 0.5f;

	//Fog
private:
	_float4 m_vFogColor = { 0.235f, 0.260f, 0.329f, 1.f };
	_float m_fFogRange = 1000000.f;
	_float m_fFogHeightFalloff = 0.f;
	_float m_fFogGlobalDensity = 0.f;
	_float m_fFogTimeOffset = 0.f;
	_float m_fNoiseIntensity = 0.f;
	_float m_fNoiseSize = 0.1f;

	_float4 m_vFogColor2 = { 0.235f, 0.260f, 0.329f, 1.f };
	_float m_fFogTimeOffset2 = 0.f;
	_float m_fNoiseIntensity2 = 0.f;
	_float m_fNoiseSize2 = 0.1f;
	_float m_fFogBlendFactor = 0.6;

	//Reflection Wave
	_float m_fWaveStrength = 0.6f;
	_float m_fWaveFrequency = 0.6f;
	_float m_fWaveTimeOffset = 1.f;
	_float m_fFresnelPower = 10.f;
	class CTexture* m_pCausticTex = { nullptr };
	_uint m_iCausticIdx = 0;
	
private:
	void Render_Priority();
	void Render_ShadowObjects();
	void Render_NonBlend();
	void Render_Decal();
	void Render_NonDecal();
	void Render_LightAcc();
	void Render_Shadow_Move();
	void Render_Shadow_NotMove();
	void Render_Shadow_Result();
	void Render_HBAO();
	void Render_DeferredResult();
	void Render_NonLight();
	void Render_Blend();
	void Render_Reflection();
	void Render_MotionBlur();
	void Render_Blur();
	void Render_Bloom();
	void Render_Distortion();
	void Render_GodRay();
	void Render_Final();
	void Compute_HDR();

	void Render_UI();

	void Update_HZB();

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugComponents;

private:
	void Render_Debug();
#endif


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END