#pragma once
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

class SteeringAgent;
class BlendedSteering;
class PrioritySteering;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_CombinedSteering final : public IApp
{
public:
	//Constructor & Destructor
	App_CombinedSteering() = default;
	virtual ~App_CombinedSteering() final;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	TargetData m_MouseTarget = {};
	bool m_UseMouseTarget = false;
	bool m_VisualizeMouseTarget = true;
	
	bool m_CanDebugRender = false;
	bool m_TrimWorld = true;
	float m_TrimWorldSize = 50.f;

	//Agent 1: Drunk seek
	SteeringAgent* m_pAgent1{ nullptr };
	BlendedSteering* m_pDrunkSeekBehaviour{ nullptr };
	Wander* m_pDrunkWander{ nullptr };
	Seek* m_pDrunkSeek{ nullptr };

	//Agent2: Wander & Evade
	SteeringAgent* m_pAgent2{};
	PrioritySteering* m_pAgent2Steering{};
	Wander* m_pAgent2Wander{};
	Evade* m_pAgent2Evade{};
};