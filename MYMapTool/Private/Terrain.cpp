#include "Terrain.h"

#include "GameInstance.h"
#include "PipeLine.h"
#include "VIBuffer_Terrain.h"
#include "ToolObj_Manager.h"
#include "NaviMgr.h"

#include "Monster.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
	, m_pImgui_Manager{ CImgui_Manager::GetInstance() }
	, m_pToolObj_Manager{ CToolObj_Manager::GetInstance() }
{

	Safe_AddRef(m_pImgui_Manager);
	Safe_AddRef(m_pToolObj_Manager);
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject{ rhs }
	, m_pImgui_Manager{ rhs.m_pImgui_Manager }
	, m_pToolObj_Manager{ rhs.m_pToolObj_Manager }
{
	Safe_AddRef(m_pImgui_Manager);
	Safe_AddRef(m_pToolObj_Manager);
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iVerticesX = 512;
	m_iVerticesZ = 512;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 네비 불러온 거 담기?
	//CNaviMgr::GetInstance()->Load_NaviData(m_pNavigationCom);

	return S_OK;
}

void CTerrain::Priority_Tick(_float fTimeDelta)
{
}

void CTerrain::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Pressing(DIK_0))
	{
		m_pVIBufferCom->Culling(m_pTransformCom->Get_WorldMatrix_Inverse());
	}

	/*if (m_pImgui_Manager->Get_IsNaviClear())
	{
		m_pNavigationCom->Clear_Navigation();
		CNaviMgr::GetInstance()->Clear_Cell();
		m_pImgui_Manager->Set_IsNaviClear();
	}*/
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	//m_pNavigationCom->Update(m_pTransformCom->Get_WorldFloat4x4());

	//m_fTest += fTimeDelta;
	//if (m_fTest > 3.f)
	//{
	//	m_fTest = 0.f;
	//	m_pVIBufferCom->Update_Height(fTimeDelta);
	//}
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTerrain::Render()
{
	_int path = 0;
	if (m_pGameInstance->Key_Pressing(DIK_0))
	{
		path = 1;
	}

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(path); // pass 선택
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();
	//
	//#ifdef _DEBUG
	//	m_pNavigationCom->Render();
	//#endif

	return S_OK;
}



void CTerrain::Brush_Picking(_vector vPos, _float fSize, _float fStrength, _float fMaxHeight)
{
	if (!m_pVIBufferCom)
		return;

	XMStoreFloat4(&m_vBrushPos, vPos);
	m_fBrushSize = fSize;
	m_fBrushStrength = fStrength;
	m_fMaxHeight = fMaxHeight;

	if (m_pGameInstance->Mouse_Pressing(DIM_LB))
	{
		// 월드 좌표를 로컬 좌표로 변환
		_matrix invWorld = m_pTransformCom->Get_WorldMatrixInverse();
		_vector vLocalPos = XMVector3Transform(vPos, invWorld);
		_float4 vBrushPos;
		XMStoreFloat4(&vBrushPos, vLocalPos);

		// 브러시 영향 영역 계산
		int brushMinX = max(0, (int)((vBrushPos.x / 4.0f) - fSize));
		int brushMaxX = min((int)m_pVIBufferCom->GetNumVerticesX() - 1, (int)((vBrushPos.x / 4.0f) + fSize));
		int brushMinZ = max(0, (int)((vBrushPos.z / 4.0f) - fSize));
		int brushMaxZ = min((int)m_pVIBufferCom->GetNumVerticesZ() - 1, (int)((vBrushPos.z / 4.0f) + fSize));

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(m_pContext->Map(m_pVIBufferCom->Get_VertexBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
			return;

		VTXNORTEX* pVertices = (VTXNORTEX*)mappedResource.pData;
		_float4* pVertexPositions = m_pVIBufferCom->Get_VertexPositions();

		_float fBrushSizeSquared = fSize * fSize;

		for (int z = brushMinZ; z <= brushMaxZ; ++z)
		{
			for (int x = brushMinX; x <= brushMaxX; ++x)
			{
				UINT index = z * m_pVIBufferCom->GetNumVerticesX() + x;
				_float3 vVertexPos = pVertices[index].vPosition;

				_float fDistanceSquared =
					((vVertexPos.x / 4.0f) - (vBrushPos.x / 4.0f)) * ((vVertexPos.x / 4.0f) - (vBrushPos.x / 4.0f)) +
					((vVertexPos.z / 4.0f) - (vBrushPos.z / 4.0f)) * ((vVertexPos.z / 4.0f) - (vBrushPos.z / 4.0f));

				if (fDistanceSquared <= fBrushSizeSquared)
				{
					_float fInfluence = 1.0f - (sqrt(fDistanceSquared) / fSize);
					_float fAdjustment = fStrength * fInfluence;

					vVertexPos.y += fAdjustment;
					vVertexPos.y = max(0.0f, min(vVertexPos.y, fMaxHeight));

					pVertices[index].vPosition = vVertexPos;
					pVertexPositions[index] = _float4(vVertexPos.x, vVertexPos.y, vVertexPos.z, 1.f);

					// Update m_vHeightMapData (r16 format)
					if (index * 2 + 1 < m_vHeightMapData.size())
					{
						USHORT heightValue = static_cast<USHORT>((vVertexPos.y / fMaxHeight) * 65535.0f);
						m_vHeightMapData[index * 2] = heightValue & 0xFF;
						m_vHeightMapData[index * 2 + 1] = (heightValue >> 8) & 0xFF;
					}
				}
			}
		}

		m_pContext->Unmap(m_pVIBufferCom->Get_VertexBuffer(), 0);

		// 법선 벡터 재계산
		//m_pVIBufferCom->Recalculate_Normals();
	}
}

void CTerrain::Brush_Flatten(_vector vPos, _float fSize, _float fStrength, _float fTargetHeight)
{
	if (!m_pVIBufferCom)
		return;

	XMStoreFloat4(&m_vBrushPos, vPos);
	m_fBrushSize = fSize;
	m_fBrushStrength = fStrength;
	//m_fMaxHeight = fTargetHeight;

	if (m_pGameInstance->Mouse_Pressing(DIM_LB))
	{
		_float4 vBrushPos;
		XMStoreFloat4(&vBrushPos, vPos);

		// 월드 좌표를 로컬 좌표로 변환
		_matrix invWorld = m_pTransformCom->Get_WorldMatrixInverse();
		_vector vLocalPos = XMVector3Transform(vPos, invWorld);
		XMStoreFloat4(&vBrushPos, vLocalPos);

		// 브러시 영향 영역 계산
		int brushMinX = max(0, (int)((vBrushPos.x / 4.0f) - fSize));
		int brushMaxX = min((int)m_pVIBufferCom->GetNumVerticesX() - 1, (int)((vBrushPos.x / 4.0f) + fSize));
		int brushMinZ = max(0, (int)((vBrushPos.z / 4.0f) - fSize));
		int brushMaxZ = min((int)m_pVIBufferCom->GetNumVerticesZ() - 1, (int)((vBrushPos.z / 4.0f) + fSize));

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(m_pContext->Map(m_pVIBufferCom->Get_VertexBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
			return;

		VTXNORTEX* pVertices = (VTXNORTEX*)mappedResource.pData;
		_float4* pVertexPositions = m_pVIBufferCom->Get_VertexPositions();

		_float fBrushSizeSquared = fSize * fSize;

		for (int z = brushMinZ; z <= brushMaxZ; ++z)
		{
			for (int x = brushMinX; x <= brushMaxX; ++x)
			{
				UINT index = z * m_pVIBufferCom->GetNumVerticesX() + x;
				_float3 vVertexPos = pVertices[index].vPosition;

				_float fDistanceSquared =
					((vVertexPos.x / 4.0f) - (vBrushPos.x / 4.0f)) * ((vVertexPos.x / 4.0f) - (vBrushPos.x / 4.0f)) +
					((vVertexPos.z / 4.0f) - (vBrushPos.z / 4.0f)) * ((vVertexPos.z / 4.0f) - (vBrushPos.z / 4.0f));

				if (fDistanceSquared <= fBrushSizeSquared)
				{
					_float fInfluence = 1.0f - (sqrt(fDistanceSquared) / fSize);
					_float fAdjustment = (fTargetHeight - vVertexPos.y) * fInfluence * fStrength;

					vVertexPos.y += fAdjustment;
					vVertexPos.y = max(0.0f, min(vVertexPos.y, m_fMaxHeight));

					pVertices[index].vPosition = vVertexPos;
					pVertexPositions[index] = _float4(vVertexPos.x, vVertexPos.y, vVertexPos.z, 1.f);

					// Update m_vHeightMapData (r16 format)
					if (index * 2 + 1 < m_vHeightMapData.size())
					{
						USHORT heightValue = static_cast<USHORT>((vVertexPos.y / m_fMaxHeight) * 65535.0f);
						m_vHeightMapData[index * 2] = heightValue & 0xFF;
						m_vHeightMapData[index * 2 + 1] = (heightValue >> 8) & 0xFF;
					}
				}
			}
		}

		m_pContext->Unmap(m_pVIBufferCom->Get_VertexBuffer(), 0);
	}
	// 법선 벡터 재계산
	//m_pVIBufferCom->Recalculate_Normals();
}

void CTerrain::AdjustTerrainHeight(_vector vPos, _float fSize, _float fStrength, _float fMaxHeight, _bool bRaise)
{
	if (!m_pVIBufferCom) return;

	_float4 vBrushPos;
	XMStoreFloat4(&vBrushPos, vPos);

	m_pVIBufferCom->AdjustHeight(vBrushPos, fSize, fStrength, fMaxHeight, bRaise);
}

HRESULT CTerrain::Setting_NewTerrain(void* pArg)
{
	return S_OK;
}

void CTerrain::Setting_LoadTerrain(void* pArg)
{
	m_isReLoad = true;

	TERRAIN_DESC* pDesc = (TERRAIN_DESC*)pArg;

	m_iVerticesX = pDesc->iVerticesX;
	m_iVerticesZ = pDesc->iVerticesZ;
}


HRESULT CTerrain::LoadHeightMap(const wstring& strHeightMapFilePath)
{
	// r16 파일에서 높이맵 데이터 로드
	HANDLE hFile = CreateFile(strHeightMapFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	// 파일 크기 확인
	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	// r16 파일의 크기로부터 지형의 크기를 계산
	int iWidth = int(sqrt(float(fileSize) / sizeof(WORD)));
	int iHeight = iWidth; // r16 파일은 보통 정사각형 형태

	// 16비트 높이 데이터를 저장할 버퍼 생성
	vector<WORD> heightData(iWidth * iHeight);

	// 파일 읽기
	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, heightData.data(), fileSize, &dwBytesRead, NULL))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	CloseHandle(hFile);

	const float terrainScale = 4.0f; // xz 간격을 2배로 설정
	const float heightScale = 1.0f; /*/ 65535.0f * 300.f*//* * 150.0f*/; // 높이 스케일 조정 (0-65535를 0-300 범위로 매핑)

	m_vHeightMapData.resize(iWidth * iHeight);
	for (int z = 0; z < iHeight; ++z)
	{
		for (int x = 0; x < iWidth; ++x)
		{
			int iIndex = z * iWidth + x;
			float height = (heightData[iIndex] * 65535.0f) /** heightScale * 300.f*/;
			m_vHeightMapData[iIndex] = height;
		}
	}

	// 높이맵 데이터를 기반으로 지형 정점 업데이트
	if (FAILED(m_pVIBufferCom->UpdateVertexBuffer(m_vHeightMapData.data(), iWidth, iHeight, terrainScale)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SaveHeightMapToR16(const wstring& strFilePath)
{
	if (m_vHeightMapData.empty())
		return E_FAIL;

	// 파일 생성
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	int iWidth = m_pVIBufferCom->GetNumVerticesX();
	int iHeight = m_pVIBufferCom->GetNumVerticesZ();

	vector<WORD> heightData(iWidth * iHeight);
	float maxHeight = 300.0f; // LoadHeightMap과 동일한 최대 높이 사용

	for (int i = 0; i < iWidth * iHeight; ++i)
	{
		heightData[i] = static_cast<WORD>((m_vHeightMapData[i] / maxHeight) * 65535.0f);
	}

	// 파일에 데이터 쓰기
	DWORD dwBytesWritten;
	if (!WriteFile(hFile, heightData.data(), sizeof(WORD) * iWidth * iHeight, &dwBytesWritten, NULL))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}


HRESULT CTerrain::Add_Components()
{
	//CVIBuffer_Terrain::TERRAIN_VIBUFFER_DESC pDesc{};

	//// 요거 값 imgui로부터 받아와서 넣어주기. 기본 크기 잡아두고
	//pDesc.iVerticesX = 512;
	//pDesc.iVerticesZ = 512;

	// Com VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	// Com Shader
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Com_TexGrass
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ground"),
		TEXT("Com_TexGround"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_DIFFUSE]))))
		return E_FAIL;

	//// Com_TexMoss
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ground_Normal"),
	//	TEXT("Com_TexNormal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_NORMAL]))))
	//	return E_FAIL;


	// Com_TexMoss
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ground_Roughness"),
		TEXT("Com_TexNormal"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_ROUGHNESS]))))
		return E_FAIL;



	// Com Mask
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_MASK]))))
		return E_FAIL;

	/* For.Com_Brush */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_BRUSH]))))
		return E_FAIL;

	// Com Calculator
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Calculator"),
	//	TEXT("Com_Calculator"), reinterpret_cast<CComponent**>(&m_pCalculatorCom))))
	//	return E_FAIL;

	// Com Navigation
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
	//	TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//// 조명 정보 Shader에 던져주기
	//const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrushRange", &m_fBrushSize, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TEX_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TEX_ROUGHNESS]->Bind_ShaderResources(m_pShaderCom, "g_RoughnessTexture")))
		return E_FAIL;
	//if (FAILED(m_pTextureCom[TEX_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
	//	return E_FAIL;
	if (FAILED(m_pTextureCom[TEX_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSnowGroundHeight", &m_fSnowGroundHeight, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSnowGroundHeightOffset", &m_fSnowGroundHeightOffset, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	//Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);

	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);

	Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pCalculatorCom);

	// calculator는?
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pToolObj_Manager);
	// 기존 높이맵 해제
	Safe_Release(m_pHeightMapSRV);
	Safe_Release(m_pHeightMapTexture);
}
