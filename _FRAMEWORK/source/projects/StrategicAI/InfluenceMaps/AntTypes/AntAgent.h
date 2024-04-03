#pragma once
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"

class Wander;
class Seek;
class BlendedSteering;

namespace Elite
{
	class InfluenceMap;
}

class AntAgent final : public SteeringAgent
{
public:
	AntAgent(Elite::InfluenceMap* pInfluenceMapFood, Elite::InfluenceMap* pInfluenceMapHome, float radius = 1.0f);
	virtual ~AntAgent();

	AntAgent(const AntAgent&) = delete;
	AntAgent& operator=(const AntAgent&) = delete;
	AntAgent(AntAgent&&) = delete;
	AntAgent& operator=(AntAgent&&) = delete;

	float GetSampleDistance() const;
	void SetSampleDistance(float sampleDistance);
	float GetSampleAngle()const;
	void SetSampleAngle(float sampleAngle);
	float GetInfluencePerSecond() const;
	void SetInfluencePerSecond(float influence);
	void SetWanderAmount(float wanderProcentage);
	bool GetHasFood() const;
	void SetHasFood(bool hasFood);
	void Update(float deltaTime);

private:
	Elite::InfluenceMap* m_pInfluenceMap_Home;
	Elite::InfluenceMap* m_pInfluenceMap_Food;
	Wander* m_pWander;
	Seek* m_pSeek;
	BlendedSteering* m_pBlendedSteering;
	bool m_HasFood;
	float m_influencePerSecond;
	float m_sampleDistance;
	float m_sampleAngle;
};