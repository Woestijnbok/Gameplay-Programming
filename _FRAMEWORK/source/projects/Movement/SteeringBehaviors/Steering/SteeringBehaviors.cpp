#include "stdafx.h"
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"
#include <limits>

void ISteeringBehavior::SetTarget(const TargetData& target)
{
	m_Target = target;
}

SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	Elite::Vector2 direction{ (m_Target.Position - pAgent->GetPosition()).GetNormalized() };
	float m = direction.Magnitude();
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();

	return steering;
}

SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	// Calculates correct direction
	Elite::Vector2 currentPosition{ pAgent->GetPosition() };
	Elite::Vector2 targetPosition{ m_Target.Position };
	Elite::Vector2 direction{ currentPosition - targetPosition };		// Away from the target position

	// Changes it magnitude to get the desired speed
	direction.Normalize();
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();


	return steering;
}

Arrive::Arrive() :
	ISteeringBehavior(),
	m_SlowRadius{ 15.0f },
	m_TargetRadius{ 3.0f }
{

}

SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	// Calculates correct direction
	Elite::Vector2 currentPosition{ pAgent->GetPosition() };
	Elite::Vector2 targetPosition{ m_Target.Position };
	Elite::Vector2 direction{ targetPosition - currentPosition };
		
	// Sets up variables we need to check if we need to slow down
	float speed{ pAgent->GetMaxLinearSpeed() };
	float distance{ direction.Magnitude() };		// Since the direction vector is from the current position to the target position
	float a = distance - m_TargetRadius;
	float b = m_SlowRadius - m_TargetRadius;

	// If the target is close enough aka in the slow radius we slow the speed
	if (distance < m_SlowRadius)		
	{
		speed *= (a / b);
	}

	// Sets the speed aka linear velocity
	steering.LinearVelocity = direction.GetNormalized() * speed;
	// std::cout << "Current speed: " << speed << std::endl;		// to check if we slow down

	// Some debuging drawings

	return steering;
}

void Arrive::SetTargetRadius(float radius)
{
	m_TargetRadius = radius;
}

SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	const float distance{ (m_Target.Position - pAgent->GetPosition()).Normalize() };
	const float time{ distance / pAgent->GetMaxLinearSpeed() };
	const Elite::Vector2 targetPosition{ m_Target.Position + (time * m_Target.LinearVelocity) };
	Elite::Vector2 direction{ (targetPosition - pAgent->GetPosition()).GetNormalized() };
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();


	return steering;
}

Evade::Evade(float evadeRadius) :
	ISteeringBehavior(),
	m_EvadeRadius{ evadeRadius }
{

}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	Elite::Vector2 toTarget{ m_Target.Position - pAgent->GetPosition() };
	float distanceSquared{ toTarget.MagnitudeSquared()};
	if (distanceSquared > m_EvadeRadius * m_EvadeRadius)
	{
		SteeringOutput output{};
		output.IsValid = false;


		return output;
	}

	float distance = sqrtf(distanceSquared);
	float time = distance / pAgent->GetMaxLinearSpeed();
	Elite::Vector2 predictedPos = m_Target.Position + m_Target.LinearVelocity * time;

	// Calculates correct direction
	Elite::Vector2 currentPosition{ pAgent->GetPosition() };
	Elite::Vector2 targetPosition{ m_Target.Position + (deltaT * m_Target.LinearVelocity) };		// This is the predicted position of the target in deltaT seconds
	Elite::Vector2 direction{ currentPosition - predictedPos };

	// Changes it magnitude to get the desired speed
	direction.Normalize();
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();


	return steering;
}

Wander::Wander() :
	ISteeringBehavior(),
	m_CircleDistance{ 6.0f },
	m_Radius{ 4.0f },
	m_MaxAngleChange{ Elite::ToRadians(45) },
	m_WanderAngle{ 0.0f }
{

}

void Wander::SetWanderOffset(float offset)
{
	m_CircleDistance = offset;
}

void Wander::SetWanderRadius(float radius)
{
	m_Radius = radius;
}

void Wander::SetMaxAngleChange(float radians)
{
	m_MaxAngleChange = radians;
}

SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	// Calculating the circle center
	Elite::Vector2 m_CenterCircle{ pAgent->GetPosition() + (pAgent->GetDirection() * m_CircleDistance) };

	// Calculating new angle
	m_WanderAngle += Elite::randomFloat(-m_MaxAngleChange, m_MaxAngleChange);

	// Calculates correct direction
	Elite::Vector2 currentPosition{ pAgent->GetPosition() };
	Elite::Vector2 targetPosition{};
	targetPosition.x = m_CenterCircle.x + m_Radius * cosf(m_WanderAngle);
	targetPosition.y = m_CenterCircle.y + m_Radius * sinf(m_WanderAngle);
	Elite::Vector2 direction{ targetPosition - currentPosition };		// Away from the target position

	// Changes it magnitude to get the desired speed
	direction.Normalize();
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();

	return steering;
}