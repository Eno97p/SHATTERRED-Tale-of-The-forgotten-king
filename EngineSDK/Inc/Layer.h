#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const wstring& strComponentTag, _uint iIndex);
	class CGameObject* Get_Object(_uint iIndex);
	list<class CGameObject* > Get_GameObjects_Ref();

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);

	void Delete_All();		//레이어 삭제기능
	void Clean();			//동적으로 비우는 기능
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Delete_Object(class CGameObject* pGameObject);
	_int Get_LayerSize() { return (_int)m_GameObjects.size();}


	string Layerwstring_to_string(const wstring& wstr) {
		string str(wstr.begin(), wstr.end());
		return str;
	}


private:
	list<class CGameObject* >		m_GameObjects;
	
public:
	static CLayer* Create();
	virtual void Free() override;
};

END