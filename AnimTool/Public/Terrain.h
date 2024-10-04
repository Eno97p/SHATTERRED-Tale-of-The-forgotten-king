#pragma once

#include "GameObject.h"
#include "Imgui_Manager.h"
#include "AnimTool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation;
class CVIBuffer_Terrain;

//class CCalculator;
END

BEGIN(AnimTool)

class CToolObj_Manager;

class CTerrain : public CGameObject
{
public:
	enum TEXTURE { TEX_DIFFUSE, TEX_MASK, TEX_BRUSH, TEX_END }; // 텍스쳐 목록

public:
	typedef struct Terrain_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint		iVerticesX;
		_uint		iVerticesZ;
	}TERRAIN_DESC;

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	_uint			Get_VerticesX() { return m_iVerticesX; }
	_uint			Get_VerticesZ() { return m_iVerticesZ; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Setting_LoadTerrain(void* pArg); // 테스트용 임시 함수

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom[TEX_END] = {nullptr};
	CNavigation*		m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };

	//CCalculator*		m_pCalculatorCom = { nullptr };
	CImgui_Manager*		m_pImgui_Manager = { nullptr };
	CToolObj_Manager*	m_pToolObj_Manager = { nullptr };

	_float4x4			m_ViewMatrix, m_ProjMatrix;

	_bool				m_isReLoad = { false }; // 테스트용 변수

	_uint				m_iVerticesX = { 0 };
	_uint				m_iVerticesZ = { 0 };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	_vector	Check_Picking();
	HRESULT	Add_ToolObj(_vector vPosition);

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;
	
};

END