#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXComponent_Vehicle;
END


BEGIN(Client)

class CHoverboard final : public CGameObject
{
public:
	enum DISOLVETYPE { TYPE_IDLE, TYPE_INCREASE, TYPE_DECREASE, TYPE_END };
	typedef	struct HoverboardInfo : public GAMEOBJECT_DESC
	{
		_float3 vRight =	{ 1.0f,0.0f,0.0f };
		_float3 vUp =		{ 0.0f,1.0f,0.0f };
		_float3 vLook =		{ 0.0f,0.0f,1.0f };
		_float3 vPosition = { 0.0f,0.0f,0.0f };
	}HoverboardInfo;
private:
	CHoverboard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHoverboard(const CHoverboard& rhs);
	virtual ~CHoverboard() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	void On_Ride() { m_bOnRide = true; }
	void Set_DisolveType(_uint iDisolveType) { m_eDisolveType = (DISOLVETYPE)iDisolveType; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Add_PxActor();



	//void SideMove(float Di)
private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CPhysXComponent_Vehicle* m_pPhysXCom = { nullptr };
private:
	
	_float3 m_fPosition = { 0.f,0.f,0.f };
	_bool m_bIsMoving = false;
	_bool m_bOnRide = false;
	_bool m_bIsBoost = false;
	_float m_fMoveThreshold = 0.1f;
	_float m_fDisolveValue = 1.f;
	CTexture* m_pDisolveTextureCom = nullptr;
	CCollider::COLLTYPE m_eColltype = CCollider::COLL_END;
	CTransform* m_pCameraTransform = { nullptr };

	DISOLVETYPE m_eDisolveType = TYPE_IDLE;
	LEVEL					m_eLevel = { LEVEL_END };
	_bool m_bShift = false;

	_matrix m_matWorld = {};

	_float m_fCurHoverBoardSpeed = 0.f;

	_bool m_bDead = false;
private:
	CGameObject* HoverTrail = nullptr;
public:
	static CHoverboard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
