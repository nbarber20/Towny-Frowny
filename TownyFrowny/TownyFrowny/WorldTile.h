#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "GroundTile.h"
struct WorldTile {
	WorldTile(World& worldref, short x, short y, int groundId, int wallID) {
		groundTile = new GroundTile(groundId, worldref, x, y);
		if (wallID >= 0)wallTile = new GroundTile(wallID,worldref,x,y);
		else wallTile = nullptr;
		this->x = x;
		this->y = y;
		tileEntitiesCount = 0;
		contentsVolume = 0;
	};

	~WorldTile() {
		delete wallTile;
		delete groundTile;
	};
	std::vector<Entity*> tileEntities;
	short x;
	short y;
	GroundTile* wallTile;
	GroundTile* groundTile;
	int tileEntitiesCount;	
	int contentsVolume;
	sf::Vector2i GetPosition() {
		return sf::Vector2i(x, y);
	}



	//ForPathFInding
	int hcost;
	int gcost;
	int fcost() {
		return gcost + hcost;
	}
	WorldTile* pathingParent;
};
