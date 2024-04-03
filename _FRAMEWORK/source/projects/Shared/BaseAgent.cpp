#include "stdafx.h"
#include "BaseAgent.h"

BaseAgent::BaseAgent(float radius) : m_Radius(radius)
{
	//Create Rigidbody
	const Elite::RigidBodyDefine define = Elite::RigidBodyDefine(0.01f, 0.1f, Elite::eDynamic, false);
	const Transform transform = Transform(Elite::ZeroVector2, {0,90});
	m_pRigidBody = new RigidBody(define, transform);

	//Add shape
	Elite::EPhysicsCircleShape shape;
	shape.radius = m_Radius;
	m_pRigidBody->AddShape(&shape);
}

BaseAgent::~BaseAgent()
{
	SAFE_DELETE(m_pRigidBody);
}

void BaseAgent::Update(float dt)
{

}

void BaseAgent::Render(float dt)
{
	// Cash information needed to debug
	const float orientation{ GetRotation() };
	const Elite::Vector2 position{ GetPosition() };
	const float angleOffset{ Elite::ToRadians(150.0f) };

	// Draw circle with bodycolor at position
	DEBUGRENDERER2D->DrawSolidCircle(position, m_Radius, Elite::Vector2{ 0.0f, 0.0f }, m_BodyColor);

	// Draw triangle which indicates orientation
	const std::vector<Elite::Vector2> points
	{
		Elite::Vector2{ cosf(orientation) * m_Radius + position.x, sinf(orientation) * m_Radius + position.y },
		Elite::Vector2{ cosf(orientation + angleOffset) * m_Radius + position.x, sinf(orientation + angleOffset) * m_Radius + position.y },
		Elite::Vector2{ cosf(orientation - angleOffset) * m_Radius + position.x, sinf(orientation - angleOffset) * m_Radius + position.y }
	};
	DEBUGRENDERER2D->DrawSolidPolygon(&points[0], 3, Elite::Color{ 0.0f, 0.0f, 0.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
}

void BaseAgent::TrimToWorld(float worldBounds, bool isWorldLooping) const 
{
	TrimToWorld({ 0, 0 }, { worldBounds, worldBounds }, isWorldLooping);
}

void BaseAgent::TrimToWorld(const Elite::Vector2& bottomLeft, const Elite::Vector2& topRight, bool isWorldLooping /* = true */) const
{
	auto pos = GetPosition();

	if (isWorldLooping)
	{
		if (pos.x > topRight.x)
			pos.x = bottomLeft.x;
		else if (pos.x < bottomLeft.x)
			pos.x = topRight.x;

		if (pos.y > topRight.y)
			pos.y = bottomLeft.y;
		else if (pos.y < bottomLeft.y)
			pos.y = topRight.y;
	}
	else
	{
		pos.x = Elite::Clamp(pos.x, bottomLeft.x, topRight.x);
		pos.y = Elite::Clamp(pos.y, bottomLeft.y, topRight.y);
	}

	SetPosition(pos);
}