#include "stdafx.h"
#include "FoodSource.h"
#include <string> 

using namespace Elite;

FoodSource::FoodSource(Vector2 position, int amount) : 
	m_position{ position },
	m_amount{ amount }
{

}

void FoodSource::Render(float dt) const
{
	// Draw green circle at food location
	DEBUGRENDERER2D->DrawSolidCircle(m_position, 0.5f, Vector2{ 1.0f, 0.0f }, Color{ 0.0f, 1.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());

	// Draw text wich indicates amount left
	DEBUGRENDERER2D->DrawString(m_position, std::to_string(m_amount).c_str());
}

int FoodSource::GetAmount() const 
{ 
	return m_amount; 
}

void FoodSource::TakeFood() 
{ 
	--m_amount;
}

Elite::Vector2 FoodSource::GetPosition() const 
{ 
	return m_position;
}