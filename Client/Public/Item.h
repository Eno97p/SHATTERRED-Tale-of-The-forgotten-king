#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
class CVIBuffer_Rect;
class CTransform;
class CCollider;
END

BEGIN(Client)
class CPlayer;

class CItem final : public CBlendObject
{
public:
	struct ITEM_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPosition = { 0.f, 0.f, 0.f };
	};


public:
	// 드랍템에 한정함
	enum ITEM_NAME { ITEM_BUFF1 = 0, ITEM_BUFF2, ITEM_BUFF3, ITEM_BUFF4, ITEM_SOUL, ITEM_ESSENCE, ITEM_ETHER, ITEM_UPGRADE1, ITEM_UPGRADE2, ITEM_END };

private:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	ITEM_NAME		Get_ItemName() { return m_eItemName; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Set_Texture();
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Distortion() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pTextureShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTransform* m_pTextureTransformCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	CPlayer* m_pPlayer = { nullptr };

	ITEM_NAME	m_eItemName = { ITEM_END };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END