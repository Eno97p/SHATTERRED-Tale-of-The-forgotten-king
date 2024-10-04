#pragma once

#include "Base.h"

/* �ִԿ��Ӽ��̤� ����ϴ� ���� �ϳ��� ����. */
/* ���� : �ð��� ���� �� ���� ����(��, ��, ��)���� ����̤����Ѵ�. == Ű������ */
/* */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	// Anim Tool Ȱ�� �Լ�
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

	// AnimTool Ȱ�� ����
	_uint				m_iCurKeyFrame = { 0 }; // ���� keyframe �ε���

	_bool				m_isKeyframeChange = { false }; // Ư�� keyframe���� ��ȯ�Ǿ����� üũ�ϴ� ����
	_int				m_iChangeKeyframe = { 0 }; // keyframe �Ѿ�� ���� �˻��ϱ� ���� ����

public:
	HRESULT Save_Channel(ostream& os);
	HRESULT Load_Channel(istream& is);
	static CChannel* LoadCreate(istream& is);
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END