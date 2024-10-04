#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CBehaviorTree;
END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(Client)

class CLegionnaire final : public CMonster
{
#define DETECTRANGE 30.f
#define JUMPATTACKRANGE 20.f
#define ATTACKRANGE 5.f

public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };
	enum STATE {
		STATE_IDLE, STATE_WALKLEFT, STATE_WALKRIGHT, STATE_WALKBACK, STATE_WALKFRONT, STATE_JUMPATTACK, STATE_ATTACK1, STATE_ATTACK2, STATE_ATTACK3, STATE_CIRCLEATTACK, STATE_HIT, STATE_DEAD, STATE_REVIVE, STATE_END
	};

private:
	CLegionnaire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLegionnaire(const CLegionnaire& rhs);
	virtual ~CLegionnaire() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Add_Hp(_int iValue) {};

public:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	_bool Intersect(PART ePartObjID, const wstring& strComponetTag, CCollider* pTargetCollider);

private:
	HRESULT Add_Nodes();

private:
	NodeStates Revive(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates JumpAttack(_float fTimeDelta);
	NodeStates Attack(_float fTimeDelta);
	NodeStates CircleAttack(_float fTimeDelta);
	NodeStates Detect(_float fTimeDelta);
	NodeStates Move(_float fTimeDelta);
	NodeStates Idle(_float fTimeDelta);
	void Get_Hp(_float iValue);

private:
	vector<class CGameObject*>					m_PartObjects;
	_uint										m_iState = { 0 };
	CBehaviorTree* m_pBehaviorCom = { nullptr };

#pragma region 상태제어 bool변수
	_bool										m_bDying = false;
	_bool										m_bReviving = false;
	_bool										m_bChasing = true;
#pragma endregion 상태제어 bool변수

	_float										m_fChasingDelay = 0.5f;
	_bool										m_bAnimFinished = false;
	_float										m_fLengthFromPlayer = 0.f;
	_uint										m_iAttackCount = 0;
	_bool										m_bCanCombo = false;
	_float										m_fMoveTime = 2.f;

#pragma region Legionnaire 스탯
	_uint m_iMaxHp = 100;
	_uint m_iCurHp = m_iMaxHp;
#pragma endregion Legionnaire 스탯




public:
	static CLegionnaire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END