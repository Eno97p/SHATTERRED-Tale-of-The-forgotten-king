#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
#include "Swing_Spiral.h"
#include "Charge_Vane.h"
#include "Charge_Ribbon.h"
#include "DefaultCylinder.h"

BEGIN(Effect)
class CSwingEffect final : public CGameObject
{
public:
	typedef struct SWINGEFFECT
	{
		_float fLifeTime = 0.f;
		const _float4x4* ParentMat = nullptr;
		CSwing_Spiral::SWING_SPIRAL SpiralDesc{};
		Charge_Vane::VANE_DESC		VaneDesc{};
		Charge_Ribbon::LARGE_RIBBON RibbonDesc{};
		CDefaultCylinder::DEFAULTCYLINDER CylinderDesc{};
	};

private:
	CSwingEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwingEffect(const CSwingEffect& rhs);
	virtual ~CSwingEffect() = default;
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
	shared_ptr<SWINGEFFECT>			m_OwnDesc;
	vector<CGameObject*>			m_EffectClasses;


public:
	static CSwingEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END