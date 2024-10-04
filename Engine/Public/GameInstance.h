#pragma once

#include"CProfileScope.h"
#include"CProfiler.h"

#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "CWorker.h"


BEGIN(Engine)
class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	

public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	//지우지 마셈 PSW
	eKEY_STATE GetKeyState(_ubyte byKeyID);
	eKEY_STATE GetMouseState(MOUSEKEYSTATE eMouseState);

public:
	_bool Key_Pressing(_ubyte byKeyID);
	_bool Key_Down(_ubyte byKeyID);
	_bool Key_Up(_ubyte byKeyID);

	_bool Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_bool Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool Mouse_Up(MOUSEKEYSTATE eMouse);

	MOUSEWHEELSTATE CheckMouseWheel(void);	//마우스 휠 스테이트를 반환

public: /* For.Timer_Manager */
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	void		Update_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint Get_CurrentLevel();			//현재 레벨 받아오기
	_uint Get_CurrentLevelIndex();		//현재 레벨 인덱스 받아오기
	_uint Get_PrevLevelIndex();			//이전 레벨 인덱스 받아오기
	void Set_NextLevel(_uint iNextLevel);			

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);
	void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	list< class CGameObject*> Get_GameObjects_Ref(_uint iLevelIndex, const wstring& strLayerTag);
	void Get_LayerTags_String(_uint iLevelIndex, vector<const char*>* vecRefLayerName);

	//240622 2113 PM 이민영 추가
	//FOR CAMERA SWITCH
	HRESULT Add_Camera(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr); // 카메라 추가 시 이 함수 불러야됨
	void Set_MainCamera(_uint iCameraIndex); // 카메라 전환 시 호출, 추가 순서가 인덱스
	vector<class CCamera*> Get_Cameras();
	CCamera* Get_MainCamera();
	_uint Get_MainCameraIdx();
	void Clear_Cameras();

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype);
	CComponent* Get_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	_bool	IsPrototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Renderer */
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	void Set_BRIS(_float fBRIS);
	void Set_Mirror(_float fMirror);
	// 화면 전체 모션블러 여부
	void Set_MotionBlur(_bool bMotionBlur) { m_iMotionBlur = bMotionBlur ? m_iMotionBlur + 1 : m_iMotionBlur; }
	_bool Get_MotionBlur() { return m_iMotionBlur > 0; }
	_bool Get_NotMoveShadow(){ return m_bNotMoveShadow; }
	_bool Get_MoveShadow(){ return m_bMoveShadow; }
private:
	_uint m_iMotionBlur = 0;
	_bool m_bNotMoveShadow = false;
	_bool m_bMoveShadow = true;
#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif
public:
	//이민영 추가 240621 1423PM
	void Set_ShadowEyeFocus(_vector vEye, _vector vFocus, _float fThreshold);
	_vector Get_ShadowEye();
	_vector Get_ShadowFocus();
	ID3D11Texture2D* Get_PrevDepthTex();

	//이민영 추가 240727 2106PM
	void Set_FogOption(const CRenderer::FOG_DESC& desc);
	_vector Get_FogColor();

	//이민영 추가 240811 2114PM
	void Set_ReflectionWave(_float strength, _float frequency, _float fWaveTimeOffset, _float fresnelPower, _uint CausticIdx);

	void Set_HBAOParams(GFSDK_SSAO_Parameters params);
public: /* For.PipeLine */
	const _float4x4* Get_Transform_float4x4(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState);
	const _float4x4* Get_Transform_float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	const _float4* Get_CamPosition_float4();
	_vector Get_CamPosition();
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);

public: /* For.Picking */	
	_bool Get_PickPos(_float4* pPickPos);
	void Update_Picking();

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Light_Clear();
	void LightOff(_uint iIndex);
	void LightOn(_uint iIndex);

	void LightOff_All();
	void Update_LightPos(_uint iIdx, _vector vPos);

	void Edit_Light(_uint iIndex, LIGHT_DESC* desc);
	list<class CLight*> Get_Lights();

public: /* For.Font_Manager */
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring & strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, _bool isClear = true, ID3D11DepthStencilView* pDSView = nullptr);
	HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DeviceContext* pDeferredContext, _bool isClear = true, ID3D11DepthStencilView* pDSView = nullptr);
	HRESULT End_MRT();
	HRESULT End_MRT(ID3D11DeviceContext* pDeferredContext);
	ID3D11RenderTargetView* Get_RTV(const wstring& strTargetTag);
	ID3D11ShaderResourceView* Get_SRV(const wstring& strTargetTag);
	HRESULT Bind_RenderTargetSRV(const wstring & strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_RenderTargetSRV_Compute(const wstring& strTargetTag, class CComputeShader_Texture* pComputeShader, const _char* pConstantName);
	HRESULT Copy_Resource(const wstring & strTargetTag, ID3D11Texture2D* pDesc);
	_float	Sample_HZB(_float2 uv, UINT mipLevel);

public:
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
	_bool isIn_WorldFrustum(_fvector vPosition, _float fRange = 0.f);
	_bool isIn_LocalFrustum(_fvector vPosition, _float fRange = 0.f);


public: /*for Calculator*/
	_vector Get_RayPos();					//마우스 레이와 충돌하기 위한 함수들임
	_vector Get_RayDir();					
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, class CTransform* pTransform);
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, XMMATRIX WorldInverse);
	POINT Get_Mouse_Point();			//마우스 받는곳은 항상 여기에서 

public:		//for Event_Manager
	HRESULT EventUpdate();				//메인 앱에서 렌더함수 끝나고 호출
	void CreateObject(_uint Level, const wchar_t* Layer, const wstring& strPrototypeTag, void* pArg = nullptr); // 동적으로 객체 생성하는 기능
	void CreateObject_Self(_uint Level, const wchar_t* Layer, class CGameObject* pObj);			//뭔가 동작을 해주고 만들어야 할 때
	void Erase(CGameObject* _pObj); //동적으로 객체 삭제하는 기능
	void Scene_Change(_uint LevelIdx, CLevel* nextLevel); // 씬체인지

public: /* For PhysX */
	HRESULT AddActor(PxActor* pActor);
	PxPhysics* GetPhysics();
	PxScene* GetScene();
	PxControllerManager* GetControllerManager();
	
public: // Sound Mgr
	void Play_Effect_Sound(const TCHAR* pSoundKey,/*사운드파일이름*/
		CHANNELID eID, /*채널*/
		_float fPosition = 0.f, /*재생 시작 지점( 초 단위 넣으면 됨, 1초부터 시작하고싶으면 1.f*/ 
		_float fPitch = 1.f,  /*소리의 높낮이 조정(0.1f 바뀔때마다 도레미파솔라시도)*/
		_float fVolume = 0.5f,
		_bool bPlayAgainstPlaying = true);
	void PlayBGM(const TCHAR* pSoundKey, _float fVolume = 1.f);
	void StopAll();
	void StopSound(CHANNELID eID);
	void Sound_Pause(CHANNELID eID, _bool bPause); //채널 사운드 일시정지
	void SetChannelVolume(CHANNELID eID, float fVolume);
	void Set_Effect_Volume(_float fVolume);
	void Set_BGM_Volume(_float fVolume);
	void Set_Reverb_Param(_float roomSize, _float decayTime, _float wetMix); //리버브 수치 조절
	void Enable_Reverb(); //리버브 활성화
	void Disable_Reverb();	//리버브 비활성화
	void Enable_Echo();	//에코 활성화
	void Disable_Echo();	//에코 비활성화
	void Set_Echo_Param(_float delay, _float wetLevel); //에코 딜레이(밀리초 단위), 메아리소리 볼륨
	void Set_OffSet(_float fOffSet); //사운드 오프셋 조절
public:	//for UISorter 박은호
	HRESULT Add_UI(class CGameObject* ui, UISORT_PRIORITY type);

//이민영 추가 240710 2204PM
public: // For OctTree
	_bool IsVisibleObject(CGameObject* obj);
	_bool isVisible(_vector vPos, PxActor* actor);
	void AddCullingObject(CGameObject* obj, PxActor* pActor);

public:	// For Worker
	template<typename T, typename... Args>
	auto  AddWork(T&& Func, Args&&... args) -> future<decltype(Func(args...))>
	{
		return 	m_pWorker->Add_Job(Func, args...);	
	}

public:
	_float4 Get_PlayerPos() { return m_fPlayerPos; }
	void Set_PlayerPos(_vector vPos) { XMStoreFloat4(&m_fPlayerPos, vPos); }

public:
	_uint Get_CascadeNum(_fvector vPosition, _float fRange);

#ifdef _DEBUG
public:
	HRESULT Ready_RTDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CPhysX*					m_pPhysX = { nullptr };

	class CBlastMgr*				m_pBlastMgr = { nullptr };
	class CEventMgr*				m_pEvent_Manager = { nullptr };
	class CCalculator*				m_pCalculator = { nullptr };
	class CCascade*					m_pCascade = { nullptr };

	class CSoundMgr*				m_pSound_Manager = { nullptr };
	class CUISorter*				m_UISorter = { nullptr };


	class COctTree*				m_pOctTree = { nullptr };

	class CWorker*				m_pWorker = { nullptr };
	class CRenderWorker*		m_pRenderWorker = { nullptr };

	_float4 m_fPlayerPos;
public:	
	static void Release_Engine();
	virtual void Free() override;
};

END