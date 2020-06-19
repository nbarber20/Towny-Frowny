#include "Entity.h"
#include "World.h"
#include "Camera.h"
#include "TileManager.h"
#include "LogHandler.h"
#include "EntityHandler.h"
#include "TaskManager.h"
#include "Entity_Living.h"
#include "Designation.h"
Entity::Entity(wchar_t EntityID, sf::Vector2i spritePos, std::vector<TargetedHumanBehaviors> TargetedBehaviors)
{
	this->spriteX= spritePos.x;
	this->spriteY = spritePos.y;
	this->EntityID = EntityID;
	this->age = 0;
	this->parent = nullptr;
	this->TargetedBehaviors = TargetedBehaviors;
	this->volume = 0;
}


void Entity::OnSpawn(World* newworld)
{
	this->parent = nullptr;
	inworld = true;
	world = newworld;
	world->tileToTick.push_back(world->GetWorldTile(this->GetPosition()));
}


void Entity::OnDespawn(World* newworld,bool doDropItems)
{
	inworld = false;
	world->tileToTick.push_back(world->GetWorldTile(this->GetPosition()));
}

void Entity::RemoveFromWorld()
{
	world->DespawnEntity(this,sf::Vector2i(x,y),false);
	OnDespawn(world,false);
}

void Entity::Tick()
{
	if(age<32768)age++;
}

void Entity::SetPosition(short x, short y)
{
	this->x = x;
	this->y = y;
}

sf::Vector2i Entity::GetPosition()
{
	sf::Vector2i p = sf::Vector2i(x, y);
	return p;
}

void Entity::DrawEntity()
{
	if (inworld == false)return;
	sf::Sprite* sprite = TileManager::Instance().GetEntityTileByID(spriteX, spriteY)->sprite;
	sf::Vector2f worldf;
	worldf.x = x;
	worldf.y = y;
	sprite->setPosition(worldf*8.0f);
	Camera::Instance().getHighLod()->draw(*sprite);
	Camera::Instance().getLowLod()->setPixel(x, world->GetWorldSize() - 1 - y, *TileManager::Instance().GetEntityTileByID(spriteX,spriteY)->tileColor);
	if (selected == true) {
		sf::Sprite* selectionsprite = TileManager::Instance().GetSelectionTile()->sprite;
		selectionsprite->setPosition(worldf*8.0f);
		Camera::Instance().getHighLod()->draw(*selectionsprite);
		float colorAlpha = TileManager::Instance().GetSelectionTile()->tileColor->a;
		sf::Color c = *TileManager::Instance().GetEntityTileByID(spriteX, spriteY)->tileColor + sf::Color(colorAlpha, colorAlpha, colorAlpha,255);
		Camera::Instance().getLowLod()->setPixel(x, world->GetWorldSize() - 1 - y, c);
	}
}

bool Entity::MoveToTile(short dx, short dy)
{	
	if (dx > world->GetWorldSize() - 1 || dx < 0 || dy<0 || dy>world->GetWorldSize() - 1)return false; //Out of bounds
	if (world->IsTileWalkable(sf::Vector2i(dx, dy),this) == false)return false; //Cannot walk here
	world->MoveEntity(this, x, y, dx, dy);	
}

bool Entity::StepTiles(short dx, short dy)
{
	if (canMove) {
		return MoveToTile(x + dx, y + dy);
	}
}

wchar_t Entity::GetID()
{
	return this->EntityID;
}

bool Entity::isInWorld()
{
	return inworld;
}

void Entity::SetObjectName(std::string to)
{
	this->objectName = to;
}

std::string Entity::GetObjectName()
{
	return this->objectName;
}

Entity_Container* Entity::GetParent()
{
	return this->parent;
}

void Entity::SetParent(Entity_Container* e)
{
	if (inworld&&e != nullptr)RemoveFromWorld();
	if (parent != nullptr) parent->RemoveItemFromInventory(this); //we are in an inv, so remove from that first
	if (designationParent != nullptr) designationParent = nullptr; //we are in a stock designation so remove ourselves from that list
	this->parent = e;
}

Designation* Entity::GetDesignationParent()
{
	return designationParent;
}

void Entity::SetDesignationParent(Designation* d)
{
	if(designationParent != nullptr) designationParent->HeldEntities.erase(std::remove(designationParent->HeldEntities.begin(), designationParent->HeldEntities.end(), this), designationParent->HeldEntities.end());
	if (d != nullptr) d->HeldEntities.push_back(this);
	designationParent = d;
}

void Entity::SetMovementCapability(bool to)
{
	canMove = to;
}

bool Entity::GetMovementCapability()
{
	return canMove;
}

void Entity::SetSpriteVariant(wchar_t x, wchar_t y)
{
	spriteX = x;
	int variants = TileManager::Instance().GetNumEntityVariants(x);
	if (y > variants) y = variants;
	spriteY = y;
	if (inworld) {
		world->tileToTick.push_back(world->GetWorldTile(GetPosition()));
	}
}

int Entity::GetVolume()
{
	return volume;
}

void Entity::SetVolume(int to)
{
	if (inworld)world->GetWorldTile(GetPosition())->contentsVolume -= volume;
	volume = to;
	if (inworld)world->GetWorldTile(GetPosition())->contentsVolume += volume;	
}
