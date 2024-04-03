#pragma once
#include "framework/EliteInterfaces/EIApp.h"
#include "../Steering/SteeringBehaviors.h"

class SteeringAgent;
class Flock;

class App_Flocking final : public IApp
{
	public:
		App_Flocking();
		virtual ~App_Flocking();

		App_Flocking(const App_Flocking&) = delete;
		App_Flocking& operator=(const App_Flocking&) = delete;
		App_Flocking(App_Flocking&&) = delete;
		App_Flocking& operator=(App_Flocking&&) = delete;

		void Start() override;
		void Update(float deltaTime) override;
		void Render(float deltaTime) const override;

	private:
		TargetData m_MouseTarget;
		bool m_UseMouseTarget;
		bool m_VisualizeMouseTarget;
		float m_TrimWorldSize;
		int m_FlockSize;
		SteeringAgent* m_pAgentToEvade;
		Flock* m_pFlock;
};