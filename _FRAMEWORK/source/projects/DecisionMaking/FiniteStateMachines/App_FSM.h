#pragma once
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/SteeringHelpers.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EPathSmoothing.h"
#include "projects/DecisionMaking/FiniteStateMachines/StatesAndTransitions.h"

class NavigationColliderElement;
class SteeringAgent;
class PathFollow;

namespace Elite
{
	class NavGraph;
}


class App_FSM final : public IApp
{
public:
	App_FSM();
	virtual ~App_FSM();

	App_FSM(const App_FSM&) = delete;
	App_FSM& operator=(const App_FSM&) = delete;
	App_FSM(App_FSM&&) = delete;
	App_FSM& operator=(App_FSM&&) = delete;

	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(float deltaTime) const override;

private:
	// --AI--
	FSMState* m_pPatrolState;
	FSMState* m_pChaseState;
	FSMState* m_pSearchState;
	FSMCondition* m_pIsTargetVisibleCondition;
	FSMCondition* m_pIsTargetNotVisibleCondition;
	FSMCondition* m_pIsSearchingTooLongCondition;
	FiniteStateMachine* m_pFiniteStateMachine;

	// --Agents--
	SteeringAgent* m_pPlayer = nullptr;
	SmartAgent* m_pGuard = nullptr;
	PathFollow* m_pPlayerPathFollow = nullptr;
	PathFollow* m_pGuardPathFollow = nullptr;
	TargetData m_Target = {};
	float m_AgentRadius = 1.0f;
	float m_PlayerSpeed = 16.0f;
	float m_GuardSpeed = 8.0f;
	float m_DetectionRadius = 10.0f;
	float m_SearchTime = 0.0f;
	float m_MaxSearchTime = 5.0f;

	// --Level--
	std::vector<NavigationColliderElement*> m_vNavigationColliders = {};

	// --Pathfinder--
	std::vector<Elite::Vector2> m_vPath;
	std::vector<Elite::Vector2> m_PatrolPath;

	// --Graph--
	Elite::NavGraph* m_pNavGraph = nullptr;

	// --Debug drawing information--
	static bool sShowPolygon;
	static bool sDrawPlayerPath;
	static bool sDrawPatrolPath;
	static bool sDrawDetectionRadius;

	void UpdateImGui();
	Elite::Blackboard* CreateBlackboard(SmartAgent* smartAgent);
};
