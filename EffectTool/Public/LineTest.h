#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
#include "VIBuffer_Line.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

class CLineTest final :public CBlendObject
{
private:
	CLineTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLineTest(const CLineTest& rhs);
	virtual ~CLineTest() = default;
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	CTexture* m_pNoiseTex = { nullptr };
	CVIBuffer_Line* m_pVIBufferCom = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();
public:
	static CLineTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

