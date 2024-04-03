#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"

using namespace Elite;

#pragma region States
class PatrolState final : public FSMState
{
	public:

		PatrolState() = default;
		virtual ~PatrolState() = default;

		PatrolState(const PatrolState&) = delete;
		PatrolState(PatrolState&&) = delete;
		PatrolState& operator=(const PatrolState& other) = delete;
		PatrolState& operator=(PatrolState&& other) = delete;

		virtual void OnEnter(Blackboard* pBlackboard) override;
		virtual void OnExit(Blackboard* pBlackboard) override;
		virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class ChaseState final : public FSMState
{
public:

	ChaseState() = default;
	virtual ~ChaseState() = default;

	ChaseState(const ChaseState&) = delete;
	ChaseState(ChaseState&&) = delete;
	ChaseState& operator=(const ChaseState& other) = delete;
	ChaseState& operator=(ChaseState&& other) = delete;

	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void OnExit(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class SearchState final : public FSMState
{
public:

	SearchState() = default;
	virtual ~SearchState() = default;

	SearchState(const SearchState&) = delete;
	SearchState(SearchState&&) = delete;
	SearchState& operator=(const SearchState& other) = delete;
	SearchState& operator=(SearchState&& other) = delete;

	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void OnExit(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};
#pragma endregion

#pragma region Conditions
class IsTargetVisible final : public FSMCondition
{
public:

	IsTargetVisible() = default;
	virtual ~IsTargetVisible() = default;

	IsTargetVisible(const IsTargetVisible&) = delete;
	IsTargetVisible(IsTargetVisible&&) = delete;
	IsTargetVisible& operator=(const IsTargetVisible& other) = delete;
	IsTargetVisible& operator=(IsTargetVisible&& other) = delete;

	virtual bool Evaluate(Blackboard* pBlackboard) const override;
};

class IsTargetNotVisible final : public FSMCondition
{
public:

	IsTargetNotVisible() = default;
	virtual ~IsTargetNotVisible() = default;

	IsTargetNotVisible(const IsTargetNotVisible&) = delete;
	IsTargetNotVisible(IsTargetNotVisible&&) = delete;
	IsTargetNotVisible& operator=(const IsTargetNotVisible& other) = delete;
	IsTargetNotVisible& operator=(IsTargetNotVisible&& other) = delete;

	virtual bool Evaluate(Blackboard* pBlackboard) const override;
};

class IsSearchingTooLong final : public FSMCondition
{
public:

	IsSearchingTooLong() = default;
	virtual ~IsSearchingTooLong() = default;

	IsSearchingTooLong(const IsSearchingTooLong&) = delete;
	IsSearchingTooLong(IsSearchingTooLong&&) = delete;
	IsSearchingTooLong& operator=(const IsSearchingTooLong& other) = delete;
	IsSearchingTooLong& operator=(IsSearchingTooLong&& other) = delete;

	virtual bool Evaluate(Blackboard* pBlackboard) const override;
};
#pragma endregion
#endif