#pragma once
#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Line final : public CVIBuffer
{
public:
	typedef struct LINE_DESC
	{
		_float3 vPos[2];
	};

private:
	CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Line(const CVIBuffer_Line& rhs);
	virtual ~CVIBuffer_Line() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

private:
	shared_ptr<LINE_DESC> LineDesc;

public:
	static CVIBuffer_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END