#pragma once
#include "Active_Element.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CBossStatue final : public CActive_Element
{
private:
	CBossStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossStatue(const CBossStatue& rhs);
	virtual ~CBossStatue() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	_bool Get_Active() { return m_bActive; }
	void Set_Active(_bool bActive) { m_bActive = bActive; }

private:
	CCollider* m_pColliderCom = { nullptr };
	CCollider::COLLTYPE m_eColltype = CCollider::COLL_NOCOLL;
	class CPlayer* m_pPlayer = { nullptr };
	_bool m_bActive = false;

	class CUI_ArrowSign*	m_pSignUI = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

	HRESULT	Create_UI();

private:
	_uint					m_iTest = 0;

public:
	static CBossStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END