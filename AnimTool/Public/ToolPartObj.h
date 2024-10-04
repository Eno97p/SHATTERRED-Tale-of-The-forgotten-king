#pragma once

#include "GameObject.h"
#include "AnimTool_Defines.h"

BEGIN(Engine)
class CTransform;
class CShader;
class CModel;
END

BEGIN(AnimTool)

class CToolPartObj final : public CGameObject
{
public:
	typedef struct PartObj_Desc : public GAMEOBJECT_DESC
	{
		_uint				iBoneIdx;
		_float				fRightRadian, fLookRadian, fUpRadian;
		_vector				vPos;
		string				strModelName;
		const _float4x4* pCombinedTransformationMatrix;
		const _float4x4* pParentMatrix;
	}PARTOBJ_DESC;

private:
	CToolPartObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolPartObj(const CToolPartObj& rhs);
	virtual ~CToolPartObj() = default;

public:
	_float			Get_RightRadian() { return m_fRightRadian; }
	void			Set_RightRadian(_float fRightRadian) { m_fRightRadian = fRightRadian; }

	_float			Get_LookRadian() { return m_fLookRadian; }
	void			Set_LookRadian(_float fLookRadian) { m_fLookRadian = fLookRadian; }

	_float			Get_UpRadian() { return m_fUpRadian; }
	void			Set_UpRadian(_float fUpRadian) { m_fUpRadian = fUpRadian; }

	_vector			Get_Pos() { return m_vPos; }
	void			Set_Pos(_vector vPos) { m_vPos = vPos; }

	void			Set_Radian(_float fRight, _float fLook, _float fUp);

	// Load
	wstring			Get_Name() { return m_wstrModelName; }
	_uint			Get_BoneIdx() { return m_iSocketBoneIdx; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint					m_iSocketBoneIdx = { 0 };

	_float					m_fRightRadian = { 0.f };
	_float					m_fLookRadian = { 0.f };
	_float					m_fUpRadian = { 0.f };
	_vector					m_vPos;

	_float4x4				m_WorldMatrix;
	const _float4x4*		m_pParentMatrix = { nullptr };

	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	const _float4x4*		m_pSocketMatrix = { nullptr };

	wstring					m_wstrModelName;

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

public:
	static CToolPartObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END