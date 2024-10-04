#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation;
class CVIBuffer_Terrain;
class CPhysXComponent_HeightField;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURE { TEX_DIFFUSE, TEX_NORMAL, TEX_ROUGHNESS, TEX_MASK, TEX_BRUSH, TEX_END }; // 텍스쳐 목록
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom[TEX_END] = { nullptr };
	CNavigation*					m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain*				m_pVIBufferCom = { nullptr };
	CPhysXComponent_HeightField*	m_pPhysXCom = { nullptr };
private:
	_float4x4						m_ViewMatrix, m_ProjMatrix;


private:
	_float m_fSnowGroundHeight = 650;
	_float m_fSnowGroundHeightOffset = 150;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END