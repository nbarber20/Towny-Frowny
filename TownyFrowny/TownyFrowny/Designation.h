#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include "WorldTile.h"
class World;
struct Designation {
public:
	enum Type {
		GeneralStoreage,
		FoodStorage,
		MaterialStorage,
		GeneralRoom,
	};

	Designation(std::vector<WorldTile*> newtiles, Type t , World* w) {
		tiles = newtiles;
		type = t;
		world = w;
	}
	std::vector<WorldTile*> tiles;
	std::vector<Entity*> HeldEntities;
	Type type;
	World* world;


	bool GetEmptySlot(sf::Vector2i& SlotPos);

	bool GetItemSlot(int ID, sf::Vector2i& SlotPos);
};