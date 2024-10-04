#pragma once

#include "Camera.h"
#include "MYMapTool_Defines.h"

BEGIN(MYMapTool)

class CCutSceneCamera final : public CCamera
{
public:
	enum class CAMERA_PHASE {CAMERA_FOLLOW = 0, CAMERA_REVOLUTION, CAMERA_RETURN, CAMERA_ZOOMIN, CAMERA_END};
public:
	
	typedef struct CUTSCENECAMERA_DESC : public CCamera::CAMERA_DESC
	{
		vector<CCamera::CameraKeyFrame> KeyFrames;
	}CUTSCENECAMERA_DESC;

private:
	CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutSceneCamera(const CCutSceneCamera& rhs);
	virtual ~CCutSceneCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void ZoomIn(_float fTimeDelta);

public:
	void ZoomIn();
	void Revolution360(_float fTime);
	void TiltAdjust(_float fAngle);
	void HeightChange(_float fHeight);
	void Set_Camera_Phase(CAMERA_PHASE ePhase) 
	{
		m_eCameraPhase = ePhase;
		if (ePhase == CAMERA_PHASE::CAMERA_FOLLOW)
		{
			m_bFirstMove = true;
			m_fHeight = 0.f;
			m_fZoomDistance = 0.f;
		}

	}
	void Set_Revolution_Time(float fTime)
	{
		m_fRevolutionTime = fTime;
	}


public:
	void Set_Height(float fHeight) { m_fHeight = fHeight; }
	void Set_ZoomSpeed(float fZoomSpeed) { m_fZoomSpeed = fZoomSpeed; }
	void Set_MaxZoomDistance(float fMaxZoomDistance) { m_fMaxZoomDistance = fMaxZoomDistance; }

	_float Get_KeyFrameTime() { return m_fKeyFrameTime; }
	void Set_KeyFrameTime(_float fTime) { m_fKeyFrameTime = fTime; }
	void Set_CamACcel(_float fAccel) { m_fAccel = fAccel; }

public:
	float Get_AnimationProgress() const;

public:
	const vector<CameraKeyFrame>& Get_KeyFrames() const { return m_KeyFrames; }
	void Set_KeyFrames(const vector<CameraKeyFrame>& keyFrames) { m_KeyFrames = keyFrames; }
	void Play_CutScene()
	{
		if (m_iCurrentCutSceneIdx < m_AllCutScenes.size())
		{
			m_KeyFrames = m_AllCutScenes[m_iCurrentCutSceneIdx];
			m_iCurrentKeyFrame = 0;
			m_fKeyFrameTime = 0.f;
			m_bAnimationFinished = false;
			m_bPaused = false;
		}
	}

	void Stop_CutScene()
	{
		m_bAnimationFinished = true;
		m_bPaused = true;
	}
private:
	vector<vector<CameraKeyFrame>> m_AllCutScenes;
	_uint m_iCurrentCutSceneIdx = 0;

public:
	_uint Get_CutSceneCount() const { return m_AllCutScenes.size(); }
	void Set_CutSceneIdx(_uint idx) { m_iCurrentCutSceneIdx = idx; }
	_uint Get_CutSceneIdx() const { return m_iCurrentCutSceneIdx; }
	void Add_CutScene(const vector<CameraKeyFrame>& keyFrames) { m_AllCutScenes.push_back(keyFrames); }
	void Pop_CutScene(_uint iIdx);

	const vector<CameraKeyFrame>& Get_CutScene(_uint iIndex) const;
	void Set_CutScene(_uint iIndex, const vector<CameraKeyFrame>& keyFrames);
	void Clear_CutScenes();

private: // FOR CAMERA ANIMATIOn
	_uint m_iCutSceneIdx = 0;
	vector<CameraKeyFrame> m_KeyFrames;
	_uint m_iCurrentKeyFrame;
	_float m_fKeyFrameTime;
	_bool m_bAnimationFinished;

private:
	_float		m_fSensor = { 0.0f };
	CTransform* m_pPlayerTrans = { nullptr };
private:
	const _vector m_HeightOffset = { 0.f, 10.f, 0.f, 0.f };
	_bool		m_bActivatePick = false;
	_vector		m_vLerpPos = { 0.f, };
	class CGameObject* m_pTarget = nullptr;			//타깃

private:
	_float4 vEye;
	_float4 vDir;
	_float	fSpeed = 0.f;
	_float4 vAt;
	_float  fDistance = 20.f;
	_float	m_fHeight = 0.f;
	_float	m_fAngle = 0.f;
	_bool m_bIsMove = false;

private:
	_vector LerpAt;
	_bool IsNear = false;
private:
	_float	m_fIntensity;	//카메라 흔들림 강도
	_float	m_fShakeAccTime;		//누적 시간
	_bool	m_bLockWidth;

private:
	_float m_fZoomDistance = 0.f;
	_float m_fRevolutionTime = 3.f;
	_float m_fRevolutionAccTime = 0.f;
	_float m_fRollAngle = 30.f;
	_float m_fTargetHeight = 6.f;
	_float m_fZoomSpeed = 5.f;
	_float m_fHeightChangeSpeed = 2.f;
	_float m_fZoomAccTime = 0.f;
	_float m_fMaxZoomDistance = 10.f;
	_bool m_bZoomIn = false;
	_bool m_bRevolution360 = false;
	_bool m_bTiltAdjust = false;
	_bool m_bHeightChange = false;
	_bool m_bFirstMove = true;

	_float m_fZoomDuration = 3.f;
	CAMERA_PHASE m_eCameraPhase = CAMERA_PHASE::CAMERA_FOLLOW;

	_float m_fZoomDelay = 0.f;
	_bool m_bZoomNow = false;

	_bool m_bPaused = false;
	_matrix test;
	_float m_fAccel = 1.f;

public:
	static CCutSceneCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	void Shaking();
	void Key_Input(_float fTimeDelta);
	void UpdateCameraTransform();
	void UpdatePlayerHeight(_float fTimeDelta);
};

END