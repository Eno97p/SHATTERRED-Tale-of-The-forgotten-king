#include "UI_UpGPage_Value.h"

#include "GameInstance.h"
#include "Inventory.h"

CUI_UpGPage_Value::CUI_UpGPage_Value(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpGPage_Value::CUI_UpGPage_Value(const CUI_UpGPage_Value& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpGPage_Value::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpGPage_Value::Initialize(void* pArg)
{
	UI_VALUE_DESC* pDesc = static_cast<UI_VALUE_DESC*>(pArg);

	m_eValueType = pDesc->eValueType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (VALUE_SOUL == m_eValueType)
	{
		m_fX = (g_iWinSizeX >> 1) - 70.f; //65
		m_fY = (g_iWinSizeY >> 1) + 165.f;
		m_fSizeX = 128.f; // 128   85.3f
	}
	else if (VALUE_MATERIAL == m_eValueType)
	{
		m_fX = (g_iWinSizeX >> 1) + 30.f; // 35
		m_fY = (g_iWinSizeY >> 1) + 165.f;
		m_fSizeX = 100.f; // 128   85.3f
	}

	m_fSizeY = 42.6f; // 64

	Setting_Position();

	return S_OK;
}

void CUI_UpGPage_Value::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpGPage_Value::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Update_Jemstone();
}

void CUI_UpGPage_Value::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_UpGPage_Value::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Rend_Font();

	return S_OK;
}

void CUI_UpGPage_Value::Update_Value(_uint iCurSlotIdx)
{
	if (CInventory::GetInstance()->Get_WeaponSize() - 1 < iCurSlotIdx) // 빈 슬롯 예외 처리
		return;

	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	for (size_t i = 0; i < iCurSlotIdx; ++i)
		++weapon;

	m_iPrive = (*weapon)->Get_Price() + ((*weapon)->Get_Level() * 5); // Level에 따라 필요 재료 늘어나도록
	m_iValue = (*weapon)->Get_Value() + ((*weapon)->Get_Level());
}

HRESULT CUI_UpGPage_Value::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_value"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPage_Value::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPage_Value::Rend_Font()
{
	if (VALUE_SOUL == m_eValueType)
	{
		_vector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);
		if (CInventory::GetInstance()->Get_Soul() < m_iPrive)
		{
			vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
			m_isEnough = false;
		}
		else
		{
			m_isEnough = true;
		}

		m_wstrText = to_wstring(CInventory::GetInstance()->Get_Soul()) + TEXT(" / ") + to_wstring(m_iPrive);

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo"), m_wstrText, _float2(m_fX - 35.f, m_fY - 10.f), vColor)))
			return E_FAIL;
	}
	else if (VALUE_MATERIAL == m_eValueType)
	{
		_vector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);
		if (m_iJemstone < m_iValue)
		{
			vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
			m_isEnough = false;
		}
		else
		{
			m_isEnough = true;
		}

		m_wstrText = to_wstring(m_iJemstone) + TEXT(" / ") + to_wstring(m_iValue);

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), m_wstrText, _float2(m_fX - 20.f, m_fY - 10.f), vColor)))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_UpGPage_Value::Update_Jemstone()
{
	vector<CItemData*>::iterator item = CInventory::GetInstance()->Get_ItemDatas()->begin();
	for (size_t i = 0; i < CInventory::GetInstance()->Get_vecItemSize(); ++i)
	{
		// Level에 따른 분기 처리도 필요함 (3렙 이상이면 더 강화된 재료 써야하는 식)

		if ((*item)->Get_ItemNameText() == TEXT("HADRONITE")) // 이름 비교해서 분기 처리하기
		{
			// 해당 아이템의 현재 개수 몇 개 들고 있는지 받아오기
			m_iJemstone = (*item)->Get_Count();
		}
		else
		{
			++item;
			m_iJemstone = 0;
		}
	}
}

CUI_UpGPage_Value* CUI_UpGPage_Value::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpGPage_Value* pInstance = new CUI_UpGPage_Value(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpGPage_Value");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpGPage_Value::Clone(void* pArg)
{
	CUI_UpGPage_Value* pInstance = new CUI_UpGPage_Value(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpGPage_Value");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpGPage_Value::Free()
{
	__super::Free();
}
