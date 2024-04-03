//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
using namespace Elite;

//Includes
#include "App_InfluenceMap.h"
#include "framework/EliteAI/EliteGraphs/EliteInfluenceMap/EInfluenceMap.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "AntTypes/AntAgent.h"
#include "AntTypes/FoodSource.h"

App_InfluenceMap::App_InfluenceMap() :
	IApp(),
	MAX_AMOUNT_ANTS{ 100 },
	AMOUNT_FOOD_ITEMS{ 4 },
	FOOD_AMOUNT{ 100 },
	FOOD_DISTANCE{ 150.f },
	m_WorldSize{ 400.0f },
	m_pInfluenceMap_Food{ new InfluenceMap{ 50, 50, 8.0f } },
	m_pInfluenceMap_Home{ new InfluenceMap{ 50, 50, 8.0f } },
	m_RenderInfluenceMap_Food{ false },
	m_RenderInfluenceMap_Home{ false },
	m_pFoodVec{},
	m_foodRadius{ 20.f },
	m_homePosition{ m_WorldSize * 0.5f, m_WorldSize * 0.5f },
	m_homeRadius{ 20.f },
	m_Ants{},
	m_AntSpawnInterval{ 0.5f },
	m_AntWanderPct{ 0.1f },
	m_AntSampleDist{ 10.f },
	m_AntSampleAngle{ 45.f },
	m_InfluencePerSecond{ 10.f },
	m_RenderAntDebug{ true }
{
	m_pFoodVec.reserve(AMOUNT_FOOD_ITEMS);

	m_Ants.reserve(MAX_AMOUNT_ANTS);
	for (int i{}; i < MAX_AMOUNT_ANTS; ++i) m_Ants.emplace_back(new AntAgent{ m_pInfluenceMap_Food, m_pInfluenceMap_Home });
}

App_InfluenceMap::~App_InfluenceMap()
{
	SAFE_DELETE(m_pInfluenceMap_Food);
	SAFE_DELETE(m_pInfluenceMap_Home);
	for (FoodSource* pFood : m_pFoodVec) SAFE_DELETE(pFood);
	for (AntAgent* pAnt : m_Ants) SAFE_DELETE(pAnt);
}

void App_InfluenceMap::Start()
{
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(200.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Vector2{ 200.0f, 200.0f });

	m_pInfluenceMap_Food->SetDecay(0.1f);
	m_pInfluenceMap_Food->SetMomentum(0.99f);

	m_pInfluenceMap_Home->SetDecay(0.1f);
	m_pInfluenceMap_Home->SetMomentum(0.99f);

	for (int idx = 0; idx < AMOUNT_FOOD_ITEMS; ++idx)
	{
		const float angle{ randomFloat(float(M_PI) * 2) };
		const Vector2 position{ m_homePosition + Elite::OrientationToVector(angle) * FOOD_DISTANCE };
		m_pFoodVec.emplace_back(new FoodSource{ position, FOOD_AMOUNT });
	}

	for (AntAgent* pAnt : m_Ants) pAnt->SetPosition(m_homePosition);
}

void App_InfluenceMap::Update(float deltaTime)
{
	const bool testWithMouseInput{ false };

	if (testWithMouseInput)
	{
		if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eRight))
		{
			const MouseData mouseData{ INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eRight) };
			Vector2 mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Elite::Vector2((float)mouseData.X, (float)mouseData.Y)) };

			m_pInfluenceMap_Food->SetInfluenceAtPosition(mousePos, -100.0f, false);
		}
		if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
		{
			const MouseData mouseData{ INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft) };
			Vector2 mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Elite::Vector2((float)mouseData.X, (float)mouseData.Y)) };

			m_pInfluenceMap_Food->SetInfluenceAtPosition(mousePos, 100.0f, false);
		}
	}

	m_pInfluenceMap_Food->Update(deltaTime);
	m_pInfluenceMap_Home->Update(deltaTime);

	for (AntAgent* pAnt : m_Ants)
	{
		pAnt->Update(deltaTime);
		pAnt->TrimToWorld(m_WorldSize, false);
		CheckTakeFoodSources(pAnt);
		CheckDropFood(pAnt);
	}

	UpdateUI();
}

void App_InfluenceMap::UpdateUI()
{
	//Setup
	int menuWidth = 200;
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

	ImGui::Text("Home");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::SliderFloat("Home Radius", &m_homeRadius, 0.0f, 40.f, "%.2");

	ImGui::Text("Food");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::SliderFloat("Food Radius", &m_foodRadius, 0.0f, 40.f, "%.2");

	ImGui::Text("Influence Maps");
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Checkbox("Food map ", &m_RenderInfluenceMap_Food);
	ImGui::Checkbox("Home map ", &m_RenderInfluenceMap_Home);

	auto momentum = m_pInfluenceMap_Food->GetMomentum();
	auto decay = m_pInfluenceMap_Food->GetDecay();
	auto propagationInterval = m_pInfluenceMap_Food->GetPropagationInterval();

	ImGui::SliderFloat("Momentum", &momentum, 0.0f, 1.f, "%.2");
	ImGui::SliderFloat("Decay", &decay, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Propagation Interval", &propagationInterval, 0.f, 2.f, "%.2");
	ImGui::Spacing();

	//Set data
	m_pInfluenceMap_Food->SetMomentum(momentum);
	m_pInfluenceMap_Food->SetDecay(decay);
	m_pInfluenceMap_Food->SetPropagationInterval(propagationInterval);
	m_pInfluenceMap_Home->SetMomentum(momentum);
	m_pInfluenceMap_Home->SetDecay(decay);
	m_pInfluenceMap_Home->SetPropagationInterval(propagationInterval);

	//Ant Parameters
	ImGui::Text("Ants");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::SliderFloat("Influence per second", &m_InfluencePerSecond, 0.0f, 100.f, "%.2");
	ImGui::SliderFloat("Wander Precentage", &m_AntWanderPct, 0.0f, 1.f, "%.2");
	ImGui::SliderFloat("Sample Distance", &m_AntSampleDist, 1.f, 20.f, "%.2");
	ImGui::SliderFloat("Sample Angle", &m_AntSampleAngle, 0.f, 180.f, "%.2");
	ImGui::Checkbox("Render debug", &m_RenderAntDebug);

	m_Ants[0]->SetDebugRenderingEnabled(m_RenderAntDebug); //Only the first ant!!!

	for (AntAgent* pAnt : m_Ants)
	{
		pAnt->SetInfluencePerSecond(m_InfluencePerSecond);
		pAnt->SetWanderAmount(m_AntWanderPct);
		pAnt->SetSampleDistance(m_AntSampleDist);
		pAnt->SetSampleAngle(m_AntSampleAngle);
	}

	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
}

void App_InfluenceMap::Render(float deltaTime) const
{
	DEBUGRENDERER2D->DrawCircle(m_homePosition, m_homeRadius, Color{ 1.0f, 1.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());

	if (m_RenderInfluenceMap_Food) m_pInfluenceMap_Food->Render();
	else if (m_RenderInfluenceMap_Home) m_pInfluenceMap_Home->Render();

	for (FoodSource* pFood : m_pFoodVec)
	{
		DEBUGRENDERER2D->DrawCircle(pFood->GetPosition(), m_foodRadius, Color{ 0.0f, 1.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
		pFood->Render(deltaTime);
	}

	for (AntAgent* pAnt : m_Ants)
	{
		pAnt->Render(deltaTime);
	}
}

void App_InfluenceMap::CheckTakeFoodSources(AntAgent* pAgent)
{
	if (pAgent->GetHasFood()) return;

	for (FoodSource* pFood : m_pFoodVec)
	{
		if (pFood->GetAmount() <= 0) continue;

		if (pAgent->GetPosition().DistanceSquared(pFood->GetPosition()) <= powf(m_foodRadius, 2))
		{
			pFood->TakeFood();
			pAgent->SetHasFood(true);
		}
	}
}

void App_InfluenceMap::CheckDropFood(AntAgent* pAgent)
{
	if (!pAgent->GetHasFood()) return;

	if (pAgent->GetPosition().DistanceSquared(m_homePosition) <= powf(m_homeRadius, 2))
	{
		pAgent->SetHasFood(false);
	}
}


