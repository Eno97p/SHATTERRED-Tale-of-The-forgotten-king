#pragma once
#include "Base.h"
BEGIN(Engine)

class CSoundMgr final : public CBase
{
private:
    CSoundMgr();
    virtual ~CSoundMgr() = default;
public:
    HRESULT Initialize();
public:
    void Play_Effect_Sound(const TCHAR* pSoundKey, CHANNELID eID, _float fPosition, _float fPitch, _float fVolume = 0.5f, _bool bPlayAgainstPlaying = true);
    void PlayBGM(const TCHAR* pSoundKey, _float fVolume = 1.f);            //일반 BGM 재생
    void PlaySubBGM(const TCHAR* pSoundKey);         //서브 BGM 재생
    void StopAll();
    void StopSound(CHANNELID eID);
    void Sound_Pause(CHANNELID eID, _bool bPause);
    void SetChannelVolume(CHANNELID eID, float fVolume);
    void Set_Effect_Volume(_float fVolume);
    void Set_BGM_Volume(_float fVolume);
    void Set_Reverb_Param(_float roomSize, _float decayTime, _float wetMix);
    void Enable_Reverb();
    void Disable_Reverb();
    void Enable_Echo();
    void Disable_Echo();
    void Set_Echo_Param(_float delay, _float wetLevel);

    void Set_OffSet(_float fOffSet) { m_fOffSet = fOffSet; }
private:
    FMOD_RESULT Load_SoundFile();
    HRESULT Reverb_Setting();
    HRESULT Echo_Setting();
private:
    // 사운드 리소스 정보를 갖는 객체
    map<TCHAR*, FMOD_SOUND*> m_mapSound;
    FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
    FMOD_SYSTEM* m_pSystem;
    FMOD_DSP* reverbDSP = nullptr;
    FMOD_DSP* echoDSP = nullptr;
    FMOD_CHANNELGROUP* m_pMasterGroup = nullptr;
    FMOD_CHANNELGROUP* m_pBGMGroup = nullptr;
    _float m_fVolume[CHANNELID::MAXCHANNEL];        //채널별 볼륨, 디폴트 0.5f


    _float m_fOffSet = 1.f;
public:
    static CSoundMgr* Create();
    virtual void Free() override;
};
END
