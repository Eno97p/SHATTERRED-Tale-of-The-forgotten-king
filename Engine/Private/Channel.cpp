#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

_bool CChannel::Check_KeyframeChange(_uint iChangeKeyFrame) // 인자로 들어온 keyframe이 되는 순간을 체크
{
	m_iChangeKeyframe = iChangeKeyFrame;

	return m_isKeyframeChange;
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	_uint		iBoneIndex = { 0 };

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(m_szName))
				return true;

			++iBoneIndex;

			return false;
		});

	m_iBoneIndex = iBoneIndex;
	/* 이 뼈의 전체 애니메이션 과정 중, 특정 시간대에 취해야할 상태들을 모두 다 읽어서 저장해놓는다.*/
	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	KEYFRAME			KeyFrame{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&KeyFrame.vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}
		if (pAIChannel->mNumRotationKeys > i)
		{
			KeyFrame.vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			KeyFrame.vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			KeyFrame.vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			KeyFrame.vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}
		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&KeyFrame.vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{
	if (0.0 == CurrentPosition)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale, vRotation, vTranslation;

	/* 현재 재생위치가 마지막 키프레임의 위치를 넘어가게되면 무조건 마지막 키프레임의 상태를 취한다.  */
	if (CurrentPosition >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
	}
	/* 특정 키프레임들 사이에 있을거다!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	else
	{
		while (CurrentPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].Time) // +++ 다음으로 넘어간 순간
		{
			++*pCurrentKeyFrameIndex;
			if (*pCurrentKeyFrameIndex == m_iChangeKeyframe) // 다음 keyframe으로 넘어갔는데 체크해야 하는 keyframe인 상황이면
				m_isKeyframeChange = true;
		}

		if (CurrentPosition < m_KeyFrames[*pCurrentKeyFrameIndex + 1].Time) // keyframe이 아직 다음으로 넘어가지 않은 상태라면
			m_isKeyframeChange = false;

		/* 현재 위치에서 왼쪽에 있는 키프렝미의 위치를 뺀다. / 내 오른쪽 키프레임의 위치 - 내 왼쪽 키프레임의 위치. */
		_float		fRatio = (CurrentPosition - m_KeyFrames[*pCurrentKeyFrameIndex].Time) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].Time - m_KeyFrames[*pCurrentKeyFrameIndex].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio);
		vTranslation = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), fRatio);
		vTranslation = XMVectorSetW(vTranslation, 1.f);
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Lerp_TransformationMatrix(const vector<class CBone*>& Bones, CChannel* pNextChannel, _double LerpTime, _double LerpTimeAcc, _uint iCurrentKeyFrameIndex)
{
	_vector			vScale, vRotation, vTranslation;

	_float		fRatio = LerpTimeAcc / LerpTime;

	vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames.back().vScale), XMLoadFloat3(&pNextChannel->m_KeyFrames.front().vScale), fRatio);
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames.back().vRotation), XMLoadFloat4(&pNextChannel->m_KeyFrames.front().vRotation), fRatio);
	vTranslation = XMVectorLerp(XMLoadFloat3(&m_KeyFrames.back().vPosition), XMLoadFloat3(&pNextChannel->m_KeyFrames.front().vPosition), fRatio);
	vTranslation = XMVectorSetW(vTranslation, 1.f);

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Reset_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{
	*pCurrentKeyFrameIndex = 0;

	KEYFRAME		FirstKeyFrame = m_KeyFrames.front();

	_vector			vScale, vRotation, vTranslation;
	vScale = XMLoadFloat3(&FirstKeyFrame.vScale);
	vRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
	vTranslation = XMVectorSetW(XMLoadFloat3(&FirstKeyFrame.vPosition), 1.f);

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

HRESULT CChannel::Save_Channel(ostream& os)
{
	os.write(m_szName, sizeof(_char) * MAX_PATH);
	os.write((char*)&m_iBoneIndex, sizeof(_uint));
	os.write((char*)&m_iNumKeyFrames, sizeof(_uint));
	writeVectorToBinary(m_KeyFrames, os);

	return S_OK;
}

HRESULT CChannel::Load_Channel(istream& is)
{
	is.read(m_szName, sizeof(_char) * MAX_PATH);
	is.read((char*)&m_iBoneIndex, sizeof(_uint));
	is.read((char*)&m_iNumKeyFrames, sizeof(_uint));
	readVectorFromBinary(is, m_KeyFrames);
	return S_OK;
}

CChannel* CChannel::LoadCreate(istream& is)
{
	CChannel* pInstance = new CChannel();
	if (FAILED(pInstance->Load_Channel(is)))
	{
		MSG_BOX("Failed To LoadCreate : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX("Failed To Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CChannel::Free()
{
	m_KeyFrames.clear();
}