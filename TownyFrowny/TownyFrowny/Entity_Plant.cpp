#include "Entity_Plant.h"
#include "EntityHandler.h"
Entity_Plant::Entity_Plant(wchar_t EntityID, sf::Vector2i spritePos, wchar_t MatierialID, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity_Container(EntityID, spritePos, worldref, TargetedBehaviors)
{
	this->MatierialID = MatierialID;
};
void Entity_Plant::RemoveItemFromInventory(Entity* e)
{
	Entity_Container::RemoveItemFromInventory(e);
	if (heldItems.size() == 0) {
		EntityHandler::Instance().DestroyEntity(this, world); //Destroy this plant
	}
}

void Entity_Plant::InitilizeSpawnInventory()
{
	AddItemToInventory(EntityHandler::Instance().CreateEntity(MatierialID));
}
