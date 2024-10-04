#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CModel;
class CPhysXComponent;

END


BEGIN(Client)

class CBlastWall final : public CGameObject
{

public:
	typedef struct CBlastWall_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4 fWorldMatrix = {};

	}CBlastWall_DESC;


private:
	CBlastWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlastWall(const CBlastWall& rhs);
	virtual ~CBlastWall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void Check_Collision();
	void Broken_Wall();

private:
	vector<_float4x4> m_vecMeshsTransforms;
	_uint	m_iNumMeshes = 0;
	_float  m_fLifeTime = 0.f;
	_bool   m_bIsHit = false;
	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CPhysXComponent** m_pPhysXCom = { nullptr };

public:
	static CBlastWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END