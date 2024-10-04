#include "..\Public\GameInstance.h"
#include"CHitReport.h"
#include "Graphic_Device.h"
#include "Input_Device.h"

#include "Object_Manager.h"
#include "Target_Manager.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Renderer.h"
#include "Picking.h"
#include "Frustum.h"
#include "CPhysX.h"
#include "CCuda.h"
#include "BlastMgr.h"
#include "EventMgr.h"
#include "Calculator.h"
#include "Cascade.h"

#include "SoundMgr.h"
#include "UISorter.h"

#include "OctTree.h"

#include"CWorker.h"
#include"CRenderWorker.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}


HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC & EngineDesc, ID3D11Device ** ppDevice, ID3D11DeviceContext ** ppContext)
{
	/* 엔진 프로젝트르 ㄹ클라이언트에서 사용하기 위해 필수적으로 해야할 여러 초기화작업을 수행한다. */
	
	/* 그래픽 디바이스를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	/* 인풋 디바이스를 초기화하낟. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;
	
	/* 사운드  디바이스를 초기화하낟. */
	m_pSound_Manager = CSoundMgr::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	/* 레벨 매니져의 준비를 하자. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	/* 오브젝트, 컴포넌트 매니져를 사용하기 위한 준비를 한다.*/
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* 렌더러 생성. */
	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	/* 파이프라인 생성. */
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pCascade = CCascade::Create();
	if (nullptr == m_pCascade)
		return E_FAIL;

	m_pPhysX = CPhysX::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPhysX)
		return E_FAIL;



	m_pBlastMgr = CBlastMgr::Create();
	if (nullptr == m_pBlastMgr)
		return E_FAIL;


	m_pEvent_Manager = CEventMgr::Create(m_pObject_Manager);
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	//Calculator 생성
	m_pCalculator = CCalculator::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pCalculator)
		return E_FAIL;

	m_UISorter = CUISorter::Create();
	if (nullptr == m_UISorter)
		return E_FAIL;



	//m_pOctTree = COctTree::Create({ -600.f, -50.f, -200.f }, {350.f, 200.f, 100.f}, 0);
	//if (nullptr == m_pOctTree)
	//	return E_FAIL;
	



	size_t iNumThreadPool = PxThread::getNbPhysicalCores();

	m_pWorker = CWorker::Create(iNumThreadPool);
	if (nullptr == m_pWorker)
		return E_FAIL;

	//m_pRenderWorker = CRenderWorker::Create(iNumThreadPool, *ppDevice);
	//if (nullptr == m_pRenderWorker)
	//	return E_FAIL;
	//

	

	return S_OK;

	
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	m_iMotionBlur = 0;

	PROFILE_RESET();
	
	if (nullptr == m_pLevel_Manager)
		return;	

	PROFILE_CALL("Input Device Update", m_pInput_Device->Update_InputDev());

	vector<future<void>> futures;

	//AddWork([this, fTimeDelta]() {
	//	PROFILE_CALL("Object Manager Priority Tick", m_pObject_Manager->Priority_Tick(fTimeDelta));
	//});
	//futures.push_back(m_pWorker->Add_Job([this, fTimeDelta]() {
	//	PROFILE_CALL("Object Manager Priority Tick", m_pObject_Manager->Priority_Tick(fTimeDelta));
	//	}));
	PROFILE_CALL("Object Manager Priority Tick", m_pObject_Manager->Priority_Tick(fTimeDelta));

	
	//AddWork([this, fTimeDelta]() {
	//	PROFILE_CALL("Object Manager Tick", m_pObject_Manager->Tick(fTimeDelta));
	//});

	//futures.push_back(m_pWorker->Add_Job([this, fTimeDelta]() {
	//	PROFILE_CALL("Object Manager Tick", m_pObject_Manager->Tick(fTimeDelta));
	//	}));

	PROFILE_CALL("Object Manager Tick", m_pObject_Manager->Tick(fTimeDelta));


	//futures.push_back(m_pWorker->Add_Job([this]() {
	//	PROFILE_CALL("PipeLine Tick", m_pPipeLine->Tick());
	//	}));
	PROFILE_CALL("PipeLine Tick", m_pPipeLine->Tick());
	

	//futures.push_back(m_pWorker->Add_Job([this]() {
	//	PROFILE_CALL("Calculator Tick", m_pCalculator->Store_MouseRay(m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ), m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW)));
	//	}));
	PROFILE_CALL("Calculator Tick", m_pCalculator->Store_MouseRay(m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ), m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW)));
	//futures.push_back(m_pWorker->Add_Job([this]() {
	//	PROFILE_CALL("Frustum Tick", m_pFrustum->Update());
	//	}));
	PROFILE_CALL("Frustum Tick", m_pFrustum->Update());

	//futures.push_back(m_pWorker->Add_Job([this]() {
	//	PROFILE_CALL("Cascade Tick", m_pCascade->Update());
	//	}));

	//PROFILE_CALL("Cascade Tick", m_pCascade->Update());

	//for (auto& worker : futures)
	//{
	//	worker.wait();
	//}
	//futures.clear();


	PROFILE_CALL("PhysX Tick", m_pPhysX->Tick(fTimeDelta));
	

#ifdef _DEBUG
	//PROFILE_CALL("Picking Update", m_pPicking->Update());
#endif // _DEBUG



	//futures.push_back(m_pWorker->Add_Job([this]() {
	//	PROFILE_CALL("OctTree Update", m_pOctTree->Update_OctTree());
	//	}));



	//PROFILE_CALL("OctTree Update", m_pOctTree->Update_OctTree());

	PROFILE_CALL("Object Manager Late_Tick", m_pObject_Manager->Late_Tick(fTimeDelta));

	PROFILE_CALL("Level Manager Tick", m_pLevel_Manager->Tick(fTimeDelta));
	
	PROFILE_CALL("Level Manager Late_Tick",m_pLevel_Manager->Late_Tick(fTimeDelta));
	
	PROFILE_CALL("UISorting Tick",m_UISorter->Sorting());

}

HRESULT CGameInstance::Draw()
{


	m_pRenderer->Draw();	

	return S_OK;


}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);



	
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}


_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);	
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

eKEY_STATE CGameInstance::GetKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->GetKeyState(byKeyID);
}

eKEY_STATE CGameInstance::GetMouseState(MOUSEKEYSTATE eMouseState)
{
	return m_pInput_Device->GetMouseState(eMouseState);
}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Mouse_Pressing(eMouse);
}

_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Mouse_Down(eMouse);
}

_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Mouse_Up(eMouse);
}

MOUSEWHEELSTATE CGameInstance::CheckMouseWheel(void)
{
	return m_pInput_Device->CheckMouseWheel();
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);	
}

void CGameInstance::Update_TimeDelta(const _tchar * pTimerTag)
{
	m_pTimer_Manager->Update_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}



_uint CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

_uint CGameInstance::Get_CurrentLevelIndex()
{
	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

_uint CGameInstance::Get_PrevLevelIndex()
{
	return m_pLevel_Manager->Get_PrevLevelIndex();

}


void CGameInstance::Set_NextLevel(_uint iNextLevel)
{
	return m_pLevel_Manager->Set_NextLevelIndex(iNextLevel);
}

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);	
}

CGameObject * CGameInstance::Clone_Object(const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_Object(strPrototypeTag, pArg);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CGameObject * CGameInstance::Get_Object(_uint iLevelIndex, const wstring & strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Object(iLevelIndex, strLayerTag, iIndex);	
}

void CGameInstance::Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	m_pObject_Manager->Clear_Layer(iLevelIndex, strLayerTag);
}

list< class CGameObject*> CGameInstance::Get_GameObjects_Ref(_uint iLevelIndex, const wstring& strLayerTag)
{
	return m_pObject_Manager->Get_GameObjects_Ref(iLevelIndex, strLayerTag);

}


void CGameInstance::Get_LayerTags_String(_uint iLevelIndex, vector<const char*>* vecRefLayerName)
{
	m_pObject_Manager->Get_LayerTags_String(iLevelIndex, vecRefLayerName);
}





HRESULT CGameInstance::Add_Camera(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_Camera(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

void CGameInstance::Set_MainCamera(_uint iCameraIndex)
{
	m_pObject_Manager->Set_MainCamera(iCameraIndex);
}

vector<class CCamera*> CGameInstance::Get_Cameras()
{
	return m_pObject_Manager->Get_Cameras();
}

CCamera* CGameInstance::Get_MainCamera()
{
	return m_pObject_Manager->Get_MainCamera();
}

_uint CGameInstance::Get_MainCameraIdx()
{
	return m_pObject_Manager->Get_MainCameraIdx();
}

void CGameInstance::Clear_Cameras()
{
	m_pObject_Manager->Clear_Cameras();
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent* CGameInstance::Get_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	return m_pComponent_Manager->Get_Prototype(iLevelIndex, strPrototypeTag);
}

_bool CGameInstance::IsPrototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	return m_pComponent_Manager->IsPrototype(iLevelIndex, strPrototypeTag);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject);	
}

void CGameInstance::Set_BRIS(_float fBRIS)
{
	return m_pRenderer->Set_BRIS(fBRIS);
}

void CGameInstance::Set_Mirror(_float fMirror)
{
	return m_pRenderer->Set_Mirror(fMirror);
}

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponent(CComponent * pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);	
}
#endif

void CGameInstance::Set_ShadowEyeFocus(_vector vEye, _vector vFocus, _float fThreshold)
{
	m_pRenderer->Set_ShadowEyeFocus(vEye, vFocus, fThreshold);
}

_vector CGameInstance::Get_ShadowEye()
{
	return m_pRenderer->Get_ShadowEye();
}

_vector CGameInstance::Get_ShadowFocus()
{
	return m_pRenderer->Get_ShadowFocus();
}

ID3D11Texture2D* CGameInstance::Get_PrevDepthTex()
{
	return m_pRenderer->Get_PrevDepthTex();
}

void CGameInstance::Set_FogOption(const CRenderer::FOG_DESC& desc)
{
	m_pRenderer->Set_FogOption(desc);
}

_vector CGameInstance::Get_FogColor()
{
	return m_pRenderer->Get_FogColor();
}

void CGameInstance::Set_ReflectionWave(_float strength, _float frequency, _float fWaveTimeOffset, _float fresnelPower, _uint CausticIdx)
{
	m_pRenderer->Set_ReflectionWave(strength, frequency, fWaveTimeOffset, fresnelPower, CausticIdx);
}

void CGameInstance::Set_HBAOParams(GFSDK_SSAO_Parameters params)
{
	m_pRenderer->Set_HBAOParams(params);
}

const _float4x4 * CGameInstance::Get_Transform_float4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4x4 * CGameInstance::Get_Transform_float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

const _float4 * CGameInstance::Get_CamPosition_float4()
{
	return m_pPipeLine->Get_CamPosition_float4();
}

_vector CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_bool CGameInstance::Get_PickPos(_float4* pPickPos)
{
	return m_pPicking->Get_PickPos(pPickPos);
	
}

void CGameInstance::Update_Picking()
{
	m_pPicking->Update();
}

const LIGHT_DESC * CGameInstance::Get_LightDesc(_uint iIndex) const
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Light_Clear()
{
	return m_pLight_Manager->Light_Clear();
}

void CGameInstance::LightOff(_uint iIndex)
{
	m_pLight_Manager->LightOff(iIndex);
}

void CGameInstance::LightOn(_uint iIndex)
{
	m_pLight_Manager->LightOn(iIndex);
}

void CGameInstance::LightOff_All()
{
	m_pLight_Manager->LightOff_All();
}

void CGameInstance::Update_LightPos(_uint iIdx, _vector vPos)
{
	m_pLight_Manager->Update_LightPos(iIdx, vPos);

}

void CGameInstance::Edit_Light(_uint iIndex, LIGHT_DESC* desc)
{
	m_pLight_Manager->Edit_Light(iIndex, desc);
}

list<class CLight*> CGameInstance::Get_Lights()
{
	return m_pLight_Manager->Get_Lights();
}

HRESULT CGameInstance::Add_Font(const wstring & strFontTag, const wstring & strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);	
}

HRESULT CGameInstance::Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor)
{
	return m_pFont_Manager->Render_Font(strFontTag, strText, vPosition, vColor);
	//return S_OK;
}

HRESULT CGameInstance::Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);	
}

HRESULT CGameInstance::Add_MRT(const wstring & strMRTTag, const wstring & strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag, _bool isClear, ID3D11DepthStencilView* pDSView)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, isClear, pDSView);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag, ID3D11DeviceContext* pDeferredContext, _bool isClear, ID3D11DepthStencilView* pDSView)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDeferredContext,isClear, pDSView);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::End_MRT(ID3D11DeviceContext* pDeferredContext)
{
	return m_pTarget_Manager->End_MRT(pDeferredContext);
}

ID3D11RenderTargetView* CGameInstance::Get_RTV(const wstring& strTargetTag)
{
	return m_pTarget_Manager->Get_RTV(strTargetTag);
}

ID3D11ShaderResourceView* CGameInstance::Get_SRV(const wstring& strTargetTag)
{
	return m_pTarget_Manager ->Get_SRV(strTargetTag);
}

HRESULT CGameInstance::Bind_RenderTargetSRV(const wstring & strTargetTag, CShader * pShader, const _char * pConstantName)
{
	return m_pTarget_Manager->Bind_RenderTargetSRV(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Bind_RenderTargetSRV_Compute(const wstring& strTargetTag, CComputeShader_Texture* pComputeShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_RenderTargetSRV_Compute(strTargetTag, pComputeShader, pConstantName);
}

HRESULT CGameInstance::Copy_Resource(const wstring & strTargetTag, ID3D11Texture2D * pDesc)
{
	return m_pTarget_Manager->Copy_Resource(strTargetTag, pDesc);	
}

_float CGameInstance::Sample_HZB(_float2 uv, UINT mipLevel)
{
	return m_pRenderer->Sample_HZB(uv, mipLevel);
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	return m_pFrustum->Transform_ToLocalSpace(WorldMatrixInv);
}

_bool CGameInstance::isIn_WorldFrustum(_fvector vPosition, _float fRange)
{
	return m_pFrustum->isIn_WorldFrustum(vPosition, fRange);
}

_bool CGameInstance::isIn_LocalFrustum(_fvector vPosition, _float fRange)
{
	return m_pFrustum->isIn_LocalFrustum(vPosition, fRange);
}

_bool CGameInstance::isVisible(_vector vPos, PxActor* actor)
{
	return m_pFrustum->isVisible(vPos, actor);
}

_vector CGameInstance::Get_RayPos()
{
	if (m_pCalculator == nullptr)
		return _vector();
	else
		return m_pCalculator->Get_RayPos();
}

_vector CGameInstance::Get_RayDir()
{
	if (m_pCalculator == nullptr)
		return _vector();
	else
		return m_pCalculator->Get_RayDir();
}

void CGameInstance::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, CTransform* pTransform)
{
	m_pCalculator->Compute_LocalRayInfo(pRayDir, pRayPos, pTransform);
}

void CGameInstance::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, XMMATRIX WorldInverse)
{
	m_pCalculator->Compute_LocalRayInfo(pRayDir, pRayPos, WorldInverse);
}

POINT CGameInstance::Get_Mouse_Point()
{
	return m_pCalculator->Get_Mouse_Point();
}

HRESULT CGameInstance::EventUpdate()
{
	
	
	return m_pEvent_Manager->EventUpdate();
}

void CGameInstance::CreateObject(_uint Level, const wchar_t* Layer, const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pObj = Clone_Object(strPrototypeTag, pArg);

	tEvent evn = {};
	evn.eEven = eEVENT_TYPE::CREATE_OBJECT;
	evn.lParam = (DWORD_PTR)Level;
	evn.wParam = (DWORD_PTR)Layer;
	evn.pParam = (DWORD_PTR)pObj;

	m_pEvent_Manager->AddEvent(evn);
}

void CGameInstance::CreateObject_Self(_uint Level, const wchar_t* Layer, CGameObject* pObj)
{
	tEvent evn = {};
	evn.eEven = eEVENT_TYPE::CREATE_OBJECT;
	evn.lParam = (DWORD_PTR)Level;
	evn.wParam = (DWORD_PTR)Layer;
	evn.pParam = (DWORD_PTR)pObj;

	m_pEvent_Manager->AddEvent(evn);
}

void CGameInstance::Erase(CGameObject* _pObj)
{
	if (_pObj == nullptr)
		return;
	tEvent evn = {};
	evn.eEven = eEVENT_TYPE::DELETE_OBJECT;
	evn.lParam = (DWORD_PTR)_pObj;
	m_pEvent_Manager->AddEvent(evn);
}

void CGameInstance::Scene_Change(_uint LevelIdx, CLevel* nextLevel)
{
	tEvent evn = {};
	evn.eEven = eEVENT_TYPE::SCENE_CHANGE;
	evn.lParam = (DWORD_PTR)LevelIdx;
	evn.eParam = (DWORD_PTR)nextLevel;
	m_pEvent_Manager->AddEvent(evn);
}


HRESULT CGameInstance::AddActor(PxActor* pActor)
{
	return m_pPhysX->AddActor(pActor);
}

PxPhysics* CGameInstance::GetPhysics()
{
	return m_pPhysX->GetPhysics();
}

PxScene* CGameInstance::GetScene()
{
	return m_pPhysX->GetScene();
}

PxControllerManager* CGameInstance::GetControllerManager()
{
	return m_pPhysX->GetControllerManager();
}


void CGameInstance::Play_Effect_Sound(const TCHAR* pSoundKey, CHANNELID eID, _float fPosition, _float fPitch, _float fVolume, _bool bPlayAgainstPlaying)
{
	m_pSound_Manager->Play_Effect_Sound(pSoundKey, eID, fPosition, fPitch, fVolume, bPlayAgainstPlaying);
}

void CGameInstance::PlayBGM(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::StopSound(CHANNELID eID)
{
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::Sound_Pause(CHANNELID eID, _bool bPause)
{
	m_pSound_Manager->Sound_Pause(eID, bPause);
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

void CGameInstance::Set_Effect_Volume(_float fVolume)
{
	m_pSound_Manager->Set_Effect_Volume(fVolume);
}

void CGameInstance::Set_BGM_Volume(_float fVolume)
{
	m_pSound_Manager->Set_BGM_Volume(fVolume);
}

void CGameInstance::Set_Reverb_Param(_float roomSize, _float decayTime, _float wetMix)
{
	m_pSound_Manager->Set_Reverb_Param(roomSize, decayTime, wetMix);
}

void CGameInstance::Enable_Reverb()
{
	m_pSound_Manager->Enable_Reverb();
}

void CGameInstance::Disable_Reverb()
{
	m_pSound_Manager->Disable_Reverb();
}

void CGameInstance::Enable_Echo()
{
	m_pSound_Manager->Enable_Echo();
}

void CGameInstance::Disable_Echo()
{
	m_pSound_Manager->Disable_Echo();
}

void CGameInstance::Set_Echo_Param(_float delay,  _float wetLevel)
{
	m_pSound_Manager->Set_Echo_Param(delay, wetLevel);
}

void CGameInstance::Set_OffSet(_float fOffSet)
{
	m_pSound_Manager->Set_OffSet(fOffSet);

}

HRESULT CGameInstance::Add_UI(CGameObject* ui, UISORT_PRIORITY type)
{
	return m_UISorter->Add_UI(ui, type);
}

//_bool CGameInstance::IsVisibleObject(CGameObject* obj)
//{
//	return m_pOctTree->IsObjectVisible(obj);
//}

//void CGameInstance::AddCullingObject(CGameObject* obj, PxActor* pActor)
//{
//	m_pOctTree->AddObject(obj, pActor);
//}

//template<typename T, typename... Args>
//auto CGameInstance::AddWork(T&& Func, Args&&... args) -> future<decltype(Func(args...))>
//{
//	return 	m_pWorker->Add_Job(Func, args...);
//}

_uint CGameInstance::Get_CascadeNum(_fvector vPosition, _float fRange)
{
	return m_pCascade->Get_CascadeNum(vPosition, fRange);
}

#ifdef _DEBUG

HRESULT CGameInstance::Ready_RTDebug(const wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);	
}

HRESULT CGameInstance::Render_RTDebug(const wstring& strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

#endif

void CGameInstance::Release_Engine()
{	
	CGameInstance::GetInstance()->Free();

	CProfiler::DestroyInstance();
	DestroyInstance();	
}

void CGameInstance::Free()
{

	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pCascade);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_UISorter);

	//Safe_Release(m_pOctTree);

	Safe_Release(m_pWorker);
	//Safe_Release(m_pRenderWorker);



	Safe_Release(m_pPhysX);

	Safe_Release(m_pBlastMgr);
	m_iRefCnt;
	int ttmp = 0;

}



