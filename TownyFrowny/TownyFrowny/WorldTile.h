#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "GroundTile.h"
struct WorldTile {
	WorldTile(FastNoise* noiseGen, World& worldref, short x, short y) {
		groundTile = new GroundTile(noiseGen, worldref,x,y);
		wallTile = nullptr;
		this->x = x;
		this->y = y;
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
