#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include "WorldTile.h"
struct Designation {
public:
	enum Type {
		GeneralStoreage,
		FoodStorage,
		MaterialStorage,
		GeneralRoom,
	};

	Designation(std::vector<WorldTile*> newtiles, Type t) {
		tiles = newtiles;
		type = t;
	}
	std::vector<WorldTile*> tiles;
	Type type;
};