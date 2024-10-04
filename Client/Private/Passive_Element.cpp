#include "stdafx.h"
#include "..\Public\Passive_Element.h"

#include "GameInstance.h"
#include "Camera.h"

CPassive_Element::CPassive_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Element{ pDevice, pContext }
{
}

CPassive_Element::CPassive_Element(const CPassive_Element& rhs)
    : CMap_Element{ rhs }
{
}

HRESULT CPassive_Element::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPassive_Element::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MAP_ELEMENT_DESC* desc = (MAP_ELEMENT_DESC*)(pArg);

    // WorldMats의 복사본 생성
    m_WorldMats.reserve(desc->WorldMats.size());
    for (const auto& mat : desc->WorldMats)
    {
        m_WorldMats.push_back(new _float4x4(*mat));
    }

    if (FAILED(Add_Components(desc)))
        return E_FAIL;

    CVIBuffer_Instance::INSTANCE_MAP_DESC instanceDesc{};
    instanceDesc.WorldMats = m_WorldMats;  // 복사본 사용
    instanceDesc.iNumInstance = desc->iInstanceCount;
    m_pModelCom->Ready_Instance_ForMapElements(instanceDesc);

    m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
    return S_OK;
}

void CPassive_Element::Late_Tick(_float fTimeDelta)
{
    //FOR CULLING
   // _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    //if (!m_bNoCullElement)
    //{
    //    if (m_pGameInstance->isVisible(vPos, m_pPhysXCom->Get_Actor()))
    //        //if (m_pGameInstance->IsVisibleObject(this))
    //    {
    //        m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
    //    }
    //}
    //else 
    if(!m_bHiddenObject)
    {
        if(m_fDisolveValue <1.0f)
            m_fDisolveValue += fTimeDelta * 0.5f;
        m_pGameInstance->Add_RenderObject(CRenderer::RENDER_MIRROR, this);
        m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
        if (m_iShaderPath != 3)
        {
            if (m_pGameInstance->Get_NotMoveShadow())
            {
                m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
            }
        }
    }



#ifdef _DEBUG

    for (_int i = 0; i < m_iInstanceCount; ++i)
    {
        m_pGameInstance->Add_DebugComponent(m_pPhysXCom[i]);

    }
#endif


}

HRESULT CPassive_Element::Render()
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

            if (FAILED(m_pModelCom->Bind_Material_Instance_ForMapElements(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Bind_Material_Instance_ForMapElements(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
                return E_FAIL;

            
        }
       
        m_pShaderCom->Begin(m_iShaderPath);

        /*   if (FAILED(m_pModelCom->Render(i)))
               return E_FAIL;   */

        if (FAILED(m_pModelCom->Render_Instance_ForMapElements(i)))
            return E_FAIL;
    }

#pragma region 모션블러
    m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

    return S_OK;
}



HRESULT CPassive_Element::Render_LightDepth()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;

    _float4x4      ViewMatrix, ProjMatrix;

    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_ShadowEye(), m_pGameInstance->Get_ShadowFocus(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
    //XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(16384, 9216, 0.1f, 3000.f));
    XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_Instance_ForMapElements(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(4);

        if (FAILED(m_pModelCom->Render_Instance_ForMapElements(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPassive_Element::Render_Mirror()
{
    //if (FAILED(Bind_ShaderResources()))
    //    return E_FAIL;

    //_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    //for (size_t i = 0; i < iNumMeshes; i++)
    //{
    //    if (i != 1) continue;

    //    m_pShaderCom->Unbind_SRVs();

    //    if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
    //        return E_FAIL;

    //    m_pShaderCom->Begin(1);

    //    if (FAILED(m_pModelCom->Render_Instance_ForMapElements(i)))
    //        return E_FAIL;
    //}
    return S_OK;
}

HRESULT CPassive_Element::Add_Components(MAP_ELEMENT_DESC* desc)
{
    // LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str(),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (desc->wstrModelName == TEXT("Prototype_Component_Model_BasicCube") || desc->wstrModelName == TEXT("Prototype_Component_Model_BasicGround"))
    {
        m_bNoCullElement = true;
        m_iShaderPath = 3;
    }
    else if (desc->wstrModelName == TEXT("Prototype_Component_Model_Hoverboard_Track") || desc->wstrModelName == TEXT("Prototype_Component_Model_Hoverboard_Track_Pillar"))
    {
        m_bHiddenObject = true;
    }

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (m_bHiddenObject)
    {
        /* For.Com_Texture */
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
            return E_FAIL;
    }
    else
    {
		m_fDisolveValue = 1.f;
	}

    //PHYSX COM 배열
    // wstrModelName을 수정하여 physxName 생성
    wstring physxName = desc->wstrModelName;
    size_t pos = physxName.find(L"Model_");
    if (pos != wstring::npos)
    {
        physxName.replace(pos, 6, L"PhysX_");
    }

    CPhysXComponent::PHYSX_DESC      PhysXDesc{};
    PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
    PhysXDesc.pComponent = m_pModelCom;
    PhysXDesc.eGeometryType = PxGeometryType::eTRIANGLEMESH;
    PhysXDesc.filterData.word0 = GROUP_ENVIRONMENT;

    m_iInstanceCount = desc->iInstanceCount;
    m_pPhysXCom.resize(m_iInstanceCount);

    for (_int i = 0; i < m_iInstanceCount; ++i)
    {
        wstring idxStr = to_wstring(i);
        


        std::string nameComStr = wstring_to_string(physxName);
        PhysXDesc.pName = nameComStr.c_str();


        
        XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, XMLoadFloat4x4(desc->WorldMats[i]));
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, physxName.c_str(),
            TEXT("Com_PhysX") + idxStr, reinterpret_cast<CComponent**>(&m_pPhysXCom[i]), &PhysXDesc)))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CPassive_Element::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

#pragma region 모션블러
    if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
        return E_FAIL;
    _bool bMotionBlur = m_pGameInstance->Get_MotionBlur();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
        return E_FAIL;
#pragma endregion 모션블러

    _float fCamFar = m_pGameInstance->Get_MainCamera()->Get_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    if (m_pDisolveTextureCom)
    {
        if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
            return E_FAIL;
    }



    if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
        return E_FAIL;


    return S_OK;
}

CPassive_Element* CPassive_Element::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPassive_Element* pInstance = new CPassive_Element(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CPassive_Element");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPassive_Element::Clone(void* pArg)
{
    CPassive_Element* pInstance = new CPassive_Element(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CPassive_Element");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPassive_Element::Free()
{
    __super::Free();

    for (auto& mat : m_WorldMats)
    {
        Safe_Delete(mat);
    }

    for (_int i = 0; i < m_iInstanceCount; ++i)
    {
        Safe_Release(m_pPhysXCom[i]);
        m_pPhysXCom[i] = nullptr;
    }
    m_pPhysXCom.clear();
    Safe_Release(m_pDisolveTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
