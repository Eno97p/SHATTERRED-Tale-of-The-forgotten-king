#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	_bool Get_PickPos(_float4* pPickPos);

public:
	HRESULT Initialize(HWND hWnd);
	void Update();

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	_uint							m_iWinSizeX, m_iWinSizeY;

	class CGameInstance*			m_pGameInstance = { nullptr };
	HWND							m_hWnd = {};

	ID3D11Texture2D*				m_pTexture2D = { nullptr };

	_bool							m_isSuccess = { false };
	_float4							m_vPickPos = {};

private:
	_float Compute_ProjZ(_float fX, _float fY);

public:
	static CPicking * Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, HWND hWnd);
	virtual void Free() override;
};

END