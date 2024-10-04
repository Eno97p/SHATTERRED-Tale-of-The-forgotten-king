#include "BlastMgr.h"

CBlastMgr::CBlastMgr()
{
}

HRESULT CBlastMgr::Initialize()
{
	
	m_pFramework = NvBlastTkFrameworkCreate();
	if (nullptr == m_pFramework)
		return E_FAIL;
	FractureTool* fTool = NvBlastExtAuthoringCreateFractureTool();
	
	
	//vector<NvBlastChunkDesc> chunkDescs;
	//chunkDescs.resize(2);
	//chunkDescs[0].parentChunkDescIndex = UINT32_MAX;
	//chunkDescs[0].centroid[0] = 0.0f;
	//chunkDescs[0].centroid[1] = 0.0f;
	//chunkDescs[0].centroid[2] = 0.0f;
	//chunkDescs[0].volume = 1.0f;    // Unit volume
	//chunkDescs[0].flags = NvBlastChunkDesc::NoFlags;
	//chunkDescs[0].userData = 0;

	//chunkDescs[1].parentChunkDescIndex = 0;
	//chunkDescs[1].centroid[0] = 2.0f;
	//chunkDescs[1].centroid[1] = 4.0f;
	//chunkDescs[1].centroid[2] = 6.0f;
	//chunkDescs[1].volume = 1.0f;    // Unit volume
	//chunkDescs[1].flags = NvBlastChunkDesc::SupportFlag; // This chunk should be represented in the support graph
	//chunkDescs[1].userData = 1;

	//vector<NvBlastBondDesc> bondDescs;
	//bondDescs.resize(2);
	//bondDescs[0].chunkIndices[0] = 1;
	//bondDescs[0].chunkIndices[1] = 2;
	//bondDescs[0].bond.normal[0] = 1.0f;
	//bondDescs[0].bond.normal[1] = 0.0f;
	//bondDescs[0].bond.normal[2] = 0.0f;
	//bondDescs[0].bond.area = 1.0f;
	//bondDescs[0].bond.centroid[0] = 1.0f;
	//bondDescs[0].bond.centroid[1] = 2.0f;
	//bondDescs[0].bond.centroid[2] = 3.0f;

	//bondDescs[1].chunkIndices[0] = 1;
	//bondDescs[1].chunkIndices[1] = ~0;		//유효하지 않은 인덱스를 나태냄
	//bondDescs[1].bond.normal[0] = 0.0f;
	//bondDescs[1].bond.normal[1] = 1.0f;
	//bondDescs[1].bond.normal[2] = 0.0f;
	//bondDescs[1].bond.area = 1.0f;
	//bondDescs[1].bond.centroid[0] = 2.0f;
	//bondDescs[1].bond.centroid[1] = 3.0f;
	//bondDescs[1].bond.centroid[2] = 4.0f;


	//TkAssetDesc assetDesc;



	//assetDesc.chunkCount = static_cast<uint32_t>(chunkDescs.size());
	//assetDesc.chunkDescs = chunkDescs.data();
	//assetDesc.bondCount = static_cast<uint32_t>(bondDescs.size());
	//assetDesc.bondDescs = bondDescs.data();

	//vector<uint8_t> bondFlags(bondDescs.size(), 0);
	//bondFlags[0] |= TkAssetDesc::BondFlags::BondJointed;
	//assetDesc.bondFlags = bondFlags.data();



	//vector<char> scratch(chunkDescs.size() * sizeof(NvBlastChunkDesc));
	//NvBlastEnsureAssetExactSupportCoverage(chunkDescs.data(), chunkDescs.size(), scratch.data(), nullptr);
	//vector<uint32_t> map(chunkDescs.size());
	//NvBlastReorderAssetDescChunks(chunkDescs.data(), chunkDescs.size(), bondDescs.data(), bondDescs.size(), map.data(), true, scratch.data(), nullptr);

	//TkAsset* m_pAsset = m_pFramework->createAsset(assetDesc);
	//if(nullptr == m_pAsset)
	//	return E_FAIL;

	//TkActorDesc actorDesc;
	//actorDesc.asset = m_pAsset;

	//TkActor* m_pActor = m_pFramework->createActor(actorDesc);
	//if (nullptr == m_pActor)
	//	return E_FAIL;

	//TkGroupDesc groupDesc;
	//groupDesc.workerCount = 1;
	//TkGroup* m_pGroup = m_pFramework->createGroup(groupDesc);
	//if (nullptr == m_pGroup)
	//	return E_FAIL;
	//
	//m_pGroup->addActor(*m_pActor);

	//

	//scratch.resize(NvBlastGetRequiredScratchForCreateAsset(&assetDesc, NULL));
	//void* mem = malloc(NvBlastGetAssetMemorySize(&assetDesc, NULL));
	//NvBlastAsset* asset = NvBlastCreateAsset(mem, &assetDesc, scratch.data(), NULL);



	//NvBlastExtRadialDamageDesc damageDesc;
	//damageDesc.position[0] = 10.f;
	//damageDesc.position[1] = 10.f;
	//damageDesc.position[2] = 10.f;
	//damageDesc.minRadius = 0.0f;
	//damageDesc.maxRadius = 10.0f;
	//damageDesc.damage = 0.5f;


	//NvBlastExtProgramParams programParams = { &damageDesc };
	//NvBlastDamageProgram program;
	
	//	NvBlastExtAuthoringCreateMesh

	return S_OK;
}

CBlastMgr* CBlastMgr::Create()
{
	CBlastMgr* pInstance = new CBlastMgr();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBlastMgr Created Failed");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CBlastMgr::Free()
{
	Safe_physX_Release(m_pFramework);
}
