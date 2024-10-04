#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMap_Element : public CBlendObject
{
public:
	
	typedef struct MAP_ELEMENT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_int iInstanceCount = 0;
		vector<_float4x4*> WorldMats;
		wstring wstrObjName = L""; // ��ü �̸�
		wstring wstrLayer = L""; // Layer �̸�
		wstring wstrModelName = L"";  // Model �̸�
		//CModel::MODELTYPE eModelType; // Anim ����

		_uint TriggerType = 0; //Trigger ���� �̺�Ʈ Ÿ��

		void Cleanup()
		{
			for (auto& mat : WorldMats)
			{
				Safe_Delete(mat);
			}
			WorldMats.clear();
		}

		~MAP_ELEMENT_DESC()
		{
			Cleanup();
		}
	}MAP_ELEMENT_DESC;



protected:
	CMap_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_Element(const CMap_Element& rhs);
	virtual ~CMap_Element() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

public:
	HRESULT Add_Components(MAP_ELEMENT_DESC* desc);
	HRESULT Bind_ShaderResources();

public:
	static CMap_Element* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END