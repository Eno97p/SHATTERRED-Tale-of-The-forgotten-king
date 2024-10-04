#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CDecal final : public CGameObject
{
public:
	typedef struct DECAL_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iDecalIdx = 0;

	}DECAL_DESC;

private:
	CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal(const CDecal& rhs);
	virtual ~CDecal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual pair<_uint, _matrix> Render_Decal() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_bool m_bDecal = false;

private:
	_uint m_iDecalIdx = 0;
	_vector m_vPosition = {0.f, 0.f, 0.f, 1.f};
	_float m_fRadius = 0.f;

public:
	static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END