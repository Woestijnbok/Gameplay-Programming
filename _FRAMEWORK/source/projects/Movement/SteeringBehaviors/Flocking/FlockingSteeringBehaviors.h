#pragma once
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

class Flock;

class Cohesion : public Seek
{
	public:
		Cohesion(Flock* pFlock);
		~Cohesion() = default;

		Cohesion(const Cohesion& other) = delete;
		Cohesion& operator=(const Cohesion& other) = delete;
		Cohesion(Cohesion&& other) = delete;
		Cohesion& operator=(Cohesion&& other) = delete;

		SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	private:
		Flock* m_pFlock;
};

class Separation : public ISteeringBehavior
{
public:
	Separation(Flock* pFlock);
	~Separation() = default;

	Separation(const Separation& other) = delete;
	Separation& operator=(const Separation& other) = delete;
	Separation(Separation&& other) = delete;
	Separation& operator=(Separation&& other) = delete;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Flock* m_pFlock;
};

class Alignment : public ISteeringBehavior
{
public:
	Alignment(Flock* pFlock);
	~Alignment() = default;

	Alignment(const Alignment& other) = delete;
	Alignment& operator=(const Alignment& other) = delete;
	Alignment(Alignment&& other) = delete;
	Alignment& operator=(Alignment&& other) = delete;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Flock* m_pFlock;
};