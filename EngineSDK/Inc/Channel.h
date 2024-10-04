#pragma once

#include "Base.h"

/* 애님에ㅣ션이ㅐ 사용하느 ㄴ뼈 하나의 정보. */
/* 정보 : 시간에 따른 이 뼈의 상태(스, 자, 이)들을 보고ㅜㅏㄴ한다. == 키프레임 */
/* */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	// Anim Tool 활용 함수
	_char* Get_Name() { return m_szName; }
	_uint		Get_NumKeyFrame() { return m_iNumKeyFrames; }
	_uint		Get_CurKeyFrame() {
		return m_iCurKeyFrame;
	}
	void		Set_CurKeyFrame(_uint iCurKeyFrame) { m_iCurKeyFrame = iCurKeyFrame; }

	_bool		Check_KeyframeChange(_uint iChangeKeyFrame);

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);
	void Lerp_TransformationMatrix(const vector<class CBone*>& Bones, class CChannel* pNextChannel, _double LerpTime, _double LerpTimeAcc, _uint iCurrentKeyFrameIndex);
	void Reset_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);

private:
	_char			m_szName[MAX_PATH] = "";
	_uint			m_iBoneIndex = { 0 };

	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	// AnimTool 활용 변수
	_uint				m_iCurKeyFrame = { 0 }; // 현재 keyframe 인덱스

	_bool				m_isKeyframeChange = { false }; // 특정 keyframe에서 변환되었는지 체크하는 변수
	_int				m_iChangeKeyframe = { 0 }; // keyframe 넘어가는 것을 검사하기 위한 변수

public:
	HRESULT Save_Channel(ostream& os);
	HRESULT Load_Channel(istream& is);
	static CChannel* LoadCreate(istream& is);
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END