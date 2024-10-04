#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance_Point final : public CVIBuffer_Instance
{
private:
	CVIBuffer_Instance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance_Point(const CVIBuffer_Instance_Point& rhs);
	CVIBuffer_Instance_Point(const CVIBuffer_Instance_Point& rhs, _bool For_Copy);
	virtual ~CVIBuffer_Instance_Point() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Setup_Onterrain(CVIBuffer_Terrain* pTerrain);

	HRESULT Ready_Instance_ForGrass(const CVIBuffer_Instance::INSTANCE_MAP_DESC& InstanceDesc);

public:
	static CVIBuffer_Instance_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END