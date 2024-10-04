#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

	void Set_LightPos(_vector lightPos) { XMStoreFloat4(&m_LightDesc.vPosition, lightPos); }
	void Set_LightDesc(const LIGHT_DESC& desc) { m_LightDesc = desc; }

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

	_bool Get_LightOn() { return m_bLightOn; }

	void LightOn() { m_bLightOn = true; }
	void LightOff() { m_bLightOn = false; }


private:
	LIGHT_DESC				m_LightDesc{};

private:
	_bool					m_bLightOn = false;
public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END