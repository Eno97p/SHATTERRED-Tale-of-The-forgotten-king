#pragma once
#include "Active_Element.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXComponent;
END

BEGIN(Client)

class CRotateGate final : public CActive_Element
{
private:
	CRotateGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotateGate(const CRotateGate& rhs);
	virtual ~CRotateGate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Rotate_Gate(_float fTIme);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pNoiseCom = { nullptr };
	CPhysXComponent* m_pPhysXCom = { nullptr };


private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	float Clamp(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

private:
	_uint m_iTest = 0;

private:
	_bool m_bIsRotating = false;
	_float m_fRotationTime = 0.f;
	_float m_fTargetRotationTime = 0.f;
	_float m_fTotalRotationAngle = XMConvertToRadians(180.f);

public:
	static CRotateGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END