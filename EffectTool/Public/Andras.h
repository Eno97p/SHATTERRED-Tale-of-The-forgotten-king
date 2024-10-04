#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
class CBehaviorTree;
END

BEGIN(Effect)
class CWeapon;
END

BEGIN(Effect)
class CAndras : public CGameObject
{
	enum STATE{ STATE_ATTACK1, STATE_ATTACK2, STATE_ATTACK3, 
		STATE_ATTACK4, STATE_ATTACK5, STATE_ATTACK6, STATE_ATTACK7, STATE_ATTACK8,
		STATE_MAGIC0, STATE_MAGIC1,
		STATE_IDLE, STATE_WALK, STATE_END };

private:
	CAndras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndras(const CAndras& rhs);
	virtual ~CAndras() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Add_Nodes();
	HRESULT Add_PartObjects();
	void Set_AnimIndex(_float fTimeDelta);

public:
	const _float4x4* Get_WorldMat();
	const _float4x4* Get_WeaponMat(_int Index);
	const _float4x4* Get_HeadMat();

private:
	NodeStates IDLE(_float fTimeDelta);
	NodeStates WALK(_float fTimeDelta);
	NodeStates ATTACK(_float fTimeDelta);
	NodeStates MAGIC0(_float fTimeDelta);
	NodeStates MAGIC1(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CBehaviorTree* m_pBehaviorCom = { nullptr };
private:
	vector<CWeapon*>					m_PartObjects;
	CGameObject*						m_Head;
	_bool								m_bAnimFinish = false;
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_uint CurState = STATE_IDLE;

public:
	static CAndras* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END