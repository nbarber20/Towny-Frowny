#pragma once
#include "Entity_Container.h"
#include "Entity_Material.h"
#include <vector>
class Entity_Plant : public Entity_Container {
public:
	Entity_Plant(wchar_t EntityID, sf::Vector2i spritePos, wchar_t MatierialID, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors);
	void RemoveItemFromInventory(Entity* e) override; 
	void InitilizeSpawnInventory() override;
	virtual Entity_Plant* clone() const { return new Entity_Plant(*this); };

private:
	wchar_t MatierialID;
};