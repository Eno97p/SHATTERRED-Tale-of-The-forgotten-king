#include "TutorialMapBridge.h"
#include "ToolObj.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
CTutorialMapBridge::CTutorialMapBridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CTutorialMapBridge::CTutorialMapBridge(const CTutorialMapBridge& rhs)
	: CToolObj(rhs)
{
}

HRESULT CTutorialMapBridge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTutorialMapBridge::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;

		strcpy_s(m_szName, pDesc->szObjName);
		strcpy_s(m_szLayer, pDesc->szLayer);
		strcpy_s(m_szModelName, pDesc->szModelName);
		m_eModelType = pDesc->eModelType;
		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);


		/*m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);*/

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((GRASS_DESC*)pArg)->vStartPos));
		 
		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTutorialMapBridge::Priority_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_5))
	{
		eDSState = DS_FADEOUT;
	}

	if (m_pGameInstance->Key_Down(DIK_6))
	{
		eDSState = DS_FADEIN;
	}
}

void CTutorialMapBridge::Tick(_float fTimeDelta)
{
	if (eDSState == DS_FADEIN)
	{
		if (m_fAccTime < 1.f)
		{
			m_fAccTime += fTimeDelta;
		}
		else
		{
			m_fAccTime = 1.f;
			eDSState = DS_END;
		}

	}
	else if(eDSState == DS_FADEOUT)
	{

		if (m_fAccTime > 0.f)
		{
			m_fAccTime -= fTimeDelta;
		}
		else
		{
			m_fAccTime = 0.f;
			eDSState = DS_END;
		}

	}
}

void CTutorialMapBridge::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTutorialMapBridge::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//if ( i != 29)
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;


		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &i, sizeof(_uint))))
		//	return;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}
}

HRESULT CTutorialMapBridge::Add_Components(void* pArg)
{


	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialMapBridge"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseCom))))
		return E_FAIL;




	return S_OK;
}

HRESULT CTutorialMapBridge::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CTutorialMapBridge* CTutorialMapBridge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTutorialMapBridge* pInstance = new CTutorialMapBridge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTutorialMapBridge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTutorialMapBridge::Clone(void* pArg)
{
	CTutorialMapBridge* pInstance = new CTutorialMapBridge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTutorialMapBridge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTutorialMapBridge::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseCom);
	Safe_Release(m_pShaderCom);
}
