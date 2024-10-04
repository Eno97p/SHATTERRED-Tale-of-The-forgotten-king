#include "BehaviorTree.h"
#include "CSequence.h"
#include "CSelector.h"
#include "CActionNode.h"
#include "CInverter.h"
#include "CoolDownNode.h"
#include "ConditionNode.h"
#include "CoolDown_Priority.h"
CBehaviorTree::CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& rhs)
	:CComponent(rhs)
{
}

HRESULT CBehaviorTree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBehaviorTree::Initialize(void* pArg)
{
	return S_OK;
}

void CBehaviorTree::Generate_Root(const wstring& rootName, NodeType _etype)
{
	switch (_etype)
	{
	case Sequence:
		m_RootNode = CSequence::create();
		break;
	case Selector:
		m_RootNode = CSelector::Create();
		break;
	default:
		MSG_BOX("루트 노드는 Selector와 Sequence 만 가능합니다");
		return;
	}
	Nodes.emplace(rootName,m_RootNode);
}

//void CBehaviorTree::Add_Node(const wstring& parents_name, const wstring& nodename, NodeType _etype,
//	function<NodeStates(_float)> action, Decorator eDeco, _float _fCooltime)
//{
//	auto	iter = Nodes.find(parents_name);
//	if (iter == Nodes.end())
//	{
//		MSG_BOX("Failed find Parents");
//		return;
//	}
//	else
//	{
//		CNode* Node = nullptr;
//		switch (_etype)
//		{
//		case Sequence:
//			if (eDeco != Deco_End)
//				Node = CSequence::create(eDeco, _fCooltime);
//			else
//				Node = CSequence::create();
//			iter->second->Add_Nodes(Node);
//			Nodes.emplace(nodename, Node);
//			break;
//		case Selector:
//			if (eDeco == CoolDown)
//				Node = CSelector::Create(eDeco, _fCooltime);
//			else
//				Node = CSelector::Create();
//			iter->second->Add_Nodes(Node);
//			Nodes.emplace(nodename, Node);
//			break;
//		case Inverter:
//			Node = CInverter::Create();
//			iter->second->Add_Nodes(Node);
//			Nodes.emplace(nodename, Node);
//			break;
//		case Action:
//			if (eDeco == CoolDown)
//				Node = CActionNode::create(action, eDeco, _fCooltime);
//			else
//				Node = CActionNode::create(action);
//			iter->second->Add_Nodes(Node);
//			break;
//		}
//	}
//}

void CBehaviorTree::Add_Composit_Node(const wstring& parents_name, const wstring& nodename, NodeType _etype)
{
	auto	iter = Nodes.find(parents_name);
	if (iter == Nodes.end())
	{
		MSG_BOX("Failed find Parents");
		return;
	}
	else
	{
		CNode* Node = nullptr;
		switch (_etype)
		{
		case Engine::CBehaviorTree::Sequence:
		{
			Node = CSequence::create();
			iter->second->Add_Nodes(Node);
			Nodes.emplace(nodename, Node);
			break;
		}
		case Engine::CBehaviorTree::Selector:
		{
			Node = CSelector::Create();
			iter->second->Add_Nodes(Node);
			Nodes.emplace(nodename, Node);
			break;
		}
		default:
			MSG_BOX("Invailed Node Type");
			return;
		}
	}
}

void CBehaviorTree::Add_Action_Node(const wstring& parents_name, const wstring& nodename, function<NodeStates(_float)> action)
{
	auto	iter = Nodes.find(parents_name);
	if (iter == Nodes.end())
	{
		MSG_BOX("Failed find Parents");
		return;
	}
	else
	{
		CNode* Node = nullptr;
		Node = CActionNode::create(action);
		iter->second->Add_Nodes(Node);
	}
}

void CBehaviorTree::Add_Condition(const wstring& parents_name, const wstring& nodename, _bool* bCondition)
{
	auto	iter = Nodes.find(parents_name);
	if (iter == Nodes.end())
	{
		MSG_BOX("Failed find Parents");
		return;
	}
	else
	{
		CNode* Node = nullptr;
		Node = CConditionNode::Create(bCondition);
		iter->second->Add_Nodes(Node);
		Nodes.emplace(nodename, Node);
	}
}

void CBehaviorTree::Add_Inverter(const wstring& parents_name, const wstring& nodename)
{
	auto	iter = Nodes.find(parents_name);
	if (iter == Nodes.end())
	{
		MSG_BOX("Failed find Parents");
		return;
	}
	else
	{
		CNode* Node = nullptr;
		Node = CInverter::Create();
		iter->second->Add_Nodes(Node);
		Nodes.emplace(nodename, Node);
	}
}

void CBehaviorTree::Add_CoolDown(const wstring& parents_name, const wstring& nodename, _float _fCooltime)
{
	auto	iter = Nodes.find(parents_name);
	if (iter == Nodes.end())
	{
		MSG_BOX("Failed find Parents");
		return;
	}
	else
	{
		CNode* Node = nullptr;
		Node = CCoolDownNode::Create(_fCooltime);
		iter->second->Add_Nodes(Node);
		Nodes.emplace(nodename, Node);
	}
}

void CBehaviorTree::Add_CoolDown_Priority(const wstring& parents_name, const wstring& nodename, _float _fCooltime, _float _firstCool)
{
	auto	iter = Nodes.find(parents_name);
	if (iter == Nodes.end())
	{
		MSG_BOX("Failed find Parents");
		return;
	}
	else
	{
		CNode* Node = nullptr;
		Node = CCoolDownPriority::Create(_fCooltime, _firstCool);
		iter->second->Add_Nodes(Node);
		Nodes.emplace(nodename, Node);
	}
}

void CBehaviorTree::Update(_float fTimeDelta)
{
	if (m_bActivate)
		m_RootNode->Evaluate(fTimeDelta);
	else
		return;
}


CBehaviorTree* CBehaviorTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBehaviorTree* pInstance = new CBehaviorTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Behavior");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBehaviorTree::Clone(void* pArg)
{
	CBehaviorTree* pInstance = new CBehaviorTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Behavior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBehaviorTree::Free()
{
	Safe_Release(m_RootNode);
	__super::Free();
}
