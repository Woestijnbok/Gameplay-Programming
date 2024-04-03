#pragma once
#include <list>
#include <vector>
#include <iterator>
#include "framework\EliteMath\EVector2.h"
#include "framework\EliteGeometry\EGeometry2DTypes.h"

class SteeringAgent;

struct Cell final
{
	Cell(float left, float bottom, float width, float height);
	Cell() = default;
	~Cell() = default;

	Cell(const Cell& other) = default;
	Cell& operator=(const Cell& other) = default;
	Cell(Cell&& other) = default;
	Cell& operator=(Cell&& other) = default;

	std::vector<Elite::Vector2> GetRectPoints() const;
	void DebugRender(bool isActive) const;
	
	std::list<SteeringAgent*> agents;
	Elite::Rect boundingBox;
};

class CellSpace final
{
	public:
		CellSpace(float width, float height, int rows, int cols, int maxEntities);
		~CellSpace() = default;

		CellSpace(const CellSpace& other) = default;
		CellSpace& operator=(const CellSpace& other) = default;
		CellSpace(CellSpace&& other) = default;
		CellSpace& operator=(CellSpace&& other) = default;

		void AddAgent(SteeringAgent* agent);
		void AgentPositionChanged(SteeringAgent* agent, Elite::Vector2 oldPos);
		void RegisterNeighbors(SteeringAgent* pAgent, float neighborhoodRadius);
		const std::vector<SteeringAgent*>& GetNeighbors() const;
		int GetNrOfNeighbors() const;
		void RenderCells(const Elite::Rect& neighbourHoodRect)const;

	private:
		std::vector<Cell> m_Cells;
		float m_SpaceWidth;
		float m_SpaceHeight;
		int m_NrOfRows;
		int m_NrOfCols;
		float m_CellWidth;
		float m_CellHeight;
		std::vector<SteeringAgent*> m_Neighbors;
		int m_NrOfNeighbors;

		int PositionToIndex(const Elite::Vector2 pos) const;
};
