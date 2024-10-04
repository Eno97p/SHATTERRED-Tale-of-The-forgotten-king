#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CAspiration final : public CGameObject
{
private:
	CAspiration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAspiration(const CAspiration& rhs);
	virtual ~CAspiration() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float m_fTexcoordY = 1.f;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CAspiration* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END