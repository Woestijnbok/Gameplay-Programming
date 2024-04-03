#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"


class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class CellSpace;

class Flock final
{
	public:
		Flock(int flockSize = 50, float worldSize = 100.f, SteeringAgent* pAgentToEvade = nullptr, bool trimWorld = false);
		~Flock();

		Flock(const Flock& other) = delete;
		Flock& operator=(const Flock& other) = delete;
		Flock(Flock&& other) = delete;
		Flock& operator=(Flock&& other) = delete;

		void Update(float deltaT);
		void UpdateAndRenderUI() ;
		void Render(float deltaT);
		void RegisterNeighbors(SteeringAgent* pAgent);
		int GetNrOfNeighbors() const;
		const std::vector<SteeringAgent*>& GetNeighbors() const;
		Elite::Vector2 GetAverageNeighborPos() const;
		Elite::Vector2 GetAverageNeighborVelocity() const;
		void SetTarget_Seek(const TargetData& target);
		void SetWorldTrimSize(float size);

	private:
		int m_FlockSize;
		std::vector<SteeringAgent*> m_Agents;
		std::vector<SteeringAgent*> m_Neighbors;
		bool m_TrimWorld;
		float m_WorldSize;
		float m_NeighborhoodRadius;
		int m_NrOfNeighbors;
		SteeringAgent* m_pAgentToEvade;
		BlendedSteering* m_pBlendedSteering;
		PrioritySteering* m_pPrioritySteering;
		Separation* m_pSeparationBehavior;
		Cohesion* m_pCohesionBehavior;
		Alignment* m_pAlignment;
		Seek* m_pSeekBehavior;
		Wander* m_pWanderBehavior;
		Evade* m_pEvadeBehavior;
		bool m_SpatialPartitioningIsOn;
		bool m_DebugRenderIsOn;
		CellSpace* m_pCellSpace;
		float m_NewPositionTimer;
		std::vector<Elite::Vector2> m_OldPositions;

		void RenderNeighborhood();
		float* GetWeight(ISteeringBehavior* pBehaviour);
};