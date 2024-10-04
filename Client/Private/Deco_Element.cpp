#include "stdafx.h"
#include "..\Public\Deco_Element.h"

#include "GameInstance.h"


CDeco_Element::CDeco_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Element{ pDevice, pContext }
{
}

CDeco_Element::CDeco_Element(const CDeco_Element& rhs)
    : CMap_Element{ rhs }
{
}

HRESULT CDeco_Element::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDeco_Element::Initialize(void* pArg)
{

    CTransform::TRANSFORM_DESC td{};
    td.fRotationPerSec = XMConvertToRadians(3.f);
    td.fSpeedPerSec = 30.f;

    if (FAILED(__super::Initialize(&td)))
        return E_FAIL;


    


    MAP_ELEMENT_DESC* desc = (MAP_ELEMENT_DESC*)(pArg);
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&desc->mWorldMatrix));
    m_vOriginPos = { desc->mWorldMatrix.m[3][0], desc->mWorldMatrix.m[3][1], desc->mWorldMatrix.m[3][2], 1.f };
    if (FAILED(Add_Components((MAP_ELEMENT_DESC*)(pArg))))
        return E_FAIL;

    /*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 3.f, 5.f, 1.f));
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);*/

    //m_pTransformCom->Scaling(100.f, 100.f, 100.f);
   // 
   //CVIBuffer_Instance::INSTANCE_MAP_DESC instanceDesc{};
   //instanceDesc.WorldMats = desc->WorldMats;
   //instanceDesc.iNumInstance = desc->iInstanceCount;
   //m_pModelCom->Ready_Instance_ForMapElements(instanceDesc);

    //FOR CULLING
    //m_pGameInstance->AddCullingObject(this, m_pPhysXCom->Get_Actor());

    return S_OK;

}


void CDeco_Element::Late_Tick(_float fTimeDelta)
{
    if (m_bRotateObj)
    {
        m_pTransformCom->Floating_Y(fTimeDelta, 1.5f, 70.f, m_vOriginPos);
        m_pTransformCom->Turn(m_vRotateAxis, fTimeDelta * m_fRotateOffset);
    }
    //FOR CULLING
    //_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    //if (!m_bNoCullElement)
    //{
    //    if (m_pGameInstance->isVisible(vPos, m_pPhysXCom->Get_Actor()))
    //        //if (m_pGameInstance->IsVisibleObject(this))
    //    {
    //        m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
    //    }
    //}
    //else 
   // if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f));
    {
      // m_pGameInstance->Add_RenderObject(CRenderer::RENDER_MIRROR, this);
       m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
    }
}

HRESULT CDeco_Element::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (m_pGameInstance->Key_Down(DIK_UP))
    {
        m_iTest++;
    }


    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        //if (i == 0)
        {
            m_pShaderCom->Unbind_SRVs();

         /*   if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
                return E_FAIL;  */
            
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
                return E_FAIL;
            

			   /* if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
					return E_FAIL;*/

		/*		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
					return E_FAIL;*/
		}
		//else if (i == 1)
		//{
		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
		//		return E_FAIL;
		//}

            /*   if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
                  return E_FAIL;

               if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
                  return E_FAIL;

               if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
                  return E_FAIL;

               if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
                  return E_FAIL;*/
        //else if (i == 1)
        //{
        //   if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
        //      return E_FAIL;
        //}


        
        //_uint red = i;
        //if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &red, sizeof(_uint))))
        //    return E_FAIL;

        //if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
        //    return E_FAIL;

        m_pShaderCom->Begin(0);

     /*   if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;   */
        
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

#pragma region 모션블러
    m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
    m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
    return S_OK;
}

HRESULT CDeco_Element::Render_Mirror()
{
    return S_OK;
}

HRESULT CDeco_Element::Add_Components(MAP_ELEMENT_DESC* desc)
{
    // LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str(),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (desc->wstrModelName == TEXT("Prototype_Component_Model_BasicCube") || desc->wstrModelName == TEXT("Prototype_Component_Model_BasicGround"))
    {
        m_bNoCullElement = true;
    }
    else if (desc->wstrModelName == TEXT("Prototype_Component_Model_AndrasArena_Deco"))
    {
        m_bRotateObj = true;

        if (desc->mWorldMatrix.m[0][0] == -0.5f)
        {
            m_fRotateOffset = 0.5f;
            m_vRotateAxis = { 0.f, -1.f, 0.f, 0.f };
        }
    }
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CDeco_Element::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
#pragma region 모션블러
    if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
        return E_FAIL;
    _bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
        return E_FAIL;
#pragma endregion 모션블러
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_MainCamera()->Get_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CDeco_Element* CDeco_Element::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDeco_Element* pInstance = new CDeco_Element(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CDeco_Element");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDeco_Element::Clone(void* pArg)
{
    CDeco_Element* pInstance = new CDeco_Element(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CDeco_Element");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDeco_Element::Free()
{
    __super::Free();

}
