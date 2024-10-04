#include "UI_LoadingCircle.h"

#include "GameInstance.h"

CUI_LoadingCircle::CUI_LoadingCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_LoadingCircle::CUI_LoadingCircle(const CUI_LoadingCircle& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_LoadingCircle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LoadingCircle::Initialize(void* pArg)
{
	UI_CIRCLE_DESC* pDesc = static_cast<UI_CIRCLE_DESC*>(pArg);

	m_eCircleType = pDesc->eCircleType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	m_vFontColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	Setting_Data();

	return S_OK;
}

void CUI_LoadingCircle::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LoadingCircle::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Turn_Animation(fTimeDelta);

	Change_FontColor(fTimeDelta);

}

void CUI_LoadingCircle::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTEENTH);
}

HRESULT CUI_LoadingCircle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if(CIRCLE_TWO == m_eCircleType)
		Render_Font();

	return S_OK;
}

HRESULT CUI_LoadingCircle::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingCircle"),  
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingCircle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_eCircleType)))
		return E_FAIL;

	_float4x4 rotationMatrix;
	XMStoreFloat4x4(&rotationMatrix, m_RotationMatrix);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_RotationMatrix", &rotationMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;*/

	return S_OK;
}

void CUI_LoadingCircle::Turn_Animation(_float fTimeDelta)
{
	if(CIRCLE_ONE == m_eCircleType)
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(5.f);
	else
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(-5.f);

	if (m_fRotationAngle > XM_2PI)
		m_fRotationAngle -= XM_2PI;

	m_RotationMatrix = XMMatrixRotationZ(m_fRotationAngle); // XMMatrix
}

void CUI_LoadingCircle::Render_Font()
{
	//Setting_Text();
	// ����� ���ƿ��� �� ��°��!!!!!!! �ٽ� m_wstrTex�� ���ư��°�?
	// ��°����°����°��? �� �и� Setting_Data( )���� ����� �� ������ �Ѿ�� �� Ȯ���ߴµ�
	// �ε� ȭ������ �Ѿ�� �Ʒ��� �ڵ忡���� �״���ΰ�???

	// �� �ٲٰ� ���� Level Loading�� �Ǿ ����� ������ 1�� �ʱ�ȭ�� �Ǿ��ִ�?�Ф�

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_HeirofLight15"), m_wstrText,
		_float2((g_iWinSizeX>> 1) - 340.f, (g_iWinSizeY >> 1) - 65.f), XMVectorSet(m_fFontRGB, m_fFontRGB, m_fFontRGB, 1.f))))
		return;
}

void CUI_LoadingCircle::Change_FontColor(_float fTimeDelta)
{
	m_fFontTimer += fTimeDelta;


	if (m_isFontOn)
	{
		m_fFontRGB += 0.02f;
		if (m_fFontRGB >= 1.f)
			m_fFontRGB = 1.f;
		if (5.f <= m_fFontTimer)
		{
			m_isFontOn = false;
			m_fFontTimer = 0.f;
		}
	}
	else
	{
		m_fFontRGB -= 0.02f;
		if (m_fFontRGB <= 0.f)
			m_fFontRGB = 0.f;
		if (0.4f <= m_fFontTimer)
		{
			m_isFontOn = true;
			m_fFontTimer = 0.f;
		}
	}
}

void CUI_LoadingCircle::Setting_Data()
{
	// ���⼭ ���� �ٲٴ� �� ��ü�� �� ��.... �ٵ� ������ ^^^
	if (0 == m_iTextNum)
	{
		m_wstrText = TEXT("              ��Ī GEODESIAN�̶�� �ڿ� ���� �˷��� ���� ���� ����.\n"
			TEXT("                               �״� �����ڶ��, ���ڶ�� �Ҹ���.\n")
			TEXT("�״� GEODESIC ����Ʈ��� ������ �������� â���� ������ �� ���� ���ΰ�?\n")
			TEXT("                                        �����θ� â���ڷ� ���� ���ΰ�."));
	}
	else if (1 == m_iTextNum)
	{
		m_wstrText = TEXT("                ���θ��׿콺�� ������ ����� Ǯ������� ��� ���� �����ƴ�.\n"
			TEXT("                               �����뽺�� ���� �� ��ġ �ʸӱ�����.\n")
			TEXT("�ڽ��� ���Ŀ� ���� ����� ��� �׳�� �ֺ��� ���� �ν��� ������ �Ҿ��.\n"));
	}
	else if (2 == m_iTextNum)
	{
		m_wstrText = TEXT("�״��� �߰����� ������ ������ ���ظ� ��� �ź��� ���� ������ ã�ư��� ����,\n"
			TEXT("                                    ��� �ӿ����� ���� �߰��ϸ���.\n")
			TEXT("               ���� ������ �ǻ�� �� ����� ������ ���ο� ������ �����϶�.\n")
			TEXT("           ������ ������ ����� Ǯ���. ���� ����� �״븦 ��ٸ��� �ִ�.\n\n")
			TEXT("                            ��� �ӿ��� ������ ������ ��Ⱑ �ִ°�?"));
	}
	else if (3 == m_iTextNum)
	{
		m_wstrText = TEXT("                                                �������� ������ ����.\n"
			TEXT("������ ���� Ȳ�� �ô븦 ��ã�� ���ؼ��� ������ ��а� ������ ��Ⱑ �ʿ��ϴ�.\n")
			TEXT("    ������ �̷��� ���� �������� ������ �״��� ������ ��� ���� �ٲ� ���̴�.\n"));
	}

	m_iTextNum++;
}

void CUI_LoadingCircle::Setting_Text()
{
	//if (0 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("              ��Ī GEODESIAN�̶�� �ڿ� ���� �˷��� ���� ���� ����.\n"
	//		TEXT("                               �״� �����ڶ��, ���ڶ�� �Ҹ���.\n")
	//		TEXT("�״� GEODESIC ����Ʈ��� ������ �������� â���� ������ �� ���� ���ΰ�?\n")
	//		TEXT("                                        �����θ� â���ڷ� ���� ���ΰ�."));
	//}
	//else if (1 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("                ���θ��׿콺�� ������ ����� Ǯ������� ��� ���� �����ƴ�.\n"
	//		TEXT("                               �����뽺�� ���� �� ��ġ �ʸӱ�����.\n")
	//		TEXT("�ڽ��� ���Ŀ� ���� ����� ��� �׳�� �ֺ��� ���� �ν��� ������ �Ҿ��.\n"));
	//}
	//else if (2 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("�״��� �߰����� ������ ������ ���ظ� ��� �ź��� ���� ������ ã�ư��� ����,\n"
	//		TEXT("                                    ��� �ӿ����� ���� �߰��ϸ���.\n")
	//		TEXT("               ���� ������ �ǻ�� �� ����� ������ ���ο� ������ �����϶�.\n")
	//		TEXT("           ������ ������ ����� Ǯ���. ���� ����� �״븦 ��ٸ��� �ִ�.\n\n")
	//		TEXT("                            ��� �ӿ��� ������ ������ ��Ⱑ �ִ°�?"));
	//}
	//else if (3 == m_iTextNum)
	//{
	//	m_wstrText = TEXT("                                                �������� ������ ����.\n"
	//		TEXT("������ ���� Ȳ�� �ô븦 ��ã�� ���ؼ��� ������ ��а� ������ ��Ⱑ �ʿ��ϴ�.\n")
	//		TEXT("    ������ �̷��� ���� �������� ������ �״��� ������ ��� ���� �ٲ� ���̴�.\n"));
	//}
}

CUI_LoadingCircle* CUI_LoadingCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LoadingCircle::Clone(void* pArg)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LoadingCircle::Free()
{
	__super::Free();
}
