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
class CHoverBoard : public CGameObject
{
	enum HOVER_STATE{ STATE_IDLE, STATE_MOVE, STATE_END };

private:
	CHoverBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHoverBoard(const CHoverBoard& rhs);
	virtual ~CHoverBoard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Add_Nodes();
	HRESULT Add_PartObjects();

public:
	const _float4x4* Get_WorldMat();
	const _float4x4* Get_WeaponMat(_int Index);

private:
	NodeStates IDLE(_float fTimeDelta);
	NodeStates MOVE(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CBehaviorTree* m_pBehaviorCom = { nullptr };
private:
	vector<CWeapon*>					m_PartObjects;
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_uint CurState = STATE_IDLE;

public:
	static CHoverBoard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END