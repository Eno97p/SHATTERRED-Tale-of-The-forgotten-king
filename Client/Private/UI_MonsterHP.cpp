#include "UI_MonsterHP.h"

#include "GameInstance.h"
#include "Monster.h"

CUI_MonsterHP::CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_MonsterHP::CUI_MonsterHP(const CUI_MonsterHP& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_MonsterHP::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MonsterHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 위치는 몬스터 따라 가야 함
	/*m_fX = 390.f;
	m_fY = 45.f;
	m_fSizeX = 768.f;
	m_fSizeY = 24.f;*/

	//Setting_Position();

	return S_OK;
}

void CUI_MonsterHP::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MonsterHP::Tick(_float fTimeDelta)
{
	if (m_fCurrentRatio < m_fPastRatio)
	{
		m_fPastRatio -= fTimeDelta * 0.2f;
		if (m_fCurrentRatio > m_fPastRatio)
		{
			m_fPastRatio = m_fCurrentRatio;
		}
	}
}

void CUI_MonsterHP::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->BillBoard();

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_MonsterHP::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_CurrentRatio"), &m_fCurrentRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_PastRatio"), &m_fPastRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue(("g_HudRatio"), &m_fHudRatio, sizeof(_float))))
		return E_FAIL;

	m_pShaderCom->Begin(2); // 비율을 위해 2로 하니 출력 크기가 안 맞는 이슈 있음
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_MonsterHP::Update_Pos(_vector vMonsterPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
}

HRESULT CUI_MonsterHP::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHP"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MonsterHP::Bind_ShaderResources()
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

CUI_MonsterHP* CUI_MonsterHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MonsterHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MonsterHP::Clone(void* pArg)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MonsterHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MonsterHP::Free()
{
	__super::Free();

	Safe_Release(m_pMonster);
}
