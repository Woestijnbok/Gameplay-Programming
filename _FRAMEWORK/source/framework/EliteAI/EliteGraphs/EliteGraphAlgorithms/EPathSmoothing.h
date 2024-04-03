#pragma once
#include <vector>
#include "framework/EliteGeometry/EGeometry2DTypes.h"

namespace Elite
{
	class GraphNode;

	struct Portal
	{
		Portal() = default;
		explicit Portal(const Elite::Line& line);

		Elite::Line Line;
	};

	class SSFA final
	{
		public:
			SSFA() = delete;
			~SSFA() = delete;

			static std::vector<Portal> FindPortals(const std::vector<GraphNode*>& nodePath, Polygon* navMeshPolygon);
			static std::vector<Elite::Vector2> OptimizePortals(const std::vector<Portal>& portals);
	};
}