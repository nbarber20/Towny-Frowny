#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include "WorldTile.h"
class World;
class Entity_Human;
struct Designation {
public:
	enum Type {
		GeneralStoreage,
		FoodStorage,
		MaterialStorage,
		GeneralRoom,
	};

	Designation(std::vector<WorldTile*> newtiles, Type t , World* w, Entity_Human* Owner) {
		tiles = newtiles;
		type = t;
		world = w;
		this->Owner = Owner;
	}
	std::vector<WorldTile*> tiles;
	std::vector<Entity*> HeldEntities;
	Type type;
	World* world;
	Entity_Human* Owner;

	bool GetEmptySlot(sf::Vector2i& SlotPos);

	bool GetItemSlot(int ID, sf::Vector2i& SlotPos);
};