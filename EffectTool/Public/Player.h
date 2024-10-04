#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
BEGIN(Engine)
class CShader;
class CModel;
class CBehaviorTree;
END


BEGIN(Effect)
class CPlayerDummy : public CGameObject
{
	enum STATE{ STATE_ATTACK, STATE_IDLE, STATE_WALK, STATE_END};
	enum WEAPONSTATE{ WHISPER_SWORD, WEAPON_END};
private:
	CPlayerDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerDummy(const CPlayerDummy& rhs);
	virtual ~CPlayerDummy() = default;

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
	const _float4x4* Get_WorldMat();
	const _float4x4* Get_HeadMAt();
	const _float4x4* Get_WeaponMat();

private:
	NodeStates IDLE(_float fTimeDelta);
	NodeStates WALK(_float fTimeDelta);
	NodeStates ATTACK(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CBehaviorTree* m_pBehaviorCom = { nullptr };

private:
	vector<class CGameObject*>			m_PartObjects;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_uint CurState = STATE_IDLE;
	WEAPONSTATE CurWeapon = WHISPER_SWORD;


public:
	static CPlayerDummy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END