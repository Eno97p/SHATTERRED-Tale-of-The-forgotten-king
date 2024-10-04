#pragma once

#include "Npc.h"

BEGIN(Client)

class CNPC_Yaak final : public CNpc
{
private:
	CNPC_Yaak(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Yaak(const CNPC_Yaak& rhs);
	virtual ~CNPC_Yaak() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool				m_isScriptOn = { false };
	_uint				m_iDialogCnt = { 0 };

	class CBody_Yaak* m_pBody = { nullptr };

private:
	HRESULT				Add_PartObjects();

	virtual HRESULT		Create_Script() override;

	void				Key_Input();

public:
	static CNPC_Yaak*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END