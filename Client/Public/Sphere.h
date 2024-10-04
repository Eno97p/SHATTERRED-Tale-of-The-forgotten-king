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

class CSphere final : public CWeapon
{
private:
	CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphere(const CSphere& rhs);
	virtual ~CSphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	_bool m_bIsParried = false;
	class CMonster* m_pJuggulus = { nullptr };

	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_float m_fPlayerY = 0.f;

	_float m_Interval = 0.f;
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END