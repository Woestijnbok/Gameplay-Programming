#include "stdafx.h"
#include "AntAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include <algorithm>
#include "projects/Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include "framework/EliteAI/EliteGraphs/EliteInfluenceMap/EInfluenceMap.h"

using namespace Elite;

AntAgent::AntAgent(InfluenceMap* pInfluenceMapFood, InfluenceMap* pInfluenceMapHome, float radius) :
	SteeringAgent(radius),
	m_pInfluenceMap_Home{ pInfluenceMapHome },
	m_pInfluenceMap_Food{ pInfluenceMapFood },
	m_pWander{ new Wander() },
	m_pSeek{ new Seek() },
	m_pBlendedSteering{ 
		new BlendedSteering
		{
			std::vector<BlendedSteering::WeightedBehavior>
			{ 
				BlendedSteering::WeightedBehavior{ m_pWander , 0.0f}, 
				BlendedSteering::WeightedBehavior{ m_pSeek, 1.0f } 
			} 
		} 
	},
	m_HasFood{ false },
	m_influencePerSecond{ 10.0f },
	m_sampleDistance{ 10.0f },
	m_sampleAngle{ ToRadians(45.0f) }
{
	SetSteeringBehavior(m_pBlendedSteering);
	SetAutoOrient(true);
	SetMaxAngularSpeed(180.0f);
	SetMaxLinearSpeed(20.0f);
	SetMass(0.1f);
	SetBodyColor(Color{ 1.0f, 0.5f, 0.0f });
	SetRotation(randomFloat(float(M_PI) * 2.0f));

	//Don't let agents collide with each other
	BaseAgent::m_pRigidBody->RemoveAllShapes();
}

AntAgent::~AntAgent()
{
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pBlendedSteering);
}

float AntAgent::GetSampleDistance() const 
{
	return m_sampleDistance; 
}

void AntAgent::SetSampleDistance(float sampleDistance) 
{
	m_sampleDistance = sampleDistance; 
}

float AntAgent::GetSampleAngle()const 
{
	return m_sampleAngle; 
}

void AntAgent::SetSampleAngle(float sampleAngle) 
{
	m_sampleAngle = sampleAngle; 
}

float AntAgent::GetInfluencePerSecond() const 
{
	return m_influencePerSecond; 
}

void AntAgent::SetInfluencePerSecond(float influence) 
{
	m_influencePerSecond = influence; 
}

void AntAgent::SetWanderAmount(float wanderProcentage)
{
	wanderProcentage = Clamp(wanderProcentage, 0.0f, 1.0f);
	std::vector<BlendedSteering::WeightedBehavior>& weightsRef{ m_pBlendedSteering->GetWeightedBehaviorsRef() };
	weightsRef[0].weight = wanderProcentage;
	weightsRef[1].weight = 1.0f - wanderProcentage;
}

bool AntAgent::GetHasFood() const 
{ 
	return m_HasFood;
}

void AntAgent::SetHasFood(bool hasFood)
{
	if (m_HasFood != hasFood)
	{
		m_HasFood = hasFood;

		SetBodyColor(m_HasFood ? Color{ 0.5f, 1.0f, 0.0f } : Color{ 1.0f, 0.5f, 0.0f });
	}
}

void AntAgent::Update(float deltaTime)
{
	// Determine which influence map to write to
	InfluenceMap* pWriteMap{ m_pInfluenceMap_Home };
	if (m_HasFood) pWriteMap = m_pInfluenceMap_Food;

	// Set influence at current position
	const Vector2 currentPosition{ GetPosition() };
	pWriteMap->SetInfluenceAtPosition(currentPosition, m_influencePerSecond * deltaTime, true);

	// Determine which influence map to read from	
	InfluenceMap* pReadMap{ m_pInfluenceMap_Food };
	if (m_HasFood) pReadMap = m_pInfluenceMap_Home;

	// Create 3 sample positions: one straight forward, two on either side
	const float rightAngle{ GetRotation() + m_sampleAngle };
	const float straightAngle{ GetRotation() };
	const float leftAngle{ GetRotation() - m_sampleAngle };
	const Vector2 rightPosition{ currentPosition + (OrientationToVector(rightAngle) * m_sampleDistance) };
	const Vector2 straightPosition{ currentPosition + (OrientationToVector(straightAngle) * m_sampleDistance) };
	const Vector2 leftPosition{ currentPosition + (OrientationToVector(leftAngle) * m_sampleDistance) };

	// sample influence on all 3 positions
	const float rightInfluence{ pReadMap->GetInfluenceAtPosition(rightPosition) };
	const float straightInfluence{ pReadMap->GetInfluenceAtPosition(straightPosition) };
	const float leftInfluence{ pReadMap->GetInfluenceAtPosition(leftPosition) };

	// Sort the sampled influences from low to high
	std::vector<float>samples{ rightInfluence, straightInfluence, leftInfluence };
	std::sort(samples.begin(), samples.end());
	
	// Set the highest sample as our desired position
	Vector2 bestSamplePos{};

	if ((samples[0] == samples[1]) && (samples[0] == samples[2]))
	{
		bestSamplePos = straightPosition;
	}
	else if (samples[2] == rightInfluence)
	{
		bestSamplePos = rightPosition;
	}
	else if (samples[2] == straightInfluence)
	{
		bestSamplePos = straightPosition;
	}
	else if(samples[2] == leftInfluence)
	{
		bestSamplePos = leftPosition;
	}

	// Seek towards our desired position
	m_pSeek->SetTarget(bestSamplePos);
	SteeringAgent::Update(deltaTime);

	if (GetDebugRenderingEnabled())
	{
		// Render Sample Position
		DEBUGRENDERER2D->DrawCircle(rightPosition, 1.0f, Color{ 0.67f, 0.84f, 0.9f }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawCircle(straightPosition, 1.0f, Color{ 0.67f, 0.84f, 0.9f }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawCircle(leftPosition, 1.0f, Color{ 0.67f, 0.84f, 0.9f }, DEBUGRENDERER2D->NextDepthSlice());

		// Render Best sample position
		DEBUGRENDERER2D->DrawCircle(bestSamplePos, 1.0f, Color{ 1.0f, 0.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
	}
}