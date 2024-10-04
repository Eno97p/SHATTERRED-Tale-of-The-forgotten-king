#pragma once

#include "Base.h"

/* ���������̽� ���� ����ü�� �����ϳ�. */
/* �� �����Ӹ��� ����ü�� ������� ��ȯ���ѳ����۾��� �����Ѵ�. */

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

	void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
	_bool isIn_WorldFrustum(_fvector vPosition, _float fRange = 0.f);
	_bool isIn_LocalFrustum(_fvector vPosition, _float fRange);
	_uint Get_FrustumNum();

	_bool isVisible(_vector vPos, PxActor* actor);
	
	//void SetHZBResource(ID3D11ShaderResourceView** pHZBSRV, UINT mipLevels)
	//{
	//	m_pHZBSRV = pHZBSRV;
	//	m_HZBMipLevels = mipLevels;
	//}
	float SampleHZB(ID3D11ShaderResourceView* pHZBSRV, float x, float y, UINT level);

private:
	struct ObjectVisibilityInfo
	{
		bool isVisible;
		float lastCheckTime;
	};

	std::unordered_map<PxActor*, ObjectVisibilityInfo> m_visibilityCache;
	float m_cacheUpdateInterval = 0.05f; // 0.1�ʸ��� ���ü� ������Ʈ
	float m_maxVisibleDistance = 1000.0f; // �ִ� ���� �Ÿ�
	float m_fTotalTime = 0;
	_float3					m_vPoints[8] = {};
	_float4					m_vWorldPoints[8] = {};
	class CGameInstance*	m_pGameInstance = { nullptr };

	_float4					m_vWorldPlanes[6] = {};
	_float4					m_vLocalPlanes[6] = {};

private:
	
	void Make_Planes(const _float4* vPoints, _float4* pPlanes);


public:
	static CFrustum* Create();
	virtual void Free() override;
};

END