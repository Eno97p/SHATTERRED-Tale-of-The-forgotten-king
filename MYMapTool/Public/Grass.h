#pragma once

//#include "MYMapTool_Defines.h"
#include "ToolObj.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(MYMapTool)

class CGrass final : public CToolObj
{
public:
	typedef struct GRASS_DESC : public CToolObj::TOOLOBJ_DESC
	{
		_float4			vStartPos;
		_float3			vTopCol = { 1.f, 0.f, 0.f };
		_float3			vBotCol = {0.f, 0.f, 0.f};
	}GRASS_DESC;
private:
	CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrass(const CGrass& rhs);
	virtual ~CGrass() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float3 Get_TopCol() { return m_vTopCol; }
	_float3 Get_BotCol() { return m_vBotCol; }
	_uint Get_NumInstance();

	vector<_float4x4*> Get_VtxMatrices();
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pNormalCom = { nullptr };
	CVIBuffer_Instance_Point* m_pVIBufferCom = { nullptr };

	CTexture* m_pNoiseCom = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	float Clamp(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

private:
	_float3 m_vTopCol = { 0.7f, 0.f, 0.f };
	_float3 m_vBotCol = {0.1f, 0.1f, 0.1f};
	_float3 m_vTopColorOffset = {0.1f, 0.1f, 0.1f};

private:
	float m_fAccTime = 0.f;
	float m_fWindStrength = 1.f;
	float m_fTargetWindStrength = 1.f;
	float m_fWindChangeTime = 0.f;
	const float m_fWindChangeDuration = 5.f; 

	_float3 m_vPivotPos = {0.f, 0.f, 0.f};

public:
	static CGrass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END