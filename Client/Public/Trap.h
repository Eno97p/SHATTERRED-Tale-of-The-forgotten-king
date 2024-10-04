#pragma once
#include "Client_Defines.h"
#include "Map_Element.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CCollider;
END

BEGIN(Client)

class CTrap final : public CMap_Element
{
public:
	typedef struct TRAP_DESC : public CMap_Element::MAP_ELEMENT_DESC
	{
		_double dStartTimeOffset = 0;

	}TRAP_DESC;

private:
	CTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrap(const CTrap& rhs);
	virtual ~CTrap() = default;

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

	const _float4x4* m_pBoneMatrix = nullptr;

private:
	_double m_dStartTime = 0.f;
	_float m_fTimeAccel = 1.2f;
	class CPlayer* m_pPlayer = { nullptr };
	CCollider::COLLTYPE m_eColltype = CCollider::COLL_NOCOLL;

	_matrix m_ColliderMat = XMMatrixIdentity();
	_bool	IsPillar = false;
private:
	HRESULT Add_Components(TRAP_DESC* desc);
	HRESULT Bind_ShaderResources();

public:
	static CTrap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END