#include "stdafx.h"
#include "EBehaviorTree.h"

using namespace Elite;

IBehavior::IBehavior() :
	m_CurrentState{ BehaviorState::Failure }
{

}

BehaviorComposite::BehaviorComposite(std::vector<IBehavior*> childBehaviors) :
	IBehavior{},
	m_ChildBehaviors{ childBehaviors }
{
	
}

BehaviorComposite::~BehaviorComposite()
{
	for (auto pb : m_ChildBehaviors) SAFE_DELETE(pb);
}

BehaviorSelector::BehaviorSelector(std::vector<IBehavior*> childBehaviors) :
	BehaviorComposite{ childBehaviors }
{

}

BehaviorState BehaviorSelector::Execute(Blackboard* pBlackBoard)
{
	for (IBehavior* pBehaviour : m_ChildBehaviors)
	{
		m_CurrentState = pBehaviour->Execute(pBlackBoard);

		if (m_CurrentState != BehaviorState::Failure) break;
	}

	return m_CurrentState;
}

BehaviorSequence::BehaviorSequence(std::vector<IBehavior*> childBehaviors) :
	BehaviorComposite{ childBehaviors }
{

}

BehaviorState BehaviorSequence::Execute(Blackboard* pBlackBoard)
{
	for (IBehavior* pBehaviour : m_ChildBehaviors)
	{
		m_CurrentState = pBehaviour->Execute(pBlackBoard);

		if (m_CurrentState != BehaviorState::Success) break;
	}

	return m_CurrentState;
}

BehaviorPartialSequence::BehaviorPartialSequence(std::vector<IBehavior*> childBehaviors) :
	BehaviorSequence{ childBehaviors },
	m_CurrentBehaviorIndex{ 0 }
{

}

BehaviorState BehaviorPartialSequence::Execute(Blackboard* pBlackBoard)
{
	/*while (m_CurrentBehaviorIndex < m_ChildBehaviors.size())
	{
		m_CurrentState = m_ChildBehaviors[m_CurrentBehaviorIndex]->Execute(pBlackBoard);

		switch (m_CurrentState)
		{
			case BehaviorState::Failure:
				m_CurrentBehaviorIndex = 0;
				return m_CurrentState;
			case BehaviorState::Success:
				++m_CurrentBehaviorIndex;
				m_CurrentState = BehaviorState::Running;
				return m_CurrentState;
			case BehaviorState::Running:
				return m_CurrentState;
		}
	}

	m_CurrentBehaviorIndex = 0;
	m_CurrentState = BehaviorState::Success;
	return m_CurrentState;*/

	for (m_CurrentBehaviorIndex; m_CurrentBehaviorIndex < m_ChildBehaviors.size(); ++m_CurrentBehaviorIndex)
	{
		m_CurrentState = m_ChildBehaviors[m_CurrentBehaviorIndex]->Execute(pBlackBoard);

		if (m_CurrentState != BehaviorState::Success) break;
	}

	if (m_CurrentState == BehaviorState::Failure) m_CurrentBehaviorIndex = 0;

	return m_CurrentState;
}

BehaviorConditional::BehaviorConditional(std::function<bool(Blackboard*)> function) :
	IBehavior{},
	m_fpConditional{ function }
{

}

BehaviorState BehaviorConditional::Execute(Blackboard* pBlackBoard)
{
	if (m_fpConditional == nullptr) m_CurrentState = BehaviorState::Failure;
	else if (m_fpConditional(pBlackBoard))
	{
		m_CurrentState = BehaviorState::Success;
	}
	else
	{
		m_CurrentState = BehaviorState::Failure;
	}

	return m_CurrentState;
}

BehaviorAction::BehaviorAction(std::function<BehaviorState(Blackboard*)> function) :
	IBehavior{},
	m_fpAction{ function }
{

}

BehaviorState BehaviorAction::Execute(Blackboard* pBlackBoard)
{
	if (m_fpAction == nullptr) m_CurrentState = BehaviorState::Failure;
	else m_CurrentState = m_fpAction(pBlackBoard);
	
	return m_CurrentState;
}

BehaviorTree::BehaviorTree(Blackboard* pBlackBoard, IBehavior* pRootBehavior) :
	IDecisionMaking{},
	m_CurrentState{ BehaviorState::Failure },
	m_pBlackBoard{ pBlackBoard },
	m_pRootBehavior{ pRootBehavior }
{

}

BehaviorTree::~BehaviorTree()
{
	SAFE_DELETE(m_pRootBehavior);
	SAFE_DELETE(m_pBlackBoard);
}

void BehaviorTree::Update(float deltaTime)
{
	if (m_pRootBehavior == nullptr) m_CurrentState = BehaviorState::Failure;
	else m_CurrentState = m_pRootBehavior->Execute(m_pBlackBoard);
}

Blackboard* BehaviorTree::GetBlackboard() const
{
	return m_pBlackBoard;
}