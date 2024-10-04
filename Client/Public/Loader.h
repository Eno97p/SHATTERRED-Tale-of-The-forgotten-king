#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CLoader() = default;

public:
	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

	_bool is_Finished() const {
		for (_uint i = 0; i < USED_THREAD_COUNT; ++i)
		{
			if (m_bIsFinish[i] == true)
			{

				return m_isFinished;
			}
			else
			{
				return false;
			}
		}
	}

public:	
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT Loading();
	HRESULT Loading_Map();
	HRESULT Load_LevelData(const _tchar* pFilePath);
	HRESULT Loading_Shader();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };	



private:
	void Finish_Thread(const _uint& iIndex)
	{
		m_bIsFinish[iIndex] = true;
		g_IsThreadFinish[iIndex] = true;  //전역 변수
		m_iFinishedThreadCount++;
		if (m_iFinishedThreadCount == USED_THREAD_COUNT)
		{
			
			lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
			m_isFinished = true;
		}
	}

private:
	/*For. Thread*/
	
	atomic<bool> 		m_bIsFinish[MAX_THREAD];
	atomic<int>			m_iFinishedThreadCount;


	/* atomic<T>  */
		//원자성 보장 : 연산이 중간에 끊기지 않고 완전히 수행됩니다.
		//동기화 오버헤드 감소 : 락(lock)을 사용하지 않고도 스레드 간 동기화가 가능합니다.
		//데이터 경쟁 방지 : 여러 스레드가 동시에 같은 데이터에 접근해도 안전합니다.
		//메모리 순서 보장 : 컴파일러나 CPU의 최적화로 인한 순서 변경을 방지합니다.



	HANDLE					m_hThread[MAX_THREAD] = { 0 };
	CRITICAL_SECTION		m_Critical_Section[MAX_THREAD] = {};

	
private:
	LEVEL					m_eNextLevel = { LEVEL_END };
	_tchar					m_szLoadingText[MAX_PATH] = { TEXT("") };
	_bool					m_isFinished = { false };
	CGameInstance*			m_pGameInstance = { nullptr };

	

private:
	HRESULT Load_LevelData(LEVEL eLevel, const _tchar* pFilePath);

	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_LogoLevel_For_Shader();
	
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_GamePlayLevel_For_Shader();

	HRESULT Loading_For_AckbarLevel();
	HRESULT Loading_For_AckbarLevel_For_Shader();

	HRESULT Loading_For_JugglasLevel();
	HRESULT Loading_For_JugglasLevel_For_Shader();

	HRESULT Loading_For_AndrasArenaLevel();
	HRESULT Loading_For_AndrasArenaLevel_For_Shader();

	HRESULT Loading_For_GrassLandLevel();
	HRESULT Loading_For_GrassLandLevel_For_Shader();
public:
	static CLoader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevel);
	virtual void Free() override;



};

END