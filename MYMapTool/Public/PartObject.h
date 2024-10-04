#pragma once

#include "MYMapTool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CCollider;
END

/* 파츠들(무기, 바디)의 부모가 되기위한 클래스이다. */

BEGIN(MYMapTool)

class CPartObject abstract : public CGameObject
{
public:
	enum DISOLVETYPE { TYPE_IDLE, TYPE_INCREASE, TYPE_DECREASE, TYPE_END };

	typedef struct PARTOBJ_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
		const _uint* pState;
		_bool* pCanCombo;
		LEVEL					eLevel;
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
	CCollider* Get_Collider() { return m_pColliderCom; }

protected:
	CCollider* m_pColliderCom = { nullptr };
	_float4x4				m_WorldMatrix;
	const _float4x4* m_pParentMatrix = { nullptr };
	const _uint* m_pState = { nullptr };
	_bool* m_pCanCombo = { nullptr };
	_float m_fDisolveValue = 1.f;

	DISOLVETYPE m_eDisolveType = TYPE_IDLE;
	LEVEL					m_eLevel = { LEVEL_END };

public:

	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END