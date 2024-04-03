#include "stdafx.h"
#include "EInfluenceMap.h"

using namespace Elite;

InfluenceMap::InfluenceMap(int columns, int rows, float cellSize) :
	DC{ 1.41421356f },
	m_NegativeColor{ 1.0f, 0.2f, 0.0f },
	m_NeutralColor{ 0.0f, 0.0f, 0.0f },
	m_PositiveColor{ 0.0f, 0.2f, 1.0f },
	m_numColumns{ columns },
	m_numRows{ rows },
	m_cellSize{ cellSize },
	m_MaxAbsInfluence{ 100.0f },
	m_Momentum{ 0.8f },
	m_Decay{ 0.1f },
	m_PropagationInterval{ 0.05f },
	m_TimeSinceLastPropagation{ 0.0f },
	m_InfluenceBackBuffer{},
	m_InfluenceFrontBuffer{},
	m_influenceCosts{ DC, 1.0f, DC, 1.0f, 0.0f, 1.0f, DC, 1.0f, DC }
{
	m_InfluenceBackBuffer.resize(columns * rows, 0.0f);
	m_InfluenceFrontBuffer.resize(columns * rows, 0.0f);
}

float InfluenceMap::GetMomentum() const 
{ 
	return m_Momentum;
}

void InfluenceMap::SetMomentum(float momentum) 
{ 
	m_Momentum = momentum;
}

float InfluenceMap::GetDecay() const 
{ 
	return m_Decay;
}

void InfluenceMap::SetDecay(float decay) 
{ 
	m_Decay = decay;
}

float InfluenceMap::GetPropagationInterval() const 
{ 
	return m_PropagationInterval;
}

void InfluenceMap::SetPropagationInterval(float propagationInterval) 
{ 
	m_PropagationInterval = propagationInterval;
}

void InfluenceMap::Update(float deltaTime)
{
	// Update the propegate timer
	m_TimeSinceLastPropagation += deltaTime;

	// If we passes a certain set time we can propegate again
	if (m_TimeSinceLastPropagation > m_PropagationInterval)
	{
		// Reset the propegate timer
		m_TimeSinceLastPropagation -= m_PropagationInterval;

		// Go through all cells in the influence map
		// Write to back buffer and read from front buffer
		for (int index{ 0 }; index < (m_numColumns * m_numRows); ++index)
		{
			// Find the max influence of it's neighbours
			float maxInfluence{ 0.0f };

			// Go through all the neighbours of the current cell
			for (int neighbourIndex : GetNeighboringIndices(index))
			{
				//Check if it's a invalid neighbour, out of bounds
				if (neighbourIndex != -1)
				{
					// Sample the influence of the neighbour
					const float sampledInfluence{ m_InfluenceFrontBuffer[neighbourIndex] };

					// Calculate the distance cost to that neighbour
					float distanceCost{};

					const int rowCurrent = index / m_numColumns;
					const int colCurrent = index % m_numColumns;
					const int rowNeighbour = neighbourIndex / m_numColumns;
					const int colNeighbour = neighbourIndex % m_numColumns;

					const int diffRow = abs(rowNeighbour - rowCurrent);
					const int diffCol = abs(colNeighbour - colCurrent);
					const int totalDiff = diffCol + diffRow;

					if (totalDiff == 0)
					{
						distanceCost = 0.0f;
					}
					else if (totalDiff == 1)
					{
						distanceCost = 1.0f;
					}
					else if (totalDiff == 2)
					{
						distanceCost = DC;
					}

					// Calculate the influce of the neighbour
					const float neighbourInfluence = sampledInfluence * expf(-distanceCost * m_Decay);

					// If this neighbour has a larger absolute influence override the current max influence
					if (abs(neighbourInfluence) > abs(maxInfluence)) maxInfluence = neighbourInfluence;
				}
			}

			// Interpolate the max influence with the current influence based on the momentum
			const float newInfluence = Lerp(maxInfluence, m_InfluenceFrontBuffer[index], m_Momentum);

			// Write this calculated influence to the back buffer
			m_InfluenceBackBuffer[index] = newInfluence;
		}

		SwapBuffers();
	}
}

void InfluenceMap::Render() const
{
	float depth = DEBUGRENDERER2D->NextDepthSlice();

	float halfSize = m_cellSize * .5f;
	int idx;
	Color col;
	Vector2 pos;
	Vector2 verts[4];
	for (int r = 0; r < m_numRows; ++r)
	{
		pos.y = r * m_cellSize;
		for (int c = 0; c < m_numColumns; ++c)
		{
			idx = r * m_numColumns + c;
			col = GetNodeColorFromInfluence(m_InfluenceFrontBuffer[idx]);
			pos.x = c * m_cellSize;

			verts[0] = Vector2(pos.x, pos.y);
			verts[1] = Vector2(pos.x, pos.y + m_cellSize);
			verts[2] = Vector2(pos.x + m_cellSize, pos.y + m_cellSize);
			verts[3] = Vector2(pos.x + m_cellSize, pos.y);
			DEBUGRENDERER2D->DrawSolidPolygon(&verts[0], 4, col, depth);
			//DEBUGRENDERER2D->DrawPolygon(&verts[0], 4, { .2f,.2f,.2f }, depth);

		};

	}
}

float InfluenceMap::GetInfluenceAtPosition(const Vector2& pos) const
{
	if (!IsInBounds(pos))return -1.f;

	auto idx = GetIndexAtPosition(pos);
	return m_InfluenceFrontBuffer[idx];
}

void InfluenceMap::SetInfluenceAtPosition(const Vector2& pos, float influence, bool additive)
{
	auto idx = GetIndexAtPosition(pos);
	if (additive)
	{
		m_InfluenceFrontBuffer[idx] += influence;
	}
	else
	{
		m_InfluenceFrontBuffer[idx] = influence;
	}
}

bool InfluenceMap::IsInBounds(const Vector2& position) const
{
	int r, c;

	c = int(floorf(position.x / m_cellSize));
	r = int(floorf(position.y / m_cellSize));
	return c >= 0 && c < m_numColumns && r >= 0 && r < m_numRows;
}

int InfluenceMap::GetIndexAtPosition(const Vector2& position) const
{
	int r, c;

	c = int(floorf(position.x / m_cellSize));
	r = int(floorf(position.y / m_cellSize));

	c = Elite::Clamp(c, 0, m_numColumns - 1);
	r = Elite::Clamp(r, 0, m_numRows - 1);

	return r * m_numColumns + c;
}

std::vector<int> InfluenceMap::GetNeighboringIndices(int nodeIdx) const
{
	std::vector<int> indices{};
	indices.resize(9);

	int row = nodeIdx / m_numColumns;
	int col = nodeIdx % m_numColumns;

	int idx = 0;
	for (int r = -1; r < 2; ++r)
	{
		int currRow = row + r;
		if (currRow < 0 || currRow >= m_numRows)
		{
			indices[idx++] = -1;
			indices[idx++] = -1;
			indices[idx++] = -1;
			continue;
		}

		for (int c = -1; c < 2; ++c)
		{
			int currCol = col + c;

			if (c == 0 && r == 0)
			{
				indices[idx++] = -1;
				continue;
			}

			if (currCol < 0 || currCol >= m_numColumns)
				indices[idx++] = -1;
			else
				indices[idx++] = currRow * m_numColumns + currCol;
		}
	}
	return indices;
}

Color InfluenceMap::GetNodeColorFromInfluence(float influence) const
{
	Color nodeColor{};
	const float relativeInfluence{ abs(influence) / m_MaxAbsInfluence };

	if (influence < 0)
	{
		nodeColor = Elite::Color
		{
			Lerp(m_NeutralColor.r, m_NegativeColor.r, relativeInfluence),
			Lerp(m_NeutralColor.g, m_NegativeColor.g, relativeInfluence),
			Lerp(m_NeutralColor.b, m_NegativeColor.b, relativeInfluence)
		};
	}
	else
	{
		nodeColor = Elite::Color
		{
			Lerp(m_NeutralColor.r, m_PositiveColor.r, relativeInfluence),
			Lerp(m_NeutralColor.g, m_PositiveColor.g, relativeInfluence),
			Lerp(m_NeutralColor.b, m_PositiveColor.b, relativeInfluence)
		};
	}

	return nodeColor;
}

void InfluenceMap::SwapBuffers()
{
	m_InfluenceBackBuffer.swap(m_InfluenceFrontBuffer);
}