#pragma once
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/SteeringHelpers.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EPathSmoothing.h"

class NavigationColliderElement;
class SteeringAgent;
class PathFollow;

namespace Elite
{
	class NavGraph;
}

class App_BT final : public IApp
{
	public:
		App_BT();
		virtual ~App_BT();

		App_BT(const App_BT&) = delete;
		App_BT& operator=(const App_BT&) = delete;
		App_BT(App_BT&&) = delete;
		App_BT& operator=(App_BT&&) = delete;

		virtual void Start() override;
		virtual void Update(float deltaTime) override;
		virtual void Render(float deltaTime) const override;

	private:
		// --Agents--
		SteeringAgent* m_pPlayer;
		SmartAgent* m_pGuard;
		PathFollow* m_pPlayerPathFollow;
		PathFollow* m_pGuardPathFollow;
		TargetData m_Target;
		float m_AgentRadius;
		float m_PlayerSpeed;
		float m_GuardSpeed;
		float m_DetectionRadius;
		__int64 m_SearchTime;
		float m_MaxSearchTime;

		// --Level--
		std::vector<NavigationColliderElement*> m_vNavigationColliders;

		// --Pathfinder--
		std::vector<Elite::Vector2> m_vPath;
		std::vector<Elite::Vector2> m_PatrolPath;

		// --Graph--
		Elite::NavGraph* m_pNavGraph;

		// --Debug drawing information--
		static bool sShowPolygon;
		static bool sDrawPlayerPath;
		static bool sDrawPatrolPath;
		static bool sDrawDetectionRadius;

		void UpdateImGui();
		Elite::Blackboard* CreateBlackboard(SmartAgent* smartAgent);
};
