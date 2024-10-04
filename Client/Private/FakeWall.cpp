#include "FakeWall.h"
#include "GameInstance.h"


#include"Mantari.h"

CFakeWall::CFakeWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
{
}

CFakeWall::CFakeWall(const CFakeWall& rhs)
	: CMap_Element(rhs)
{
}

HRESULT CFakeWall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFakeWall::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;

		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CFakeWall::Priority_Tick(_float fTimeDelta)
{
	
}

void CFakeWall::Tick(_float fTimeDelta)
{
	list<CGameObject*> ObjectLis;
	ObjectLis= m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), L"Layer_Monster");
	if (!ObjectLis.empty())		//몬스터가 있을 때 만타리만 체크
	{
		for (auto& iter : ObjectLis)
		{
			if (typeid(*iter) == typeid(CMantari))
			{
				if (iter->Get_Dead())
					m_pGameInstance->Erase(this);
				else
				{
					int temp = 0;
				}

			}
		}

	}
	else //몬스터가 없을 때
	{
		m_pGameInstance->Erase(this);
	}
	
	



	int temp = 0;
}

void CFakeWall::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
#endif

}

HRESULT CFakeWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(4);

		m_pModelCom->Render(i);
	}
}

HRESULT CFakeWall::Add_Components(void* pArg)
{
	CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;
	
	wstring wstr = pDesc->wstrModelName;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// wstrModelName을 수정하여 physxName 생성


	CPhysXComponent::PHYSX_DESC		PhysXDesc{};
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	PhysXDesc.pComponent = m_pModelCom;
	PhysXDesc.eGeometryType = PxGeometryType::eTRIANGLEMESH;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_PhysX_BasicDonut"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CFakeWall::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CFakeWall* CFakeWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFakeWall* pInstance = new CFakeWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFakeWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFakeWall::Clone(void* pArg)
{
	CFakeWall* pInstance = new CFakeWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFakeWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFakeWall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPhysXCom);
}
