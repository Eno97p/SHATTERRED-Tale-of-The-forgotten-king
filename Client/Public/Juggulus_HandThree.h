#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CJuggulus_HandThree final : public CPartObject
{
#define ASPIRATIONDELAY 0.5f

public:
	enum STATE {
		STATE_IDLE, STATE_ATTACK, STATE_END
	};

private:
	CJuggulus_HandThree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJuggulus_HandThree(const CJuggulus_HandThree& rhs);
	virtual ~CJuggulus_HandThree() = default;

public:
	virtual bool	Get_AnimFinished() { return m_isAnimFinished; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_LightDepth() override;

private:
	HRESULT Add_Nodes();
	NodeStates Attack(_float fTimeDelta);
	NodeStates Idle(_float fTimeDelta);
	void Add_Hp(_int iValue);

private:
	CBehaviorTree* m_pBehaviorCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_vector m_vParentPos;
	_bool			m_isAnimFinished = { false };
	_uint			m_iPastAnimIndex = 0;
	class CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	_float* m_pCurHp;
	_float* m_pMaxHp;
	_uint m_iState = 0;
	_float m_fAttackDelay = 2.f;
	_uint m_iAttackCount = 0;
	_float m_fAspirationDelay = 0.f;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void					Change_Animation(_float fTimeDelta);

public:
	static CJuggulus_HandThree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};

END