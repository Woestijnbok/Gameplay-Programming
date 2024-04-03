#pragma once
#include "../SteeringHelpers.h"

class SteeringAgent;
class Obstacle;

class ISteeringBehavior
{
	public:
		ISteeringBehavior() = default;
		virtual ~ISteeringBehavior() = default;

		ISteeringBehavior(const ISteeringBehavior& other) = delete;
		ISteeringBehavior& operator=(const ISteeringBehavior& other) = delete;
		ISteeringBehavior(ISteeringBehavior&& other) = delete;
		ISteeringBehavior& operator=(ISteeringBehavior&& other) = delete;

		virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;
		void SetTarget(const TargetData& target);

		template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
		T* As()
		{
			return static_cast<T*>(this);
		}

	protected:
		TargetData m_Target;

	private:

};

class Seek : public ISteeringBehavior
{
	public:
		Seek() = default;
		virtual ~Seek() = default;

		Seek(const Seek& other) = delete;
		Seek& operator=(const Seek& other) = delete;
		Seek(Seek&& other) = delete;
		Seek& operator=(Seek&& other) = delete;

		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	protected:


	private:

};

class Flee : public ISteeringBehavior
{
	public:
		Flee() = default;
		virtual ~Flee() = default;

		Flee(const Flee& other) = delete;
		Flee& operator=(const Flee& other) = delete;
		Flee(Flee&& other) = delete;
		Flee& operator=(Flee&& other) = delete;

		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	protected:


	private:

};

class Arrive : public ISteeringBehavior
{
	public:
		Arrive();
		virtual ~Arrive() = default;

		Arrive(const Arrive& other) = delete;
		Arrive& operator=(const Arrive& other) = delete;
		Arrive(Arrive&& other) = delete;
		Arrive& operator=(Arrive&& other) = delete;

		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
		void SetTargetRadius(float radius);
		void SetSlowRadius(float radius) { m_SlowRadius = radius; }

	protected:


	private:
		float m_SlowRadius;
		float m_TargetRadius;
};

class Pursuit : public ISteeringBehavior
{
	public:
		Pursuit() = default;
		virtual ~Pursuit() = default;

		Pursuit(const Pursuit& other) = delete;
		Pursuit& operator=(const Pursuit& other) = delete;
		Pursuit(Pursuit&& other) = delete;
		Pursuit& operator=(Pursuit&& other) = delete;

		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	protected:


	private:
	
};

class Evade : public ISteeringBehavior
{
	public:
		Evade(float evadeRadius = 15.0f);
		virtual ~Evade() = default;

		Evade(const Evade& other) = delete;
		Evade& operator=(const Evade& other) = delete;
		Evade(Evade&& other) = delete;
		Evade& operator=(Evade&& other) = delete;

		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	protected:


	private:
		float m_EvadeRadius;
};

class Wander : public ISteeringBehavior
{
	public:
		Wander();
		virtual ~Wander() = default;

		Wander(const Wander& other) = delete;
		Wander& operator=(const Wander& other) = delete;
		Wander(Wander&& other) = delete;
		Wander& operator=(Wander&& other) = delete;

		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
		void SetWanderOffset(float offset);
		void SetWanderRadius(float radius);
		void SetMaxAngleChange(float radians);

	protected:


	private:
		float m_CircleDistance;
		float m_Radius;
		float m_MaxAngleChange;
		float m_WanderAngle;
};