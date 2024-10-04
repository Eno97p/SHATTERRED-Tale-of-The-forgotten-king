#pragma once

#include "Base.h"
#include "Camera.h"
/* 원형객체들을 보관한다. */
/* 특정 원형객체를 검색하여 복제하고, 복제한 객체(사본)를 레벨(동적할당)별로 그룹(Layer)지어 보관한다. */
/* 보관하고 있는 사본 객체들의 반복적인 Tick함수들을 호출해준다. */


extern "C"
{

	__declspec(dllexport) void Free_LayerTags_String(std::vector<const char*>* vecRefLayerName);
}

BEGIN(Engine)


class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex);
	class CGameObject* Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);
	list< class CGameObject*> Get_GameObjects_Ref(_uint iLevelIndex, const wstring& strLayerTag);
public:	
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	class CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);


	HRESULT Delete_Event(vector<class CGameObject*> DeadObjs); //이벤트 매니저 동적 삭제용도
	void Add_Object(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pGameObject); //이벤트 매니저 동적 생성
	void Delete_All(_uint iLevelIndex, const wstring& strLayerTag); 
	void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	__declspec(dllexport) void Get_LayerTags_String(_uint iLevelIndex, vector<const char*>* vecRefLayerName);

	void Garbage_Collection(map<const wstring, class CLayer*>* Layer);


	//FOR CAMERA SWITCH
	HRESULT Add_Camera(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	void Set_MainCamera(_uint iCameraIndex);
	CCamera* Get_MainCamera();
	vector<class CCamera*> Get_Cameras();
	void Clear_Cameras();

	_uint Get_MainCameraIdx() { return m_iMainCameraIdx; };



private:
	map<const wstring, class CGameObject*>				m_Prototypes;
	map<const wstring, class CLayer*>*					m_pLayers = { nullptr };

	_uint												m_iNumLevels = { 0 };
	_uint												m_iNumLayers = { 0 };

	// For Camera
	vector<class CCamera* >		m_Cameras;
	_uint						m_iMainCameraIdx = 0;

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);


public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};


END
