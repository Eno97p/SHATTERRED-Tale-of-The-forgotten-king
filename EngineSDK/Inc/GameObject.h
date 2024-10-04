#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
	{
		_int iData;
		const char* pModelName = nullptr;
		

	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	/*  지정한 이름의 컴포넌트를 가져온다. */
	class CComponent* Get_Component(const wstring& strComponentTag);
	
	void Get_Components(map<const wstring, class CComponent*>& pOut) const { pOut = m_Components; } //함수를 통해 복사본을 만들어서 반환 무거움
	const map<const wstring, class CComponent*>& Get_Components() const  { return m_Components; }//직접 참조로 반환 가벼움 // 캡슐화 깨트림

	// 0713 김보민 추가
	HRESULT				Delete_Component(const wstring& strComponentTag);

//PSW
	template<typename T>
	void Set_Desc(const T& t)
	{
		if (m_pDesc)
		{
			free(m_pDesc);
		}
		m_DescSize = sizeof(T);
		m_pDesc = malloc(m_DescSize);
		memcpy(m_pDesc, &t, m_DescSize);
	}
	void* Get_Desc() { return m_pDesc; }
	size_t Get_DescSize() { return m_DescSize; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render(ID3D11DeviceContext* pDeferredContext){ return S_OK; }
	virtual HRESULT Render_LightDepth() { return S_OK; }
	virtual HRESULT Render_Mirror() { return S_OK; }
	virtual HRESULT Render_Reflection() { return S_OK; }
	virtual HRESULT Render_Blur() { return S_OK; }
	virtual HRESULT Render_Bloom() { return S_OK; }
	virtual HRESULT Render_Distortion() { return S_OK; }
	virtual pair<_uint, _matrix> Render_Decal() { return pair(0, _matrix()); }
	void Set_Dead() { IsDead = true; }
	_bool Get_Dead() { return IsDead; }
	// 오브젝트마다 다르게 작성해야할 수도 있음
	virtual _float Get_LengthFromCamera();
	void Set_ProtoTypeTag(const wstring& wstrPrototypeTag) {m_wstrPrototypeTag = wstrPrototypeTag;}
	const wstring& Get_ProtoTypeTag() { return m_wstrPrototypeTag; }
	virtual _float4	Get_InitPos() { return m_vInitialPos; }
	virtual _float4x4 Get_worldMat();
	
protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	_bool						m_bisVisible = false;
	_bool						m_bMotionBlur = false;
	wstring						m_wstrMoDelName = L"";
	_float4x4					m_PrevWorldMatrix;
	_float4x4					m_PrevViewMatrix;


protected:
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };
	static const _tchar*		m_pTransformTag;
	wstring						m_wstrPrototypeTag = L"";
	void*						m_pDesc = nullptr;
	size_t						m_DescSize = 0;
	_float4 m_vInitialPos = { 0.f, 0.f, 0.f, 1.f };
protected:
	map<const wstring, class CComponent*>		m_Components;		//복사된 컴포넌트를 들고 있음
	_bool IsCloned = false;				//복사된 객체인지 확인용
private:
	_bool IsDead = false;				//이벤트매니저가 true인 애들을 렌더콜이 끝나면 지운다.

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, 
		CComponent** ppOut, void* pArg = nullptr);



public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END