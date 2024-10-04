#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "BossStatue.h"

BEGIN(Client)

class CBoss_Juggulus final : public CMonster
{
#define CIRCLESPHERESPAWNTIME 4.5f
#define LIGHTNINGSPAWNTIME 10.f
#define FLAMESPAWNTIME 7.f
#define STATUECOUNT 3

public:
	enum STATE {
		STATE_IDLE_FIRST = 0, STATE_IDLE_SEC, STATE_NEXTPHASE, STATE_CREATE_HAMMER,
		STATE_FLAME_ATTACK, STATE_HAMMER_ATTACK, STATE_SPHERE_ATTACK, STATE_THUNDER_ATTACK, STATE_TORNADO_ATTACK, STATE_DEAD, STATE_GROGGY, STATE_END
	};
	enum PHASE { PHASE_ONE, PHASE_TWO, PHASE_END };

private:
	CBoss_Juggulus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Juggulus(const CBoss_Juggulus& rhs);
	virtual ~CBoss_Juggulus() = default;

public:
#pragma region 상태제어 bool변수
	_bool										m_bReviving = false;
	_bool										m_bChasing = true;
	_bool										m_bHit = false;
	_bool	m_bTrigger = false;
#pragma endregion 상태제어 bool변수

	_float										m_fChasingDelay = 0.5f;
	_uint										m_iAttackCount = 0;
	_bool										m_bCanCombo = false;
	_float										m_fMoveTime = 2.f;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Add_Hp(_int iValue) override;
	void Add_Hands();
	void Juggulus_Activate();
private:
	_bool							m_isHammerCreate = { false };
	_bool							m_isAttackDone = { true };
	_bool							m_isPhaseChanged = { false };
	_bool							m_isHandOne_On = { false };
	_bool							m_isHandTwo_On = { false };
	_bool							m_isHandAnimFinished = { false };
	_bool							m_isHandTwoAnimFinished = { false };
	_bool							m_isHandThreeAnimFinished = { false };
	_bool							m_bDeadSound = false;
	CBossStatue* m_pBossStatues[STATUECOUNT] = { nullptr };
	_float m_fGroggyTime = 10.f;

	_float							m_fTargettingTimer = { 0.f };

	map<string, class CGameObject*>	m_PartObjects;

	PHASE							m_ePhase = { PHASE_END };

	_float m_fHammerCreationDelay = 4.f;
	_float m_fCircleSphereSpawnTime = CIRCLESPHERESPAWNTIME;
	_uint m_iCircleSphereCount = 0;
	_float m_fLightningSpawnTime = LIGHTNINGSPAWNTIME;
	_uint m_iLightningCount = 0;
	_float m_fFlameSpawnTime = FLAMESPAWNTIME;
	_uint m_iFlameCount = 0;

	//For Groggy State Camera Change
	_bool m_bGroggyCamChange = false;
private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();

	virtual HRESULT		Add_Nodes() override;

	HRESULT				Create_Hammer();
	void				Check_AnimFinished();
	void Spawn_CircleSphere(_float fTimeDelta);
	void Spawn_Lightning(_float fTimeDelta);
	void Spawn_Flame(_float fTimeDelta);

private:
	NodeStates			Dead(_float fTimedelta);
	NodeStates			NextPhase(_float fTimedelta);
	NodeStates			CreateHammer(_float fTimedelta);
	NodeStates			Groggy(_float fTimeDelta);
	NodeStates			Idle(_float fTimeDelta);

	NodeStates			Select_Pattern(_float fTimeDelta);
	NodeStates			HammerAttack(_float fTimeDelta);
	NodeStates			FlameAttack(_float fTimeDelta);
	NodeStates			SphereAttack(_float fTimeDelta);
	NodeStates			ThunderAttack(_float fTimeDelta);
	NodeStates			TornadoAttack(_float fTimeDelta);



public:
	static CBoss_Juggulus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

END