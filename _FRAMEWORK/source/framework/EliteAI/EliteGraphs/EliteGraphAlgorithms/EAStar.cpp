#include "stdafx.h"
#include "EAStar.h"

using namespace Elite;
AStar::AStar(Graph* pGraph, Heuristic hFunction) :
	m_pGraph{ pGraph },
	m_HeuristicFunction{ hFunction }
{

}

std::vector<GraphNode*>AStar::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{
	std::vector<GraphNode*> path{};			// Final path we return
	std::list<NodeRecord> openList{};		// List of node records we are going to check
	std::list<NodeRecord> closedList{};		// List of nodes we already checked

	NodeRecord startNodeRecord{ pStartNode, nullptr, 0.0f, GetHeuristicCost(pStartNode, pGoalNode) };
	NodeRecord currentNodeRecord{};			// Node record which showcases the current node we are working with

	// 1. Add the start node record to the open list to kick start the algorithm
	openList.push_front(startNodeRecord);

	// 2. While the open list is not empty, we still have nodes we didn't check and are valid
	while (!openList.empty())
	{
		// 2.A Get the node record with the lowest f-cost from the openlist
		currentNodeRecord = *std::min_element(openList.begin(), openList.end());

		// 2.B Check if the current node record is the goal node
		if (currentNodeRecord.pNode == pGoalNode) break;

		// 2.C For each connection starting from the current node (record)
		std::vector<GraphConnection*> connections{ m_pGraph->GetConnectionsFromNode(currentNodeRecord.pNode) };
		for (GraphConnection* connection : connections)
		{
			// We make a new node record which represents the neighbour of the current node (record), the other side of the connection
			GraphNode* pNextNode{ m_pGraph->GetNode(connection->GetToNodeId()) };
			const float gCost{ currentNodeRecord.costSoFar + connection->GetCost() };
			const float hCost{ GetHeuristicCost(pNextNode, pGoalNode) };
			const float fCost{ gCost + hCost };
			NodeRecord neighbour{ pNextNode, connection, gCost, fCost };

			bool CanAddNodeRecord{ true };

			// 2.C.D Check if the connection leads to a node already in the closed list
			for (const NodeRecord& nodeRecord : closedList)
			{
				if (nodeRecord.pNode == pNextNode)
				{
					if (nodeRecord.costSoFar > gCost) closedList.remove(currentNodeRecord);
					else CanAddNodeRecord = false;
				}
			}

			// 2.C.E Check if the connection leads to a node already in the open list
			for (const NodeRecord& nodeRecord : openList)
			{
				if (nodeRecord.pNode == pNextNode)
				{
					if (nodeRecord.costSoFar > gCost) openList.remove(currentNodeRecord);
					else CanAddNodeRecord = false;
				}
			}

			// Both 2.C.D & 2.C.E are a special case where we visit a node twice, then we make sure we take the path that is the cheapest

			// 2.C.F Only if 2.C.D & 2.C.E were not cheaper, add teh neighbour to the open list
			if (CanAddNodeRecord) openList.push_front(neighbour);
		}

		// 2.G Remove the current node record from the open list and add it to the closed list
		openList.remove(currentNodeRecord);
		closedList.push_front(currentNodeRecord);
	}

	// This is so we handle the node record which corrolates to the goal node since we break when we meet him in the while loop
	openList.remove(currentNodeRecord);
	closedList.push_front(currentNodeRecord);

	// 3. Reconstruct the path starting with the end / goal node (backt racking)
	while (currentNodeRecord.pNode != pStartNode)
	{
		// Add the node of the current node record to the path
		path.push_back(currentNodeRecord.pNode);

		// Go trought each node record in the closed list
		for (const NodeRecord& nodeRecord : closedList)
		{
			// Just to avoid a compiler warning
			if (currentNodeRecord.pConnection != nullptr)
			{
				// If there is a node record which has the node that is connected to the current node record
				if (nodeRecord.pNode == m_pGraph->GetNode(currentNodeRecord.pConnection->GetFromNodeId()))
				{
					// Here we do the back tracking and make, make sure to break in order to build the path
					currentNodeRecord = nodeRecord;
					break;
				}
			}
		}
	}

	// Add the start node since we stop the while loop when it happens, also reverse the path because we went from goal to start
	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());

	return path;
}


float AStar::GetHeuristicCost(GraphNode* pStartNode, GraphNode* pEndNode) const
{
	Vector2 toDestination = m_pGraph->GetNodePos(pEndNode->GetId()) - m_pGraph->GetNodePos(pStartNode->GetId());
	return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
}