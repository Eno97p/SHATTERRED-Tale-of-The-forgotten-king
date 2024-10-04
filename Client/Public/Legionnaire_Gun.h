#pragma once

#include "Monster.h"

BEGIN(Client)

class CLegionnaire_Gun final : public CMonster
{
#define DETECTRANGE 30.f
#define MELEERANGE 5.f
#define ATTACKRANGE 2.0f

public:
	enum STATE { STATE_IDLE = 0, STATE_IDLE_GUN, STATE_IDLE_MELEE, STATE_WALK, STATE_RUN, STATE_BACK, STATE_LEFT, STATE_RIGHT, 
		STATE_DEAD, STATE_HIT, STATE_PARRIED, STATE_WAKEUP, STATE_KNOCKDOWN,
		STATE_GUNATTACK, STATE_MELEEATTACK1, STATE_MELEEATTACK2, STATE_END };
	enum MODE { MODE_IDLE, MODE_GUN, MODE_MELEE, MODE_END };

private:
	CLegionnaire_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLegionnaire_Gun(const CLegionnaire_Gun& rhs);
	virtual ~CLegionnaire_Gun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Idle();
	void Set_Weapon();
	void Add_Hp(_int iValue);

private:
	_bool m_bSound = false;
	_uint m_eMode = MODE_IDLE;
	_bool							m_isHit = { false };
	_bool							m_isAttackDistance = { false };

	_float							m_fCastingTimer = { 0.f };
	_float										m_fMoveTime = 2.f;
	_bool										m_bCanCombo = false;
	// 0 : body, 1 : Gun, 2 : sword, 3 : arrow
	vector<class CGameObject*>		m_PartObjects;
	_float m_fBackStepTime = 1.1f;

	_bool										m_bParryFirstHit = true;

private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();

	virtual HRESULT		Add_Nodes() override;

	void				Check_AnimFinished();

private:
	NodeStates			Dead(_float fTimeDelta);
	NodeStates			Hit(_float fTimeDelta);
	NodeStates			Parried(_float fTimeDelta);
	
	NodeStates			GunAttack(_float fTimeDelta);
	NodeStates			MeleeAttack(_float fTimeDelta);

	NodeStates			Detect(_float fTimeDelta);
	NodeStates			Move(_float fTimeDelta);
	NodeStates			Idle(_float fTimeDelta);

public:
	static CLegionnaire_Gun*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END