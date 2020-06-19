#pragma once
#include "Entity_Living.h"
class Entity_Crafter : public Entity_Living
{
public:
	Entity_Crafter(wchar_t EntityID, sf::Vector2i spritePos, TaskManager* manager, std::vector<int> craftable, std::vector<TargetedHumanBehaviors> TargetedBehaviors) :Entity_Living(EntityID, spritePos, manager, TargetedBehaviors) {
		this->craftable = craftable;
	};
	virtual Entity_Crafter* clone() const { return new Entity_Crafter(*this); };
	
	std::vector<int> GetCraftable();
	bool CraftItem(int id, Entity_Living* crafterOwner, Entity* outEntity = nullptr);
private:
	bool ConsumeCraftingItem(std::vector<int > recipeItem);
	std::vector<int> craftable;
};