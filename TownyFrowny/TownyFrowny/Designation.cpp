#include "Designation.h"
#include "World.h"
bool Designation::GetEmptySlot(sf::Vector2i& SlotPos)
{
	sf::Vector2i beginTile = tiles[0]->GetPosition();
	sf::Vector2i endTile = tiles[tiles.size() - 1]->GetPosition();
	for (int x = beginTile.x; x <= endTile.x; x++) {
		for (int y = beginTile.y; y <= endTile.y; y++) {
			int count = world->GetWorldTile(sf::Vector2i(x, y))->tileEntitiesCount;
			if (count == 0) {
				SlotPos = sf::Vector2i(x, y);
				return true;
			}
		}
	}
	return false;
}

bool Designation::GetItemSlot(int ID, sf::Vector2i& SlotPos)
{
	for (int i = 0; i < HeldEntities.size(); i++) {
		if (HeldEntities[i]->GetID() == ID) {
			if (HeldEntities[i]->GetParent() == nullptr&&HeldEntities[i]->isInWorld() == true) {
				sf::Vector2i pos = HeldEntities[i]->GetPosition();
				sf::Vector2i beginTile = tiles[0]->GetPosition();
				if (pos.x >= beginTile.x && pos.y >= beginTile.y)
				{
					sf::Vector2i endTile = tiles[tiles.size() - 1]->GetPosition();
					if (pos.x <= endTile.x && pos.y <= endTile.y)
					{
						SlotPos = pos;
						return true;
					}
				}
			}
		}
	}
	return false;
}
