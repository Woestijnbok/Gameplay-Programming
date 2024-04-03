#include "stdafx.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"

Flock::Flock(int flockSize, float worldSize, SteeringAgent* pAgentToEvade, bool trimWorld) :
	m_FlockSize{ flockSize },
	m_Agents{},
	m_Neighbors{},
	m_TrimWorld{ trimWorld },
	m_WorldSize{ worldSize },
	m_NeighborhoodRadius{ 15.0f },
	m_NrOfNeighbors{ 0 },
	m_pAgentToEvade{ pAgentToEvade },
	m_pBlendedSteering{ nullptr },
	m_pPrioritySteering{ nullptr },
	m_pSeparationBehavior{ new Separation(this) },
	m_pCohesionBehavior{ new Cohesion(this) },
	m_pAlignment{ new Alignment(this) },
	m_pSeekBehavior{ new Seek() },
	m_pWanderBehavior{ new Wander() },
	m_pEvadeBehavior{ new Evade() },
	m_SpatialPartitioningIsOn{ false },
	m_DebugRenderIsOn{ true },
	m_pCellSpace{ nullptr },
	m_NewPositionTimer{ 0.0f },
	m_OldPositions{}
{
	m_Agents.resize(m_FlockSize);
	m_OldPositions.resize( m_FlockSize);
	m_Neighbors.resize(m_FlockSize - 1);

	m_pCellSpace = new CellSpace{ m_WorldSize, m_WorldSize, 30, 30, m_FlockSize };

	m_pBlendedSteering = new BlendedSteering{ std::vector<BlendedSteering::WeightedBehavior>{
		BlendedSteering::WeightedBehavior{ m_pSeekBehavior, 0.0f },
		BlendedSteering::WeightedBehavior{ m_pWanderBehavior, 0.0f },
		BlendedSteering::WeightedBehavior{ m_pAlignment, 0.0f },
		BlendedSteering::WeightedBehavior{ m_pCohesionBehavior, 0.0f },
		BlendedSteering::WeightedBehavior{ m_pSeparationBehavior, 0.0f }
	} };

	m_pPrioritySteering = new PrioritySteering{ std::vector<ISteeringBehavior*>{ m_pEvadeBehavior, m_pBlendedSteering} };

	for (int i{}; i < m_FlockSize; i++)
	{
		m_Agents[i] = new SteeringAgent{};
		m_Agents[i]->SetPosition(Elite::randomVector2(0.0f, m_WorldSize));
		m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);
		m_Agents[i]->SetMaxLinearSpeed(15.0f);
		m_Agents[i]->SetAutoOrient(true);
		m_Agents[i]->SetMass(1.0f);

		m_pCellSpace->AddAgent(m_Agents[i]);

		m_OldPositions[i] = m_Agents[i]->GetPosition();
	}

	m_pAgentToEvade->SetBodyColor(Elite::Color{ 1.0f, 0.0f, 0.0f });
	m_pAgentToEvade->SetSteeringBehavior(m_pWanderBehavior);
}

Flock::~Flock()
{
	for (auto& pAgent : m_Agents)
	{
		SAFE_DELETE(pAgent);
	}

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pAlignment);
	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pEvadeBehavior);
	SAFE_DELETE(m_pCellSpace);
}

void Flock::Update(float deltaT)
{
	m_NewPositionTimer += deltaT;
	bool positionTimerFinished{ false };

	if (m_NewPositionTimer > 0.5f)
	{
		m_NewPositionTimer = 0.0f;
		positionTimerFinished = true;
	}

	for (int i{}; i < m_FlockSize; i++)
	{
		if (m_Agents[i] != nullptr)
		{
			(m_SpatialPartitioningIsOn) ? m_pCellSpace->RegisterNeighbors(m_Agents[i], m_NeighborhoodRadius) : RegisterNeighbors(m_Agents[i]);
			m_Agents[i]->Update(deltaT);
			m_Agents[i]->TrimToWorld(m_WorldSize);

			if (positionTimerFinished)
			{
				m_pCellSpace->AgentPositionChanged(m_Agents[i], m_OldPositions[i]);
				m_OldPositions[i] = m_Agents[i]->GetPosition();
			}
		}
	}

	m_pEvadeBehavior->SetTarget(m_pAgentToEvade->GetPosition());
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Behaviour Weights");
	ImGui::Spacing();

	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.0f);
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.0f);
	ImGui::SliderFloat("Alignment", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.0f);
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.0f);
	ImGui::SliderFloat("Seperation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.0f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox("Spatial Partitioning", &m_SpatialPartitioningIsOn);
	ImGui::Checkbox("Debug Render", &m_DebugRenderIsOn);

	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
}

void Flock::Render(float deltaT)
{
	for (auto& agent : m_Agents)
	{
		if (agent != nullptr)
		{
			agent->Render(deltaT);
		}
	}

	// Set the first agent's color to blue this will be the agent we base RenderNeighborhood() on
	m_Agents[0]->SetBodyColor((m_DebugRenderIsOn) ? Elite::Color{ 0.0f, 0.0f, 1.0f } : Elite::Color{ 1.0f, 1.0f, 0.0f });

	if (m_DebugRenderIsOn)
	{
		RenderNeighborhood();
	}
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	if (pAgent != nullptr)
	{
		// Reseting the memory pool / neighbours
		m_NrOfNeighbors = 0;

		// Get the position of pAgent
		Elite::Vector2 positionAgent{ pAgent->GetPosition() };

		for (int i{}; i < m_FlockSize; i++)	// Going trough every agent in the flock
		{
			if (m_Agents[i] != nullptr)
			{
				SteeringAgent* const pPossibleNeighbour{ m_Agents[i] };

				if (pPossibleNeighbour != pAgent)	// Making sure that we are not setting itslef as a neighbour
				{
					const float distanceSquared{ (pPossibleNeighbour->GetPosition() - positionAgent).MagnitudeSquared() };	// Distance between possible neighbour and agent

					if (distanceSquared <= powf(m_NeighborhoodRadius, 2))	// Checking if the possible neighbour is inside the radius
					{
						m_Neighbors[m_NrOfNeighbors] = pPossibleNeighbour;	// overriding the old neighbour with the new one
						++m_NrOfNeighbors;	// changing the index for the next neighbour
					}
				}
			}
		}
	}
}

int Flock::GetNrOfNeighbors() const
{
	return (m_SpatialPartitioningIsOn) ? m_pCellSpace->GetNrOfNeighbors() : m_NrOfNeighbors;
}

const std::vector<SteeringAgent*>& Flock::GetNeighbors() const
{
	return (m_SpatialPartitioningIsOn) ? m_pCellSpace->GetNeighbors() : m_Neighbors;
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 sumPositions{};

	for (int i{}; i < GetNrOfNeighbors(); i++)
	{
		if (GetNeighbors()[i] != nullptr)
		{
			sumPositions += GetNeighbors()[i]->GetPosition();
		}
	}

	if (GetNrOfNeighbors() != 0)
	{
		return (sumPositions / float(GetNrOfNeighbors()));
	}
	else
	{
		return Elite::Vector2{ 0.f, 0.0f };
	}
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 sumVelocities{};

	for (int i{}; i < GetNrOfNeighbors(); i++)
	{
		if (GetNeighbors()[i] != nullptr)
		{
			sumVelocities += GetNeighbors()[i]->GetLinearVelocity();
		}
	}

	if (GetNrOfNeighbors() != 0)
	{
		return (sumVelocities / float(GetNrOfNeighbors()));

	}
	else
	{
		return Elite::Vector2{};
	}
}

void Flock::SetTarget_Seek(const TargetData& target)
{
	m_pSeekBehavior->SetTarget(target);
}

void Flock::SetWorldTrimSize(float size)
{
	m_WorldSize = size;
}

void Flock::RenderNeighborhood()
{
	// We will debug render the neighborhood for the first agent
	(m_SpatialPartitioningIsOn) ? m_pCellSpace->RegisterNeighbors(m_Agents[0], m_NeighborhoodRadius) : RegisterNeighbors(m_Agents[0]);

	// Draws a big black dot on the neighbours of the first agent
	for (int i{}; i < GetNrOfNeighbors(); i++)
	{
		DEBUGRENDERER2D->DrawPoint(GetNeighbors()[i]->GetPosition(), 8.0f, Elite::Color{ 0.0f, 0.0, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
	}

	// Draws a blue circle which corrosponds to the neighbourhood radius of the first agent
	DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, Elite::Color{ 0.f, 1.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());

	if (m_SpatialPartitioningIsOn)
	{
		const Elite::Vector2 bottomLeft{ m_Agents[0]->GetPosition().x - m_NeighborhoodRadius, m_Agents[0]->GetPosition().y - m_NeighborhoodRadius };

		std::vector<Elite::Vector2>neighbourhoodRect{
			Elite::Vector2{ bottomLeft },
			Elite::Vector2{ bottomLeft.x, bottomLeft.y + 2 * m_NeighborhoodRadius },
			Elite::Vector2{ bottomLeft.x + 2 * m_NeighborhoodRadius, bottomLeft.y + 2 * m_NeighborhoodRadius },
			Elite::Vector2{ bottomLeft.x + 2 * m_NeighborhoodRadius, bottomLeft.y }
		};

		DEBUGRENDERER2D->DrawPolygon(&neighbourhoodRect[0], 4, Elite::Color{ 0.5, 9.0f, 0.2f }, DEBUGRENDERER2D->NextDepthSlice());

		m_pCellSpace->RenderCells(Elite::Rect{bottomLeft, 2 * m_NeighborhoodRadius, 2 * m_NeighborhoodRadius });
	}
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior)
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if (it != weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}