#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CShader;
class CModel;
END

BEGIN(Client)

class CTargetLock final : public CGameObject
{
public:
	typedef struct TargetLock_Desc : public GAMEOBJECT_DESC
	{
		
		_float				fScale;
		_vector				vOffsetPos;
		const _float4x4*	pParentMatrix;
		const _float4x4*	pCombinedTransformationMatrix;
	}TARGETLOCK_DESC;

private:
	CTargetLock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTargetLock(const CTargetLock& rhs);
	virtual ~CTargetLock() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };

	_float4x4			m_WorldMatrix;
	const _float4x4*	m_pParentMatrix = { nullptr };
	const _float4x4*	m_pSocketMatrix = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTargetLock*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END