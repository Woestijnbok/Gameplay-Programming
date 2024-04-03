#include "stdafx.h"
#include "ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"

using namespace Elite;

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
{
	debugNodePositions.clear();
	debugPortals.clear();

	//Get the startTriangle and endTriangle
	Polygon* const pNavMesh{ pNavGraph->GetNavMeshPolygon() };
	const Triangle* const pStartTriangle{ pNavMesh->GetTriangleFromPosition(startPos) };
	const Triangle* const pEndTriangle{ pNavMesh->GetTriangleFromPosition(endPos) };

	//If we don't have a valid startTriangle or endTriangle -> return empty path
	if (pStartTriangle == nullptr || pEndTriangle == nullptr) return std::vector<Vector2>{};

	//If the startTriangle and endTriangle are the same -> return straight line path
	if (pStartTriangle == pEndTriangle) return std::vector<Vector2>{ startPos, endPos };
	
	//=> Start looking for a path
	//Clone the graph (returns shared_ptr!)
	std::shared_ptr<NavGraph> clonedNavGraph{ pNavGraph->Clone() };

	//Create extra node for the Start Node (Agent's position) and add it to the graph. 
	NavGraphNode* pStartNode{ new NavGraphNode{ invalid_node_id, startPos } };
	clonedNavGraph->AddNode(pStartNode);

	//Make connections between the Start Node and the startTriangle nodes.
	for (int indexLine : pStartTriangle->metaData.IndexLines)
	{
		int nodeId{ pNavGraph->GetNodeIdFromLineIndex(indexLine) };

		if (nodeId != invalid_node_id)
		{
			GraphNode* node{ pNavGraph->GetNode(nodeId) };
			clonedNavGraph->AddConnection(new GraphConnection{ pStartNode->GetId(), nodeId, Distance(startPos, node->GetPosition()) });
		}
	}
	
	//Create extra node for the End Node (endpos) and add it to the graph. 
	NavGraphNode* pEndNode{ new NavGraphNode{ invalid_node_id, endPos } };
	clonedNavGraph->AddNode(pEndNode);

	//Make connections between the End Node and the endTriangle nodes.
	for (int indexLine : pEndTriangle->metaData.IndexLines)
	{
		int nodeId{ pNavGraph->GetNodeIdFromLineIndex(indexLine) };

		if (nodeId != invalid_node_id)
		{
			GraphNode* node{ pNavGraph->GetNode(nodeId) };
			clonedNavGraph->AddConnection(new GraphConnection{ pEndNode->GetId(), nodeId, Distance(startPos, node->GetPosition()) });
		}
	}
	
	//Run AStar on the new graph
	AStar aStart{ clonedNavGraph.get(), Elite::HeuristicFunctions::Chebyshev };
	std::vector<GraphNode*> nodePath{ aStart.FindPath(pStartNode, pEndNode) };
	for (GraphNode* pGraphNode : nodePath)
	{
		debugNodePositions.push_back(pGraphNode->GetPosition());
	}

	//Run optimiser on new graph, MAKE SURE the AStar path is working properly before starting the following section:
	debugPortals = SSFA::FindPortals(nodePath, pNavMesh);
	return SSFA::OptimizePortals(debugPortals);
}

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph)
{
	std::vector<Vector2> debugNodePositions{};
	std::vector<Portal> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}