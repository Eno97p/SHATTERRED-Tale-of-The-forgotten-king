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

class CBossDeco final : public CMap_Element
{
public:

private:
	CBossDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossDeco(const CBossDeco& rhs);
	virtual ~CBossDeco() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;


public:
	void Play_DeadAnimation();


private:
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	const _float4x4* m_pBoneMatrix = nullptr;

private:
	_double m_dStartTime = 0.f;
	_float m_fTimeAccel = 1.2f;
	class CPlayer* m_pPlayer = { nullptr };

	_matrix m_ColliderMat = XMMatrixIdentity();

	_float m_fDisolveValue = 1.f;
	CTexture* m_pDisolveTextureCom = nullptr;
	_bool m_bDissolve = false;
	_uint m_iShaderPath = 0;

	_bool m_bBloom = false;

private:
	HRESULT Add_Components(MAP_ELEMENT_DESC* desc);
	HRESULT Bind_ShaderResources();

public:
	static CBossDeco* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END