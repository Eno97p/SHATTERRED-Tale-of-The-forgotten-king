#include "ToolEffect.h"

#include "GameInstance.h"

CToolEffect::CToolEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CToolEffect::CToolEffect(const CToolEffect& rhs)
	: CGameObject{rhs}
{
}

HRESULT CToolEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((TOOLEFFECT_DESC*)pArg)->vStartPos));
	}

	return S_OK;
}

void CToolEffect::Priority_Tick(_float fTimeDelta)
{
}

void CToolEffect::Tick(_float fTimeDelta)
{
	// Effect Binary File에서 읽어온 데이터로 Tick을 돌려주어야 함
}

void CToolEffect::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CToolEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CToolEffect::Add_Components()
{
	return S_OK;
}

HRESULT CToolEffect::Bind_ShaderResources()
{
	return S_OK;
}

CToolEffect* CToolEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolEffect* pInstance = new CToolEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CToolEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolEffect::Clone(void* pArg)
{
	CToolEffect* pInstance = new CToolEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CToolEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolEffect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
