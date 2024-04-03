#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS

#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"
#include <chrono>

namespace BT_Actions
{
	Elite::BehaviorState ChangeToPatrol(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		pBlackboard->GetData("Guard", pGuard);

		PathFollow* pPathFollow{};
		pBlackboard->GetData("GuardPathBehaviour", pPathFollow);

		pGuard->SetSteeringBehavior(pPathFollow);

		if (pPathFollow->HasArrived())
		{
			std::vector<Elite::Vector2> path;
			pBlackboard->GetData("GuardPath", path);
			pPathFollow->SetPath(path);
		}

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToChase(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		pBlackboard->GetData("Guard", pGuard);
		SteeringAgent* pPlayer{};
		pBlackboard->GetData("Player", pPlayer);

		using namespace std::chrono;
		{
			const __int64 currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
			pBlackboard->ChangeData("SearchTime", currentTime);
		}

		PathFollow* pGuardPath{};
		pBlackboard->GetData("GuardPathBehaviour", pGuardPath);

		Elite::NavGraph* pNavGraph{};
		pBlackboard->GetData("NavGraph", pNavGraph);


		std::vector<Elite::Vector2> path{};
		path = Elite::NavMeshPathfinding::FindPath(pGuard->GetPosition(), pPlayer->GetPosition(), pNavGraph);
		pGuardPath->SetPath(path);
		//pBlackboard->ChangeData("PlayerPathBehaviour", pGuardPath);

		pGuard->GetSeekBehavior()->SetTarget(pPlayer->GetPosition());
		pGuard->SetSteeringBehavior(pGuard->GetSeekBehavior());

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToSearch(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		pBlackboard->GetData("Guard", pGuard);

		SteeringAgent* pPlayer{};
		pBlackboard->GetData("Player", pPlayer);

		PathFollow* pPlayerPath{};
		pBlackboard->GetData("PlayerPathBehaviour", pPlayerPath);

		if (pGuard->GetSteeringBehavior() != pGuard->GetWanderBehavior()) pGuard->SetSteeringBehavior(pPlayerPath);

		if (pPlayerPath->HasArrived()) pGuard->SetSteeringBehavior(pGuard->GetWanderBehavior());

		return Elite::BehaviorState::Success;
	}
}

namespace BT_Conditions
{
	bool HasSeenPlayer(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		pBlackboard->GetData("Agent", pGuard);

		SteeringAgent* pPlayer{};
		pBlackboard->GetData("Player", pPlayer);

		float detectionRadius{};
		pBlackboard->GetData("DetectionRadius", detectionRadius);

		bool isInLOS = pGuard->HasLineOfSight(pPlayer->GetPosition());
		bool isInRadius = DistanceSquared(pGuard->GetPosition(), pPlayer->GetPosition()) < Elite::Square(detectionRadius);

		return isInLOS && isInRadius;
	}

	bool ShouldSearch(Elite::Blackboard* pBlackboard)
	{
		using namespace std::chrono;

		float maxSearchTime{};
		__int64 startTime{};
		const __int64 currentTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

		pBlackboard->GetData("MaxSearchTime", maxSearchTime);
		pBlackboard->GetData("SearchTime", startTime);

		bool isSearchingTooLong = currentTime - startTime >= maxSearchTime;

		return !isSearchingTooLong;
	}
}

#endif