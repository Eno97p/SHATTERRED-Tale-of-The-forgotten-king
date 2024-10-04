#pragma once

#include "AnimTool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CShader;
class CModel;
END

BEGIN(AnimTool)

class CBone_Sphere final : public CGameObject
{
public:
	typedef struct BoneSphere_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_int			iBoneIdx;
		const _float4x4* pParentMatrix;
		const _float4x4* pCombinedTransformationMatrix;
	}BONESPHERE_DSC;

private:
	CBone_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBone_Sphere(const CBone_Sphere& rhs);
	virtual ~CBone_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_int				m_iBoneIdx = { 0 };
	_float				m_fScale = { 0.f };

	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	_float4x4			m_WorldMatrix;
	const _float4x4*	m_pParentMatrix = { nullptr };
	const _float4x4*	m_pSocketMatrix = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBone_Sphere*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};

END