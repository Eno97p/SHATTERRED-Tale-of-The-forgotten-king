#pragma once
#include "Active_Element.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CTreasureChest final : public CActive_Element
{
public:
	enum TREASURE_COLOR { TREASURE_NORMAL, TREASURE_EPIC, TREASURE_CLOAKING, TREASURE_END }; // 노멀상자, 에픽상자, 클로킹상자

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
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXComponent_static* m_pPhysXCom = { nullptr };
private:
	TREASURE_COLOR m_eTreasureState = TREASURE_END; // 상자 타입
	_float4 m_TreasureColor = { 0.f, 0.f, 0.f, 1.f };
	_uint m_iShaderPath = 0;
	_uint m_iBloomShaderPath = 2;

	_vector m_vTargetPos;

	_bool m_bChestOpened = false;
	_bool m_bReward = { false };

	class CUI_Activate* m_pActivateUI = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

	HRESULT	Create_Activate();

	void	Drop_Item();

private:
	_uint m_iTest = 0;

public:
	static CTreasureChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END