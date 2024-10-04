#pragma once

#include "Effect_Define.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Effect)

class CWeapon : public CPartObject
{
public:
	typedef struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*			pCombinedTransformationMatrix;
		_float3						vOffset;
	}WEAPON_DESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

protected:
	void Set_Weapon_Offset(WEAPON_DESC* pDesc, class CTransform* TransCom);

protected:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };

public:
	virtual void Free() override;
};

END