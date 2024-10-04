#include "stdafx.h"
#include "EffectManager.h"
#include "GameInstance.h"
#include "Particle_Rect.h"
#include "ParticleMesh.h"
#include "Particle_Point.h"
#include "FireEffect.h"

IMPLEMENT_SINGLETON(CEffectManager)

CEffectManager::CEffectManager()
{
}

HRESULT CEffectManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_GameObjects()))
	{
		MSG_BOX("Failed_Ready_Prototype");
		return E_FAIL;
	}
	if (FAILED(Load_Trails()))
	{
		MSG_BOX("FAILED_Load_Trail");
		return E_FAIL;
	}
	if (FAILED(Load_SwordTrails()))
	{
		MSG_BOX("FAILED_Load_SwordTrail");
		return E_FAIL;
	}
	if (FAILED(Load_Particles()))
	{
		MSG_BOX("FAILED_Load_SwordTrail");
		return E_FAIL;
	}
	if (FAILED(Load_Distortions()))
	{
		MSG_BOX("FAILED_Load_Distortion");
		return E_FAIL;
	}
	if (FAILED(Load_Lightnings()))
	{
		MSG_BOX("FAILED_Load_Lightnings");
		return E_FAIL;
	}
	if (FAILED(Load_Tornados()))
	{
		MSG_BOX("FAILED_Load_Tornado");
		return E_FAIL;
	}
	if (FAILED(Load_Heals()))
	{
		MSG_BOX("FAILED_Load_Heal");
		return E_FAIL;
	}
	if (FAILED(Load_Lazers()))
	{
		MSG_BOX("FAILED_Load_Lazer");
		return E_FAIL;
	}
	if (FAILED(Load_Swing()))
	{
		MSG_BOX("FAILED_Load_Swing");
		return E_FAIL;
	}
	if (FAILED(Load_Meteor()))
	{
		MSG_BOX("FAILED_Load_Meteor");
		return E_FAIL;
	}
	if (FAILED(Load_FirePillar()))
	{
		MSG_BOX("FAILED_Load_FirePillar");
		return E_FAIL;
	}
	if (FAILED(Load_Needle()))
	{
		MSG_BOX("FAILED_Load_Needle");
		return E_FAIL;
	}
	if (FAILED(Load_GroundSlash()))
	{
		MSG_BOX("FAILED_Load_GroundSlash");
		return E_FAIL;
	}
	if (FAILED(Load_HammerSpawn()))
	{
		MSG_BOX("FAILED_Load_HammerSpawn");
		return E_FAIL;
	}
	if (FAILED(Load_HexaShield()))
	{
		MSG_BOX("FAILED_Load_HexaShield");
		return E_FAIL;
	}
	if (FAILED(Load_FireFly()))
	{
		MSG_BOX("FAILED_Load_FireFly");
		return E_FAIL;
	}
	if (FAILED(Load_BlackHole()))
	{
		MSG_BOX("FAILED_Load_BlackHole");
		return E_FAIL;
	}
	if (FAILED(Load_WellCylinder()))
	{
		MSG_BOX("FAILED_Load_WellCylinder");
		return E_FAIL;
	}
	if (FAILED(Load_Magic_Cast()))
	{
		MSG_BOX("FAILED_Load_Magic_Cast");
		return E_FAIL;
	}
	if (FAILED(Load_CutSceneAndras()))
	{
		MSG_BOX("FAILED_Load_CutSceneAndras");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CEffectManager::Generate_Trail(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= Trailes.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}

	CParticle_Trail::TRAIL_DESC* traildesc = Trailes[iIndex].get();
	traildesc->traildesc.ParentMat = BindMat;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), traildesc);
	return S_OK;
}

CGameObject* CEffectManager::Member_Trail(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= Trailes.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	CParticle_Trail::TRAIL_DESC* traildesc = Trailes[iIndex].get();
	traildesc->traildesc.ParentMat = BindMat;
	return CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_Trail"), traildesc);
}

HRESULT CEffectManager::Generate_SwordTrail(const _int iIndex, const _float4x4* Swordmat)
{
	if (iIndex >= m_pSwordTrailes.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CSTrail::STRAIL_DESC* SwordTrail = m_pSwordTrailes[iIndex].get();
		SwordTrail->traildesc.ParentMat = Swordmat;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SwordTrail"), SwordTrail);
	}
	return S_OK;
}

CGameObject* CEffectManager::Generate_Particle(const _int iIndex,
	const _float4 vStartpos,
	CGameObject* pTarget,
	const _vector vAxis,
	const _float fRadians,
	const _vector vLook)
{
	if (iIndex >= m_Particles.size())
	{
		MSG_BOX("인덱스 사이즈 초과, 파티클 생성 실패");
		return nullptr;
	}

	PARTICLETYPE eType = m_Particles[iIndex].first;

	switch (eType)
	{
	case Client::PART_POINT:
	{
		((CParticle_Point::PARTICLEPOINT*)m_Particles[iIndex].second)->SuperDesc.vStartPos = vStartpos;
		CParticle_Point* pPoint = static_cast<CParticle_Point*>(CGameInstance::GetInstance()->Clone_Object(
			TEXT("Prototype_GameObject_ParticlePoint"),
			m_Particles[iIndex].second));
		if (pTarget != nullptr)
			pPoint->Set_Target(pTarget);

		if(!XMVector4Equal(vAxis, XMVectorZero()))
			pPoint->Set_Rotation(fRadians, vAxis);
		if (!XMVector4Equal(vLook, XMVectorZero()))
			pPoint->AdJustLook(vLook);
		CGameInstance::GetInstance()->CreateObject_Self(CGameInstance::GetInstance()->Get_CurrentLevelIndex(),
			TEXT("Layer_Effect"), pPoint);
		return pPoint;
		break;
	}
	case Client::PART_MESH:
	{
		((CParticleMesh::PARTICLEMESH*)m_Particles[iIndex].second)->SuperDesc.vStartPos = vStartpos;
		CParticleMesh* pMesh = static_cast<CParticleMesh*>(CGameInstance::GetInstance()->Clone_Object(
			TEXT("Prototype_GameObject_ParticleMesh"),
			m_Particles[iIndex].second));
		if (pTarget != nullptr)
			pMesh->Set_Target(pTarget);
		if (!XMVector4Equal(vAxis, XMVectorZero()))
			pMesh->Set_Rotation(fRadians, vAxis);
		if (!XMVector4Equal(vLook, XMVectorZero()))
			pMesh->AdJustLook(vLook);
		CGameInstance::GetInstance()->CreateObject_Self(CGameInstance::GetInstance()->Get_CurrentLevelIndex(),
			TEXT("Layer_Effect"), pMesh);
		return pMesh;
		break;
	}
	case Client::PART_RECT:
	{
		((CParticle_Rect::PARTICLERECT*)m_Particles[iIndex].second)->SuperDesc.vStartPos = vStartpos;
		CParticle_Rect* pRect = static_cast<CParticle_Rect*>(CGameInstance::GetInstance()->Clone_Object(
			TEXT("Prototype_GameObject_ParticleRect"),
			m_Particles[iIndex].second));
		if (pTarget != nullptr)
			pRect->Set_Target(pTarget);
		if (!XMVector4Equal(vAxis, XMVectorZero()))
			pRect->Set_Rotation(fRadians, vAxis);
		if (!XMVector4Equal(vLook, XMVectorZero()))
			pRect->AdJustLook(vLook);
		CGameInstance::GetInstance()->CreateObject_Self(CGameInstance::GetInstance()->Get_CurrentLevelIndex(),
			TEXT("Layer_Effect"), pRect);
		return pRect;
		break;
	}
	case Client::PART_END:
		return nullptr;
	default:
		return nullptr;
	}

	return nullptr;
}

HRESULT CEffectManager::Generate_Distortion(const _int iIndex, const _float4 vStartpos)
{
	if (iIndex >= m_pDistortions.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CDistortionEffect::DISTORTIONEFFECT* Distortion = m_pDistortions[iIndex].get();
		Distortion->vStartpos = vStartpos;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Distortion_Effect"), Distortion);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Lightning(const _int iIndex, const _float4 vStartpos)
{
	if (iIndex >= m_pLightnings.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CElectronic::ELECTRONICDESC* Lightning = m_pLightnings[iIndex].get();
		Lightning->vStartPos = vStartpos;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electronic_Effect"), Lightning);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Tornado(const _int iIndex, const _float4 vStartpos, CGameObject* pTarget)
{
	if (iIndex >= m_Tornados.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CTornadoEffect::TORNADODESC* pDesc = m_Tornados[iIndex].get();
		pDesc->vStartPos = vStartpos;
		pDesc->pTarget = pTarget;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Tornado"), pDesc);

	}
	return S_OK;
}

HRESULT CEffectManager::Generate_HealEffect(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= m_Heals.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CHealEffect::HEALEFFECT* pDesc = m_Heals[iIndex].get();
		pDesc->ParentMat = BindMat;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_HealEffect"), pDesc);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Magic_Cast(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= m_MagicCast.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CMagicCast::MAGIC_CAST* pDesc = m_MagicCast[iIndex].get();
		pDesc->ParentMatrix = BindMat;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Magic_Cast"), pDesc);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Lazer(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= m_Lazers.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CAndrasLazer::ANDRAS_LAZER_TOTALDESC* Desc = m_Lazers[iIndex].get();
		Desc->ShooterMat = BindMat;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AndrasLazerSpawner"), Desc);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Swing(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= m_Swings.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CSwingEffect::SWINGEFFECT* Desc = m_Swings[iIndex].get();
		Desc->ParentMat = BindMat;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SwingEffect"), Desc);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Meteor(const _float4 vStartPos)
{
	CMeteor::METEOR_DESC* Desc = m_Meteor.get();
	Desc->vTargetPos = vStartPos;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Meteor"), Desc);
	return S_OK;
}

HRESULT CEffectManager::Generate_FirePillar(const _float4 vStartPos)
{
	CFirePillar::FIREPILLAR* Desc = m_FirePillar.get();
	Desc->vStartPos = vStartPos;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_FirePillar"), Desc);
	return S_OK;
}

HRESULT CEffectManager::Generate_Needle(const _float4 vStartPos)
{
	CNeedleSpawner::NEEDLESPAWNER* Desc = m_Needle.get();
	Desc->vStartPos = vStartPos;
	Desc->vStartPos.y += 0.1f;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_NeedleSpawner"), Desc);
	return S_OK;
}

HRESULT CEffectManager::Generate_GroundSlash(const _float4 vStartPos, const _float4 vDirection)
{
	CGroundSlash::GROUNDSLASH* Desc = m_GroundSlash.get();
	Desc->vStartPos = vStartPos;
	Desc->vDirection = vDirection;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_GroundSlash"), Desc);
	return S_OK;
}

HRESULT CEffectManager::Generate_HammerSpawn(const _float4 vStartPos)
{
	CHammerSpawn::HAMMERSPAWN* Desc = m_HammerSpawn.get();
	Desc->vStartPos = vStartPos;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_HammerSpawn"), Desc);
	return S_OK;
}

HRESULT CEffectManager::Generate_BlackHole(const _int iIndex, const _float4 vStartPos , const _uint Level)
{
	if (iIndex >= m_BlackHoles.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CBlackHole::BLACKHOLE* Desc = m_BlackHoles[iIndex].get();
		Desc->vStartPos = vStartPos;
		LEVEL eLevel =(LEVEL)CGameInstance::GetInstance()->Get_CurrentLevelIndex();
		CGameInstance::GetInstance()->Add_CloneObject(Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BlackHole"), Desc);
		//CGameInstance::GetInstance()->CreateObject(Level,
		//	TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BlackHole"), Desc);

		if (iIndex == 0)
		{
			Generate_Particle(103, vStartPos);
			Generate_Particle(104, vStartPos);
			Generate_Particle(106, vStartPos);
			Generate_Particle(107, vStartPos, nullptr, XMVectorSet(1.f,0.f,0.f,0.f), 90.f);
			Generate_Particle(108, vStartPos);
			Generate_Particle(109, vStartPos);
			Generate_Distortion(6, vStartPos);


			_float4 vLastPos = vStartPos;
			vLastPos.y -= 3.f;
			Generate_Particle(105, vLastPos);
		}
		else
		{
			Generate_Particle(111, vStartPos);
			Generate_Particle(112, vStartPos);
			Generate_Particle(113, vStartPos);
			Generate_Particle(114, vStartPos);
		}

	}
	return S_OK;
}

HRESULT CEffectManager::Generate_WellCylinder(const _float4x4* BindMat)
{
	CWellCylinder::WELLCYLINDER* Desc = m_WellCylinder.get();
	Desc->ParentMatrix = BindMat;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_WellCylinder"), Desc);
	return S_OK;
}

HRESULT CEffectManager::Generate_CutSceneAndras(const _float4 vStartPos)
{
	CutSceneAndras::CUTSCENEANDRAS* Desc = m_CutSceneAndras.get();
	Desc->vPosition = vStartPos;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_CutSceneAndras"), Desc);
	return S_OK;
}

CGameObject* CEffectManager::Generate_HexaShield(const _float4x4* BindMat)
{
	CHexaShield::HEXASHIELD* Desc = m_HexaShield.get();
	Desc->ParentMatrix = BindMat;

	CGameObject* HexaShield = CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_HexaShield"), Desc);
	CGameInstance::GetInstance()->CreateObject_Self(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), HexaShield);
	return HexaShield;
}

HRESULT CEffectManager::Generate_FireFly(const _float4x4* BindMat)
{
	CFireFlyCube::FIREFLYCUBE* Desc = m_FireFly.get();
	Desc->ParentMatrix = BindMat;

	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_FireFly"), Desc);
	return S_OK;
}


HRESULT CEffectManager::Load_Trails()
{
	string FilePath = "../Bin/BinaryFile/Effect/Trail.Bin";
	ifstream Load(FilePath, std::ios::binary);
	if (!Load.good())
		return E_FAIL;
	if (!Load.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	Load.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CParticle_Trail::TRAIL_DESC> readFile = make_shared<CParticle_Trail::TRAIL_DESC>();
		Load.read((char*)&readFile->traildesc, sizeof(CVIBuffer_Trail::TRAILDESC));
		readFile->traildesc.ParentMat = nullptr;
		Load.read((char*)&readFile->vStartColor, sizeof(_float3));
		Load.read((char*)&readFile->vEndColor, sizeof(_float3));
		Load.read((char*)&readFile->vBloomColor, sizeof(_float3));
		Load.read((char*)&readFile->fBloompower, sizeof(_float));
		Load.read((char*)&readFile->Desolve, sizeof(_bool));
		Load.read((char*)&readFile->Blur, sizeof(_bool));
		Load.read((char*)&readFile->Bloom, sizeof(_bool));
		Load.read((char*)&readFile->eFuncType, sizeof(TRAILFUNCTYPE));
		Load.read((char*)&readFile->DesolveNum, sizeof(_int));
		readFile->Texture = load_wstring_from_stream(Load);
		readFile->TexturePath = load_wstring_from_stream(Load);
		if (FAILED(Add_Texture_Prototype(readFile->TexturePath, readFile->Texture)))
			return E_FAIL;
		Trailes.emplace_back(readFile);
	}
	Load.close();


	return S_OK;
}

HRESULT CEffectManager::Load_SwordTrails()
{
	string finalPath = "../Bin/BinaryFile/Effect/SwordTrail.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CSTrail::STRAIL_DESC> readFile = make_shared<CSTrail::STRAIL_DESC>();
		inFile.read((char*)&readFile->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		readFile->traildesc.ParentMat = nullptr;
		inFile.read((char*)&readFile->isBloom, sizeof(_bool));
		inFile.read((char*)&readFile->isDestortion, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vColor, sizeof(_float3));
		inFile.read((char*)&readFile->vBloomColor, sizeof(_float3));
		inFile.read((char*)&readFile->fBloomPower, sizeof(_float));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_pSwordTrailes.emplace_back(readFile);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Particles()
{
	string finalPath = "../Bin/BinaryFile/Effect/Particles.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		PARTICLETYPE type;
		inFile.read((char*)&type, sizeof(PARTICLETYPE));
		switch (type)
		{
		case Client::PART_POINT:
		{
			CParticle_Point::PARTICLEPOINT* Arg = new CParticle_Point::PARTICLEPOINT;
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->SuperDesc.vStartPos = { 0.f,0.f,0.f,1.f };
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_POINT;
			m_Particles.emplace_back(make_pair(PART_POINT, Arg));
			break;
		}
		case Client::PART_MESH:
		{
			CParticleMesh::PARTICLEMESH* Arg = new CParticleMesh::PARTICLEMESH();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->SuperDesc.vStartPos = { 0.f,0.f,0.f,1.f };
			inFile.read((char*)&Arg->eModelType, sizeof(EFFECTMODELTYPE));
			Arg->particleType = PART_MESH;
			m_Particles.emplace_back(make_pair(PART_MESH, Arg));
			break;
		}
		case Client::PART_RECT:
		{
			CParticle_Rect::PARTICLERECT* Arg = new CParticle_Rect::PARTICLERECT();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->SuperDesc.vStartPos = { 0.f,0.f,0.f,1.f };
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_RECT;
			m_Particles.emplace_back(make_pair(PART_RECT, Arg));
			break;
		}
		default:
			break;
		}
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_Distortions()
{
	string finalPath = "../Bin/BinaryFile/Effect/Distortion.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CDistortionEffect::DISTORTIONEFFECT> readFile = make_shared<CDistortionEffect::DISTORTIONEFFECT>();
		inFile.read((char*)&readFile->bFuncType, sizeof(_bool));
		inFile.read((char*)&readFile->bDisolve, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vStartScale, sizeof(_float2));
		inFile.read((char*)&readFile->vStartpos, sizeof(_float4));
		inFile.read((char*)&readFile->fSpeed, sizeof(_float));
		inFile.read((char*)&readFile->fLifeTime, sizeof(_float));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_pDistortions.emplace_back(readFile);
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_Lightnings()
{
	string finalPath = "../Bin/BinaryFile/Effect/Lightning.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CElectronic::ELECTRONICDESC readFile{};
		inFile.read((char*)&readFile, sizeof(CElectronic::ELECTRONICDESC));
		shared_ptr<CElectronic::ELECTRONICDESC> StockValue = make_shared<CElectronic::ELECTRONICDESC>(readFile);
		m_pLightnings.emplace_back(StockValue);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Tornados()
{
	string finalPath = "../Bin/BinaryFile/Effect/Tornados.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CTornadoEffect::TORNADODESC readFile{};
		inFile.read((char*)&readFile, sizeof(CTornadoEffect::TORNADODESC));
		readFile.pTarget = nullptr;
		shared_ptr<CTornadoEffect::TORNADODESC> StockValue = make_shared<CTornadoEffect::TORNADODESC>(readFile);
		m_Tornados.emplace_back(StockValue);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Heals()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Heals.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CHealEffect::HEALEFFECT readFile{};
		inFile.read((char*)&readFile, sizeof(CHealEffect::HEALEFFECT));
		readFile.ParentMat = nullptr;
		shared_ptr<CHealEffect::HEALEFFECT> StockValue = make_shared<CHealEffect::HEALEFFECT>(readFile);
		m_Heals.emplace_back(StockValue);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Lazers()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lazer.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CAndrasLazer::ANDRAS_LAZER_TOTALDESC readFile{};
		inFile.read((char*)&readFile, sizeof(CAndrasLazer::ANDRAS_LAZER_TOTALDESC));
		readFile.ShooterMat = nullptr;
		shared_ptr<CAndrasLazer::ANDRAS_LAZER_TOTALDESC> StockValue = make_shared<CAndrasLazer::ANDRAS_LAZER_TOTALDESC>(readFile);
		m_Lazers.emplace_back(StockValue);
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_Swing()
{
	string finalPath = "../Bin/BinaryFile/Effect/Swings.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CSwingEffect::SWINGEFFECT readFile{};
		inFile.read((char*)&readFile, sizeof(CSwingEffect::SWINGEFFECT));
		readFile.ParentMat = nullptr;
		shared_ptr<CSwingEffect::SWINGEFFECT> StockValue = make_shared<CSwingEffect::SWINGEFFECT>(readFile);
		m_Swings.emplace_back(StockValue);
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_BlackHole()
{
	string finalPath = "../Bin/BinaryFile/Effect/BlackHole.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CBlackHole::BLACKHOLE readFile{};
		inFile.read((char*)&readFile, sizeof(CBlackHole::BLACKHOLE));
		shared_ptr<CBlackHole::BLACKHOLE> StockValue = make_shared<CBlackHole::BLACKHOLE>(readFile);
		m_BlackHoles.emplace_back(StockValue);
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_Meteor()
{
	string finalPath = "../Bin/BinaryFile/Effect/Meteor.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_Meteor = make_shared<CMeteor::METEOR_DESC>();
	inFile.read((char*)m_Meteor.get(), sizeof(CMeteor::METEOR_DESC));
	inFile.close();

	m_Meteor->vTargetPos = { 0.f,0.f,0.f,1.f };
	return S_OK;
}

HRESULT CEffectManager::Load_FirePillar()
{
	string finalPath = "../Bin/BinaryFile/Effect/FirePillar.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_FirePillar = make_shared<CFirePillar::FIREPILLAR>();
	inFile.read((char*)m_FirePillar.get(), sizeof(CFirePillar::FIREPILLAR));
	inFile.close();

	m_FirePillar->vStartPos = { 0.f,0.f,0.f,1.f };
	return S_OK;
}

HRESULT CEffectManager::Load_Needle()
{
	string finalPath = "../Bin/BinaryFile/Effect/HedgeHog.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_Needle = make_shared<CNeedleSpawner::NEEDLESPAWNER>();
	inFile.read((char*)m_Needle.get(), sizeof(CNeedleSpawner::NEEDLESPAWNER));
	inFile.close();
	m_Needle->vStartPos = { 0.f,0.f,0.f,1.f };
	return S_OK;
}

HRESULT CEffectManager::Load_GroundSlash()
{
	string finalPath = "../Bin/BinaryFile/Effect/GroundSlash.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_GroundSlash = make_shared<CGroundSlash::GROUNDSLASH>();
	inFile.read((char*)m_GroundSlash.get(), sizeof(CGroundSlash::GROUNDSLASH));
	inFile.close();
	m_GroundSlash->vStartPos = { 0.f,0.f,0.f,1.f };
	m_GroundSlash->vDirection = { 0.f,0.f,0.f,0.f };
	return S_OK;
}

HRESULT CEffectManager::Load_HammerSpawn()
{
	string finalPath = "../Bin/BinaryFile/Effect/HammerSpawn.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_HammerSpawn = make_shared<CHammerSpawn::HAMMERSPAWN>();
	inFile.read((char*)m_HammerSpawn.get(), sizeof(CHammerSpawn::HAMMERSPAWN));
	inFile.close();
	m_HammerSpawn->vStartPos = { 0.f,0.f,0.f,1.f };
	return S_OK;
}

HRESULT CEffectManager::Load_HexaShield()
{
	string finalPath = "../Bin/BinaryFile/Effect/HexaShield.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_HexaShield = make_shared<CHexaShield::HEXASHIELD>();
	inFile.read((char*)m_HexaShield.get(), sizeof(CHexaShield::HEXASHIELD));
	inFile.close();
	m_HexaShield->ParentMatrix = nullptr;
	return S_OK;
}

HRESULT CEffectManager::Load_FireFly()
{
	string finalPath = "../Bin/BinaryFile/Effect/FireFly.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_FireFly = make_shared<CFireFlyCube::FIREFLYCUBE>();
	inFile.read((char*)m_FireFly.get(), sizeof(CFireFlyCube::FIREFLYCUBE));
	inFile.close();
	m_FireFly->ParentMatrix = nullptr;
	return S_OK;
}

HRESULT CEffectManager::Load_WellCylinder()
{
	string finalPath = "../Bin/BinaryFile/Effect/WellCylinder.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_WellCylinder = make_shared<CWellCylinder::WELLCYLINDER>();
	inFile.read((char*)m_WellCylinder.get(), sizeof(CWellCylinder::WELLCYLINDER));
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Magic_Cast()
{
	string finalPath = "../Bin/BinaryFile/Effect/MagicCast.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CMagicCast::MAGIC_CAST readFile{};
		inFile.read((char*)&readFile, sizeof(CMagicCast::MAGIC_CAST));
		shared_ptr<CMagicCast::MAGIC_CAST> StockValue = make_shared<CMagicCast::MAGIC_CAST>(readFile);
		StockValue->ParentMatrix = nullptr;
		m_MagicCast.emplace_back(StockValue);
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_CutSceneAndras()
{
	string finalPath = "../Bin/BinaryFile/Effect/AndrasCutScene.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	m_CutSceneAndras = make_shared<CutSceneAndras::CUTSCENEANDRAS>();
	inFile.read((char*)m_CutSceneAndras.get(), sizeof(CutSceneAndras::CUTSCENEANDRAS));
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Ready_GameObjects()
{
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ParticleMesh"),
		CParticleMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ParticlePoint"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ParticleRect"),
		CParticle_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Trail"),
		CParticle_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_SwordTrail"),
		CSTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Distortion_Effect"),
		CDistortionEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Fire_Effect"),
		CFireEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Electronic_Effect"),
		CElectronic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado"),
		CTornadoEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado_Wind"),
		CTornado_Wind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado_Root"),
		CTornado_Root::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado_Ring"),
		CTornado_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Heal
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect"),
		CHealEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect_Ribbon"),
		CHeal_Ribbon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect_Spiral"),
		CHeal_Spiral::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect_Line"),
		CHeal_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Andras_LazerBase"),
		CAndrasLazerBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Andras_LazerCylinder"),
		CAndrasLazerCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Andras_Screw"),
		CAndrasScrew::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ElectricCylinder"),
		CElectricCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_AndrasRain"),
		CAndrasRain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_AndrasLazerSpawner"),
		CAndrasLazer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_LazerCast"),
		CLazerCast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ShieldShpere"),
		CShieldSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Swing
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_SwingEffect"),
		CSwingEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Swing_Spiral"),
		CSwing_Spiral::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Vane"),
		Charge_Vane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ChargeRibbon"),
		Charge_Ribbon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_DefaultCylinder"),
		CDefaultCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Meteor"),
		CMeteor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Meteor_Core"),
		CMeteor_Core::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Meteor_Wind"),
		CMeteor_Wind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_FirePillar"),
		CFirePillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_FirePillar_Effect"),
		CFirePillarEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_FirePillar_Bottom"),
		CFirePillar_Bottom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_FirePillar_Charge"),
		CFirePillar_Charge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Rock_Ground"),
		CRock_Ground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Hedgehog"),
		CHedgehog::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_NeedleSpawner"),
		CNeedleSpawner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_GroundSlash"),
		CGroundSlash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HammerSpawn"),
		CHammerSpawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HexaShield"),
		CHexaShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HexaHit"),
		CHexaHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_FireFly"),
		CFireFlyCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_BlackHole"),
		CBlackHole::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_BlackSphere"),
		CBlackSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_BlackHoleRing"),
		CBlackHole_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_BlackHoleRing_Bill"),
		CBlackHole_Ring_Bill::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Black_Horizon"),
		CBlackHorizon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_WellCylinder"),
		CWellCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//MagicCast
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Magic_Cast"),
		CMagicCast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HelixCast"),
		CHelixCast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_BezierCurve"),
		CBezierCurve::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_NewAspiration"),
		CNewAspiration::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_CutSceneAndras"),
		CutSceneAndras::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_AndrasPillar"),
		CAndrasCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_AndrasSphere"),
		CAndrasSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectManager::Add_Texture_Prototype(const wstring& path, const wstring& name)
{
	if (CGameInstance::GetInstance()->IsPrototype(LEVEL_STATIC, name) == true)
		return S_OK;
	else
	{
		if (FAILED(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, name,
			CTexture::Create(m_pDevice, m_pContext, path, 1))))
			return E_FAIL;
	}
	return S_OK;
}

void CEffectManager::Dynamic_Deallocation()
{
	for (auto& iter : m_Particles)
	{
		switch (iter.first)
		{
		case PARTICLETYPE::PART_POINT:
			delete ((CParticle_Point::PARTICLEPOINT*)iter.second);
			iter.second = nullptr;
			break;
		case PARTICLETYPE::PART_MESH:
			delete ((CParticleMesh::PARTICLEMESH*)iter.second);
			iter.second = nullptr;
			break;
		case PARTICLETYPE::PART_RECT:
			delete ((CParticle_Rect::PARTICLERECT*)iter.second);
			iter.second = nullptr;
			break;
		default:
			break;
		}
	}
	
}

void CEffectManager::Free()
{
	Dynamic_Deallocation();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
