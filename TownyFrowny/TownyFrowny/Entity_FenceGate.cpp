#include "Entity_FenceGate.h"
#include "World.h"
#include "Entity_Fence.h"
void Entity_FenceGate::Rotate()
{
	rotation++;
	if (rotation >= 2)rotation = 0;
	if (horizontal) {
		SetSpriteVariant(spriteX, rotation + 1);
	}
	else {
		SetSpriteVariant(spriteX, rotation + 3);
	}
}

void Entity_FenceGate::OnSpawn(World* newworld)
{
	Entity::OnSpawn(newworld);
	std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());
	for (auto neighbor : neighbors) {
		if (neighbor != nullptr) {
			Entity* otherfence = world->GetEntityInTileByID(48, neighbor->GetPosition());
			if (otherfence != nullptr) {
				Entity_Fence* fence = dynamic_cast<Entity_Fence*>(otherfence);
				if (fence != NULL) {
					fence->UpdateFencePost();
				}
			}
		}
	}
	SetVolume(closedVolume);
}

void Entity_FenceGate::Tick()
{
	if (inworld) {
		std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());

		//blocked in
		if (neighbors[0] != nullptr&&neighbors[1] != nullptr && neighbors[2] != nullptr&&neighbors[3] != nullptr) {
			if (neighbors[2]->wallTile != nullptr&&neighbors[3]->wallTile != nullptr&&neighbors[0]->wallTile != nullptr&&neighbors[1]->wallTile != nullptr) {
				return;
			}
			else if (world->GetEntityInTileByID(48, neighbors[0]->GetPosition()) != nullptr &&
				world->GetEntityInTileByID(48, neighbors[1]->GetPosition()) != nullptr &&
				world->GetEntityInTileByID(48, neighbors[2]->GetPosition()) != nullptr &&
				world->GetEntityInTileByID(48, neighbors[3]->GetPosition()) != nullptr
				) {
				return;
			}
		}

		if (horizontal == true) {
			if (neighbors[2] != nullptr&&neighbors[3] != nullptr) {
				if (neighbors[2]->wallTile != nullptr&&neighbors[3]->wallTile != nullptr) {
					horizontal = false;
					Rotate();
				}
				else if (world->GetEntityInTileByID(48, neighbors[2]->GetPosition()) != nullptr && world->GetEntityInTileByID(48, neighbors[3]->GetPosition()) != nullptr) {
					horizontal = false;
					Rotate();
				}
			}
		}
		else {
			if (neighbors[0] != nullptr&&neighbors[1] != nullptr) {
				if (neighbors[0]->wallTile != nullptr&&neighbors[1]->wallTile != nullptr) {
					horizontal = true;
					Rotate();
				}
				else if (world->GetEntityInTileByID(48, neighbors[0]->GetPosition()) != nullptr && world->GetEntityInTileByID(48, neighbors[1]->GetPosition()) != nullptr) {
					horizontal = true;
					Rotate();
				}
			}
		}
	}
}
