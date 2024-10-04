#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
#include "HelixCast.h"
#include "BezierCurve.h"
#include "NewAspiration.h"
BEGIN(Effect)
class CMagicCast final : public CGameObject
{

public:
	typedef struct MAGIC_CAST
	{
		_float fMaxLifeTime = 1.f;
		_float2 fThreadRatio = { 0.f,0.f };
		_float fSlowStrength = 0.5f;
		const _float4x4* ParentMatrix = nullptr;
		_bool					isHelix = true;
		CHelixCast::HELIXCAST HelixDesc{};
		_bool					isBezierCurve = true;
		CBezierCurve::BEZIER_CURVE BezierDesc{};
		_bool					isAspiration = true;
		CNewAspiration::NEWASPIRATION AspDesc{};
	};

private:
	CMagicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagicCast(const CMagicCast& rhs);
	virtual ~CMagicCast() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_Components();
	HRESULT Add_Child_Effects();

private:
	unique_ptr<MAGIC_CAST>	m_OwnDesc;

public:
	static CMagicCast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END