#include "stdafx.h"
#include "EBFS.h"

#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"

using namespace Elite;

BFS::BFS(Graph* pGraph)
	: m_pGraph(pGraph)
{

}

//Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<GraphNode*> BFS::FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode)
{
	std::vector<GraphNode*> path{};
	std::queue<GraphNode*> openList{};
	std::map<GraphNode*, GraphNode*> closedList{};
	

	openList.push(pStartNode);

	while (!openList.empty())
	{
		GraphNode* currentNode{ openList.front() };
		openList.pop();

		if (currentNode == pDestinationNode) break;

		for (GraphConnection* connection : m_pGraph->GetConnectionsFromNode(currentNode))
		{
			GraphNode* nextNode = m_pGraph->GetNode(connection->GetToNodeId());

			if (!closedList.contains(nextNode))
			{
				openList.push(nextNode);
				closedList[nextNode] = currentNode;
			}
		}
	}

	if (!closedList.contains(pDestinationNode)) return path;

	GraphNode* currentNode{ pDestinationNode };
	while (currentNode != pStartNode)
	{
		path.push_back(currentNode);
		currentNode = closedList[currentNode];
	}

	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());

	return path;
}