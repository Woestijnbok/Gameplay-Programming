#include "stdafx.h"
#include "SandboxAgent.h"

using namespace Elite;

SandboxAgent::SandboxAgent(): BaseAgent()
{
	m_Target = GetPosition();
}

void SandboxAgent::Update(float dt)
{
	Vector2 position{ GetPosition() };
	Vector2 direction{ m_Target - position };
	const float speed{ 10.0f };
	SetLinearVelocity(direction.GetNormalized() * speed);

	DEBUGRENDERER2D->DrawSegment(position, m_Target, { 1.0f, 0.0f, 0.0f });
	//Orientation
	AutoOrient();
}

void SandboxAgent::Render(float dt)
{
	BaseAgent::Render(dt); //Default Agent Rendering
}

void SandboxAgent::AutoOrient()
{
	//Determine angle based on direction
	Vector2 velocity = GetLinearVelocity();
	if (velocity.MagnitudeSquared() > 0)
	{
		SetRotation(VectorToOrientation(velocity));
	}
}