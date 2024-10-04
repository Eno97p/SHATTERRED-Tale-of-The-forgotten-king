#pragma once
#include "MYMapTool_Defines.h"
#include "ToolObj.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CCollider;
END

BEGIN(MYMapTool)

class CTrap final : public CToolObj
{
public:
	// Client에 객체 생성 시 들고 가야 할 정보를 담을 구조체
	typedef struct TRAP_DESC : public CToolObj::TOOLOBJ_DESC
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

public:
	_double Get_StartTime() { return m_dStartTime; }

private:
	CTexture* m_pNoiseCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	const _float4x4* m_pBoneMatrix = nullptr;

private:
	_double m_dStartTime = 0.f;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTrap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END