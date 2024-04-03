#include "stdafx.h"
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::WeightedBehavior::WeightedBehavior(ISteeringBehavior* pBehavior, float weight) :
	pBehavior{ pBehavior },
	weight{ weight }
{

}

BlendedSteering::BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors) :
	ISteeringBehavior(),
	m_WeightedBehaviors{ weightedBehaviors }
{

}

void BlendedSteering::AddBehaviour(WeightedBehavior weightedBehavior)
{
	m_WeightedBehaviors.push_back(weightedBehavior);
}

std::vector<BlendedSteering::WeightedBehavior>& BlendedSteering::GetWeightedBehaviorsRef()
{
	return m_WeightedBehaviors;
}

SteeringOutput BlendedSteering::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput blendedSteering = {};
	float totalWeight{};

	for (auto& WeightedBehavior : m_WeightedBehaviors)
	{
		SteeringOutput steering = WeightedBehavior.pBehavior->CalculateSteering(deltaT, pAgent);

		blendedSteering.LinearVelocity += steering.LinearVelocity * WeightedBehavior.weight;
		blendedSteering.AngularVelocity += steering.AngularVelocity * WeightedBehavior.weight;

		totalWeight += WeightedBehavior.weight;
	}

	if (totalWeight > 0.0f)
	{
		blendedSteering.LinearVelocity /= totalWeight;
		blendedSteering.AngularVelocity /= totalWeight;
	}

	return blendedSteering;
}

PrioritySteering::PrioritySteering(std::vector<ISteeringBehavior*> priorityBehaviors) :
	ISteeringBehavior(),
	m_PriorityBehaviors{ priorityBehaviors }
{

}

void PrioritySteering::AddBehaviour(ISteeringBehavior* pBehavior)
{
	m_PriorityBehaviors.push_back(pBehavior);
}

SteeringOutput PrioritySteering::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	for (auto pBehavior : m_PriorityBehaviors)
	{
		steering = pBehavior->CalculateSteering(deltaT, pAgent);

		if (steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return steering;
}