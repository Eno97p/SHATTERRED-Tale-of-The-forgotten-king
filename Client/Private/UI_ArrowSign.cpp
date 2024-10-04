#include "UI_ArrowSign.h"

#include "GameInstance.h"

CUI_ArrowSign::CUI_ArrowSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ArrowSign::CUI_ArrowSign(const CUI_ArrowSign& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_ArrowSign::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ArrowSign::Initialize(void* pArg)
{
	UI_ARROWSIGN_DESC* pDesc = static_cast<UI_ARROWSIGN_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_vector vPos = pDesc->vPos;
	vPos.m128_f32[1] += 5.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fDefaultY = vPos.m128_f32[1];

	return S_OK;
}

void CUI_ArrowSign::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ArrowSign::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);
}

void CUI_ArrowSign::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->BillBoard();

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_ArrowSign::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();


	return S_OK;
}
 
HRESULT CUI_ArrowSign::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ArrowSign"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ArrowSign::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

void CUI_ArrowSign::Move(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fY = vPos.m128_f32[1];
	
	if (m_isUpAnim) // 올라가는 중
	{
		fY -= fTimeDelta * 1.f;

		if (fY <= (m_fDefaultY - ANIM_Y))
		{
			fY = m_fDefaultY - ANIM_Y;
			m_isUpAnim = false;
		}
	}
	else // 내려가는 중
	{
		fY += fTimeDelta * 1.f;

		if (fY >= (m_fDefaultY + ANIM_Y))
		{
			fY = m_fDefaultY + ANIM_Y;
			m_isUpAnim = true;
		}
	}

	vPos.m128_f32[1] = fY;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

CUI_ArrowSign* CUI_ArrowSign::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ArrowSign* pInstance = new CUI_ArrowSign(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_ArrowSign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_ArrowSign::Clone(void* pArg)
{
	CUI_ArrowSign* pInstance = new CUI_ArrowSign(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_ArrowSign");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ArrowSign::Free()
{
	__super::Free();
}
