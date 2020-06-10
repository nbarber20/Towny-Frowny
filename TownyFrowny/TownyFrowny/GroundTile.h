#pragma once
#include <SFML/Graphics.hpp>
#include "FastNoise.h"
class World;
class GroundTile
{
public:
	GroundTile(wchar_t id, World& worldref, short x, short y);
	~GroundTile();
	void Draw(bool isWall, bool isFrontWall, bool isTopWall);
	void SetTile(int id);
	int GetTileID();
private:
	short x;
	short y;
	World* world;
	wchar_t tileID = 0;
};

