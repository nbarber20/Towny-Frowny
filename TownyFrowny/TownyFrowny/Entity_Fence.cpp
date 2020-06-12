#pragma once
#include "Entity_Fence.h"
#include "World.h"
#include "Entity_FenceGate.h"

void Entity_Fence::UpdateFencePost()
{
	std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());
	bool north = false, south = false, east = false, west = false;
	for (int i = 0; i < neighbors.size();i++) {
		if (neighbors[i] != nullptr) {
			Entity* otherfence = world->GetEntityInTileByID(EntityID, neighbors[i]->GetPosition());
			if (otherfence != nullptr) {
				Entity_Fence* fence = dynamic_cast<Entity_Fence*>(otherfence);
				if (fence != NULL) {
					if (i == 0)east = true;
					else if (i == 1)west = true;
					else if (i == 2)south = true;
					else if (i == 3)north = true;
				}
			}
			Entity* otherGate = world->GetEntityInTileByID(49, neighbors[i]->GetPosition());
			if (otherGate != nullptr) {
				Entity_FenceGate* gate = dynamic_cast<Entity_FenceGate*>(otherGate);
				if (gate != NULL) {
					if (i == 0)east = true;
					else if (i == 1)west = true;
					else if (i == 2)south = true;
					else if (i == 3)north = true;
				}
			}
			if (neighbors[i]->wallTile != nullptr) {
				if (i == 0)east = true;
				else if (i == 1)west = true;
				else if (i == 2)south = true;
				else if (i == 3)north = true;
			}			
		}
	}

	if (west)  SetSpriteVariant(20, 0);
	if (north) SetSpriteVariant(21, 0);
	if (east)  SetSpriteVariant(22, 0);
	if (south)  SetSpriteVariant(20, 1);
	if (north&&west) SetSpriteVariant(21, 1);
	if (north&&east)  SetSpriteVariant(22, 1);
	if (south&&east)  SetSpriteVariant(20, 2);
	if (south&&west)  SetSpriteVariant(21, 2);
	if (south&&north)  SetSpriteVariant(21, 4);
	if (west&&east)  SetSpriteVariant(22, 4);
	if (north&&west&&east)  SetSpriteVariant(22, 2);
	if (south&&west&&east)  SetSpriteVariant(20, 3);
	if (north&&south&&west)  SetSpriteVariant(21, 3);
	if (north&&south&&east)  SetSpriteVariant(22, 3);
	if (north&&south&&east&&west)  SetSpriteVariant(20, 4);
	if (!north&&!south&&!east&&!west)  SetSpriteVariant(23, 0);
}

void Entity_Fence::OnSpawn(World* newworld)
{
	Entity::OnSpawn(newworld);
	UpdateFencePost();

	std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());
	for (auto neighbor : neighbors) {
		if (neighbor != nullptr) {
			Entity* otherfence = world->GetEntityInTileByID(EntityID, neighbor->GetPosition());
			if(otherfence!= nullptr){
				Entity_Fence* fence = dynamic_cast<Entity_Fence*>(otherfence);
				if (fence != NULL) {
					fence->UpdateFencePost();
				}
			}
		}
	}
}

void Entity_Fence::OnDespawn(World* newworld)
{
	Entity::OnDespawn(newworld);
	std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(GetPosition());
	for (auto neighbor : neighbors) {
		if (neighbor != nullptr) {
			Entity* otherfence = world->GetEntityInTileByID(EntityID, neighbor->GetPosition());
			if (otherfence != nullptr) {
				Entity_Fence* fence = dynamic_cast<Entity_Fence*>(otherfence);
				if (fence != NULL) {
					fence->UpdateFencePost();
				}
			}
		}
	}
}
