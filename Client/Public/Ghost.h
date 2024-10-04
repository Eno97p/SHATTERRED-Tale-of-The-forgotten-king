#pragma once

#include "Monster.h"

BEGIN(Client)

class CGhost final : public CMonster
{
#define DETECTRANGE 20.f
#define ATTACKRANGE 2.f

public:
	enum STATE { STATE_IDLE = 0, STATE_DEAD, STATE_HIT, STATE_PARRIED, 
		STATE_DEFAULTATTACK_1, STATE_DEFAULTATTACK_2, STATE_DEFAULTATTACK_3, STATE_DEFAULTATTACK_4, STATE_DOWNATTACK,
		STATE_MOVE, STATE_LEFT, STATE_RIGHT, STATE_END };

private:
	CGhost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGhost(const CGhost& rhs);
	virtual ~CGhost() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool							m_isHit = { false };
	_bool							m_isDefaultAttack = { false };

	vector<class CGameObject*>		m_PartObjects;

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

	NodeStates			Default_Attack(_float fTimeDelta);
	NodeStates			Down_Attack(_float fTimeDelta);

	NodeStates			Detect(_float fTimeDelta);
	NodeStates			Move(_float fTimeDelta);
	NodeStates			Idle(_float fTimeDelta);


	void Add_Hp(_int iValue);


public:
	static CGhost*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END