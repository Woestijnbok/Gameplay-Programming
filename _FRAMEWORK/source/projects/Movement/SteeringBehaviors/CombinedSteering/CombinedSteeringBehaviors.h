#pragma once
#include "../Steering/SteeringBehaviors.h"

class Flock;

class BlendedSteering final: public ISteeringBehavior
{
	public:
		struct WeightedBehavior final
		{
			ISteeringBehavior* pBehavior;
			float weight;

			WeightedBehavior(ISteeringBehavior* pBehavior, float weight);
			~WeightedBehavior() = default;

			WeightedBehavior(const WeightedBehavior& other) = default;
			WeightedBehavior& operator=(const WeightedBehavior& other) = default;
			WeightedBehavior(WeightedBehavior&& other) = default;
			WeightedBehavior& operator=(WeightedBehavior&& other) = default;
		};

		BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors);
		virtual ~BlendedSteering() = default;

		BlendedSteering(const BlendedSteering& other) = delete;
		BlendedSteering& operator=(const BlendedSteering& other) = delete;
		BlendedSteering(BlendedSteering&& other) = delete;
		BlendedSteering& operator=(BlendedSteering&& other) = delete;

		void AddBehaviour(WeightedBehavior weightedBehavior);
		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
		std::vector<WeightedBehavior>& GetWeightedBehaviorsRef();

	protected:


	private:
		std::vector<WeightedBehavior> m_WeightedBehaviors;
		using ISteeringBehavior::SetTarget; // made private because targets need to be set on the individual behaviors, not the combined behavior
};

class PrioritySteering final: public ISteeringBehavior
{
	public:
		PrioritySteering(std::vector<ISteeringBehavior*> priorityBehaviors);
		virtual ~PrioritySteering() = default;

		PrioritySteering(const PrioritySteering& other) = delete;
		PrioritySteering& operator=(const PrioritySteering& other) = delete;
		PrioritySteering(PrioritySteering&& other) = delete;
		PrioritySteering& operator=(PrioritySteering&& other) = delete;

		void AddBehaviour(ISteeringBehavior* pBehavior);
		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	protected:


	private:
		std::vector<ISteeringBehavior*> m_PriorityBehaviors;
		using ISteeringBehavior::SetTarget; // made private because targets need to be set on the individual behaviors, not the combined behavior
};