#pragma once
#include <stack>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
		public:
			EulerianPath(Graph* pGraph);
			~EulerianPath() = default;

			Eulerianity IsEulerian() const;
			std::vector<GraphNode*> FindPath(Eulerianity& eulerianity) const;

		private:
			void VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex) const;
			bool IsConnected() const;

			Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* pGraph) :
		m_pGraph{ pGraph }
	{

	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected()) return Eulerianity::notEulerian;

		// Count nodes with odd degree 
		auto nodes = m_pGraph->GetAllNodes();
		int count{ 0 };

		for (const auto& node : nodes)
		{
			auto connections = m_pGraph->GetConnectionsFromNode(node);
			int amtConnections{ int(connections.size()) };

			if (amtConnections & 1)	// checking if oneven is the same as using modulo
			{
				++count;
			}
		}

		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (count > 2) return Eulerianity::notEulerian;

		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// An Euler trail can be made, but only starting and ending in these 2 nodes
		if (count == 2) return Eulerianity::semiEulerian;

		// A connected graph with no odd nodes is Eulerian
		return Eulerianity::eulerian;
	}

	inline std::vector<GraphNode*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy{ m_pGraph->Clone() };
		std::vector<GraphNode*> path{};
		int nrOfNodes = graphCopy->GetAmountOfNodes();
		const std::vector<GraphNode*>& nodes{ m_pGraph->GetAllNodes() };

		// Check if there can be an Euler path and if so choose a start node
		GraphNode* currentNode{ nullptr };
		GraphNode* startNode{ nullptr };

		switch (eulerianity)
		{
			case Eulerianity::eulerian:
				// Choose any node
				currentNode = nodes[0];
				startNode = currentNode;
				break;
			case Eulerianity::semiEulerian:
				// Chose a node with on odd degree of connections
				{
					int index{ 0 };
					while (!(m_pGraph->GetConnectionsFromNode(nodes[index]).size() & 1)) ++index;
					currentNode = nodes[index];
					startNode = currentNode;
				}
				break;
			case Eulerianity::notEulerian:
				// Return the empty path
				return path;
				break;
		}

		// Start algorithm loop
		std::stack<int> nodeStack;
		while (!(nodeStack.empty() && graphCopy->GetConnectionsFromNode(currentNode).size() == 0))
		{
			const std::vector<GraphConnection*>& connections{ graphCopy->GetConnectionsFromNode(currentNode) };

			// If it has any neighbours
			if(connections.size() != 0)
			{
				// Add the node to the stack
				nodeStack.push(currentNode->GetId());

				// Take any of its neighbours
				GraphNode* neighbournode{ graphCopy->GetNode(connections[0]->GetToNodeId()) };

				// Remove the connection between the neighbour and current node
				graphCopy->RemoveConnection(connections[0]);

				// Set the neighbour as the current node
				currentNode = neighbournode;
			}
			else // it doens't have neighbours
			{
				// Add the current node to the path
				path.push_back(m_pGraph->GetNode(currentNode->GetId()));

				// Set last node in stack as current node
				currentNode = m_pGraph->GetNode(nodeStack.top());

				// remove this last node from the stack
				nodeStack.pop();
			}
		}

		// Add the start node
		path.push_back(startNode);

		// reverses order of the path
		std::reverse(path.begin(), path.end()); 

		return path;
	}


	inline void EulerianPath::VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex ) const
	{
		// Mark the node as visited
		visited[startIndex] = true;

		// Get all connections from node
		std::vector<GraphConnection*> connections{ m_pGraph->GetConnectionsFromNode(pNodes[startIndex]) };

		// Go over all connections from that node
		for (GraphConnection* graphConnection : connections)
		{
			// Get node on the other side of that connection
			GraphNode* toNode{ m_pGraph->GetNode(graphConnection->GetToNodeId()) };

			// Get the index of that toNode
			int index{ 0 };
			while (pNodes[index] != toNode) ++index;

			// If node hasn't been visited we call VisitAllNodesDFS()
			if (!visited[index]) VisitAllNodesDFS(pNodes, visited, index);
		}
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Elite::GraphNode*> nodes = m_pGraph->GetAllNodes();
		if (nodes.size() == 0) return false;

		// start a depth-first-search traversal from the node that has at least one connection
		std::vector<bool> visited{};
		visited.reserve(nodes.size());
		for (size_t i{}; i < nodes.size(); i++)
		{
			visited.emplace_back(false);
		}

		VisitAllNodesDFS(nodes, visited, 0);

		// if a node was never visited, this graph is not connected
		for (bool visit : visited)
		{
			if (!visit) return false;
		}

		return true;
	}
}