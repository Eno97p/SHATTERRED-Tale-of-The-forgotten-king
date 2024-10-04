#pragma once

//#include "MYMapTool_Defines.h"
#include "Map_Element.h"

BEGIN(Client)

class CTree final : public CMap_Element
{
public:
	typedef struct TREE_DESC : public CMap_Element::MAP_ELEMENT_DESC
	{
		_float4			vStartPos;
		_float3			vLeafCol = { 0.f, 0.f, 0.f };
		_bool			isBloom = false;
	}TREE_DESC;
private:
	CTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTree(const CTree& rhs);
	virtual ~CTree() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_LightDepth() override;

	_float3 Get_LeafCol() { return m_LeafCol; }
	_bool Get_Bloom() { return m_bBloom; }
private:
	CTexture* m_pNoiseCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };

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
	_uint m_iTest = 0;
	_float m_fTime = 0.f;
	_float3 m_LeafCol = {0.f, 0.f, 0.f};


	vector<_uint> m_LeafMeshes;
	vector<_uint> m_BloomMeshes;
	_bool m_bBloom = false;

	_uint m_iTrunkPath = 0;
	_uint m_iBloomPath = 6;
public:
	static CTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END