#include "UI_ItemIcon.h"

#include "GameInstance.h"

#include "UI_RedDot.h"

CUI_ItemIcon::CUI_ItemIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ItemIcon::CUI_ItemIcon(const CUI_ItemIcon& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_ItemIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ItemIcon::Initialize(void* pArg)
{
	UI_ITEMICON_DESC* pDesc = static_cast<UI_ITEMICON_DESC*>(pArg);

	m_eUISort = pDesc->eUISort;
	m_wszTexture = pDesc->wszTexture;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX; // 512
	m_fSizeY = pDesc->fSizeY; // 512

	Setting_Position();

	return S_OK;
}

void CUI_ItemIcon::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ItemIcon::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	if (nullptr != m_pRedDot)
		m_pRedDot->Tick(fTimeDelta);
}

void CUI_ItemIcon::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort);

	if (nullptr != m_pRedDot)
		m_pRedDot->Late_Tick(fTimeDelta);
}

HRESULT CUI_ItemIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_ItemIcon::Change_Texture(wstring wstrTextureName)
{
	// Texture를 변경하기
	Safe_Release(m_pTextureCom);
	if (Delete_Component(TEXT("Com_Texture")));

	m_wszTexture = wstrTextureName;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_wszTexture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return;
}

void CUI_ItemIcon::Update_Pos(_float fX, _float fY)
{
	m_fX = fX;
	m_fY = fY;
	Setting_Position();
}

HRESULT CUI_ItemIcon::Create_RedDot()
{
	if (nullptr != m_pRedDot)
		return S_OK;

	CUI_RedDot::UI_REDDOT_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.eUISort = static_cast<UISORT_PRIORITY>(m_eUISort + 2);
	pDesc.fX = m_fX + 25.f;
	pDesc.fY = m_fY - 25.f;
	pDesc.fSizeX = 15.f;
	pDesc.fSizeY = 15.f;

	m_pRedDot = dynamic_cast<CUI_RedDot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_RedDot"), &pDesc));
	if (nullptr == m_pRedDot)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ItemIcon::Delete_RedDot()
{
	Safe_Release(m_pRedDot);
	m_pRedDot = nullptr;

	return S_OK;
}

HRESULT CUI_ItemIcon::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_wszTexture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_ItemIcon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_ItemIcon* CUI_ItemIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ItemIcon* pInstance = new CUI_ItemIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_ItemIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_ItemIcon::Clone(void* pArg)
{
	CUI_ItemIcon* pInstance = new CUI_ItemIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_ItemIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ItemIcon::Free()
{
	__super::Free();

	Safe_Release(m_pRedDot);
}
