#pragma once

#include "Base.h"
#include "AnimTool_Defines.h"
#include "..\Public\imgui.h"

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

private:
	enum LIST_LAYER { LAYER_MONSTER, LAYER_MAPELEMENT, LAYER_MAPOBJ, LAYER_END };

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	_int	Get_FileIdx() { return m_iFileIdx; }

	_bool	Get_IsSave() { return m_IsSave; }
	void	Set_DontSave() { m_IsSave = false; }

	_bool	Get_IsLoad() { return m_IsLoad; }
	void	Set_DontLoad() { m_IsLoad = false; }

	_int	Get_AnimIdx() { return m_iAnimIdx; }
	void	Set_NumKeyFrame(_uint iNumKeyFrame) { m_iNumKeyFrame = iNumKeyFrame; }

	_bool					Get_isLoop() { return m_isLoop; }
	void					Set_isLoop(_bool isLoop) { m_isLoop = isLoop; }
	_int					Get_BoneIdx() { return m_iBoneIdx; }
	vector<string>*			Get_ApplySound(_uint iAnimIdx);
	vector<string>*			Get_ApplyEffect(_uint iAnimIdx);
	vector<_int>*			Get_EffectBoneIdx(_uint iAnimIdx);
	_bool					Get_isAble(_uint iAnimIdx);
	_uint					Get_ColliderBoneIdx(_uint iAnimIdx);
	_float					Get_StartKeyframe(_uint iAnimIdx);
	_float					Get_EndKeyframe(_uint iAnimIdx);


	void	Add_vecAnim(_char* pName) { m_vecAnim.emplace_back(pName); }
	void	Add_vecChannel(_char* pName) { m_vecChannel.emplace_back(pName); }
	void	Add_vecBone(_char* pName) { m_vecBone.emplace_back(pName); }

	void	Clear_vecAnim();
	void	Clear_vecChannel();
	void	Clear_vecBone();

	// Load
	void	Load_ApplySound(_uint iAnimIdx, string pSoundFile);
	void	Load_ApplyEffect(_uint iAnimIdx, string pEffectFile);
	void	Load_EffectBoneIdx(_uint iAnimIdx, _int iBoneIdx);
	void	Load_isAble(_uint iAnimIdx, _bool isAble);
	void	Load_ColliderIdx(_uint iAnimIdx, _uint iColliderBoneIdx);
	void	Load_StartKeyframe(_uint iAnimIdx, _float fStartKeyframe);
	void	Load_EndKeyframe(_uint iAnimIdx, _float fEndKeyframe);
	void	Load_PartObj(const _char* szName);


	_bool	IsColliderSave(_uint iAnimIdx);

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

	void	EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);

private:
	_bool	m_bShowWindow;
	_bool	m_bShowAnotherWindow;
	_bool	m_bShowPartObjWindow;

	_bool	m_IsSave = { false };
	_bool	m_IsLoad = { false };

	_bool	m_isLoop = { false };
	_bool	m_isCollderAble = { false }; // Collider »∞º∫»≠ ø©∫Œ

	// Imguizmo
	_bool		useWindow = { false };
	_int		gizmoCount = 1;

	_int		m_iFileIdx;					// ∆ƒ¿œ Index
	_int		m_iAnimIdx;					// Anim Index
	_int		m_iChannelIdx;				// Channel Index
	_int		m_iNumKeyFrame;				// KeyFrame ∞≥ºˆ
	_int		m_iCurrentKeyframe = { 0 }; // «ˆ¿Á KeyFrame
	_int		m_iBoneIdx;					// Bone Index
	_int		m_iSoundIdx;				// Sound Index
	_int		m_iApplySoundIdx;			// Apply Sound Index
	_int		m_iEffectIdx;				// Effect Index
	_int		m_iApplyEffectIdx;			// Apply Effect Index
	_int		m_iPartObjIdx;
	_int		m_iAddPartObjIdx;

	_int		m_iCollideBoneIdx;
	_double		m_dMaxDuration;		// √— Duration
	_double		m_dCurrentPos;		// «ˆ¿Á Anim ¿ßƒ°
	_float		m_fStartKeyframe = { 0.0 };
	_float		m_fEndKeyframe = { 0.0 };

	vector<_char*>					m_vecAnim;			// Anim ∫§≈Õ
	vector<_char*>					m_vecChannel;		// Channel ∫§≈Õ
	vector<_char*>					m_vecBone;			// Bone ∫§≈Õ
	vector<const _char*>			m_vecSound;			// Sound ∫§≈Õ
	map<_int, vector<string>>		m_mapApplySound; // const _char*
	vector<const _char*>			m_vecEffect;		// Effect ∫§≈Õ
	vector<string>					m_vecPartObj;		// PartObj ∫§≈Õ
	vector<string>					m_vecAddPartObj;	// √ﬂ∞°µ» PartObj ∫§≈Õ

	map<_int, vector<string>>			m_mapApplyEffect;
	map<_int, vector<_int>>				m_mapBoneIdx;
	map < _int, map<string, string>>	m_mapColliderMap;

	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	void	Delete_Obj();

	void	Select_List_File();
	void	Select_List_Anim();
	void	Slider_AnimDuration();
	void	Table_Channel();

	void	Delete_ChannelVec();
	void	Select_List_Bone();
	
	void	Select_List_Sound();
	void	Apply_List_Sound();
	void	Select_List_Effect();
	void	Apply_List_Effect();
	void	Apply_List_EffectPos();

	// Sound
	void	Initialize_vecSound();
	void	Initialize_ApplySound();
	void	Add_ApplySound();
	void	Remove_ApplySound();
	void	Play_Sound();
	// Effect
	void	Initialize_vecEffect();
	void	Initialize_ApplyEffect();
	void	Add_ApplyEffect();
	void	Remove_ApplyEffect();

	// Collider
	void	Insert_Collider();
	void	Apply_Collider(_uint iType);
	void	Initialize_Collider();
	void	Clear_Collider();
	void	Setting_Collider(_uint iType);

	// PartObj
	void	Initialize_PartObj();
	void	Select_List_PartObj();
	void	Select_List_AddPartObj();
	void	Add_PartObject();
	void	Setting_PartObj();

public:
	virtual void Free() override;
};

