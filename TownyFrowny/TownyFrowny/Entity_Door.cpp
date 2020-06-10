#include "Entity_Door.h"
#include "World.h"

void Entity_Door::Rotate()
{
	rotation++;
	if (rotation >= 2)rotation = 0;

	SetSpriteVariant(spriteX, rotation);
	std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());
	if (spriteY == 0) {
		if (neighbors[2] != nullptr) {
			if (neighbors[2]->wallTile == nullptr)
			{
				SetSpriteVariant(spriteX, 2);
			}
		}
	}
}

void Entity_Door::UseDoor()
{
	if (volume == closedVolume) {
		OpenDoor();
	}
	else {
		CloseDoor();
	}
}
void Entity_Door::CloseDoor() {
	if (volume == 0) {
		SetVolume(closedVolume);
		Rotate();
	}
}

void Entity_Door::OpenDoor() {
	if (volume == closedVolume) {
		SetVolume(0); 
		Rotate();
	}
}

void Entity_Door::OnSpawn(World* newworld)
{
	Entity::OnSpawn(newworld);
	std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());
	SetVolume(closedVolume);
}

void Entity_Door::Tick()
{
	if (inworld) {
		std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());
		if (spriteY == 0) {
			if (neighbors[2] != nullptr) {
				if (neighbors[2]->wallTile == nullptr)
				{
					SetSpriteVariant(spriteX, 2);
				}
			}
		}

		//blocked in
		if (neighbors[0] != nullptr&&neighbors[1] != nullptr && neighbors[2] != nullptr&&neighbors[3] != nullptr) {
			if (neighbors[2]->wallTile != nullptr&&neighbors[3]->wallTile != nullptr&&neighbors[0]->wallTile != nullptr&&neighbors[1]->wallTile != nullptr) {
				return;
			}
		}

		if (horizontal == true) {
			if (neighbors[2] != nullptr&&neighbors[3] != nullptr) {
				if (neighbors[2]->wallTile != nullptr&&neighbors[3]->wallTile != nullptr) {
					Rotate(); 
					horizontal = false;
				}
			}
		}
		else {
			if (neighbors[0] != nullptr&&neighbors[1] != nullptr) {
				if (neighbors[0]->wallTile != nullptr&&neighbors[1]->wallTile != nullptr) {
					Rotate();
					horizontal = false;
				}
			}
		}
	}
}
