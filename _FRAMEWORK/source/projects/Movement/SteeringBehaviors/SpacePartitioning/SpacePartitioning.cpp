#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"
#include<cassert>
#include<string>

Cell::Cell(float left, float bottom, float width, float height) :
	agents{},
	boundingBox{ Elite::Vector2{left, bottom}, width, height }
{

}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	const float left{ boundingBox.bottomLeft.x };
	const float bottom{ boundingBox.bottomLeft.y };
	const float width{ boundingBox.width };
	const float height{ boundingBox.height };

	return std::vector<Elite::Vector2>
	{
		Elite::Vector2{ left, bottom },
			Elite::Vector2{ left, bottom + height },
			Elite::Vector2{ left + width, bottom + height },
			Elite::Vector2{ left + width, bottom },
	};
}

void Cell::DebugRender(bool isActive) const
{
	if (isActive)
	{
		DEBUGRENDERER2D->DrawPolygon(GetRectPoints().data(), 4, Elite::Color{ 0.0f, 1.0f, 0.0f, 0.5f }, DEBUGRENDERER2D->NextDepthSlice());
	}
	else
	{
		DEBUGRENDERER2D->DrawPolygon(GetRectPoints().data(), 4, Elite::Color{ 0.5f, 0.0f, 0.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawString(Elite::Vector2{ boundingBox.bottomLeft.x + 1.0f, boundingBox.bottomLeft.y + 3.0f }, std::to_string(agents.size()).c_str());
	}
}

CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities) :
	m_Cells{ size_t(rows * cols) },
	m_SpaceWidth{ width },
	m_SpaceHeight{ height },
	m_NrOfRows{ rows },
	m_NrOfCols{ cols },
	m_CellWidth{ width / cols },
	m_CellHeight{ height / rows },
	m_Neighbors{ size_t(maxEntities) },
	m_NrOfNeighbors{ 0 }
{
	for (int row{}; row < rows; row++)
	{
		for (int collumn{}; collumn < cols; collumn++)
		{
			m_Cells[row * cols + collumn] = Cell{ collumn * m_CellWidth, m_SpaceHeight - ((1 + row) * m_CellHeight), m_CellWidth, m_CellHeight };
		}
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::AgentPositionChanged(SteeringAgent* agent, Elite::Vector2 oldPos)
{
	int oldIndex{ PositionToIndex(oldPos) };
	int newIndex{ PositionToIndex(agent->GetPosition()) };

	if (oldIndex != newIndex)
	{
		m_Cells[oldIndex].agents.remove(agent);
		m_Cells[newIndex].agents.push_front(agent);
	}
}

void CellSpace::RegisterNeighbors(SteeringAgent* pAgent, float neighborhoodRadius)
{
	if (pAgent != nullptr)
	{
		m_NrOfNeighbors = 0;
		Elite::Vector2 positionAgent{ pAgent->GetPosition() };

		Elite::Rect NeighbourhoodRect{ 
			Elite::Vector2{ positionAgent.x - neighborhoodRadius, positionAgent.y - neighborhoodRadius},	// Bottom left cornor
			2 * neighborhoodRadius,	// Width
			2 * neighborhoodRadius	// Height
		};

		for (const auto& cell : m_Cells)
		{
			if (Elite::IsOverlapping(NeighbourhoodRect, cell.boundingBox))
			{
				for (auto pPossibleNeighbour : cell.agents)
				{
					if(pPossibleNeighbour != nullptr && pPossibleNeighbour != pAgent)
					{
						const float distanceSquared{ (pPossibleNeighbour->GetPosition() - positionAgent).MagnitudeSquared() };

						if (distanceSquared <= powf(neighborhoodRadius, 2))	// Checking if the possible neighbour is inside the radius
						{
							m_Neighbors[m_NrOfNeighbors] = pPossibleNeighbour;	// overriding the old neighbour with the new one
							++m_NrOfNeighbors;	// changing the index for the next neighbour
						}
					}
				}
			}
		}
	}
}

const std::vector<SteeringAgent*>& CellSpace::GetNeighbors() const
{
	return m_Neighbors;
}

int CellSpace::GetNrOfNeighbors() const
{
	return m_NrOfNeighbors;
}

void CellSpace::RenderCells(const Elite::Rect& neighbourHoodRect) const
{
	for (const auto& cell : m_Cells)
	{
		cell.DebugRender(false);
	}

	for (const auto& cell : m_Cells)
	{
		if (Elite::IsOverlapping(neighbourHoodRect, cell.boundingBox))
		{
			cell.DebugRender(true);
		}
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	const int collumn{ (Elite::AreEqual(pos.x, m_SpaceWidth)) ? m_NrOfCols - 1 : int(pos.x / m_CellWidth) };

	const int row{ (Elite::AreEqual(pos.y, 0.0f)) ? m_NrOfRows - 1 : int((m_SpaceHeight - pos.y) / m_CellHeight) };

 	const int index{ row * m_NrOfCols + collumn };

	assert(index >= 0 && index < (m_NrOfCols * m_NrOfRows));

	return index;
}