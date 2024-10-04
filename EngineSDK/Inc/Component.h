#pragma once

#include "Base.h"

/* 한달 늘었다고 해서 뭐가 대단히 바뀌지않는다. */
/* 게!! 얼음을 물고있어서. */

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
public:
	struct ComponentDesc 
	{

	};


protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);	
	virtual HRESULT Render() { return S_OK; };


	virtual ComponentDesc* GetData() { return nullptr; };

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	_bool						m_isCloned = { false };


	//필요할 까?? 나중에 가면 필요할 수도 있음
	ComponentDesc				m_ComponentDesc;

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END