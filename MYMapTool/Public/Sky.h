#pragma once

#include "GameObject.h"
#include "MYMapTool_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;

class CTexture;
class CVIBuffer_Cube;
END

BEGIN(MYMapTool)

class CSky : public CGameObject
{
private:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };

	_vector		m_vPosition;
	_float fTime = 0.f;
public:
	HRESULT	Add_Component();
	HRESULT	Bind_ShaderResources();

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;
};

END