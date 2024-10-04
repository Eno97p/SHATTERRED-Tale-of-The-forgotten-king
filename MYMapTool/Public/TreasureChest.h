#pragma once

//#include "MYMapTool_Defines.h"
#include "ToolObj.h"


BEGIN(MYMapTool)

class CTreasureChest final : public CToolObj
{
public:
	enum TREASURE_COLOR { TREASURE_NORMAL, TREASURE_EPIC, TREASURE_CLOAKING, TREASURE_END };
private:
	CTreasureChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTreasureChest(const CTreasureChest& rhs);
	virtual ~CTreasureChest() = default;

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
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };
private:
	_vector m_vTargetPos;
	_float m_fReboundTimer = 0.f;

	_bool m_bChestOpened = false;

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
	//	_bool m_bElevate = false;
	TREASURE_COLOR m_eTreasureColor = TREASURE_END;
	_float m_fElevateTimer= 0.f;
	_uint m_iTest = 0;


	_float3 m_vPivotPos = { 0.f, 0.f, 0.f };
	_float4 m_TreasureColor = { 0.f, 0.f, 0.f, 1.f };
	_uint m_iShaderPath = 0;
	_uint m_iBloomShaderPath = 2;

public:
	static CTreasureChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END