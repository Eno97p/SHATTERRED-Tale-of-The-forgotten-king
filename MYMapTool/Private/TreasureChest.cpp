#include "TreasureChest.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
#include "Player.h"

CTreasureChest::CTreasureChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CTreasureChest::CTreasureChest(const CTreasureChest& rhs)
	: CToolObj(rhs)
{
}

HRESULT CTreasureChest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTreasureChest::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_TreasureChest");
		strcpy_s(m_szLayer, "Layer_Active_Element");
		strcpy_s(m_szModelName, "Prototype_Component_Model_TreasureChest");
		m_eModelType = CModel::TYPE_ANIM;
		m_iTriggerType = pDesc->TriggerType;

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

		//TRIGGER STATE SET
	}
	switch (m_iTriggerType)
	{
	case TREASURE_NORMAL:
		m_iShaderPath = 0;
		m_iBloomShaderPath = 2;
		break;
	case TREASURE_EPIC:
		m_iShaderPath = 3;
		m_iBloomShaderPath = 4;
		m_TreasureColor = { 0.265282571f, 0.f, 0.5637f, 1.f };
		break;
	case TREASURE_CLOAKING:
		m_iShaderPath = 3;
		m_iBloomShaderPath = 4;
		m_TreasureColor = {0.161764681f,  0.161764681f, 0.161764681, 1.f };
		break;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, false));

	m_pModelCom->Play_Animation(0.001);

	return S_OK;
}

void CTreasureChest::Priority_Tick(_float fTimeDelta)
{
	if(m_pGameInstance->Key_Down(DIK_F))
	{
		m_bChestOpened = true;
	}
}

void CTreasureChest::Tick(_float fTimeDelta)
{
	if (m_bChestOpened)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CTreasureChest::Late_Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif

	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CTreasureChest::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
		//	return E_FAIL;


		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
		//	return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
		//	return E_FAIL;

		//if ( i != 29)
		_float4 col = CImgui_Manager::GetInstance()->Get_GlobalColorPicker();
		if (FAILED(m_pShaderCom->Bind_RawValue("g_EpicColor", &col, sizeof(_float4))))
			return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
		//	return E_FAIL;

		i == 2 ? m_pShaderCom->Begin(m_iShaderPath) : m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
}

HRESULT CTreasureChest::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_iTest++;
	}

	if (m_pGameInstance->Key_Down(DIK_0))
	{
		m_iTest--;
	}


	
	_float4 col = CImgui_Manager::GetInstance()->Get_GlobalColorPicker();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EpicColor", &col, sizeof(_float4))))
		return E_FAIL;

	m_pShaderCom->Unbind_SRVs();

	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", 2);

	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", 2, aiTextureType_EMISSIVE)))
		return E_FAIL;


	m_pShaderCom->Begin(m_iBloomShaderPath);

	m_pModelCom->Render(2);
}

HRESULT CTreasureChest::Add_Components(void* pArg)
{
	wstring wstr = string_to_wstring(m_szModelName);

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CTreasureChest::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	return S_OK;
}

CTreasureChest* CTreasureChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTreasureChest* pInstance = new CTreasureChest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTreasureChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasureChest::Clone(void* pArg)
{
	CTreasureChest* pInstance = new CTreasureChest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTreasureChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreasureChest::Free()
{
	__super::Free();
}
