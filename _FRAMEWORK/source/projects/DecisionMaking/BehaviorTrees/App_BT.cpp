#include <stdafx.h>
#include "App_BT.h"
#include "projects/Shared/NavigationColliderElement.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"
#include "Behaviors.h"

using namespace Elite;

bool App_BT::sShowPolygon{ false };
bool App_BT::sDrawPlayerPath{ false };
bool App_BT::sDrawPatrolPath{ true };
bool App_BT::sDrawDetectionRadius{ true };

App_BT::App_BT() :
	IApp{},
	m_pPlayer{ nullptr },
	m_pGuard{ nullptr },
	m_pPlayerPathFollow{ nullptr },
	m_pGuardPathFollow{ nullptr },
	m_Target{},
	m_AgentRadius{ 1.0f },
	m_PlayerSpeed{ 16.0f },
	m_GuardSpeed{ 8.0f },
	m_DetectionRadius{ 10.0f },
	m_SearchTime{},
	m_MaxSearchTime{ 5.0f },
	m_vNavigationColliders{},
	m_vPath{},
	m_PatrolPath{},
	m_pNavGraph{ nullptr }
{

}

App_BT::~App_BT()
{
	for (auto pNC : m_vNavigationColliders) SAFE_DELETE(pNC);
	SAFE_DELETE(m_pNavGraph);
	SAFE_DELETE(m_pPlayerPathFollow);
	SAFE_DELETE(m_pGuardPathFollow);
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pGuard);
}

void App_BT::Start()
{
	//----------- CAMERA ------------
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(36.782f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(12.9361f, 0.2661f));

	//----------- WORLD ------------
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(15.0f, 0.0f), 3.0f, 15.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-15.0f, 0.0f), 3.0f, 15.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.0f, 15.0f), 15.0f, 3.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.0f, -15.0f), 15.0f, 3.0f));

	//----------- NAVMESH  ------------
	m_pNavGraph = new Elite::NavGraph(PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider), 120.0f, 60.0f, m_AgentRadius);

	//----------- PLAYER AGENT ------------
	m_pPlayerPathFollow = new PathFollow();
	m_Target = TargetData(Elite::ZeroVector2);
	m_pPlayer = new SteeringAgent(m_AgentRadius);
	m_pPlayer->SetPosition(Elite::Vector2{ -20.0f, -20.0f });
	m_pPlayer->SetSteeringBehavior(m_pPlayerPathFollow);
	m_pPlayer->SetMaxLinearSpeed(m_PlayerSpeed);
	m_pPlayer->SetAutoOrient(true);
	m_pPlayer->SetMass(0.1f);

	//----------- PATROL PATH  ------------
	m_PatrolPath = std::vector<Elite::Vector2>
	{
		Elite::Vector2(20.f, 20.f),
		Elite::Vector2(-20.f, 20.f),
		Elite::Vector2(-20.f, -20.f),
		Elite::Vector2(20.f, -20.f)
	};

	//----------- GUARD (AI) AGENT ------------
	m_pGuardPathFollow = new PathFollow();
	m_pGuard = new SmartAgent(m_AgentRadius);
	m_pGuard->SetPosition(Elite::Vector2{ 20.0f, 0.0f });
	m_pGuard->SetSteeringBehavior(m_pGuardPathFollow);
	m_pGuard->SetMaxLinearSpeed(m_GuardSpeed);
	m_pGuard->SetAutoOrient(true);
	m_pGuard->SetMass(0.1f);

	//----------- AI SETUP ------------
	//1. Create Blackboard
	Blackboard* const pBlackboard{ CreateBlackboard(m_pGuard) };

	//2. Create BehaviorTree (make more conditions/actions and create a more advanced tree than the simple agents
	// Actions
	BehaviorTree* pBehaviorTree = new BehaviorTree(pBlackboard,
		new BehaviorSelector(
			{
				//Chase
				new BehaviorSequence(
				{
					new BehaviorConditional(BT_Conditions::HasSeenPlayer),
					new BehaviorAction(BT_Actions::ChangeToChase)
				}),
				//Search
				new BehaviorSequence(
				{
					new BehaviorConditional(BT_Conditions::ShouldSearch),
					new BehaviorAction(BT_Actions::ChangeToSearch)
				}),
				//Patrol
				new BehaviorAction(BT_Actions::ChangeToPatrol),
				})
		);
	
	//3. Set the BehaviorTree active on the agent 
	m_pGuard->SetDecisionMaking(pBehaviorTree);
}

void App_BT::Update(float deltaTime)
{
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		const MouseData mouseData{ INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft) };
		const Elite::Vector2 mouseTarget{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Elite::Vector2{ (float)mouseData.X, (float)mouseData.Y }) };
		m_vPath = NavMeshPathfinding::FindPath(m_pPlayer->GetPosition(), mouseTarget, m_pNavGraph);

		if (m_vPath.size() > 0) m_pPlayerPathFollow->SetPath(m_vPath);
	}

	UpdateImGui();
	m_pPlayer->Update(deltaTime);
	m_pGuard->Update(deltaTime);
}

void App_BT::Render(float deltaTime) const
{
	if (sShowPolygon)
	{
		DEBUGRENDERER2D->DrawPolygon(m_pNavGraph->GetNavMeshPolygon(), Color{ 0.1f, 0.1f, 0.1f });
		DEBUGRENDERER2D->DrawSolidPolygon(m_pNavGraph->GetNavMeshPolygon(), Color{ 0.0f, 0.5f, 0.1f, 0.05f }, DEBUGRENDERER2D->NextDepthSlice());
	}

	if (sDrawPlayerPath && m_vPath.size() > 0)
	{
		for (const Vector2& pathPoint : m_vPath) DEBUGRENDERER2D->DrawCircle(pathPoint, 2.0f, Color{ 1.0f, 0.0f, 0.0f }, -0.2f);

		for (size_t i = 0; i < m_vPath.size() - 1; i++)
		{
			const float g{ float(i) / m_vPath.size() };
			DEBUGRENDERER2D->DrawSegment(m_vPath[i], m_vPath[i + 1], Color{ 1.0f, g, g }, -0.2f);
		}
	}

	if (sDrawPatrolPath && !m_PatrolPath.empty())
	{
		for (const Vector2& pathPoint : m_PatrolPath) DEBUGRENDERER2D->DrawCircle(pathPoint, 2.0f, Color{ 0.0f, 0.0f, 1.0f }, -0.2f);

		for (size_t i = 0; i < m_PatrolPath.size(); i++) DEBUGRENDERER2D->DrawSegment(m_PatrolPath[i], m_PatrolPath[(i + 1) % m_PatrolPath.size()], Color{ 0.0f, 0.0f, 1.0f }, -0.2f);
	}

	if (sDrawDetectionRadius) DEBUGRENDERER2D->DrawCircle(m_pGuard->GetPosition(), m_DetectionRadius, Color{ 0.0f, 0.0f, 1.0f }, -0.2f);

	m_pPlayer->Render(deltaTime);
	m_pGuard->Render(deltaTime);
}

Elite::Blackboard* App_BT::CreateBlackboard(SmartAgent* smartAgent)
{
	Blackboard* pBlackboard = new Blackboard();

	pBlackboard->AddData("Agent", smartAgent);
	pBlackboard->AddData("Player", m_pPlayer);
	pBlackboard->AddData("Guard", m_pGuard);
	pBlackboard->AddData("PlayerPathBehaviour", m_pPlayerPathFollow);
	pBlackboard->AddData("GuardPathBehaviour", m_pGuardPathFollow);
	pBlackboard->AddData("TargetData", m_Target);
	pBlackboard->AddData("AgentRadius", m_AgentRadius);
	pBlackboard->AddData("PlayerSpeed", m_PlayerSpeed);
	pBlackboard->AddData("GuardSpeed", m_GuardSpeed);
	pBlackboard->AddData("SearchTime", m_SearchTime);
	pBlackboard->AddData("MaxSearchTime", m_MaxSearchTime);
	pBlackboard->AddData("DetectionRadius", m_DetectionRadius);
	pBlackboard->AddData("GuardPath", m_PatrolPath);
	pBlackboard->AddData("NavGraph", m_pNavGraph);

	return pBlackboard;
}

void App_BT::UpdateImGui()
{
	#ifdef PLATFORM_WINDOWS
	//Setup
	int menuWidth = 150;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);
	ImGui::SetWindowFocus();
	ImGui::PushItemWidth(70);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Checkbox("Show Polygon", &sShowPolygon);
	ImGui::Checkbox("Show Player Path", &sDrawPlayerPath);
	ImGui::Checkbox("Show Patrol Path", &sDrawPatrolPath);
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::SliderFloat("PlayerSpeed", &m_PlayerSpeed, 0.0f, 22.0f))
	{
		m_pPlayer->SetMaxLinearSpeed(m_PlayerSpeed);
	}

	if (ImGui::SliderFloat("GuardSpeed", &m_GuardSpeed, 0.0f, 22.0f))
	{
		m_pGuard->SetMaxLinearSpeed(m_GuardSpeed);
	}

	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	#endif
}