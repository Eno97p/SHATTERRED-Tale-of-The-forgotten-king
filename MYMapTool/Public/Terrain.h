#pragma once

#include "GameObject.h"
#include "Imgui_Manager.h"
#include "MYMapTool_Defines.h"

#include <wincodec.h>
#include <vector>
#include <wrl/client.h>

#pragma comment(lib, "windowscodecs.lib")

BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation;
class CVIBuffer_Terrain;

//class CCalculator;
END

BEGIN(MYMapTool)

class CToolObj_Manager;

class CTerrain : public CGameObject
{
public:
	enum TEXTURE { TEX_DIFFUSE, TEX_NORMAL, TEX_ROUGHNESS, TEX_MASK, TEX_BRUSH, TEX_END }; // 텍스쳐 목록

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

	void Brush_Picking(_vector vPos, _float fSize, _float fStrength, _float fMaxHeight);

	void Brush_Flatten(_vector vPos, _float fSize, _float fStrength, _float fTargetHeight);

	void AdjustTerrainHeight(_vector vPos, _float fSize, _float fStrength, _float fMaxHeight, _bool bRaise);

	HRESULT			Setting_NewTerrain(void* pArg = nullptr);
	void			Setting_LoadTerrain(void* pArg); // 테스트용 임시 함수

public:
	void Set_SnowGroundHeight(_float fHeight) { m_fSnowGroundHeight =  fHeight; }
	void Set_SnowGroundHeightOffset(_float fHeight) { m_fSnowGroundHeightOffset =  fHeight; }
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TEX_END] = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };

	//CCalculator*		m_pCalculatorCom = { nullptr };
	CImgui_Manager* m_pImgui_Manager = { nullptr };
	CToolObj_Manager* m_pToolObj_Manager = { nullptr };

	_float4x4			m_ViewMatrix, m_ProjMatrix;

	_bool				m_isReLoad = { false }; // 테스트용 변수

	_uint				m_iVerticesX = { 0 };
	_uint				m_iVerticesZ = { 0 };

	_float4				m_vBrushPos = { 0.f, 0.f, 0.f, 1.f };
	_float				m_fBrushSize = 10.f;
	_float				m_fBrushStrength = 1.f;
	_float m_fTest = 0.f;
	_float m_fMaxHeight = 10.f;

private:
	_float m_fSnowGroundHeight = 500;
	_float m_fSnowGroundHeightOffset = 50;

private:
private:
	ID3D11Texture2D* m_pHeightMapTexture = nullptr;
	ID3D11ShaderResourceView* m_pHeightMapSRV = nullptr;
	vector<_ushort> m_vHeightMapData;


public:
	HRESULT LoadHeightMap(const wstring& strHeightMapFilePath);
	void SplatBrushOnHeightMap(const _float3& vBrushPos, float fBrushRadius, float fBrushStrength, float fMaxHeight);
	HRESULT SaveHeightMapToR16(const wstring& strFilePath);
	//void UpdateTerrainFromHeightMap();

	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;

};

END