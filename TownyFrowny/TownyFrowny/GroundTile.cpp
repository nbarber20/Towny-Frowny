#include "GroundTile.h"
#include "World.h"
#include "Camera.h"
#include "TileManager.h"
#include "EntityHandler.h"

GroundTile::GroundTile(wchar_t id, World & worldref, short x, short y)
{
	world = &worldref;
	this->tileID = id;
	this->x = x;
	this->y = y;
}

GroundTile::~GroundTile()
{
}

void GroundTile::Draw(bool isWall, bool isFrontWall, bool isTopWall)
{
	sf::Sprite* sprite = TileManager::Instance().GetWorldTileByID(tileID,isWall,isFrontWall,isTopWall)->sprite;
	sf::Vector2f worldf;
	worldf.x = x;
	worldf.y = y;
	sprite->setPosition(worldf*8.0f);
	Camera::Instance().getHighLod()->draw(*sprite);
	Camera::Instance().getLowLod()->setPixel(x, world->GetWorldSize()- 1 -y, *TileManager::Instance().GetWorldTileByID(tileID,false,false,false)->tileColor);
}
void GroundTile::SetTile(int id) 
{
	tileID = id;
}

int GroundTile::GetTileID()
{
	return tileID;
}
