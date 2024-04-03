//=== General Includes ===
#include "stdafx.h"

using namespace Elite;

FiniteStateMachine::FiniteStateMachine(FSMState* startState, Blackboard* pBlackboard)
    : m_pCurrentState(nullptr),
    m_pBlackboard(pBlackboard)
{
    ChangeState(startState);
}

FiniteStateMachine::~FiniteStateMachine()
{
    SAFE_DELETE(m_pBlackboard);
}

void FiniteStateMachine::AddTransition(FSMState* startState, FSMState* toState, FSMCondition* condition)
{
    auto it = m_Transitions.find(startState);
    if (it == m_Transitions.end())
    {
        m_Transitions[startState] = Transitions();
    }
   
    m_Transitions[startState].push_back(std::make_pair(condition, toState));
}

void FiniteStateMachine::Update(float deltaTime)
{ 
    auto transitionItr = m_Transitions.find(m_pCurrentState);

    if (transitionItr != m_Transitions.end())
    {
        for (TransitionStatePair& transitionStatePair : transitionItr->second)
        {
            if (transitionStatePair.first->Evaluate(m_pBlackboard))
            {
                ChangeState(transitionStatePair.second);
            }
        }
    }

    if (m_pCurrentState != nullptr) m_pCurrentState->Update(m_pBlackboard, deltaTime);
}

Blackboard* FiniteStateMachine::GetBlackboard() const
{
    return m_pBlackboard;
}

void FiniteStateMachine::ChangeState(FSMState* newState)
{
    if (m_pCurrentState != nullptr)  m_pCurrentState->OnExit(m_pBlackboard);

    m_pCurrentState = newState;

    if (m_pCurrentState != nullptr) m_pCurrentState->OnEnter(m_pBlackboard);
}
