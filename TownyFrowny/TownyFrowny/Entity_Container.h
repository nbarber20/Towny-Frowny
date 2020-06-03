#pragma once
#include "Entity.h"
class Entity_Container : public Entity {
public:
	Entity_Container(wchar_t EntityID, sf::Vector2i spritePos, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID,spritePos, worldref, TargetedBehaviors)
	{
	};
	void AddItemToInventory(Entity* e)
	{
		e->SetParent(this);
		heldItems.push_back(e);
	};
	virtual void RemoveItemFromInventory(Entity* e) 
	{
		if (std::find(heldItems.begin(), heldItems.end(), e) != heldItems.end()) {
			heldItems.erase(std::remove(heldItems.begin(), heldItems.end(), e), heldItems.end());
			heldItems.shrink_to_fit();
		}
	};
	std::vector<Entity*>* GetInventory() {
		return &heldItems;
	};

	virtual Entity_Container* clone() const { return new Entity_Container(*this); };

protected:
	std::vector<Entity*> heldItems;
};