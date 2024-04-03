#include "stdafx.h"
#include "ENavGraph.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraphNodeFactory/EGraphNodeFactory.h"
//#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

NavGraph::NavGraph(const std::vector<Polygon>& colliderShapes, float widthWorld, float heightWorld, float playerRadius = 1.0f) :
	Graph(false, new GraphNodeFactoryTemplate<NavGraphNode>{}),
	m_pNavMeshPolygon{ nullptr }
{
	float const halfWidth = widthWorld / 2.0f;
	float const halfHeight = heightWorld / 2.0f;
	std::list<Vector2> baseBox
	{ { -halfWidth, halfHeight },{ -halfWidth, -halfHeight },{ halfWidth, -halfHeight },{ halfWidth, halfHeight } };

	m_pNavMeshPolygon = new Polygon(baseBox); // Create copy on heap

	//Store all children
	for (auto p : colliderShapes)
	{
		p.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(p);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

NavGraph::NavGraph(const NavGraph& other) :
	Graph(other)
{
}

NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon;
	m_pNavMeshPolygon = nullptr;
}

std::shared_ptr<NavGraph> NavGraph::Clone()
{
	return std::shared_ptr<NavGraph>(new NavGraph(*this));
}

int NavGraph::GetNodeIdFromLineIndex(int lineIdx) const
{

	for (auto& pNode : m_pNodes)
	{
		if (reinterpret_cast<NavGraphNode*>(pNode)->GetLineIndex() == lineIdx)
		{
			return pNode->GetId();
		}
	}

	return invalid_node_id;
}

Elite::Polygon* NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void NavGraph::CreateNavigationGraph()
{
	// Loop over all the lines of the polygon
	for (Line* pLine : m_pNavMeshPolygon->GetLines())
	{
		// Check if the line is connected to more then one triangle
		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(pLine->index).size() > size_t(1))
		{
			// If so add a node to the graph at the midle of line
			AddNode(new NavGraphNode(pLine->index, (pLine->p1 + pLine->p2) / 2.0f));
		}
	}

	// For each triangle in the mesh / polygon find the nodes and connect them
	for (Triangle* pTriangle : m_pNavMeshPolygon->GetTriangles())
	{
		// Temporary node id's vector used to make connections
		std::vector<int> nodeIds{};

		// Loop over all the line indexes of the triangle
		for (int indexLine : pTriangle->metaData.IndexLines)
		{
			int nodeId{ GetNodeIdFromLineIndex(indexLine) };

			// if we found a valid node id we add it to the temporary node id's vector
			if (nodeId != invalid_node_id) nodeIds.push_back(nodeId);
		}

		// We found two valid nodes in triangle so add 1 connection
		if (nodeIds.size() == size_t(2))
		{
			AddConnection(new GraphConnection{ nodeIds[0], nodeIds[1] });
		}

		// We found three valid nodes in triangle so add 3 connections
		else if (nodeIds.size() == size_t(3))
		{
			AddConnection(new GraphConnection{ nodeIds[0], nodeIds[1] });
			AddConnection(new GraphConnection{ nodeIds[1], nodeIds[2] });
			AddConnection(new GraphConnection{ nodeIds[2], nodeIds[0] });
		}
	}

	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistances();
}