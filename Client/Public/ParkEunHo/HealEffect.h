#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Heal_Ribbon.h"
#include "Heal_Spiral.h"
#include "Heal_Line.h"
BEGIN(Client)
class CHealEffect final : public CGameObject
{
public:
	typedef struct HEALEFFECT
	{
		_float fLifeTime = 0.f;
		_float3 vOffset = { 0.f,1.f,0.f };
		const _float4x4* ParentMat = nullptr;
		CHeal_Ribbon::HEAL_RIBBON RibbonDesc{};
		CHeal_Spiral::HEAL_SPIRAL SpiralDesc{};
		CHeal_Line::HEAL_LINE LineDesc{};
		_int	iNumParticle = 0.f;
	};

private:
	CHealEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHealEffect(const CHealEffect& rhs);
	virtual ~CHealEffect() = default;
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
	shared_ptr<HEALEFFECT>			m_OwnDesc;
	vector<CGameObject*>			m_EffectClasses;


public:
	static CHealEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END