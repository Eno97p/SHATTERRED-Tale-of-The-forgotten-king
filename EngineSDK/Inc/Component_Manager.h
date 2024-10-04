#pragma once

#include "Model.h"
#include "Shader.h"
#include "ComputeShader_Texture.h"
#include "Texture.h"
#include "Texture3D.h"
#include "Collider.h"
#include "Navigation.h"
#include "BehaviorTree.h"
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

#pragma region VIBUFFER
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_SwordTrail.h"
#include "VIBuffer_Instance_Rect.h"
#include "VIBuffer_Instance_Point.h"
#include "VIBuffer_Lightning.h"
#include "VIBuffer_Line.h"
#pragma endregion



#pragma region PHYSXC
#include"VIBuffer_PhysXBuffer.h"
#include"PhysXComponent.h"
#include"PhysXComponent_static.h"
#include"PhysXComponent_Vehicle.h"
#include"PhysXComponent_Character.h"
#include"PhysXComponent_HeightField.h"
#include "PhysXParticle.h"
#pragma endregion


/* 컴포넌트의 원형들을 레벨별로 보관한다. */

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	CComponent* Get_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	void Clear(_uint iLevelIndex);
	_bool IsPrototype(_uint iLevelIndex, const wstring& strPrototypeTag);
private:
	map<const wstring, CComponent*>*			m_pPrototypes = { nullptr };
	typedef map<const wstring, CComponent*>		PROTOTYPES;

	_uint									m_iNumLevels = { 0 };

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	
public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END