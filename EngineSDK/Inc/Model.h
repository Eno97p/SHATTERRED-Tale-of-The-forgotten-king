#pragma once

#include "Component.h"
#include "Animation.h"
#include "VIBuffer_Instance.h"
#include "PhysXParticle.h"
BEGIN(Engine)

class CMesh;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tagAnimationDesc
	{
		tagAnimationDesc() {}
		tagAnimationDesc(_uint iAnimIndex, _bool isLoop)
			: iAnimIndex{ iAnimIndex }, isLoop{ isLoop } {}
		_uint		iAnimIndex = { 0 };
		_bool		isLoop = { false };
	}ANIMATION_DESC;
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

#pragma region PARTICLE_FUNCTION
public:														//파티클 함수들
	void Spread(_float fTimeDelta);
	void Drop(_float fTimeDelta);
	void GrowOut(_float fTimeDelta);
	void Spread_Size_Up(_float fTimeDelta);
	void Spread_Non_Rotation(_float fTimeDelta);
	void CreateSwirlEffect(_float fTimeDelta);
	void Spread_Speed_Down(_float fTimeDelta);
	void SlashEffect(_float fTimeDelta);
	void Spread_Speed_Down_SizeUp(_float fTimeDelta);
	void Gather(_float fTimeDelta);
	void Extinction(_float fTimeDelta);
	void GrowOutY(_float fTimeDelta);
	void GrowOut_Speed_Down(_float fTimeDelta);
	void Leaf_Fall(_float fTimeDelta);
	void Spiral_Extinction(_float fTimeDelta);
	void Spiral_Expansion(_float fTimeDelta);
	void Lenz_Flare(_float fTimeDelta);
	void Blow(_float fTimeDelta);
	void Up_To_Stop(_float fTimeDelta);
	void Only_Up(_float fTimeDelta);

	void PhysX_Particle_Simulate(_float fTimeDelta);
#pragma endregion PARTICLE_FUNCTION
	_uint Get_Model_Vertices();						//모델의 정점 갯수 반환

	_bool Check_Instance_Dead();						//파티클이 끝났는지 검사해줌
	_bool Check_PhysX_Dead();

	HRESULT Ready_Instance(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);		//인스턴싱할 모델만 호출해준다
	HRESULT Ready_PhysXParticle(const CPhysXParticle::PhysX_Particle_Desc& InstanceDesc);

	HRESULT Ready_Instance_ForMapElements(const CVIBuffer_Instance::INSTANCE_MAP_DESC& InstanceDesc);

	_uint Get_NumMeshes() const {							//메쉬의 갯수 반환
		return m_iNumMeshes;
	}
	_uint Get_NumAnim() const {								//애니메이션의 갯수 반환
		return m_iNumAnimations;
	}
	_bool Get_AnimFinished() const {
		return m_Animations[m_AnimDesc.iAnimIndex]->Get_Finished();
	}

	_double Get_Current_Ratio();							//애니메이션의 현재 진행 비율 (0에서 1사이)
	_bool Get_Ratio_Betwin(_float min, _float max);				//애니메이션의 현재 진행 비율이 min과 max 사이에 있는지
	void StopAnimation(_bool _stop) { m_bAnimStop = _stop; }	//애니메이션 멈추게 하기.

	const _float4x4* Get_BoneCombinedTransformationMatrix(const _char* pBoneName) const;

	// Anim Tool 활용 함수
	vector<string>* Get_AnimNameVec_MemVar();
	vector<string>* Get_ChannelNameVec_MemVar(_uint iAnimIdx);
	vector<string>* Get_BoneNameVec_MemVar();
	_uint			Get_NumKeyFrame(_uint iAnimIdx, _uint iChannelIdx);
	_uint			Get_CurKeyFrame(_uint iAnimIdx, _uint iChannelIdx);

	void			Set_CurKeyFrame(_uint iAnimIdx, _uint iChannelIdx);

	_double			Get_Duration(_uint iAnimIdx) { return m_Animations[iAnimIdx]->Get_Duration(); }
	const _double* Get_CurrentPos(_uint iAnimIdx) const {
		return m_Animations[iAnimIdx]->Get_CurrentPos();
	}

	// 현재 애니메이션의 Duration 체크
	_bool			Check_CurDuration(_double CurDuration);

	// 애니메이션 보완

	void			Set_LerpTime(_double LerpTime);

	void Setting_StartTime(_double StartPos);



public:/*For.Physx*/
	vector<CMesh*> Get_Meshes() { return m_Meshes; }
	vector<CBone*>* Get_Bones() { return &m_Bones; }

	string Get_ModelPath() { return m_fullpath; }

public:
	virtual HRESULT	Initialize_Prototype(MODELTYPE eModelType, const  _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT	Initialize(void* pArg) override;
	HRESULT Render(_uint iMeshIndex);
	HRESULT Render(_uint iMeshIndex, ID3D11DeviceContext* pDeferredContext);
	HRESULT Render_Instance(_uint iMeshIndex);
	HRESULT Render_PhysXInstance(_uint iMeshIndex);
	HRESULT Bind_Material_Instance_ForMapElements(CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Render_Instance_ForMapElements(_uint iMeshIndex);

	HRESULT Bind_Material(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_Material_Instance(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_Material_PhysX(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);

	HRESULT Bind_BoneMatrices(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_PrevBoneMatrices(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex);

	void Play_Animation(_float fTimeDelta, _bool isLerp = true);
	void Set_AnimationIndex(const ANIMATION_DESC& AnimDesc);
	vector<string> Get_AnimNameVec();
	vector<string> Get_BoneNameVec();
	HRESULT Save_AnimName();										// 애니메이션 이름 텍스트로 저장하는거
	HRESULT Save_BoneName();										// 뼈이름 텍스트로 저장하는거


	_bool Culling(_uint iIndex, PxActor* actor);
private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
	_float4x4					m_PreTransformMatrix;
	MODELTYPE					m_eModelType = { TYPE_END };
	_float4x4					m_MeshBoneMatrices[512];
	_float4x4					m_PrevMeshBoneMatrices[256];

private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;
	vector<class CInstance_Mesh*>	m_InstanseMesh;
	vector<CPhysXParticle*>			m_PhysxParticleMesh;
	
private:
	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;


private:
	vector<class CBone*>		m_Bones;

private:
	_uint						m_iNumAnimations = { 0 };
	ANIMATION_DESC				m_AnimDesc{ 0, true};
	vector<class CAnimation*>	m_Animations;

	// AnimTool 활용 변수
	vector<string>				m_szAnimName; // Animation 이름 vec
	vector<string>				m_szBoneName; // Bone 이름 vec
	vector<string>				m_szChannelName; // Channel 이름 vec
	_uint						m_iNumKeyFrame = { 0 }; // Keyframe 개수

	string						m_fullpath;
	_bool						m_bAnimStop = false;

	CAnimation* m_pNextAnimation = nullptr;
	CAnimation* m_pCurrentAnimation = nullptr;
private:
	_bool						isFile = false;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

	void	Add_AnimName();
	void	Add_BoneName();

public:
	HRESULT Save_Model();
	HRESULT Load_Model(_fmatrix PreTransformMatrix);
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END