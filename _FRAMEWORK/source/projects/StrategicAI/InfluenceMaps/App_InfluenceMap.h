#pragma once
#include "framework/EliteInterfaces/EIApp.h"

class AntAgent;
class FoodSource;

namespace Elite
{
	class InfluenceMap;	
}

class App_InfluenceMap final : public IApp
{
public:
	App_InfluenceMap();
	virtual ~App_InfluenceMap();

	App_InfluenceMap(const App_InfluenceMap&) = delete;
	App_InfluenceMap& operator=(const App_InfluenceMap&) = delete;
	App_InfluenceMap(App_InfluenceMap&&) = delete;
	App_InfluenceMap& operator=(App_InfluenceMap&&) = delete;

	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void UpdateUI();
	virtual void Render(float deltaTime) const override;

private:
	const int MAX_AMOUNT_ANTS;
	const int AMOUNT_FOOD_ITEMS;
	const int FOOD_AMOUNT;
	const float FOOD_DISTANCE;
	float m_WorldSize;
	Elite::InfluenceMap* m_pInfluenceMap_Food;
	Elite::InfluenceMap* m_pInfluenceMap_Home;
	bool m_RenderInfluenceMap_Food;
	bool m_RenderInfluenceMap_Home;
	std::vector<FoodSource*> m_pFoodVec;
	float m_foodRadius;
	Elite::Vector2 m_homePosition;
	float m_homeRadius;
	std::vector<AntAgent*> m_Ants;
	float m_AntSpawnInterval;
	float m_AntWanderPct;
	float m_AntSampleDist;
	float m_AntSampleAngle;
	float m_InfluencePerSecond;
	bool m_RenderAntDebug;

	void CheckTakeFoodSources(AntAgent* pAgent);
	void CheckDropFood(AntAgent* pAgent);
};
