#pragma once

#include "GameObject.h"
#include "MYMapTool_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(MYMapTool)

class CMonster : public CGameObject
{
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	//FOR LOCKON
	_vector Get_MonsterPos();
	void Setting_WorldMatrix(void* pArg);
private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };

	_vector		m_vPosition;

public:
	HRESULT	Add_Component();
	HRESULT	Bind_ShaderResources();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;
};

END