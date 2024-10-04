#include "..\Public\Animation.h"
#include "Channel.h"



CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration{ rhs.m_Duration }
	, m_TickPerSecond{ rhs.m_TickPerSecond }
	, m_CurrentPosition{ rhs.m_CurrentPosition }
	, m_isFinished{ rhs.m_isFinished }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
	, m_Channels{ rhs.m_Channels }
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

_uint CAnimation::Get_CurKeyFrame(_uint iChannelIdx)
{
	// 인자로 들어온 채널의 현재 keyframe 위치를 반환해야함

	return		m_CurrentKeyFrameIndices[iChannelIdx];
	//return	m_Channels[iChannelIdx]->Get_CurKeyFrame(); // 여기서는 전부 0이 들어가 있음
}

_uint CAnimation::Set_CurKeyFrame(_uint iChannelIdx)
{
	return m_CurrentKeyFrameIndices[iChannelIdx];
}

_bool CAnimation::Check_CurDuration(_double CurDuration)
{
	_double PrePositionRatio = m_PrevPosition / m_Duration;
	_double	CurrentPositionRatio = m_CurrentPosition / m_Duration;

	return (CurDuration > PrePositionRatio && CurDuration < CurrentPositionRatio);

	//return (CurDuration > m_PrevPosition && CurDuration < m_CurrentPosition);
}

void CAnimation::Setting_StartPos(_double StartPos)
{
	m_CurrentPosition = StartPos * m_Duration;
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션을 구동하는 필요한 뼈의 갯수 */
	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	/*Save_Anim();*/

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
	m_PrevPosition = m_CurrentPosition;
	m_CurrentPosition += m_TickPerSecond * fTimeDelta;

	if (m_CurrentPosition >= m_Duration)
	{
		if (false == isLoop)
		{
			m_isFinished = true;
		}
		else
		{
			m_CurrentPosition = 0.0;
		}
	}
	currentPositionRatio = m_CurrentPosition / m_Duration;
	currentPositionRatio = min(currentPositionRatio, 1.0);
	currentPositionRatio = max(currentPositionRatio, 0.0);


	if (false == m_isFinished)
	{
		_uint      iChannelIndex = { 0 };

		for (auto& pChannel : m_Channels)
		{
			pChannel->Update_TransformationMatrix(m_CurrentPosition, Bones, &m_CurrentKeyFrameIndices[iChannelIndex++]);
		}
	}

}

void CAnimation::Reset()
{
	m_CurrentPosition = 0.0;
	m_PrevPosition = 0.0;
	m_isFinished = false;
	ZeroMemory(&m_CurrentKeyFrameIndices.front(), sizeof(_uint) * m_iNumChannels);
}

void CAnimation::Reset(const vector<class CBone*>& Bones)
{
	m_CurrentPosition = 0.0;
	m_PrevPosition = 0.0;
	m_isFinished = false;

	_uint		iChannelIndex = { 0 };
	for (auto& pChannel : m_Channels)
	{
		pChannel->Reset_TransformationMatrix(Bones, &m_CurrentKeyFrameIndices[iChannelIndex++]);
	}
}

_bool CAnimation::Lerp_NextAnimation(_double TimeDelta, CAnimation* pNextAnimation, const vector<class CBone*>& Bones)
{
	m_LerpTimeAcc += TimeDelta;

	if (pNextAnimation && m_LerpTimeAcc <= m_LerpTime)
	{
		_uint iChannelIndex = 0;
		for (auto& pChannel : m_Channels)
		{
			CChannel* pNextChannel = pNextAnimation->m_Channels[iChannelIndex];
			pChannel->Lerp_TransformationMatrix(Bones, pNextChannel, m_LerpTime, m_LerpTimeAcc, m_CurrentKeyFrameIndices[iChannelIndex]);
			++iChannelIndex;
			if (iChannelIndex >= pNextAnimation->Get_NumChannel())
				break;
		}
		return true;
	}
	else
	{
		m_LerpTimeAcc = 0.0f;
		return false;
	}
}

HRESULT CAnimation::Save_Anim(ostream& os)
{
	os.write(m_szName, sizeof(_char) * MAX_PATH);
	os.write((char*)&m_Duration, sizeof(_double));
	os.write((char*)&m_TickPerSecond, sizeof(_double));
	os.write((char*)&m_CurrentPosition, sizeof(_double));
	os.write((char*)&m_isFinished, sizeof(_bool));
	os.write((char*)&m_iNumChannels, sizeof(_uint));
	/*writeVectorToBinary(m_CurrentKeyFrameIndices, os);*/


	for (auto& channel : m_Channels)
		channel->Save_Channel(os);


	return S_OK;
}

HRESULT CAnimation::Load_Anim(istream& is)
{
	is.read(m_szName, sizeof(_char) * MAX_PATH);
	is.read((char*)&m_Duration, sizeof(_double));
	is.read((char*)&m_TickPerSecond, sizeof(_double));
	is.read((char*)&m_CurrentPosition, sizeof(_double));
	is.read((char*)&m_isFinished, sizeof(_bool));
	is.read((char*)&m_iNumChannels, sizeof(_uint));
	/*readVectorFromBinary(is, m_CurrentKeyFrameIndices);*/
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::LoadCreate(is);
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::LoadCreate(istream& is)
{
	CAnimation* pInstance = new CAnimation();
	if (FAILED(pInstance->Load_Anim(is)))
	{
		MSG_BOX("Failed To LoadCreated : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
