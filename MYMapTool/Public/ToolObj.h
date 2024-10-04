#pragma once

#include "BlendObject.h"
#include "MYMapTool_Defines.h"

#include "Model.h"
#include "Imgui_Manager.h"
// IMGUI���� ������ ��� Obj���� ����ϴ� Ŭ����
// �ִϸ��̼� ����, 

BEGIN(Engine)
class CShader;

END

BEGIN(MYMapTool)

class CToolObj : public CBlendObject//CBlendObject
{
public:
	// Client�� ��ü ���� �� ��� ���� �� ������ ���� ����ü
	typedef struct TOOLOBJ_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_char szObjName[MAX_PATH] = ""; // ��ü �̸�
		_char szLayer[MAX_PATH] = ""; // Layer �̸�
		_char szModelName[MAX_PATH] = ""; // Model �̸�
		_float4x4 mWorldMatrix{};
		CModel::MODELTYPE eModelType; // Anim ����

		_float4 f4Color = {0.f, 0.f, 0.f, 0.f};
		_uint TriggerType = 0;
		//CEventTrigger::TRIGGER_TYPE eTriggerType;

	}TOOLOBJ_DESC;

protected:
	CToolObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolObj(const CToolObj& rhs);
	virtual ~CToolObj() = default;

public:
	_char* Get_Name() { return m_szName; }								//�̸�
	_char* Get_Layer() { return m_szLayer; }
	_char* Get_ModelName();
	_matrix			Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }		// ���� ���
	const _float4x4* Get_WorldFloat4x4() { return m_pTransformCom->Get_WorldFloat4x4(); }
	CModel::MODELTYPE	Get_ModelType() { return m_eModelType; }
	_uint	Get_TriggerType() { return m_iTriggerType; }

	string	Get_ModelPath();

	_bool Get_AlphaBlend() { return m_bisAlphaBlend; }
	void Set_AlphaBlendOn() { m_bisAlphaBlend = true;  }
	void Set_AlphaBlendOff() { m_bisAlphaBlend = false;  }
	_vector Get_Position();
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Render_LightDepth();

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
protected:
	_char				m_szName[MAX_PATH] = "";
	_char				m_szLayer[MAX_PATH] = "";
	_char				m_szModelName[MAX_PATH] = "";
	_vector				m_vPosition;
	CModel::MODELTYPE	m_eModelType;
	_uint				m_iTriggerType = 0;

	_char				m_szListName[MAX_PATH] = "";  // Imgui�� ���� ����� ���� �̸�... ���е�
	_float4				m_vColorOffset = { 0.f, 0.f, 0.f, 0.f };
	_uint				m_iShaderPath = 0;

protected:
	_bool				m_bIsChild = false;
	//for Wind Move Grass
private:
	XMFLOAT3 m_WindDirection;
	float m_WindStrength;
	float m_WindFrequency;
	float m_WindGustiness;
	_float				m_fTimeDelta = 0.f;

	_bool				m_bisAlphaBlend = false;

	ID3D11DepthStencilState* m_pDSS_MapObject_FirstPass = nullptr;
	ID3D11DepthStencilState* m_pDSS_MapObject_SecondPass = nullptr;
	ID3D11DepthStencilState* m_pDSS_RestObjects = nullptr;


public:
	HRESULT	Add_Component();
	HRESULT	Bind_ShaderResources();

	HRESULT	Check_AnimType();
	void	Setting_WorldMatrix(void* pArg);

public:
	HRESULT Create_DepthStencilStates();
	static CToolObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;

};

END