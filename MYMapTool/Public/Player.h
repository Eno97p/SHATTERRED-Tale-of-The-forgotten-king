#pragma once
#include "Engine_Defines.h"
#include "MYMapTool_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CBehaviorTree;
class CModel;
END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(MYMapTool)

class CPlayer final : public CLandObject
{
#define	CLONEDELAY 0.15f
#define BUTTONCOOLTIME 0.5f
#define JUMPCOOLTIME 0.3f
#define WALKSPEED 3.f
#define RUNSPEED 20.f
#define ROLLSPEED 30.f
#define ATTACKPOSTDELAY 1.5f

public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };
	enum STATE {
		STATE_IDLE, STATE_FIGHTIDLE, STATE_WALK, STATE_RUN, STATE_JUMPSTART, STATE_DOUBLEJUMPSTART, STATE_JUMP, STATE_LAND, STATE_PARRY, STATE_JUMPATTACK, STATE_JUMPATTACK_LAND, STATE_ROLLATTACK, STATE_LCHARGEATTACK, STATE_RCHARGEATTACK, STATE_BACKATTACK,
		STATE_LATTACK1, STATE_LATTACK2, STATE_LATTACK3, STATE_RATTACK1, STATE_RATTACK2, STATE_RUNLATTACK1, STATE_RUNLATTACK2, STATE_RUNRATTACK, STATE_COUNTER, STATE_ROLL, STATE_HIT, STATE_DASH, STATE_DEAD, STATE_REVIVE, STATE_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	CGameObject* Get_Weapon();

private:
	HRESULT Add_Nodes();

private:
	NodeStates Revive(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Parry(_float fTimeDelta);
	NodeStates JumpAttack(_float fTimeDelta);
	NodeStates RollAttack(_float fTimeDelta);
	NodeStates LChargeAttack(_float fTimeDelta);
	NodeStates RChargeAttack(_float fTimeDelta);
	NodeStates LAttack(_float fTimeDelta);
	NodeStates RAttack(_float fTimeDelta);
	NodeStates Dash(_float fTimeDelta);
	NodeStates Jump(_float fTimeDelta);
	NodeStates Roll(_float fTimeDelta);
	NodeStates Move(_float fTimeDelta);
	NodeStates Idle(_float fTimeDelta);
	//void Add_Hp(_int iValue);
	//void Add_Stamina(_int iValue);
	//void Add_Mp(_int iValue);

private:
	vector<class CGameObject*>					m_PartObjects;
	_uint										m_iState = { 0 };
	class CPhysXComponent_Character* m_pPhysXCom = { nullptr };
	CBehaviorTree* m_pBehaviorCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_float										m_fButtonCooltime = 0.f;
	_float										m_fJumpCooltime = 0.f;

#pragma region 상태제어 bool변수
	_bool										m_bJumping = false;
	_bool										m_bDoubleJumping = false;
	_bool										m_bLAttacking = false;
	_bool										m_bRAttacking = false;
	_bool										m_bRunning = false;
	_bool										m_bReviving = false;
	_bool										m_bParrying = false;
	_bool										m_bIsLanded = false;
	_bool										m_bIsRunAttack = false;
	_bool										m_bDisolved_Weapon = false;
	_bool										m_bDisolved_Yaak = false;
	_bool										m_bHit = false;
#pragma endregion 상태제어 bool변수

	_float										m_fFightIdle = 0.f;
	_float										m_fLChargeAttack = 0.f;
	_float										m_fRChargeAttack = 0.f;
	_bool										m_bAnimFinished = false;
	_float										m_fAnimDelay = 0.f;
	_float										m_fCloneDelay = 0.f;
	_uint										m_iAttackCount = 1;
	_bool										m_bCanCombo = false;

#pragma region 플레이어 스탯
	_uint m_iMaxHp = 100;
	_uint m_iCurHp = m_iMaxHp;

	_uint m_iMaxStamina = 100;
	_uint m_iCurStamina = m_iMaxStamina;

	_uint m_iMaxMp = 100;
	_uint m_iCurMp = m_iMaxMp;

	_uint m_iMoney = 0;
#pragma endregion 플레이어 스탯



private:
	void OnShapeHit(const PxControllerShapeHit& hit);


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END