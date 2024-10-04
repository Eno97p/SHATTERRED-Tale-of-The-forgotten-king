#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CBehaviorTree;
class CModel;
END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(Client)

class CPlayer final : public CLandObject
{
public:



#define	CLONEDELAY 0.15f
#define BUTTONCOOLTIME 0.5f
#define JUMPCOOLTIME 0.3f
#define WALKSPEED 5.f // 4.
#define RUNSPEED 8.f
#define ROLLSPEED 12.f
#define ATTACKPOSTDELAY 1.5f
#define STAMINARECOVERDELAY 1.5f
#define PARRYSTART 0.01f
#define PARRYEND 0.3f
#define JUMPSPEED 13.f
#define BRISDELAY 0.05f

public:
	typedef struct Player_Status_Data // 다음 Level로 넘어갈 때 저장되어야 하는 Player 스탯 데이터
	{
		_bool		isReviveFadeing;
		_uint		iLevel;
		_uint		iVitalityLv;
		_uint		iStaminaLv;
		_uint		iStrenghtLv;
		_uint		iMysticismLv;
		_uint		iKnowledgeLv;
		_uint		iPhysicalDmg;
		_uint		iEtherDmg  ;

		_float		fCurHP;
		_float      fCurMP;

		Player_Status_Data() = default;
		
	}PLAYER_STATUS_DATA;

public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };
	enum STATE {
		STATE_IDLE, STATE_FIGHTIDLE, STATE_WALK, STATE_LOCKON_STRAIGHT, STATE_LOCKON_BACKWARD, STATE_LOCKON_LEFT, STATE_LOCKON_RIGHT, STATE_RUN, STATE_JUMPSTART, STATE_DOUBLEJUMPSTART, STATE_JUMP, STATE_LAND,
		STATE_PARRY, STATE_JUMPATTACK, STATE_JUMPATTACK_LAND, STATE_ROLLATTACK, STATE_SPECIALATTACK, STATE_SPECIALATTACK2, STATE_SPECIALATTACK3, STATE_SPECIALATTACK4, 
		STATE_LCHARGEATTACK, STATE_RCHARGEATTACK, STATE_BACKATTACK,	STATE_LATTACK1, STATE_LATTACK2, STATE_LATTACK3, STATE_RATTACK1, STATE_RATTACK2, 
		STATE_RUNLATTACK1, STATE_RUNLATTACK2, STATE_RUNRATTACK, STATE_COUNTER, STATE_ROLL, STATE_HIT, STATE_SLOWHIT, 
		STATE_DASH, STATE_DASH_FRONT, STATE_DASH_BACK, STATE_DASH_LEFT, STATE_DASH_RIGHT, STATE_USEITEM, STATE_BUFF, STATE_DEAD, STATE_REVIVE, STATE_RIDE, STATE_SLIDE, STATE_KNOCKBACK, STATE_END
	};
	enum WEAPON { WEAPON_DURGASWORD, WEAPON_PRETORIANSWORD, WEAPON_RADAMANTHESWORD, WEAPON_ELISH, WEAPON_END };

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
	_uint Get_State() { return m_iState; }
	CGameObject* Get_Weapon();
	void PlayerHit(_float iValue, _bool bSlowHit = false);
	_float Get_HpRatio() { return m_fCurHp / m_fMaxHp; }
	_float Get_StaminaRatio() { return m_fCurStamina / m_fMaxStamina; }
	_float Get_MpRatio() { return m_fCurMp / m_fMaxMp; }
	_bool Get_Parry() { return m_bParrying; }
	_bool Get_m_bParry() { return m_bParry; }
	void Parry_Succeed();
	void Set_ParriedMonsterFloat4x4(const _float4x4* pMatrix) { m_pParriedMonsterFloat4x4 = pMatrix; }
	_bool Get_Cloaking() { return m_bIsCloaking; }
	void Set_Cloaking(_bool bCloaking) { m_bIsCloaking = bCloaking; }
	void Set_Position(_vector vPosition);

	void			Add_Hp(_float iValue);

	// UI에 출력하기 위한 함수
	_float			Get_MaxHP() { return m_fMaxHp; }
	_float			Get_MaxStamina() { return m_fMaxStamina; }
	_float			Get_MaxMP() { return m_fMaxMp; }
	_uint			Get_Level() { return m_iLevel; }
	void			Set_Level(_int iAdd) { m_iLevel += iAdd; }

	_uint			Get_VitalityLv() { return m_iVitalityLv; }
	void			Set_VitalityLv(_int iValue) { m_iVitalityLv += iValue; }
	_uint			Get_StaminaLv() { return m_iStaminaLv; }
	void			Set_StaminaLv(_int iValue) { m_iStaminaLv += iValue; }
	_uint			Get_StrenghtLv() { return m_iStrenghtLv; }
	void			Set_StrengthLv(_int iValue) { m_iStrenghtLv += iValue; }
	_uint			Get_MysticismLv() { return m_iMysticismLv; }
	void			Set_MysticismLv(_int iValue) { m_iMysticismLv += iValue; }
	_uint			Get_KnowledgeLv() { return m_iKnowledgeLv; }
	void			Set_KnowledgeLv(_int iValue) { m_iKnowledgeLv += iValue; }
	_uint			Get_PhysicalDmg() { return m_iPhysicalDmg; }
	void			Set_PhysicalDmg(_int iValue) { m_iPhysicalDmg += iValue; }
	_uint			Get_EtherDmg() { return m_iEtherDmg; }
	void			Set_EtherDmg(_int iValue) { m_iEtherDmg += iValue; }
	void			Set_Cloaking(){
		m_bIsCloaking = true;
		Add_Mp(-10.f);
	}
	void			Set_isBuffState(_bool isBuffState) { m_isBuffState = isBuffState; }

	Player_Status_Data Get_PlayerStatusData();

	void			Pull_Status(); // HP / Stamina / MP 를 Pull
	void			Update_LvData();
	void			Update_Weapon(wstring wstrTextureName);
	void			KnockBack(_vector vDir, _float fTimeDelta);

	// Buff 관련 함수
	void			Set_Shield(_bool isOn) { if (isOn) m_fShield = 0.4f; else m_fShield = 1.f; }
	void			Set_HPBuff(_bool isOn) { if (isOn) m_fHPBuff = 1.5f; else m_fHPBuff = 1.f; }
	void			Set_StaminaBuff(_bool isOn) { if (isOn) m_fStaminaBuff = 0.5f; else m_fStaminaBuff = 1.f; }
	void			Set_BuffNum(_uint iBuffNum) { m_iBuffNum = iBuffNum; }

	void			Generate_HoverBoard();
	void			OnWater(_bool bOnWater) { m_bOnWater = bOnWater; }

private:
	HRESULT Add_Nodes();

private:
	NodeStates Revive(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);
	NodeStates Knockback(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Counter(_float fTimeDelta);
	void Move_Counter();
	NodeStates Parry(_float fTimeDelta);
	NodeStates JumpAttack(_float fTimeDelta);
	NodeStates RollAttack(_float fTimeDelta);
	NodeStates SpecialAttack(_float fTimeDelta);
	NodeStates Special1(_float fTimeDelta);
	NodeStates Special2(_float fTimeDelta);
	NodeStates Special3(_float fTimeDelta);
	NodeStates Special4(_float fTimeDelta);
	NodeStates LChargeAttack(_float fTimeDelta);
	NodeStates RChargeAttack(_float fTimeDelta);
	NodeStates LAttack(_float fTimeDelta);
	_bool CanBackAttack();
	NodeStates RAttack(_float fTimeDelta);
	// void Generate_HoverBoard();
	NodeStates Slide(_float fTimeDelta);
	NodeStates Dash(_float fTimeDelta);
	NodeStates Jump(_float fTimeDelta);
	NodeStates Roll(_float fTimeDelta);
	NodeStates UseItem(_float fTimeDelta);
	NodeStates Buff(_float fTimeDelta);
	NodeStates Move(_float fTimeDelta);
	NodeStates Idle(_float fTimeDelta);

	void Add_Stamina(_float iValue);
	void Add_Mp(_float iValue);
	_uint* Get_CurWeapon() { return &m_iCurWeapon; }
	void Change_Weapon();

private:
	vector<class CGameObject*>					m_PartObjects;
	_uint										m_iState = { 0 };
	_uint										m_iCurWeapon = { WEAPON_RADAMANTHESWORD };
	class CPhysXComponent_Character* m_pPhysXCom = { nullptr };
	CBehaviorTree* m_pBehaviorCom = { nullptr };
	_float										m_fButtonCooltime = 0.f;
	_float										m_fJumpCooltime = 0.f;

#pragma region 상태제어 bool변수
	_bool										m_bJumping = false;
	_bool										m_bFalling = false;
	_bool										m_bDoubleJumping = false;
	_bool										m_bLAttacking = false;
	_bool										m_bRAttacking = false;
	_bool										m_bRunning = false;
	// 패링중
	_bool										m_bParrying = false;
	// 패링 성공
	_bool										m_bParry = false;
	_bool										m_bIsLanded = false;
	_bool										m_bIsRunAttack = false;
	_bool										m_bDisolved_Weapon = false;
	_bool										m_bDisolved_Yaak = false;
	_bool										m_bStaminaCanDecrease = true;
	_bool										m_bIsCloaking = false;
	_bool										m_bRiding = false;
	_bool										m_bRided = false;
	_bool										m_bChase = true;
	_bool										m_bOnWater = false;
#pragma endregion 상태제어 bool변수

	_float										m_fFightIdle = 0.f;
	_float										m_fSpecialAttack = 0.f;
	_float										m_fLChargeAttack = 0.f;
	_float										m_fRChargeAttack = 0.f;
	_bool										m_bAnimFinished = false;
	_float										m_fAnimDelay = 0.f;
	_float										m_fCloneDelay = 0.f;
	_uint										m_iAttackCount = 1;
	_bool										m_bCanCombo = false;
	_float										m_fSlowDelay = 0.f;
	_float										m_fParryFrame = 0.f;
	_float										m_fJumpAttackdelay = 0.7f;
	_float										m_fStaminaRecoverDelay = STAMINARECOVERDELAY;
	_float m_fBRIS = 0.f;
	_bool m_bSound = false;
	const _float4x4* m_pParriedMonsterFloat4x4 = { nullptr }; 
	CTransform* m_pCameraTransform = { nullptr };
	class CHoverboard* m_pHoverBoard = nullptr;
	CTransform* m_pHoverBoardTransform = nullptr;

#pragma region 플레이어 스탯

#ifdef _DEBUG
	//_float m_fMaxHp = 100.f;
	_float m_fMaxHp = 10000.f;
#else
	//_float m_fMaxHp = 300.f;
	_float m_fMaxHp = 300.f;
#endif // _DEBUG

	_float m_fCurHp = m_fMaxHp;

#ifdef _DEBUG
	_float m_fMaxStamina = 300.f;
#else
	_float m_fMaxStamina = 300.f;
#endif // _DEBUG
	_float m_fCurStamina = m_fMaxStamina;

	_float m_fMaxMp = 300.f;
	_float m_fCurMp = m_fMaxMp;

	_float m_iMoney = 0;
#pragma endregion 플레이어 스탯

	_float m_fParticleAcctime = 0.f;

	//초기 위치
	_float3 m_InitialPosition = { 0.f, 0.f, 0.f };

#pragma region UI관련 Data
	_bool		m_isReviveFadeing = { false };
	_uint		m_iLevel = { 1 };
	_uint		m_iVitalityLv = { 0 };
	_uint		m_iStaminaLv = { 0 };
	_uint		m_iStrenghtLv = { 0 };
	_uint		m_iMysticismLv = { 0 };
	_uint		m_iKnowledgeLv = { 0 };
	_uint		m_iPhysicalDmg = { 80 };
	_uint		m_iEtherDmg = { 60 };
#pragma endregion UI관련 Data


#pragma region Buff 관련 Data
	_bool		m_isBuffState = { false };
	_uint		m_iBuffNum = { 0 };
	_float		m_fShield = { 1.f }; // 쉴드 값
	_float		m_fHPBuff = { 1.f }; // HP 회복 값
	_float		m_fStaminaBuff = { 1.f }; // 스태미나 감소 값
#pragma endregion Buff 관련 Data



#pragma region 비동기 로드 Data
	_bool m_bIsLoadStart = false;
	_vector m_vDest = {};
	LEVEL m_eCurLevel = LEVEL_END;
	Player_Status_Data m_tPlayerStatusData = {};

#pragma endregion 비동기 로드 Data

	//벽력일섬 셰이킹
	_bool m_bSpecialAttackShake = false;
	_bool m_bSpecialAttackZoom = false;

	_float		m_GrassBlowInterval = 1.f;


private:
	void OnShapeHit(const PxControllerShapeHit& hit);
	void OnControllerHit(const PxControllersHit& hit);
	bool OnFilterCallback(const PxController& Caller, const PxController& Ohter);


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END