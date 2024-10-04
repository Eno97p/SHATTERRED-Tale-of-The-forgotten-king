#include "..\Public\Bone.h"



CBone::CBone()
{

}

HRESULT CBone::Initialize(const aiNode* pBoneNode, _int iParentIndex)
{
	strcpy_s(m_szName, pBoneNode->mName.data);

	m_iParentBoneIndex = iParentIndex;

	/*  */

	memcpy(&m_TransformationMatrix, &pBoneNode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
	}
}

HRESULT CBone::Save_Bone(std::ostream& os)
{
	os.write(m_szName, sizeof(_char) * MAX_PATH);
	os.write((char*)&m_iParentBoneIndex, sizeof(_int));
	os.write((char*)&m_TransformationMatrix, sizeof(_float4x4));
	return S_OK;
}

HRESULT CBone::Load_Bone(std::istream& os)
{
	os.read(m_szName, sizeof(_char) * MAX_PATH);
	os.read((char*)&m_iParentBoneIndex, sizeof(_int));
	os.read((char*)&m_TransformationMatrix, sizeof(_float4x4));
	return S_OK;
}

CBone* CBone::Create(const aiNode* pBoneNode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pBoneNode, iParentIndex)))
	{
		MSG_BOX("Failed To Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::LoadCreate(std::istream& os)
{
	CBone* pInstance = new CBone();
	if (FAILED(pInstance->Load_Bone(os)))
	{
		MSG_BOX("Failed To LoadCreated : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}

