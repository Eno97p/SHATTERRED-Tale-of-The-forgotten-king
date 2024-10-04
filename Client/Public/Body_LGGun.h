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

class CBody_LGGun final : public CPartObject
{
private:
	CBody_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_LGGun(const CBody_LGGun& rhs);
	virtual ~CBody_LGGun() = default;

public:
	virtual bool	Get_AnimFinished() { return m_isAnimFinished; }
	void Set_Gun(class CWeapon* pWeapon) { m_pGun = pWeapon; }
	void Set_Sword(class CWeapon* pWeapon) { m_pSword = pWeapon; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Distortion();
	virtual HRESULT Render_LightDepth() override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };

	_bool		m_isAnimFinished = { false };
	_uint		m_iPastAnimIndex = 0;
	class CWeapon* m_pGun = nullptr;
	class CWeapon* m_pSword = nullptr;
	_float m_fDamageTiming = 0.f;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void	Change_Animation(_float fTimeDelta);

public:
	static CBody_LGGun*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END