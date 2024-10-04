#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct BODY_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _bool* pIsCloaking;
		const _bool* pOnWater;
	}BODY_DESC;

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& rhs);
	virtual ~CBody_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection() override;
	virtual HRESULT Render_Distortion() override;
	virtual HRESULT Render_LightDepth() override;
	virtual bool Get_AnimFinished() { return m_bAnimFinished; }
	void Set_Weapon(class CWeapon* pWeapon, _uint iIndex) { m_pWeapon[iIndex] = pWeapon; }

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	const _bool* m_pIsCloaking = false;
	const _bool* m_pOnWater = { nullptr };

	_bool m_bAnimFinished = false;
	_uint m_iPastAnimIndex = 0;
	_float m_fDistortionValue = 0.f;
	class CWeapon* m_pWeapon[4] = { nullptr };
	_float m_fDamageTiming = 0.f;

public:
	_bool m_fGoStraight = false;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END