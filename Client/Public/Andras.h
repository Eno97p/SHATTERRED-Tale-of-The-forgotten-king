#pragma once

#include "Monster.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CBehaviorTree;
END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(Client)

class CAndras final : public CMonster
{
#define MOVESPEED 50.f
#define SPAWNCOOLTIME 0.05f

public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };
	enum STATE {
		STATE_IDLE, STATE_DASHLEFT, STATE_DASHRIGHT, STATE_DASHBACK, STATE_DASHFRONT, STATE_ATTACK1, STATE_ATTACK2, STATE_ATTACK3, STATE_ATTACK4, STATE_SPRINTATTACK,
		STATE_GROUNDATTACK, STATE_KICKATTACK, STATE_LASERATTACK, STATE_BABYLONATTACK, STATE_SHOOTINGSTARATTACK, STATE_DEAD, STATE_END
	};

private:
	CAndras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndras(const CAndras& rhs);
	virtual ~CAndras() = default;

public:
	_float			Get_ShieldRatio() { return m_fCurShield / m_fMaxShield; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Chase_Player(_float fTimeDelta);
	void KickStop();

public:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	
	_bool Intersect(PART ePartObjID, const wstring& strComponetTag, CCollider* pTargetCollider);
	void Phase_Two();

private:
	HRESULT Add_Nodes();

private:
	NodeStates Dead(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Attack(_float fTimeDelta);
	NodeStates SprintAttack(_float fTimeDelta);
	NodeStates GroundAttack(_float fTimeDelta);
	NodeStates KickAttack(_float fTimeDelta);
	NodeStates LaserAttack(_float fTimeDelta);
	NodeStates BabylonAttack(_float fTimeDelta);
	NodeStates ShootingStarAttack(_float fTimeDelta);
	NodeStates Select_Pattern(_float fTimeDelta);
	NodeStates Backstep(_float fTimeDelta);
	NodeStates Idle(_float fTimeDelta);
	void Add_Hp(_int iValue);

public:
	void Activate_Andras() { m_bTrigger = true; }

private:
	vector<class CGameObject*>					m_PartObjects;
	vector<class CGameObject*>					m_Particles;
#pragma region 상태제어 bool변수
	_bool										m_bReviving = false;
	_bool										m_bChasing = true;
	_bool										m_bHit = false;
	_bool m_bSprint = false;
	_bool m_bKick = false;
	_bool m_bSlash = false;
	_bool	m_bTrigger = false;
	_bool m_bDashBack = false;
	_bool m_bLaser = false;
	_bool m_bPhase2 = false;
#pragma endregion 상태제어 bool변수

	_float										m_fChasingDelay = 0.5f;
	_uint										m_iAttackCount = 0;
	_bool										m_bCanCombo = false;
	_float										m_fMoveTime = 2.f;
	_float										m_fTurnDelay = 0.5f;
	_float m_fSpawnCoolTime = SPAWNCOOLTIME;
	_float m_fKickSwordDelay = 0.5f;
	_float m_fKickAttackDelay = 2.f;
	_float m_fSpawnDelay = 2.f;
	_uint m_iZigzag = 0;
	_bool m_bRushSwordSound = true;

	_float m_fCutSceneWaitDelay = 10.f;
	_float m_fTriggerDelay = 0.0f;


#pragma region Shield 
	_float m_fMaxShield = 100.f;
	_float m_fCurShield = 0.f;
#pragma endregion Shield

private:
	CGameObject* HexaShieldText = nullptr;

public:
	static CAndras* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END