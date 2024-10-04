#pragma once

#include "LandObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CNpc abstract : public CLandObject
{
public:
	const _float ACTIVATE_DISTANCE = 2.f;

	typedef struct Npc_Desc : public LANDOBJ_DESC
	{
		LEVEL		eLevel = LEVEL::LEVEL_END;
	}NPC_DESC;

protected:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader*							m_pShaderCom = { nullptr };
	CModel*								m_pModelCom = { nullptr };
	class CPhysXComponent_Character*	m_pPhysXCom = { nullptr };

	LEVEL								m_eLevel = { LEVEL_END };

	class CTransform*					m_pPlayerTransform = { nullptr };
	class CUIGroup_Script*				m_pScriptUI = { nullptr };
	class CUI_Activate*					m_pActivateUI = { nullptr };

protected:
	HRESULT				Create_Activate();
	virtual HRESULT		Create_Script() = 0;

	_bool				Check_Distance();

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END