#include "Entity_Plant.h"
#include "EntityHandler.h"
Entity_Plant::Entity_Plant(wchar_t EntityID, sf::Vector2i spritePos, std::vector<std::pair<wchar_t, int>> MatierialIDs, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity_Container(EntityID, spritePos, worldref, TargetedBehaviors)
{
	this->MatierialIDs = MatierialIDs;
}
void Entity_Plant::RemoveItemFromInventory(Entity* e)
{
	Entity_Container::RemoveItemFromInventory(e);
	if (heldItems.size() == 0) {
		EntityHandler::Instance().DestroyEntity(this, world); //Destroy this plant
	}
}

void Entity_Plant::Initilize()
{
	for (int i = 0; i < MatierialIDs.size(); i++) {
		for (int j = 0; j < MatierialIDs[i].second; j++) {
			AddItemToInventory(EntityHandler::Instance().CreateEntity(MatierialIDs[i].first));
		}
	}
}
