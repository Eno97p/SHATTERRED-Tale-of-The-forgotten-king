#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CComponent;
END



BEGIN(Client)
class CUI_Activate;



class CSavePoint final : public CGameObject
{
public:
	typedef struct _tagSavePoint_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPosition;
	}_tagSavePoint_Desc;



private:
	CSavePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSavePoint(const CSavePoint& rhs);
	virtual ~CSavePoint() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


	HRESULT				Create_Activate();
	void Test();
private:
	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CPhysXComponent_static* m_pPhysXCom = nullptr;


	 CUI_Activate* m_pActivateUI = { nullptr };
public:
	static CSavePoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END