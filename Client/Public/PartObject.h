#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTransform;
class CCollider;
END

/* 파츠들(무기, 바디)의 부모가 되기위한 클래스이다. */

BEGIN(Client)

class CPartObject abstract : public CBlendObject
{
public:
	enum DISOLVETYPE { TYPE_IDLE, TYPE_INCREASE, TYPE_DECREASE, TYPE_END };

	typedef struct PARTOBJ_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
		const _uint* pState;
		const _uint* pCurWeapon;
		_bool* pCanCombo;
		LEVEL					eLevel;
		_float* pCurHp;
		_float* pMaxHp;
	}PARTOBJ_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_DisolveType(_uint iDisolveType) { m_eDisolveType = (DISOLVETYPE)iDisolveType; }
	virtual CCollider* Get_Collider() { return m_pColliderCom; }
	_float3 Get_Collider_Center() {
		return m_pColliderCom->Get_Center();
 }
	const _float4x4* Get_Part_Mat() { return &m_WorldMatrix; }
	virtual _float Get_LengthFromCamera() override;

protected:
	CCollider* m_pColliderCom = { nullptr };
	_float4x4				m_WorldMatrix;
	const _float4x4* m_pParentMatrix = { nullptr };
	const _uint* m_pState = { nullptr };
	const _uint* m_pCurWeapon = { nullptr };
	_bool* m_pCanCombo = { nullptr };
	_float m_fDisolveValue = 1.f;
	CTexture* m_pDisolveTextureCom = nullptr;
	CCollider::COLLTYPE m_eColltype = CCollider::COLL_END;

	DISOLVETYPE m_eDisolveType = TYPE_IDLE;
	LEVEL					m_eLevel = { LEVEL_END };


public:

	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END