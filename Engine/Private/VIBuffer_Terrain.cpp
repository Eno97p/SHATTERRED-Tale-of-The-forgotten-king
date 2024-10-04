#include "..\Public\VIBuffer_Terrain.h"

#include "GameInstance.h"
#include "QuadTree.h"
#include "OctTree.h"

#include <omp.h>
#include <immintrin.h> // SSE/AVX ��ɾ ���� ���
#include "CPhysX.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer{ rhs }
	, m_iNumVerticesX{ rhs.m_iNumVerticesX }
	, m_iNumVerticesZ{ rhs.m_iNumVerticesZ }
	, m_pOctTree { rhs.m_pOctTree }
	, m_pQuadTree { rhs.m_pQuadTree }
	, m_validGrassPositions {rhs.m_validGrassPositions }
{
	Safe_AddRef(m_pOctTree);
	Safe_AddRef(m_pQuadTree);
}


HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& strHeightMapFilePath)
{
	//���¸� ����



	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& strHeightMapFilePath, _bool realmap)
{
	_ulong			dwByte = { 0 };

	HANDLE			hFile = CreateFile(strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// RAW ������ ũ�⸦ Ȯ��
	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	// ���� ũ�� ���
	int originalSize = static_cast<int>(sqrt(static_cast<float>(fileSize) / sizeof(WORD)));

	// RAW ������ ũ��κ��� ������ ũ�⸦ ���
	m_iNumVerticesX = m_iNumVerticesZ = originalSize + 1;

	// RAW �����͸� ������ ���� ����
	vector<WORD> rawData(m_iNumVerticesX * m_iNumVerticesZ);

	// ���� RAW ���� �б�
	vector<WORD> originalData(originalSize * originalSize);
	if (!ReadFile(hFile, originalData.data(), fileSize, &dwByte, nullptr))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	CloseHandle(hFile);

	// ������ Ȯ�� (1024x1024 -> 1025x1025)
	for (int z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (int x = 0; x < m_iNumVerticesX; ++x)
		{
			if (x < originalSize && z < originalSize)
			{
				// ���� ������ ����
				rawData[z * m_iNumVerticesX + x] = originalData[z * originalSize + x];
			}
			else
			{
				// ������ ��/�� ����
				int sourceX = min(x, originalSize - 1);
				int sourceZ = min(z, originalSize - 1);
				rawData[z * m_iNumVerticesX + x] = originalData[sourceZ * originalSize + sourceX];
			}
		}
	}

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iIndexStride = 4;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;

#pragma region VERTEX_BUFFER 
	// ���ؽ� ���� ���� �κ� ����
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	m_pVertexPositions = new _float4[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float4) * m_iNumVertices);

	const float terrainScaleX = 3.f;  // X ���� ������
	const float terrainScaleZ = 3.f;  // Z ���� ������
	const float heightScale = 1000.f;  // ���� ������ ��, �ʿ信 ���� ����

	// ������ �߽��� (0,0,0)���� �̵��ϱ� ���� ������ ���
	float offsetX = (m_iNumVerticesX - 1) * terrainScaleX * 0.5f;
	float offsetZ = (m_iNumVerticesZ - 1) * terrainScaleZ * 0.5f;

	// 16��Ʈ ������ ������ �б�
	vector<uint16_t> rawHeightData(m_iNumVertices);
	memcpy(rawHeightData.data(), rawData.data(), m_iNumVertices * sizeof(uint16_t));

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			// Big Endian���� Little Endian���� ��ȯ
			uint16_t bigEndianHeight = rawHeightData[iIndex];
			uint16_t littleEndianHeight = (bigEndianHeight >> 8) | (bigEndianHeight << 8);

			// 16��Ʈ ������ 0-1 ������ float�� ��ȯ �� ���� ������ ����
			float height = (static_cast<float>(littleEndianHeight) / 65535.f) * heightScale;

			// ���� ��ġ ��� �� ������ ����
			float posX = (j * terrainScaleX) - offsetX;
			float posZ = (i * terrainScaleZ) - offsetZ;

			pVertices[iIndex].vPosition = _float3(posX, height, posZ);
			m_pVertexPositions[iIndex] = _float4(pVertices[iIndex].vPosition.x, pVertices[iIndex].vPosition.y, pVertices[iIndex].vPosition.z, 1.f);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}
	// ���⿡ ���ο� �ڵ� �߰�
	m_validGrassPositions.clear();
	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;
			float height = pVertices[iIndex].vPosition.y;

			if (height >= 348.f && height <= 395.f)
			{
				m_validGrassPositions.push_back(_float3(pVertices[iIndex].vPosition.x, height, pVertices[iIndex].vPosition.z));
			}
		}
	}

#pragma endregion

#pragma region INDEX_BUFFER 

	// �ε��� ���� ũ�� ����
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}

	/*for (size_t i = 0; i < m_iNumVertices; i++)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;*/

	for (size_t i = 0; i < m_iNumVertices; i++)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	//Safe_Delete_Array(pPixel);
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);
#pragma endregion

	// OctTree ����

	float minHeight = FLT_MAX;
	float maxHeight = -FLT_MAX;

	// �ּ� �� �ִ� ���� ã��
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		minHeight = min(minHeight, m_pVertexPositions[i].y);
		maxHeight = max(maxHeight, m_pVertexPositions[i].y);
	}


	//_float3 vMin = _float3(-offsetX, minHeight, -offsetZ);
	//_float3 vMax = _float3(
	//	(m_iNumVerticesX - 1) * terrainScaleX - offsetX,
	//	maxHeight,
	//	(m_iNumVerticesZ - 1) * terrainScaleZ - offsetZ
	//);

	//m_pOctTree = COctTree::Create(vMin, vMax, 0);
	//if (nullptr == m_pOctTree)
	//	return E_FAIL;

	//m_pOctTree->Make_Neighbors();


 // // QuadTree ���� �κ� ����
	//m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	//if (nullptr == m_pQuadTree)
	//	return E_FAIL;
	//
	//m_pQuadTree->Make_Neighbors();

	
	return S_OK;

}


HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{

	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Height(const _float3& vWorldPos)
{
	// ���� ��ǥ�� ���� ��ǥ�� ��ȯ
	_float3 vLocalPos = vWorldPos;
	vLocalPos.x += (m_iNumVerticesX - 1) * 3.f * 0.5f;  // offsetX ����
	vLocalPos.z += (m_iNumVerticesZ - 1) * 3.f * 0.5f;  // offsetZ ����

	// ���� ���ο� �ִ��� Ȯ��
	if (vLocalPos.x < 0 || vLocalPos.x >(m_iNumVerticesX - 1) * 3.f ||
		vLocalPos.z < 0 || vLocalPos.z >(m_iNumVerticesZ - 1) * 3.f)
		return 0.f;

	// �ش� ��ġ�� �׸��� ��ǥ ���
	_uint iX = _uint(vLocalPos.x / 3.f);
	_uint iZ = _uint(vLocalPos.z / 3.f);

	_uint iIndex = iZ * m_iNumVerticesX + iX;

	// �׸��� �� 4���� ���� �ε���
	_uint iIndices[4] = {
		iIndex,
		iIndex + 1,
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1
	};

	// �׸��� �� ��� ��ġ ���
	_float fRatioX = (vLocalPos.x / 3.f) - iX;
	_float fRatioZ = (vLocalPos.z / 3.f) - iZ;

	_vector vPlane;

	// �ﰢ�� ���� �� ��� ���
	if (fRatioX > fRatioZ)  // ������ �� �ﰢ��
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat4(&m_pVertexPositions[iIndices[0]]),
			XMLoadFloat4(&m_pVertexPositions[iIndices[1]]),
			XMLoadFloat4(&m_pVertexPositions[iIndices[3]]));
	}
	else  // ���� �Ʒ� �ﰢ��
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat4(&m_pVertexPositions[iIndices[0]]),
			XMLoadFloat4(&m_pVertexPositions[iIndices[2]]),
			XMLoadFloat4(&m_pVertexPositions[iIndices[3]]));
	}

	// ��� �������� ����Ͽ� ���� ���
	// ax + by + cz + d = 0
	// y = (-ax - cz - d) / b
	return (-XMVectorGetX(vPlane) * vWorldPos.x - XMVectorGetZ(vPlane) * vWorldPos.z - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}
void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrixInv)
{
	/* ���ý����̽����� ����� �����Ѵ�. */
	m_pGameInstance->Transform_ToLocalSpace(WorldMatrixInv);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	_uint* pIndices = ((_uint*)SubResource.pData);

	_uint		iNumIndices = { 0 };

	m_pQuadTree->Culling(m_pVertexPositions, pIndices, &iNumIndices);

	/*
	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool		isIn[4] = {
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&m_pVertexPositions[iIndices[0]])),
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&m_pVertexPositions[iIndices[1]])),
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&m_pVertexPositions[iIndices[2]])),
				m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&m_pVertexPositions[iIndices[3]]))
			};


			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[1];
				pIndices[iNumIndices++] = iIndices[2];
			}


			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[2];
				pIndices[iNumIndices++] = iIndices[3];
			}
		}
	}
	*/

	m_iNumIndices = iNumIndices;

	m_pContext->Unmap(m_pIB, 0);
}

void CVIBuffer_Terrain::Update_Height(float fDeltaTime)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		VTXNORTEX* pVertices = (VTXNORTEX*)mappedResource.pData;

		for (size_t i = 0; i < m_iNumVertices; ++i)
		{
			// ���� ������Ʈ ���� �߰�
			pVertices[i].vPosition.y += sinf(fDeltaTime * 0.1f + pVertices[i].vPosition.x * 0.1f + pVertices[i].vPosition.z * 0.1f) * 0.1f;
		}

		m_pContext->Unmap(m_pVB, 0);
	}
}

_float3 CVIBuffer_Terrain::Compute_Normal(const _float3& vWorldPos)
{
	// ���� ��ǥ�� ���� ��ǥ�� ��ȯ
	_float3 vLocalPos = vWorldPos;
	vLocalPos.x += (m_iNumVerticesX - 1) * 3.f * 0.5f;
	vLocalPos.z += (m_iNumVerticesZ - 1) * 3.f * 0.5f;

	// ���� ���ο� �ִ��� Ȯ��
	if (vLocalPos.x < 0 || vLocalPos.x >(m_iNumVerticesX - 1) * 3.f ||
		vLocalPos.z < 0 || vLocalPos.z >(m_iNumVerticesZ - 1) * 3.f)
		return _float3(0, 1, 0);  // ���� ���̸� �⺻ ���� ���� ��ȯ

	// �ش� ��ġ�� �׸��� ��ǥ ���
	_uint iX = _uint(vLocalPos.x / 3.f);
	_uint iZ = _uint(vLocalPos.z / 3.f);
	_uint iIndex = iZ * m_iNumVerticesX + iX;

	// �׸��� �� 4���� ���� �ε���
	_uint iIndices[4] = {
		iIndex,
		iIndex + 1,
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1
	};

	// �׸��� �� ��� ��ġ ���
	_float fRatioX = (vLocalPos.x / 3.f) - iX;
	_float fRatioZ = (vLocalPos.z / 3.f) - iZ;

	_vector vNormal;

	// �ﰢ�� ���� �� ��� ���
	if (fRatioX > fRatioZ)  // ������ �� �ﰢ��
	{
		vNormal = XMVector3Normalize(XMVector3Cross(
			XMLoadFloat4(&m_pVertexPositions[iIndices[3]]) - XMLoadFloat4(&m_pVertexPositions[iIndices[0]]),
			XMLoadFloat4(&m_pVertexPositions[iIndices[1]]) - XMLoadFloat4(&m_pVertexPositions[iIndices[0]])
		));
	}
	else  // ���� �Ʒ� �ﰢ��
	{
		vNormal = XMVector3Normalize(XMVector3Cross(
			XMLoadFloat4(&m_pVertexPositions[iIndices[2]]) - XMLoadFloat4(&m_pVertexPositions[iIndices[0]]),
			XMLoadFloat4(&m_pVertexPositions[iIndices[3]]) - XMLoadFloat4(&m_pVertexPositions[iIndices[0]])
		));
	}

	_float3 normal;
	XMStoreFloat3(&normal, vNormal);
	return normal;
}

void CVIBuffer_Terrain::AdjustHeight(const _float4& vBrushPos, float fBrushSize, float fBrushStrength, float fMaxHeight, bool bRaise)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
		return;

	VTXNORTEX* pVertices = (VTXNORTEX*)mappedResource.pData;

	for (UINT i = 0; i < m_iNumVertices; ++i)
	{
		_float3 vVertexPos = pVertices[i].vPosition;
		_float2 vDiff = _float2(vVertexPos.x - vBrushPos.x, vVertexPos.z - vBrushPos.z);
		float fDistance = sqrt(vDiff.x * vDiff.x + vDiff.y * vDiff.y);

		if (fDistance <= fBrushSize)
		{
			float fInfluence = (fBrushSize - fDistance) / fBrushSize;
			float fAdjustment = fBrushStrength * fInfluence;

			if (bRaise)
				vVertexPos.y += fAdjustment;
			else
				vVertexPos.y -= fAdjustment;

			vVertexPos.y = max(0.0f, min(vVertexPos.y, fMaxHeight));
			pVertices[i].vPosition = vVertexPos;
		}
	}

	m_pContext->Unmap(m_pVB, 0);

	//Recalculate_Normals();
}

HRESULT CVIBuffer_Terrain::UpdateHeightMap(ID3D11Texture2D* pHeightMapTexture)
{
	if (!pHeightMapTexture) return E_FAIL;

	D3D11_TEXTURE2D_DESC heightMapDesc;
	pHeightMapTexture->GetDesc(&heightMapDesc);

	// ������¡ �ؽ�ó ����
	ID3D11Texture2D* pStagingTexture = nullptr;
	D3D11_TEXTURE2D_DESC stagingDesc = heightMapDesc;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingDesc.MiscFlags = 0;

	HRESULT hr = m_pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStagingTexture);
	if (FAILED(hr))
		return hr;

	// ���̸� �ؽ�ó�� ������¡ �ؽ�ó�� ����
	m_pContext->CopyResource(pStagingTexture, pHeightMapTexture);

	D3D11_MAPPED_SUBRESOURCE mappedTexture;
	hr = m_pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mappedTexture);
	if (FAILED(hr))
	{
		Safe_Release(pStagingTexture);
		return hr;
	}

	// �ӽ� ���ؽ� ������ ����
	vector<VTXNORTEX> vertices(m_iNumVertices);

	BYTE* pHeightMap = (BYTE*)mappedTexture.pData;
	float maxHeight = 0.0f;  // ������� ���� �ִ� ���� ����

	for (UINT z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (UINT x = 0; x < m_iNumVerticesX; ++x)
		{
			UINT terrainIndex = z * m_iNumVerticesX + x;
			UINT heightMapX = (UINT)((float)x / m_iNumVerticesX * heightMapDesc.Width);
			UINT heightMapZ = (UINT)((float)z / m_iNumVerticesZ * heightMapDesc.Height);
			UINT heightMapIndex = heightMapZ * mappedTexture.RowPitch + heightMapX * 4;

			// R8G8B8A8_UNORM ���� ����
			float height = pHeightMap[heightMapIndex] / 255.0f * 100.0f;  // �������� 100���� ����

			vertices[terrainIndex].vPosition.y = height;
			m_pVertexPositions[terrainIndex].y = height;

			maxHeight = max(maxHeight, height);  // �ִ� ���� ����

			// ����� ��� (�ʿ��� ���)
			// OutputDebugStringA(("Height at (" + std::to_string(x) + "," + std::to_string(z) + "): " + std::to_string(height) + "\n").c_str());
		}
	}

	m_pContext->Unmap(pStagingTexture, 0);
	Safe_Release(pStagingTexture);

	// �ִ� ���� ��� (������)
	OutputDebugStringA(("Max Height: " + std::to_string(maxHeight) + "\n").c_str());

	// ���ؽ� ���� ������Ʈ
	D3D11_MAPPED_SUBRESOURCE mappedVB;
	hr = m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
	if (FAILED(hr))
		return hr;

	memcpy(mappedVB.pData, vertices.data(), sizeof(VTXNORTEX) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);

	Recalculate_Normals();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::UpdateVertexBuffer(_ushort* pHeightMapData, int iWidth, int iHeight, float terrainScale)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return E_FAIL;

	VTXNORTEX* pVertices = (VTXNORTEX*)mappedResource.pData;

	for (int z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (int x = 0; x < m_iNumVerticesX; ++x)
		{
			int iIndex = z * m_iNumVerticesX + x;
			int iHeightMapX = min(x * (iWidth - 1) / (m_iNumVerticesX - 1), iWidth - 1);
			int iHeightMapZ = min(z * (iHeight - 1) / (m_iNumVerticesZ - 1), iHeight - 1);
			int iHeightMapIndex = iHeightMapZ * iWidth + iHeightMapX;

			// unsigned short ���Ŀ��� ���� ���� ������ 0~1 ������ ����ȭ
			float fHeight = pHeightMapData[iHeightMapIndex] / 65535.0f;

			// ���� ������ ���ϴ� ��� ���� (��: 0~20)
			fHeight *= 20.0f;

			pVertices[iIndex].vPosition = _float3(x * terrainScale, fHeight, z * terrainScale);
			m_pVertexPositions[iIndex] = _float4(pVertices[iIndex].vPosition.x, pVertices[iIndex].vPosition.y, pVertices[iIndex].vPosition.z, 1.f);
		}
	}

	m_pContext->Unmap(m_pVB, 0);

	//Recalculate_Normals();

	return S_OK;
}

void CVIBuffer_Terrain::SplatBrushOnHeightMap(ID3D11Texture2D* pHeightMapTexture, const _float4& vBrushPos, float fBrushSize, float fBrushStrength, CTexture* pBrushTexture)
{
	if (!pHeightMapTexture) return;

	D3D11_TEXTURE2D_DESC texDesc;
	pHeightMapTexture->GetDesc(&texDesc);

	// ������¡ �ؽ�ó ���� (������ ����)
	ID3D11Texture2D* pStagingTexture = nullptr;
	D3D11_TEXTURE2D_DESC stagingDesc = texDesc;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	stagingDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStagingTexture)))
		return;

	// ���� �ؽ�ó ������ ������¡ �ؽ�ó�� ����
	m_pContext->CopyResource(pStagingTexture, pHeightMapTexture);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(m_pContext->Map(pStagingTexture, 0, D3D11_MAP_READ_WRITE, 0, &mappedResource)))
	{
		BYTE* pHeightMap = (BYTE*)mappedResource.pData;

		// �귯�� ���� ���� ���
		int brushMinX = max(0, (int)(vBrushPos.x - fBrushSize));
		int brushMaxX = min(texDesc.Width - 1, (int)(vBrushPos.x + fBrushSize));
		int brushMinY = max(0, (int)(vBrushPos.z - fBrushSize));
		int brushMaxY = min(texDesc.Height - 1, (int)(vBrushPos.z + fBrushSize));

		float fBrushSizeSquared = fBrushSize * fBrushSize;
		__m128 vBrushCenter = _mm_set_ps(0, vBrushPos.z, vBrushPos.x, 0);
		__m128 vBrushStrength = _mm_set1_ps(fBrushStrength);
		__m128 vMaxHeight = _mm_set1_ps(255.0f);

#pragma omp parallel for
		for (int y = brushMinY; y <= brushMaxY; ++y)
		{
			for (int x = brushMinX; x <= brushMaxX; x += 4)
			{
				__m128 vPixelPos = _mm_set_ps(0, y, x + 3, x + 2);
				__m128 vDiff = _mm_sub_ps(vPixelPos, vBrushCenter);
				__m128 vDistSquared = _mm_mul_ps(vDiff, vDiff);
				__m128 vDist = _mm_hadd_ps(vDistSquared, vDistSquared);

				__m128 vMask = _mm_cmplt_ps(vDist, _mm_set1_ps(fBrushSizeSquared));

				if (_mm_movemask_ps(vMask))
				{
					for (int i = 0; i < 4; ++i)
					{
						if (x + i <= brushMaxX)
						{
							UINT index = y * mappedResource.RowPitch + (x + i) * 4;
							//_float2 vTexCoord = _float2((float)(x + i) / texDesc.Width, (float)y / texDesc.Height);
							_float2 vTexCoord = _float2((float)(x + i) / (m_iNumVerticesX - 1), (float)y / (m_iNumVerticesZ - 1));


							_float4 vBrushColor;
							pBrushTexture->Fetch_Pixel_FromPixelShader(vTexCoord, &vBrushColor, 0);

							float fHeightChange = vBrushColor.w * fBrushStrength;
							int newHeight = pHeightMap[index] + (int)(fHeightChange * 255.0f);
							pHeightMap[index] = (BYTE)max(0, min(255, newHeight));
						}
					}
				}
			}
		}

		m_pContext->Unmap(pStagingTexture, 0);

		// ������ ������ �ٽ� ���� �ؽ�ó�� ����
		m_pContext->CopyResource(pHeightMapTexture, pStagingTexture);
	}

	Safe_Release(pStagingTexture);
}

void CVIBuffer_Terrain::Recalculate_Normals()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
		return;

	VTXNORTEX* pVertices = (VTXNORTEX*)mappedResource.pData;

	for (UINT z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (UINT x = 0; x < m_iNumVerticesX; ++x)
		{
			UINT index = z * m_iNumVerticesX + x;
			XMVECTOR normal = XMVectorZero();
			UINT numTriangles = 0;

			// �ֺ� �ﰢ������ ������ ����ϰ� ����� ���ϴ�.
			for (int dz = -1; dz <= 1; ++dz)
			{
				for (int dx = -1; dx <= 1; ++dx)
				{
					if (x + dx >= 0 && x + dx < m_iNumVerticesX && z + dz >= 0 && z + dz < m_iNumVerticesZ)
					{
						UINT neighborIndex = (z + dz) * m_iNumVerticesX + (x + dx);
						XMVECTOR v1 = XMLoadFloat3(&pVertices[index].vPosition);
						XMVECTOR v2 = XMLoadFloat3(&pVertices[neighborIndex].vPosition);
						XMVECTOR v3 = XMLoadFloat3(&pVertices[index].vPosition);

						if (dx != 0 && dz != 0)
						{
							v3 = XMLoadFloat3(&pVertices[(z + dz) * m_iNumVerticesX + x].vPosition);
						}
						else if (dx != 0)
						{
							v3 = XMLoadFloat3(&pVertices[z * m_iNumVerticesX + (x + dx)].vPosition);
						}
						else if (dz != 0)
						{
							v3 = XMLoadFloat3(&pVertices[(z + dz) * m_iNumVerticesX + x].vPosition);
						}
						else
						{
							continue;
						}

						XMVECTOR edge1 = XMVectorSubtract(v2, v1);
						XMVECTOR edge2 = XMVectorSubtract(v3, v1);
						XMVECTOR triangleNormal = XMVector3Cross(edge1, edge2);
						normal = XMVectorAdd(normal, XMVector3Normalize(triangleNormal));
						++numTriangles;
					}
				}
			}

			if (numTriangles > 0)
			{
				normal = XMVector3Normalize(XMVectorDivide(normal, XMVectorReplicate((float)numTriangles)));
				XMStoreFloat3(&pVertices[index].vNormal, normal);
			}
		}
	}

	m_pContext->Unmap(m_pVB, 0);
}

PxHeightField* CreateHeightField(PxPhysics* physics, const vector<uint16_t>& rawHeightData, int numRows, int numCols)
{
	vector<PxHeightFieldSample> samples(numRows * numCols);

	for (int i = 0; i < numRows * numCols; i++)
	{
		samples[i].height = static_cast<PxI16>(rawHeightData[i]);
		samples[i].materialIndex0 = 0;
		samples[i].materialIndex1 = 0;
	}

	PxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns = numCols;
	heightFieldDesc.nbRows = numRows;
	heightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
	heightFieldDesc.samples.data = samples.data();
	heightFieldDesc.samples.stride = sizeof(PxHeightFieldSample);

	return PxCreateHeightField(heightFieldDesc, physics->getPhysicsInsertionCallback());
}

PxRigidStatic* CreateTerrainActor(PxPhysics* physics, PxScene* scene, const vector<uint16_t>& rawHeightData, int numRows, int numCols, float rowScale, float heightScale, float colScale)
{
	PxHeightField* heightField = CreateHeightField(physics, rawHeightData, numRows, numCols);

	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, rowScale, colScale);

	PxRigidStatic* terrainActor = physics->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*terrainActor, hfGeom, *physics->createMaterial(0.5f, 0.5f, 0.1f));

	scene->addActor(*terrainActor);

	return terrainActor;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strHeightMapFilePath)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath, _bool realmap)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath, realmap)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pOctTree);
	Safe_Release(m_pQuadTree);
}

