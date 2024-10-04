#pragma once
#include "Client_Defines.h"



BEGIN(Client)

template<typename... T>
class CInitLoader 
{

public:
	CInitLoader(CInitLoader** ppSelf)
	{
		*ppSelf = this;
		s_m_vecInitLoader.push_back(this);
		if (s_m_vecInitLoader.size() == 1)		//첫 번째 생성할 때만 등록 
		{
			std::atexit(&CInitLoader::CheckDestroy);
		}
	}

	virtual ~CInitLoader() = default;
	//{
	//	auto iter = find(s_m_vecInitLoader.begin(), s_m_vecInitLoader.end(), this);
	//	if(iter != s_m_vecInitLoader.end())
	//	{
	//		s_m_vecInitLoader.erase(iter);
	//	}
	//}


private:
	static void CheckDestroy()
	{
		for(auto& pLoader : s_m_vecInitLoader)
		{
			if (pLoader != nullptr)
			{
				delete pLoader;
				pLoader = nullptr;
			}
			
		}
		s_m_vecInitLoader.clear();
	}

public:
	virtual void Save_Start(T... Args);
	virtual void Load_Start(T... Args);

	virtual void Save_TriggerStart(T... Args);
	virtual void Load_TriggerStart(T... Args);

	virtual void Save_BlastWallStart(T... Args);
	virtual void Load_BlastWallStart(T... Args);

	
private:

	static vector<CInitLoader<T ...>*> s_m_vecInitLoader;
private:

	
	vector<_tagMonsterInit_Property> m_pvecMonsterInitProperty;
	vector<_tagTriggrrInit_Property> m_pvecTriggerInitProperty;
	vector<_tagBlastWallInit_Property> m_pvecBlastWallInitProperty;
};

END


template<typename... T>
vector<CInitLoader<T...>*> CInitLoader<T...>::s_m_vecInitLoader;



template<typename ...T>
inline void CInitLoader<T...>::Save_Start(T ...Args)
{
	auto pGameInstance = CGameInstance::GetInstance();
	auto eLevel = get<0>(tuple<T...>(Args ...));
	auto pLayer = get<1>(tuple<T...>(Args ...));
	
	auto future = pGameInstance->AddWork([this, pGameInstance, eLevel, pLayer]() {

		list<CGameObject*> pList = pGameInstance->Get_GameObjects_Ref(eLevel, pLayer);
		_uint iLayerSize = pList.size();
		m_pvecMonsterInitProperty.clear();
		m_pvecMonsterInitProperty.resize(iLayerSize);
		_uint i = 0;
		for(auto& pObject : pList)
		{
			m_pvecMonsterInitProperty[i].vPos = pObject->Get_InitPos();
			m_pvecMonsterInitProperty[i].strMonsterTag = pObject->Get_ProtoTypeTag();
			m_pvecMonsterInitProperty[i].pArgDesc = pObject->Get_Desc();
			m_pvecMonsterInitProperty[i].uArgDescSize = pObject->Get_DescSize();
			++i;
		}
		wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
		wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".bin";
		Engine::Save_Data(wstrFilePath, false, m_pvecMonsterInitProperty.size(), m_pvecMonsterInitProperty.data());

	});

}

template<typename ...T>
inline void CInitLoader<T...>::Save_TriggerStart(T ...Args)
{

	auto pGameInstance = CGameInstance::GetInstance();
	auto eLevel = get<0>(tuple<T...>(Args ...));
	auto pLayer = get<1>(tuple<T...>(Args ...));
	auto future = pGameInstance->AddWork([this, pGameInstance, eLevel, pLayer]() {

		list<CGameObject*> pList = pGameInstance->Get_GameObjects_Ref(eLevel, pLayer);
		_uint iLayerSize = pList.size();
		m_pvecTriggerInitProperty.clear();
		m_pvecTriggerInitProperty.resize(iLayerSize);
		_uint i = 0;
		for (auto& pObject : pList)
		{
			m_pvecTriggerInitProperty[i].strMonsterTag = pObject->Get_ProtoTypeTag();
			m_pvecTriggerInitProperty[i].iTriggerType = dynamic_cast<CEventTrigger*>(pObject)->Get_TriggerType();
			m_pvecTriggerInitProperty[i].mWorldMatrix = pObject->Get_worldMat();

			
			++i;
		}
		wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
		wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".bin";
		Engine::Save_Data(wstrFilePath, false, m_pvecTriggerInitProperty.size(), m_pvecTriggerInitProperty.data());

	});



}

template<typename ...T>
inline void CInitLoader<T...>::Load_TriggerStart(T ...Args)
{
	 auto pGameInstance = CGameInstance::GetInstance();
	 auto eLevel = get<0>(std::forward_as_tuple(Args ...));
	 auto pLayer = get<1>(std::forward_as_tuple(Args ...));

	wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
	wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".dat";


	decltype(auto) pLoadData = Engine::Load_Data<size_t, _tagTriggrrInit_Property*>(wstrFilePath);
	if (pLoadData)
	{
		size_t iVecSize = get<0>(*pLoadData);
		_tagTriggrrInit_Property* pProperty = get<1>(*pLoadData);
		vector<_tagTriggrrInit_Property> vecProperty(pProperty, pProperty + iVecSize);
		for (_uint i = 0; i < iVecSize; ++i)
		{
			CMap_Element::MAP_ELEMENT_DESC tDesc;
			tDesc.mWorldMatrix = vecProperty[i].mWorldMatrix;
			tDesc.TriggerType = vecProperty[i].iTriggerType;

			pGameInstance->CreateObject(eLevel, pLayer, vecProperty[i].strMonsterTag, &tDesc);


		}



	}
}

template<typename ...T>
inline void CInitLoader<T...>::Save_BlastWallStart(T ...Args)
{

	auto pGameInstance = CGameInstance::GetInstance();
	auto eLevel = get<0>(tuple<T...>(Args ...));
	auto pLayer = get<1>(tuple<T...>(Args ...));
	auto future = pGameInstance->AddWork([this, pGameInstance, eLevel, pLayer]() {

		list<CGameObject*> pList = pGameInstance->Get_GameObjects_Ref(eLevel, pLayer);
		_uint iLayerSize = pList.size();
		m_pvecBlastWallInitProperty.clear();
		m_pvecBlastWallInitProperty.resize(iLayerSize);
		_uint i = 0;
		for (auto& pObject : pList)
		{
			m_pvecBlastWallInitProperty[i].strMonsterTag = pObject->Get_ProtoTypeTag();
			m_pvecBlastWallInitProperty[i].mWorldMatrix = pObject->Get_worldMat();
			++i;
		}

		wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
		wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".bin";
		Engine::Save_Data(wstrFilePath, false, m_pvecBlastWallInitProperty.size(), m_pvecBlastWallInitProperty.data());

	});

}

template<typename ...T>
inline void CInitLoader<T...>::Load_BlastWallStart(T ...Args)
{

	auto pGameInstance = CGameInstance::GetInstance();
	auto eLevel = get<0>(std::forward_as_tuple(Args ...));
	auto pLayer = get<1>(std::forward_as_tuple(Args ...));



	 wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
	 wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".bin";


	decltype(auto) pLoadData = Engine::Load_Data<size_t, _tagBlastWallInit_Property*>(wstrFilePath);
	if (pLoadData)
	{
		size_t iVecSize = get<0>(*pLoadData);
		_tagBlastWallInit_Property* pProperty = get<1>(*pLoadData);
		vector<_tagBlastWallInit_Property> vecProperty(pProperty, pProperty + iVecSize);
		for (_uint i = 0; i < iVecSize; ++i)
		{
			CBlastWall::CBlastWall_DESC tDesc;
			tDesc.fWorldMatrix = vecProperty[i].mWorldMatrix;
			pGameInstance->CreateObject(eLevel, pLayer, vecProperty[i].strMonsterTag, &tDesc);
		}
	}


}



template<typename ...T>
inline void CInitLoader<T...>::Load_Start(T ...Args)
{
	auto pGameInstance = CGameInstance::GetInstance();
	auto eLevel = get<0>(std::forward_as_tuple(Args ...));
	auto pLayer = get<1>(std::forward_as_tuple(Args ...));
	//auto Desc	= get<2>(tuple<T...>(Args ...));

	
	 wstring wstrLevelName = Client::Get_CurLevelName(eLevel);
	 wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrLevelName + L"_" + pLayer + L".bin";


	decltype(auto) pLoadData = Engine::Load_Data<size_t , _tagMonsterInit_Property*>(wstrFilePath);
	if (pLoadData)
	{

		size_t iVecSize = get<0>(*pLoadData);
		_tagMonsterInit_Property* pProperty = get<1>(*pLoadData);
		vector<_tagMonsterInit_Property> vecProperty(pProperty, pProperty + iVecSize);
		for(_uint i = 0 ; i< iVecSize; ++i)
		{
			CLandObject::LANDOBJ_DESC tDesc;
			tDesc.mWorldMatrix._41 = vecProperty[i].vPos.x;
			tDesc.mWorldMatrix._42 = vecProperty[i].vPos.y;
			tDesc.mWorldMatrix._43 = vecProperty[i].vPos.z;
			tDesc.mWorldMatrix._44 = 1.f;
			vecProperty[i].pArgDesc;
			
			pGameInstance->CreateObject(eLevel,pLayer, vecProperty[i].strMonsterTag, &tDesc);
			
			 
		}



	}

	


}
