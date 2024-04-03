// Some links about Simple stupid funnel algorithm
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work

#include <stdafx.h>
#include "EPathSmoothing.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraphNode.h"

using namespace Elite;

Portal::Portal(const Elite::Line& line) :
	Line{ line }
{

}

std::vector<Portal> SSFA::FindPortals(const std::vector<GraphNode*>& nodePath, Polygon* navMeshPolygon)
{
	//Container
	std::vector<Portal> vPortals{};

	vPortals.push_back(Portal(Line(nodePath[0]->GetPosition(), nodePath[0]->GetPosition())));

	//For each node received, get it's corresponding line
	for (size_t i = 1; i < nodePath.size() - 1; ++i)
	{
		//Local variables
		auto pNode = static_cast<NavGraphNode*>(nodePath[i]); //Store node, except last node, because this is our target node!
		auto pLine = navMeshPolygon->GetLines()[pNode->GetLineIndex()];

		//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
		auto centerLine = (pLine->p1 + pLine->p2) / 2.0f;
		auto previousPosition = i == 0 ? nodePath[0]->GetPosition() : nodePath[i - 1]->GetPosition();
		auto cp = Cross((centerLine - previousPosition), (pLine->p1 - previousPosition));
		Line portalLine = {};
		if (cp > 0)//Left
			portalLine = Line(pLine->p2, pLine->p1);
		else //Right
			portalLine = Line(pLine->p1, pLine->p2);

		//Store portal
		vPortals.push_back(Portal(portalLine));
	}
	//Add degenerate portal to force end evaluation
	vPortals.push_back(Portal(Line(nodePath[nodePath.size() - 1]->GetPosition(), nodePath[nodePath.size() - 1]->GetPosition())));

	return vPortals;
}

std::vector<Elite::Vector2> SSFA::OptimizePortals(const std::vector<Portal>& portals)
{
	//P1 == right point of portal, P2 == left point of portal
	std::vector<Vector2> path{};

	int apexIdx{ 0 }; 
	int	leftLegIdx{ 1 };
	int	rightLegIdx{ 1 };

	Vector2 apexPos = portals[apexIdx].Line.p1;
	Vector2 rightLeg = portals[rightLegIdx].Line.p1 - apexPos;
	Vector2 leftLeg = portals[leftLegIdx].Line.p2 - apexPos;

	// Add the apex point to the path.
	path.push_back(apexPos);

	// Loop over all the portals starting from the second one.
	for (int portalIdx{ 1 }; portalIdx < int(portals.size()); portalIdx++)
	{
		const Portal& portal{ portals[portalIdx] };

		//--- RIGHT CHECK ---
		const Vector2 newRightLeg{ portal.Line.p1 - apexPos };

		//1. See if moving funnel inwards - RIGHT
		if (Cross(rightLeg, newRightLeg) > 0.0f)
		{
			//2. See if new line degenerates a line segment - RIGHT
			if (Cross(newRightLeg, leftLeg) <= 0.0f)
			{
				apexPos += leftLeg;
				apexIdx = leftLegIdx;
				portalIdx = leftLegIdx + 1;
				leftLegIdx = portalIdx;
				rightLegIdx = portalIdx;
				path.push_back(apexPos);
				if (portalIdx < int(portals.size()))
				{
					rightLeg = portals[rightLegIdx].Line.p1 - apexPos;
					leftLeg = portals[leftLegIdx].Line.p2 - apexPos;
					continue;
				}
			}
			else
			{
				rightLeg = newRightLeg;
				rightLegIdx = portalIdx;
			}
		}
			
		//--- LEFT CHECK ---
		const Vector2 newLeftLeg{ portal.Line.p2 - apexPos };

		//1. See if moving funnel inwards - LEFT
		if (Cross(leftLeg, newLeftLeg) < 0.0f)
		{
			//2. See if new line degenerates a line segment - LEFT
			if (Cross(rightLeg, newLeftLeg) <= 0.0f)
			{
				apexPos += rightLeg;
				apexIdx = rightLegIdx;
				portalIdx = rightLegIdx + 1;
				leftLegIdx = portalIdx;
				rightLegIdx = portalIdx;
				path.push_back(apexPos);
				if (portalIdx < int(portals.size()))
				{
					rightLeg = portals[rightLegIdx].Line.p1 - apexPos;
					leftLeg = portals[leftLegIdx].Line.p2 - apexPos;
					continue;
				}
			}
			else
			{
				leftLeg = newLeftLeg;
				leftLegIdx = portalIdx;
			}
		}

	}

	// Add last path point (You can use the last portal p1 or p2 points as both are equal to the endPoint of the path
	path.push_back(portals.back().Line.p1);

	return path;
}