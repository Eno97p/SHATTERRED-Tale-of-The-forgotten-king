#include "Bone_Sphere.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"

CBone_Sphere::CBone_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CBone_Sphere::CBone_Sphere(const CBone_Sphere& rhs)
    : CGameObject{rhs}
{
}

HRESULT CBone_Sphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBone_Sphere::Initialize(void* pArg)
{
    BONESPHERE_DSC* pDesc = static_cast<BONESPHERE_DSC*>(pArg);

    m_iBoneIdx = pDesc->iBoneIdx;
    m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;
    m_pParentMatrix = pDesc->pParentMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fScale = 2.f;

    return S_OK;
}

void CBone_Sphere::Priority_Tick(_float fTimeDelta)
{
}

void CBone_Sphere::Tick(_float fTimeDelta)
{
    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
    SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
    SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

    m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);
}

void CBone_Sphere::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBone_Sphere::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pShaderCom->Unbind_SRVs();

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(4);

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CBone_Sphere::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bone_Sphere"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBone_Sphere::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _int iBoneIdx = CImgui_Manager::GetInstance()->Get_BoneIdx();
    _float4 fColor = _float4(0.f, 1.f, 0.f, 1.f);

    if (m_iBoneIdx != iBoneIdx)
    {
        m_fScale = 2.f;
        fColor = _float4(0.f, 1.f, 0.f, 1.f);
    }
    else
    {
        m_fScale = 5.f;
        fColor = _float4(1.f, 0.f, 0.f, 1.f);
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &fColor, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CBone_Sphere* CBone_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBone_Sphere* pInstance = new CBone_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBone_Sphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBone_Sphere::Clone(void* pArg)
{
    CBone_Sphere* pInstance = new CBone_Sphere(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CBone_Sphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBone_Sphere::Free()
{
    __super::Free();

    //Safe_Delete(m_pParentMatrix);
    //Safe_Delete(m_pSocketMatrix);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
