#pragma once

#include "Client_Defines.h"
#include "Base.h"


#ifdef _DEBUG

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CImGuiMgr final : public CBase
{
	DECLARE_SINGLETON(CImGuiMgr)

private:
	CImGuiMgr();
	virtual ~CImGuiMgr()=default;

public:
	HRESULT	Ready_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Render_ImGui();
	HRESULT Update_ImGui();
	HRESULT End_ImGui();


private:/*For. Property*/
	void Render_MainMenu();
	void Render_Layer();
	void Render_Object(list<CGameObject*>& listGameObject);
	void Render_Component(CGameObject* pGameObject);
	void Render_Component_Properties(CComponent* pComponent,const char* szComponentName);
private:/*For. Profile*/
	void Render_Profile();

private:/*For. Memory*/
	void Render_Memory();

private: /*For. Vehicle_Property*/
	void RigidBody_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle );
	void Tire_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle);
	void Wheel_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle);
	void Steering_Property(CPhysXComponent_Vehicle::PhysX_Vehicle_Editable_Desc* pVehicle);

	
private:/*For.Imgui Property*/
	char m_szFPS[MAX_PATH] = {};
	_bool m_bComponentPanel = false;


private: /*For.Engine Property*/
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	


private:
	vector<const char*> m_LayerTags;
	vector<string> m_ComponentNames;
	int m_iCurrentLevel = 0;
	int m_iCurrentLayer = 0;
	int m_iCurrentObject = 0;
	int m_iCurrentComponent = 0;
	int m_iCurrentComponentVehicleProperty = 0;
	CGameObject* m_pSelectedObject=nullptr;
	wstring m_wstrLayerTag;

	CLevel* m_pNewLevel = nullptr;
public:
	//static CImGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


	_bool m_bIsAllRender = true;
	static _bool FrameLimit;
};

END
#endif // _DEBUG