#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Player.h"

#include "UIGroup_BossHP.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CBehaviorTree;
END

BEGIN(Client)

class CMonster abstract : public CLandObject
{
public:
	typedef struct Mst_Desc : public LANDOBJ_DESC
	{
		LEVEL		eLevel = LEVEL::LEVEL_END;
		_bool bPlayerIsFront = false;
	}MST_DESC;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	_float			Get_Ratio() { return m_fCurHp / m_fMaxHp; }
	//_float4			Get_InitPos() { return m_vInitialPos; }
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Add_Hp(_int iValue) PURE;
	void Set_Lock(_bool bLock) { m_bIsLocked = bLock; }
	_bool CanBackAttack();
	_bool Get_MonsterDead() { return m_bDead; }

public:
	//FOR LOCKON
	_vector Get_MonsterPos();

protected:
	HRESULT Add_BehaviorTree();

protected:
	_bool							m_isAnimFinished = { false };

#pragma region 몬스터 스탯
	_float m_fMaxHp = 100.f;
	_float m_fCurHp = m_fMaxHp;
#pragma endregion 몬스터 스탯

	_bool m_bDead = false;
	_float m_fDeadDelay = 2.f;
	_float	m_fLengthFromPlayer = 100.f;
	_bool m_bIsLocked = false;
	_bool m_bPlayerIsFront = false;
	_float m_fDegreeBetweenPlayerAndMonster = 180.f;

	_uint							m_iState = { 0 };
	_uint m_iPastState = { 0 };

	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	class CPhysXComponent_Character*				m_pPhysXCom = { nullptr };
	CBehaviorTree*					m_pBehaviorCom = { nullptr };

	LEVEL							m_eLevel = { LEVEL_END };

	CPlayer*						m_pPlayer = { nullptr };
	CTransform*						m_pPlayerTransform = { nullptr };
	
	// UI
	class CUIGroup*		m_pUI_HP = { nullptr };
	class CTargetLock*	m_pTargetLock = { nullptr };

	//맵툴 배치용
	//_float4 m_vInitialPos = {0.f, 0.f, 0.f, 1.f};		//CGameObject에 있음
protected:
	virtual HRESULT		Add_Nodes();

	void				Create_UI();
	void				Create_BossUI(CUIGroup_BossHP::BOSSUI_NAME eBossName); // 인자에 값을 넣어주는 게 좋을 듯함
	HRESULT				Create_TargetLock(CModel* pBodyModel, string strBoneName, _vector vOffsetPos, _float fScale);

	void				Update_UI(_float fHeight = 0.f);
	void				Reward_Soul(_bool isBoss = false);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END