#pragma once

namespace Elite
{
	class GraphNode;
	class InfluenceNode;

	class InfluenceMap final
	{
	public:
		InfluenceMap(int columns, int rows, float cellSize);
		~InfluenceMap() = default;

		InfluenceMap(const InfluenceMap&) = delete;
		InfluenceMap& operator=(const InfluenceMap&) = delete;
		InfluenceMap(InfluenceMap&&) = delete;
		InfluenceMap& operator=(InfluenceMap&&) = delete;

		float GetMomentum() const;
		void SetMomentum(float momentum);
		float GetDecay() const;
		void SetDecay(float decay);
		float GetPropagationInterval() const;
		void SetPropagationInterval(float propagationInterval);
		void Update(float deltaTime);
		void Render() const;
		float GetInfluenceAtPosition(const Vector2 & pos) const;
		void SetInfluenceAtPosition(const Vector2& pos, float influence, bool additive = false);

	private:
		const float DC;
		const Color m_NegativeColor;
		const Color m_NeutralColor;
		const Color m_PositiveColor;
		int m_numColumns;
		int m_numRows;
		float m_cellSize;
		float m_MaxAbsInfluence;
		float m_Momentum;							// Higher momentum means a higher tendency to retain the current influence
		float m_Decay;								// Determines the decay in influence over distance
		float m_PropagationInterval;				// In Seconds
		float m_TimeSinceLastPropagation;
		std::vector<float> m_InfluenceBackBuffer;
		std::vector<float> m_InfluenceFrontBuffer;
		const float m_influenceCosts[9];

		bool IsInBounds(const Vector2& position) const;
		int GetIndexAtPosition(const Vector2& position) const;
		std::vector<int> GetNeighboringIndices(int nodeIndex) const;
		Color GetNodeColorFromInfluence(float influence) const;
		void SwapBuffers();
	};
}