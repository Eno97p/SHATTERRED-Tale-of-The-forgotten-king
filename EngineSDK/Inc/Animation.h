#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

	// AnimTool 활용 함수
	_uint	Get_CurKeyFrame(_uint iChannelIdx);
	_uint	Set_CurKeyFrame(_uint iChannelIdx); // 현재 KeyFrame 세팅을 위해 값을 반환

	_bool	Check_CurDuration(_double CurDuration); // 현재 Duration을 체크

	void Setting_StartPos(_double StartPos);

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	void Reset();
	void Reset(const vector<class CBone*>& Bones);
	_bool Lerp_NextAnimation(_double TimeDelta, class CAnimation* pNextAnimation, const vector<class CBone*>& Bones);
	string Get_Name() { return std::string(m_szName); }
	_double Get_Current_Ratio() { return currentPositionRatio; }
	_uint Get_NumChannel() { return m_iNumChannels; }

	// Anim Tool 활용 함수
	vector<class CChannel*>* Get_Channels() { return &m_Channels; }
	_double			Get_Duration() { return m_Duration; }
	const _double* Get_CurrentPos() const { return &m_CurrentPosition; }

	// 애니메이션 보완

	void			Set_LerpTime(_double LerpTime) { m_LerpTime = LerpTime; }


private:
	_char			m_szName[MAX_PATH] = "";
	_double			m_Duration = { 0.0 };
	_double			m_TickPerSecond = { 0.0 };
	_double			m_CurrentPosition = { 0.0 };
	_double			m_PrevPosition = { 0.0 };
	_bool			m_isFinished = { false };
	_double			currentPositionRatio = { 0.0 };

	_uint					m_iNumChannels = { 0 };

	vector<_uint>			m_CurrentKeyFrameIndices;
	vector<class CChannel*>	m_Channels;


	CAnimation* m_pNextAnimation = nullptr;
	_double m_LerpTime = 1.2f;
	_double m_LerpTimeAcc = 0.f;

public:
	HRESULT Save_Anim(ostream& os);
	HRESULT Load_Anim(istream& is);

	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	static CAnimation* LoadCreate(istream& is);
	CAnimation* Clone();
	virtual void Free() override;
};

END