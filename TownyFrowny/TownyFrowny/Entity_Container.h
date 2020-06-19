#pragma once
#include "Entity.h"
#include "World.h"
class Entity_Container : public Entity {
public:
	Entity_Container(wchar_t EntityID, sf::Vector2i spritePos, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID,spritePos, TargetedBehaviors)
	{
	};
	void AddItemToInventory(Entity* e)
	{
		e->SetParent(this);
		heldItems.push_back(e);
		e->SetPosition(x, y);
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

	virtual bool MoveToTile(short dx, short dy) override {
		if (Entity::MoveToTile(dx, dy)==true) {
			for (int i = 0; i < heldItems.size(); i++) {
				heldItems[i]->SetPosition(dx, dy);
			}
			return true;
		}
		return false;
	};

	virtual bool StepTiles(short dx, short dy) override {
		if (canMove) {
			return MoveToTile(x + dx, y + dy);
		}
	}
	virtual void OnDespawn(World* newworld, bool doDropItems) override {
		Entity::OnDespawn(newworld, doDropItems);
		if (doDropItems == true) {
			while (heldItems.size() > 0) {
				DropItem(heldItems[0], newworld);
			}
		}
	}
	virtual void DropItem(Entity* item, World* newworld)
	{
		item->SetParent(nullptr);
		newworld->SpawnEntity(item, item->GetPosition());
	}
	virtual Entity_Container* clone() const { return new Entity_Container(*this); };

protected:
	std::vector<Entity*> heldItems;
};