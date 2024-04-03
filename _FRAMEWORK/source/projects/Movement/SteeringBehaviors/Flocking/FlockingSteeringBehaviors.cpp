#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"

#include <cmath>

Cohesion::Cohesion(Flock* pFlock) :
	Seek(),
	m_pFlock{pFlock}
{
	
}

SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_Target.Position = m_pFlock->GetAverageNeighborPos();

	return Seek::CalculateSteering(deltaT, pAgent);
}

Separation::Separation(Flock* pFlock) :
	ISteeringBehavior(),
	m_pFlock{ pFlock }
{

}

SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	Elite::Vector2 velocity{};

	for (int i{}; i < m_pFlock->GetNrOfNeighbors(); i++)
	{
		Elite::Vector2 direction{ pAgent->GetPosition() - m_pFlock->GetNeighbors()[i]->GetPosition()};	// Away from the neighbour

		if (direction.MagnitudeSquared() > 0.001)
		{
			velocity += (direction / direction.MagnitudeSquared() );
		}
	}
	
	return SteeringOutput{ velocity.GetNormalized() * pAgent->GetMaxLinearSpeed() };
}

Alignment::Alignment(Flock* pFlock) :
	ISteeringBehavior(),
	m_pFlock{ pFlock }
{

}

SteeringOutput Alignment::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	return SteeringOutput{ m_pFlock->GetAverageNeighborVelocity().GetNormalized() * pAgent->GetMaxLinearSpeed() };
}