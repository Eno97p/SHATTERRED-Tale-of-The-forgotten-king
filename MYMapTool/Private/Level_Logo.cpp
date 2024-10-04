#include "Level_Logo.h"

#include "GameInstance.h"

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{pDevice, pContext}
{

}

HRESULT CLevel_Logo::Initialize()
{
    return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        // 로딩으로 넘어가기
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
            return;
    }
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Level : Logo"));
#endif
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : Level_Logo");
        return nullptr;
    }

    return pInstance;
}

void CLevel_Logo::Free()
{
    __super::Free();
    /*Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);*/
}
