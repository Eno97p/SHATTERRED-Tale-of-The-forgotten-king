#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon : public CPartObject
{

public:
	typedef struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*			pCombinedTransformationMatrix;
		_float3						vOffset = { 0.f,0.f,0.f };
	}WEAPON_DESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual void Set_Active(_bool isActive = true);
	void Generate_Trail(_int iIndex);

	_bool Get_Active() { return m_bIsActive; }
	void Set_RenderAvailable(_bool render) { m_bRenderAvailable = render; }
	void Set_Dir(_vector vDir) { m_vDir = vDir; }
	_float Get_Damage() { return m_fDamage; }
	void Set_Damage(_float fDamage) { m_fDamage = fDamage; }
	void Add_Damage(_float fDamage) { m_fDamage += fDamage; }

protected:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };
	_bool							m_bIsActive = false;
	_bool							m_bRenderAvailable = true;
	_vector							m_vDir;
	_bool							m_bGenerageTrail = false;
	_float							m_fDamage = 0.f;

protected:
	_bool							m_GenerateTrail = false;
public:
	virtual void Free() override;
};

END