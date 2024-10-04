#pragma once

#include "Monster.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CBehaviorTree;
END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(Client)

class CMalkhel final : public CMonster
{
#define MALKHELSPEED 15.f
#define SPAWNCOOLTIME 0.05f
#define METEORCOOLTIME 0.25f
#define EXPLODECOOLTIME 0.9f
#define TRIPPLEATTACK 1.2f
#define UPSPEED 5.f
#define DOWNSPEED 20.f
#define SPEARATTACK 1.7f

public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };
	enum STATE {
		STATE_IDLE, STATE_DASHLEFT, STATE_DASHRIGHT, STATE_DASHBACK, STATE_DASHFRONT, STATE_TELEPORT, STATE_ATTACK1, STATE_ATTACK2, STATE_ATTACK3, STATE_ATTACK4,
		STATE_ATTACK5, STATE_ATTACK6, STATE_ATTACK7, STATE_DEAD, STATE_END
	};

private:
	CMalkhel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMalkhel(const CMalkhel& rhs);
	virtual ~CMalkhel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Chase_Player(_float fTimeDelta);

public:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	_bool Intersect(PART ePartObjID, const wstring& strComponetTag, CCollider* pTargetCollider);

private:
	HRESULT Add_Nodes();

private:
	NodeStates Dead(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Teleport(_float fTimeDelta);
	NodeStates Move(_float fTimeDelta);
	NodeStates Chase(_float fTimeDelta);
	NodeStates Attack1(_float fTimeDelta);
	NodeStates Attack2(_float fTimeDelta);
	NodeStates Attack3(_float fTimeDelta);
	NodeStates Attack4(_float fTimeDelta);
	NodeStates Attack5(_float fTimeDelta);
	NodeStates Attack6(_float fTimeDelta);
	NodeStates Attack7(_float fTimeDelta);
	NodeStates Select_Pattern(_float fTimeDelta);
	NodeStates Idle(_float fTimeDelta);
	void Add_Hp(_int iValue);

public:
	void Activate_Malkhel() { m_bTrigger = true; }
private:
	vector<class CGameObject*>					m_PartObjects;

#pragma region 상태제어 bool변수
	_bool										m_bReviving = false;
	_bool										m_bChasing = true;
	_bool										m_bHit = false;
	_bool m_bSprint = false;
	_bool	m_bTrigger = false;
	_bool m_bDashBack = false;
	_bool m_bTeleport = false;
#pragma endregion 상태제어 bool변수

	_float										m_fDashBackDelay = 1.f;
	_float										m_fChasingDelay = 0.5f;
	_uint										m_iAttackCount = 0;
	_bool										m_bCanCombo = false;
	_float										m_fMoveTime = 2.f;
	_float										m_fTurnDelay = 0.5f;
	_float m_fSpawnCoolTime = SPAWNCOOLTIME;
	_float m_fSpawnDelay = 2.f;
	_float m_fTrippleAttack = TRIPPLEATTACK + 1.f;
	_float m_fSpearAttack = SPEARATTACK;
	_uint m_iTrippleAttackCount = 3;

public:
	static CMalkhel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END