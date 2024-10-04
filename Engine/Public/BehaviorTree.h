#pragma once
#include "Component.h"
BEGIN(Engine)
class CNode;
END

BEGIN(Engine)

class ENGINE_DLL CBehaviorTree : public CComponent
{
public:
	enum NodeType { Sequence, Selector, Inverter, Action, Node_End};

private:
	CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	void Generate_Root(const wstring& rootName , NodeType _etype);
	void Add_Composit_Node(const wstring& parents_name, const wstring& nodename, NodeType _etype);			//Selector, Sequence 추가할때
	void Add_Action_Node(const wstring& parents_name, const wstring& nodename, function<NodeStates(_float)> action = nullptr);	//액션노드 추가
	void Add_Condition(const wstring& parents_name, const wstring& nodename, _bool* bCondition = nullptr);	//Condition노드 추가
	void Add_Inverter(const wstring& parents_name, const wstring& nodename);								//Inverter 추가
	void Add_CoolDown(const wstring& parents_name, const wstring& nodename, _float _fCooltime = 0.f);
	void Add_CoolDown_Priority(const wstring& parents_name, const wstring& nodename, _float _fCooltime = 0.f, _float _firstCool = 0.f);


	void			Update(_float fTimeDelta);
	void			Set_Activate(_bool _bool) { m_bActivate = _bool; }
private:
	class CNode* m_RootNode = nullptr;
	map<const wstring, class CNode*>				Nodes;
	_bool m_bActivate = true;

public:
	static CBehaviorTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

END