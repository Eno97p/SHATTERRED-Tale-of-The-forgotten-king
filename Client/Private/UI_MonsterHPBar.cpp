#include "UI_MonsterHPBar.h"

#include "GameInstance.h"

CUI_MonsterHPBar::CUI_MonsterHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_MonsterHPBar::CUI_MonsterHPBar(const CUI_MonsterHPBar& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_MonsterHPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MonsterHPBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_fX = (g_iWinSizeX >> 1) + 50.f;
	//m_fY = g_iWinSizeY - 100.f;
	//m_fSizeX = 1265.3f; // 2048
	//m_fSizeY = 128.f; // 128

	//Setting_Position();

	return S_OK;
}

void CUI_MonsterHPBar::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MonsterHPBar::Tick(_float fTimeDelta)
{
}

void CUI_MonsterHPBar::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->BillBoard();

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_MonsterHPBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_MonsterHPBar::Update_Pos(_vector vMonsterPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
}

HRESULT CUI_MonsterHPBar::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHPBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MonsterHPBar::Bind_ShaderResources()
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

CUI_MonsterHPBar* CUI_MonsterHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MonsterHPBar* pInstance = new CUI_MonsterHPBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MonsterHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MonsterHPBar::Clone(void* pArg)
{
	CUI_MonsterHPBar* pInstance = new CUI_MonsterHPBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MonsterHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MonsterHPBar::Free()
{
	__super::Free();
}
