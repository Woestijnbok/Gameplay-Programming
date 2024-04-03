#pragma once

namespace Elite
{
	struct Vector2;
}

class FoodSource final
{
public:
	FoodSource(Elite::Vector2 position, int amount);
	~FoodSource() = default;

	FoodSource(const FoodSource&) = delete;
	FoodSource& operator=(const FoodSource&) = delete;
	FoodSource(FoodSource&&) = delete;
	FoodSource& operator=(FoodSource&&) = delete;

	void Render(float dt) const;
	int GetAmount() const;
	void TakeFood();
	Elite::Vector2 GetPosition() const;

private:

	Elite::Vector2 m_position;
	int m_amount;
};