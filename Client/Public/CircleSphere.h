#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
class CCollider;
END

BEGIN(Client)
class CPlayer;

class CCircleSphere final : public CWeapon
{
private:
	CCircleSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCircleSphere(const CCircleSphere& rhs);
	virtual ~CCircleSphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_float m_fPlayerY = 0.f;

	class CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	_bool m_bIsParried = false;

	class CMonster* m_pJuggulus = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

	_float m_fShootDelay = 3.f;
	_bool m_bLookatPlayer = false;
	CGameObject* Particle = nullptr;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCircleSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END