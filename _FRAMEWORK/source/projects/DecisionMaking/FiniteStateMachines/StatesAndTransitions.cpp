#include "stdafx.h"
#include "StatesAndTransitions.h"
#include "projects/Shared/NavigationColliderElement.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"
#include "iostream"

using namespace Elite;

#pragma region States
void PatrolState::OnEnter(Blackboard* pBlackboard)
{
	SmartAgent* pGuard{ nullptr };
	pBlackboard->GetData("Guard", pGuard);

	PathFollow* pPathBehaviour{ nullptr };
	pBlackboard->GetData("GuardPathBehaviour", pPathBehaviour);

	std::vector<Vector2> path{};
	pBlackboard->GetData("GuardPath", path);

	pPathBehaviour->SetPath(path);

	pGuard->SetSteeringBehavior(pPathBehaviour);
}

void PatrolState::OnExit(Blackboard* pBlackboard)
{
	PathFollow* pPathBehaviour{ nullptr };
	pBlackboard->GetData("GuardPathBehaviour", pPathBehaviour);

	std::vector<Vector2> emptyPath{};

	pPathBehaviour->SetPath(emptyPath);
}

void PatrolState::Update(Blackboard* pBlackboard, float deltaTime)
{
	PathFollow* pPathBehaviour{ nullptr };
	pBlackboard->GetData("GuardPathBehaviour", pPathBehaviour);

	std::vector<Vector2> path{};
	pBlackboard->GetData("GuardPath", path);

	if (pPathBehaviour->HasArrived()) pPathBehaviour->SetPath(path);
}

void ChaseState::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pPlayer{ nullptr };
	pBlackboard->GetData("Player", pPlayer);
	
	TargetData target{ pPlayer->GetPosition(), pPlayer->GetRotation(), pPlayer->GetLinearVelocity(), pPlayer->GetAngularVelocity() };
	pBlackboard->ChangeData("TargetData", target);

	SmartAgent* pGuard{ nullptr };
	pBlackboard->GetData("Guard", pGuard);
	pGuard->SetSteeringBehavior(pGuard->GetSeekBehavior());
	pGuard->GetSeekBehavior()->SetTarget(target);
}

void ChaseState::OnExit(Blackboard* pBlackboard)
{
	
}

void ChaseState::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pPlayer{ nullptr };
	pBlackboard->GetData("Player", pPlayer);

	TargetData target{ pPlayer->GetPosition(), pPlayer->GetRotation(), pPlayer->GetLinearVelocity(), pPlayer->GetAngularVelocity() };
	pBlackboard->ChangeData("TargetData", target);

	SmartAgent* pGuard{ nullptr };
	pBlackboard->GetData("Guard", pGuard);
	pGuard->GetSeekBehavior()->SetTarget(target);
}

void SearchState::OnEnter(Blackboard* pBlackboard)
{
	
}

void SearchState::OnExit(Blackboard* pBlackboard)
{
	pBlackboard->ChangeData("SearchTime", 0.0f);
}

void SearchState::Update(Blackboard* pBlackboard, float deltaTime)
{
	float searchTime{};
	pBlackboard->GetData("SearchTime", searchTime);
	searchTime += deltaTime;
	pBlackboard->ChangeData("SearchTime", searchTime);

	SmartAgent* pGuard{ nullptr };
	pBlackboard->GetData("Guard", pGuard);

	TargetData target{};
	pBlackboard->GetData("TargetData", target);

	if (pGuard->GetPosition() == target.Position)
	{
		pGuard->SetSteeringBehavior(pGuard->GetWanderBehavior());
	}
}
#pragma endregion

#pragma region Conditions
bool IsTargetVisible::Evaluate(Elite::Blackboard* pBlackboard) const
{
	SteeringAgent* pPlayer{ nullptr };
	pBlackboard->GetData("Player", pPlayer);
	const Vector2 playerPosition{ pPlayer->GetPosition() };
	const float playerX{ playerPosition.x };
	const float playerY{ playerPosition.y };

	SmartAgent* pGuard{ nullptr };
	pBlackboard->GetData("Guard", pGuard);
	const Vector2 guardPosition{ pGuard->GetPosition() };
	const float guardX{ guardPosition.x };
	const float guardY{ guardPosition.y };

	float detectionRadius{ 0.0f };
	pBlackboard->GetData("DetectionRadius", detectionRadius);

	if (powf(playerX - guardX, 2.0f) + powf(playerY - guardY, 2.0f) < powf(detectionRadius, 2.0f))
	{
		if (pGuard->HasLineOfSight(playerPosition))
		{
			return true;
		}
	}

	return false;
}

bool IsTargetNotVisible::Evaluate(Elite::Blackboard* pBlackboard) const
{
	SteeringAgent* pPlayer{ nullptr };
	pBlackboard->GetData("Player", pPlayer);
	const Vector2 playerPosition{ pPlayer->GetPosition() };
	const float playerX{ playerPosition.x };
	const float playerY{ playerPosition.y };

	SmartAgent* pGuard{ nullptr };
	pBlackboard->GetData("Guard", pGuard);
	const Vector2 guardPosition{ pGuard->GetPosition() };
	const float guardX{ guardPosition.x };
	const float guardY{ guardPosition.y };

	float detectionRadius{ 0.0f };
	pBlackboard->GetData("DetectionRadius", detectionRadius);

	if (powf(playerX - guardX, 2.0f) + powf(playerY - guardY, 2.0f) < powf(detectionRadius, 2.0f))
	{
		if (pGuard->HasLineOfSight(playerPosition))
		{
			return false;
		}
	}

	return true;
}

bool IsSearchingTooLong::Evaluate(Elite::Blackboard* pBlackboard) const
{
	float searchTimer{0.0f };
	pBlackboard->GetData("SearchTime", searchTimer);

	float maxSearchTime{0.0f };
	pBlackboard->GetData("MaxSearchTime", maxSearchTime);

	if (searchTimer > maxSearchTime)
	{
		return true;
	}
	else return false;
}
#pragma endregion