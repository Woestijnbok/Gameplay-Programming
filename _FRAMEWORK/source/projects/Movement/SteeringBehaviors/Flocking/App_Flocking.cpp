#include "stdafx.h"
#include "App_Flocking.h"
#include "../SteeringAgent.h"
#include "Flock.h"

App_Flocking::App_Flocking() :
	IApp(),
	m_MouseTarget{},
	m_UseMouseTarget{ true },
	m_VisualizeMouseTarget{ true },
	m_TrimWorldSize{ 300.0f },
	m_FlockSize{ 200 },
	m_pAgentToEvade{ new SteeringAgent{} },
	m_pFlock{ new Flock(m_FlockSize, m_TrimWorldSize, m_pAgentToEvade, true) }
	
{

}

App_Flocking::~App_Flocking()
{	
	SAFE_DELETE(m_pFlock);
	SAFE_DELETE(m_pAgentToEvade);
}

void App_Flocking::Start()
{
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(55.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(m_TrimWorldSize / 1.5f, m_TrimWorldSize / 2));
}

void App_Flocking::Update(float deltaTime)
{
	if (INPUTMANAGER->IsMouseButtonUp(Elite::InputMouseButton::eLeft) && m_VisualizeMouseTarget)
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
	}

	m_pFlock->UpdateAndRenderUI();
	m_pFlock->Update(deltaTime);
	m_pAgentToEvade->Update(deltaTime);
	m_pAgentToEvade->TrimToWorld(m_TrimWorldSize);

	if (m_UseMouseTarget) m_pFlock->SetTarget_Seek(m_MouseTarget);
}

void App_Flocking::Render(float deltaTime) const
{
	RenderWorldBounds(m_TrimWorldSize);

	m_pFlock->Render(deltaTime);
	//Render Target
	if(m_VisualizeMouseTarget)
		DEBUGRENDERER2D->DrawSolidCircle(m_MouseTarget.Position, 0.3f, { 0.f,0.f }, { 1.f,0.f,0.f },-0.8f);
}
