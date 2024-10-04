#include "SoundMgr.h"

CSoundMgr::CSoundMgr()
{
}

HRESULT CSoundMgr::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	if (FMOD_System_Create(&m_pSystem, FMOD_VERSION) != FMOD_OK)
	{
		MSG_BOX("Failed Sound Initialize");
		return E_FAIL;
	}
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	if (FMOD_System_Init(m_pSystem, 128, FMOD_INIT_NORMAL, NULL) != FMOD_OK)
	{
		MSG_BOX("Failed Sound Initialize");
		return E_FAIL;
	}
	if (Load_SoundFile() != FMOD_OK)
	{
		MSG_BOX("Failed Sound Initialize");
		return E_FAIL;
	}
	if (FAILED(Reverb_Setting()))
		return E_FAIL;
	if (FAILED(Echo_Setting()))
		return E_FAIL;

	if (FMOD_System_CreateChannelGroup(m_pSystem, "BGMGroup", &m_pBGMGroup) != FMOD_OK)
		return E_FAIL;

	for (auto& iter : m_fVolume)
		iter = 1.f;
	//LoadSoundFile();
	return S_OK;
}

void CSoundMgr::Play_Effect_Sound(const TCHAR* pSoundKey, CHANNELID eID, _float fPosition, _float fPitch, _float fVolume, _bool bPlayAgainstPlaying)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL play = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &play) != bPlayAgainstPlaying)
	{
		_uint StartPosition = static_cast<_uint>(fPosition * 1000);
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pMasterGroup, TRUE, &m_pChannelArr[eID]);
		FMOD_Channel_SetPitch(m_pChannelArr[eID], fPitch);
		FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
		FMOD_Channel_SetPosition(m_pChannelArr[eID], StartPosition, FMOD_TIMEUNIT_MS);
		FMOD_Channel_SetPaused(m_pChannelArr[eID], FALSE);
	}
	FMOD_System_Update(m_pSystem);
}


void CSoundMgr::PlayBGM(const TCHAR* pSoundKey, _float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, m_pBGMGroup, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);

	FMOD_Channel_SetPriority(m_pChannelArr[SOUND_BGM], 255);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], m_fVolume[SOUND_BGM] * fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlaySubBGM(const TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, m_pBGMGroup, FALSE, &m_pChannelArr[SOUND_SUBBGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_SUBBGM], FMOD_LOOP_NORMAL);

	FMOD_Channel_SetPriority(m_pChannelArr[SOUND_SUBBGM], 254);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_SUBBGM], m_fVolume[SOUND_SUBBGM]);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopAll()
{
	FMOD_ChannelGroup_Stop(m_pMasterGroup);
	FMOD_ChannelGroup_Stop(m_pBGMGroup);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::Sound_Pause(CHANNELID eID, _bool bPause)
{
	FMOD_Channel_SetPaused(m_pChannelArr[eID], bPause);
}


void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_fVolume[eID] = fVolume;

	/*FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);*/
}

void CSoundMgr::Set_Effect_Volume(_float fVolume)
{
	FMOD_ChannelGroup_SetVolume(m_pMasterGroup, fVolume);
}

void CSoundMgr::Set_BGM_Volume(_float fVolume)
{
	FMOD_ChannelGroup_SetVolume(m_pBGMGroup, fVolume);
}

void CSoundMgr::Set_Reverb_Param(_float roomSize, _float decayTime, _float wetMix)
{
	FMOD_DSP_SetParameterFloat(reverbDSP, FMOD_DSP_SFXREVERB_DENSITY, roomSize);
	FMOD_DSP_SetParameterFloat(reverbDSP, FMOD_DSP_SFXREVERB_DECAYTIME, decayTime);
	FMOD_DSP_SetParameterFloat(reverbDSP, FMOD_DSP_SFXREVERB_WETLEVEL, wetMix);
}

void CSoundMgr::Enable_Reverb()
{
	FMOD_DSP_SetBypass(reverbDSP, false);
}

void CSoundMgr::Disable_Reverb()
{
	FMOD_DSP_SetBypass(reverbDSP, true);
}

void CSoundMgr::Enable_Echo()
{
	FMOD_DSP_SetBypass(echoDSP, false);
}

void CSoundMgr::Disable_Echo()
{
	FMOD_DSP_SetBypass(echoDSP, true);
}

void CSoundMgr::Set_Echo_Param(_float delay, _float wetLevel)
{
	FMOD_DSP_SetParameterFloat(echoDSP, FMOD_DSP_ECHO_DELAY, delay);
	FMOD_DSP_SetParameterFloat(echoDSP, FMOD_DSP_ECHO_WETLEVEL, wetLevel);
}

FMOD_RESULT CSoundMgr::Load_SoundFile()
{
	FMOD_RESULT result = FMOD_OK;
	stack<wstring> directories;
	directories.push(L"../../Client/Bin/Resources/Sound/");
	while (!directories.empty() && result == FMOD_OK)
	{
		wstring currentPath = directories.top();
		directories.pop();

		wstring searchPath = currentPath + L"*.*";
		_wfinddata64_t fd;
		__int64 handle = _wfindfirst64(searchPath.c_str(), &fd);

		if (handle == -1 || handle == 0)
			continue;

		do
		{
			if (wcscmp(fd.name, L".") != 0 && wcscmp(fd.name, L"..") != 0)
			{
				if (fd.attrib & _A_SUBDIR)
				{
					// 하위 디렉토리를 스택에 추가
					directories.push(currentPath + fd.name + L"\\");
				}
				else
				{
					// 파일 처리
					wstring fullPath = currentPath + fd.name;
					char szFullPath[MAX_PATH];
					WideCharToMultiByte(CP_UTF8, 0, fullPath.c_str(), -1, szFullPath, sizeof(szFullPath), NULL, NULL);

					FMOD_SOUND* pSound = nullptr;
					result = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D | 0x00000020, 0, &pSound);

					if (result == FMOD_OK)
					{
						int iLength = wcslen(fd.name) + 1;
						TCHAR* pSoundKey = new TCHAR[iLength];
						wcscpy_s(pSoundKey, iLength, fd.name);

						m_mapSound.emplace(pSoundKey, pSound);
					}
					else
					{
						MSG_BOX("Failed_Load_Sound");
						break;
					}
				}
			}
		} while (_wfindnext64(handle, &fd) == 0);

		_findclose(handle);
	}

	if (result == FMOD_OK)
	{
		result = FMOD_System_Update(m_pSystem);
	}

	return result;
}

HRESULT CSoundMgr::Reverb_Setting()
{
	if (FMOD_OK != FMOD_System_GetMasterChannelGroup(m_pSystem, &m_pMasterGroup))
	{
		MSG_BOX("Failed_Get_ChannelGroup");
		return E_FAIL;
	}
	if(FMOD_OK != FMOD_System_CreateDSPByType(m_pSystem, FMOD_DSP_TYPE_SFXREVERB, &reverbDSP))
	{
		MSG_BOX("Failed_Create_DSP");
		return E_FAIL;
	}
	if (FMOD_OK != FMOD_ChannelGroup_AddDSP(m_pMasterGroup, 0, reverbDSP))
	{
		MSG_BOX("Failed_Add_DSP");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSoundMgr::Echo_Setting()
{
	if (FMOD_OK != FMOD_System_CreateDSPByType(m_pSystem, FMOD_DSP_TYPE_ECHO, &echoDSP))
	{
		MSG_BOX("Failed_Create_Echo_DSP");
		return E_FAIL;
	}
	if (FMOD_OK != FMOD_ChannelGroup_AddDSP(m_pMasterGroup, 0, echoDSP))
	{
		MSG_BOX("Failed_Add_Echo_DSP");
		return E_FAIL;
	}
	return S_OK;
}

CSoundMgr* CSoundMgr::Create()
{
	CSoundMgr* pInstance = new CSoundMgr();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : SoundManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
	//FMOD_DSP_Release(reverbDSP);
	//FMOD_DSP_Release(echoDSP);
	FMOD_ChannelGroup_Release(m_pMasterGroup);
	FMOD_ChannelGroup_Release(m_pBGMGroup);
}
