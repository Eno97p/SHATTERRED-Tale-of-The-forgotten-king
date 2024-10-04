#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Mantari final : public CPartObject
{
private:
	CBody_Mantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Mantari(const CBody_Mantari& rhs);
	virtual ~CBody_Mantari() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Reflection() override;
	virtual HRESULT Render_LightDepth() override;
	virtual bool Get_AnimFinished() { return m_bAnimFinished; }
	void Set_Weapon(class CWeapon* pWeapon) { m_pWeapon = pWeapon; }

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool m_bAnimFinished = false;
	_uint m_iPastAnimIndex = 0;
	_float m_fDistortionValue = 0.f;
	class CWeapon* m_pWeapon = nullptr;
	_float m_fDamageTiming = 0.f;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Mantari* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END