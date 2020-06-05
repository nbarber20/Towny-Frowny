#include "World.h"
#include "vectorHelper.h"
#include "Camera.h"
#include "LogHandler.h"
#include "EntityHandler.h"
World::World(FastNoise* noiseGen)
{
	GenerateTiles(noiseGen);
}


World::~World()
{
}


void World::GenerateTiles(FastNoise* noiseGen)
{
	for (int i = 0; i < GetWorldSize(); i++) {
		for (int j = 0; j < GetWorldSize(); j++) {
			WorldTile* tile = new WorldTile(noiseGen,*this,i, j);
			worldTiles.push_back(tile);
			tileToTick.push_back(tile);
		}
	}
	delete noiseGen;
}

void World::GenerateEntities(FastNoise* noiseGen)
{
}

void World::Draw() {
	const int num = tileToTick.size();
	WorldTile** ptr = (num > 0) ? tileToTick.data() : nullptr;
	for (int i = 0; i < num; i++)
	{
		DrawTile(ptr[i]);	
	}
	tileToTick.clear();
}

void World::SpawnEntity(Entity* entity, sf::Vector2i tilePosition)
{
	tilePosition = VectorHelper::ClampVector(tilePosition, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	AddEntity(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y],entity);
	entity->OnSpawn();
	tileToTick.push_back(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y]);
}

void World::DespawnEntity(Entity* entity, sf::Vector2i tilePosition)
{
	tilePosition = VectorHelper::ClampVector(tilePosition, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	if (DoesTileContainEntity(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y], entity)) {
		RemoveEntity(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y], entity);
		tileToTick.push_back(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y]);
		return;
	}
	LogHandler::Instance().WriteLog( "Failed to despawn "+entity->GetObjectName(), tilePosition, logContext::ERROR);
}

void World::MoveEntity(Entity* entity, short fromx, short fromy, short tox, short toy)
{
	sf::Vector2i from = sf::Vector2i(fromx, fromy);
	sf::Vector2i to = sf::Vector2i(tox, toy);
	from = VectorHelper::ClampVector(from, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize()-1));
	to = VectorHelper::ClampVector(to, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	if (DoesTileContainEntity(worldTiles[(GetWorldSize()*from.x) + from.y],entity)) {
		RemoveEntity(worldTiles[(GetWorldSize()*from.x) + from.y],entity);
		AddEntity(worldTiles[(GetWorldSize()*to.x) + to.y],entity);
		tileToTick.push_back(worldTiles[(GetWorldSize()*from.x) + from.y]);
		tileToTick.push_back(worldTiles[(GetWorldSize()*to.x) + to.y]);
	}
}

void World::SetGroundTile(sf::Vector2i tile, wchar_t id)
{
	SetGroundTileID(worldTiles[(GetWorldSize()*tile.x) + tile.y],id);
	tileToTick.push_back(worldTiles[(GetWorldSize()*tile.x) + tile.y]);
}

void World::SetWallTile(sf::Vector2i tile, int id)
{
	SetWallTileID(worldTiles[(GetWorldSize()*tile.x) + tile.y], id);
	tileToTick.push_back(worldTiles[(GetWorldSize()*tile.x) + tile.y]);
}

int World::GetGroundTileIDAtPosition(sf::Vector2i tile)
{
	tile = VectorHelper::ClampVector(tile, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	return GetGroundTileID(worldTiles[(GetWorldSize()*tile.x) + tile.y]);
}

int World::GetWallTileIDAtPosition(sf::Vector2i tile)
{
	tile = VectorHelper::ClampVector(tile, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	return GetWallTileID(worldTiles[(GetWorldSize()*tile.x) + tile.y]);
}

bool World::DoesTileContainEntity(WorldTile* tile, Entity* entity)
{
	if (std::find(tile->tileEntities.begin(), tile->tileEntities.end(), entity) != tile->tileEntities.end()) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

bool World::DoesTileContainEntity(sf::Vector2i position, Entity* entity)
{
	WorldTile* tile = GetWorldTile(position);
	if (std::find(tile->tileEntities.begin(), tile->tileEntities.end(), entity) != tile->tileEntities.end()) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

bool World::DoesTileContainEntity(sf::Vector2i tile, wchar_t entityId, bool deepSearch)
{
	tile = VectorHelper::ClampVector(tile, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	if (deepSearch) {		
		const int num = worldTiles[(GetWorldSize()*tile.x) + tile.y]->tileEntitiesCount;
		Entity** ptr = (num > 0) ? worldTiles[(GetWorldSize()*tile.x) + tile.y]->tileEntities.data() : nullptr;
		for (int i = 0; i < num; i++)
		{
			wchar_t id = (*ptr[i]).GetID();
			if (id == entityId) return true;
		}

		return false;
	}
	else {
		WorldTile* t = worldTiles[(GetWorldSize()*tile.x) + tile.y];
		if (t->tileEntitiesCount > 0) {
			if (entityId == t->tileEntities[t->tileEntitiesCount - 1]->GetID()) {
				return true;
			}
		}		
		return false;
	}
}

Entity* World::GetEntityInTileByID(wchar_t entityId, sf::Vector2i tile)
{
	tile = VectorHelper::ClampVector(tile, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	const int num = worldTiles[(GetWorldSize()*tile.x) + tile.y]->tileEntitiesCount;
	Entity** ptr = (num > 0) ? worldTiles[(GetWorldSize()*tile.x) + tile.y]->tileEntities.data() : nullptr;
	for (int i = 0; i < num; i++)
	{
		wchar_t id = (*ptr[i]).GetID();
		if (id == entityId) return (ptr[i]);
	}
	return nullptr;
	//TODO: throw an error??
}

int World::GetWorldSize()
{
	return Camera::Instance().getWorldSize();
}


//Designation Functions//
void World::NewDesignation(sf::Vector2i pos, sf::Vector2i size, Designation::Type t)
{
	std::vector<WorldTile*> tiles;
	for (int x = pos.x; x < pos.x + size.x; x++) {
		for (int y = pos.y; y < pos.y + size.y; y++) {
			WorldTile* t = GetWorldTile(sf::Vector2i(x, y));
			tiles.push_back(t);
			tileToTick.push_back(t);
		}
	}
	Designation* des = new Designation(tiles, t,this);
	designations.push_back(des);
}

void World::DeleteDesignation(sf::Vector2i pos)
{
	for (auto a : GetTileDesignations(pos))
	{
		DeleteDesignation(a);
	}
}

void World::DeleteDesignation(Designation* d)
{
	if (std::find(designations.begin(), designations.end(), d) != designations.end()) {
		designations.erase(std::remove(designations.begin(), designations.end(), d), designations.end());
		delete d;
		designations.shrink_to_fit();
	}
}

std::vector<Designation*> World::GetTileDesignations(sf::Vector2i pos)
{
	std::vector<Designation*> d;
	for (auto a : designations){
		for (auto t : a->tiles) {
			if (t->GetPosition() == pos) {
				d.push_back(a);
				continue;
			}
		}
	}
	return d;
}

std::vector<Designation::Type> World::GetTileDesignationsTypes(sf::Vector2i pos)
{
	std::vector<Designation::Type> types;
	for (auto a : GetTileDesignations(pos)) {
		types.push_back(a->type);
	}
	return types;
}

void World::DrawDesignation(WorldTile* tile, Designation::Type t)
{
	sf::Vector2f worldf;
	worldf.x = tile->GetPosition().x;
	worldf.y = tile->GetPosition().y;
	sf::RectangleShape* s = Camera::Instance().getDesignationTile(t);
	s->setPosition(worldf*8.0f);
	Camera::Instance().getHighLod()->draw(*s);
	sf::Color c = Camera::Instance().getDesignationTileColor(t);
	c.a = 255;
	Camera::Instance().getLowLod()->setPixel(worldf.x, GetWorldSize() - 1 - worldf.y, c);
}

bool World::DoesTileContainDesignations(sf::Vector2i tilepos, Designation::Type t)
{
	tilepos = VectorHelper::ClampVector(tilepos, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	for (int i = 0; i < designations.size(); i++)
	{		
		if (designations[i]->type == t) {
			for (int j = 0; j < designations[i]->tiles.size(); j++)
			{
				if (designations[i]->tiles[j]->GetPosition() == tilepos) {
					return true;
				}
			}
		}
	}

	return false;
}

//worldTileFunctions//

WorldTile* World::GetWorldTile(sf::Vector2i tilePos)
{
	return worldTiles[(GetWorldSize()*tilePos.x) + tilePos.y];
}

std::vector<WorldTile*> World::GetNeighborsOfTile(sf::Vector2i tilePos)
{
	std::vector<WorldTile*> neighbours;
	if(tilePos.x+1 < GetWorldSize()) neighbours.push_back(worldTiles[(GetWorldSize()*(tilePos.x + 1)) + tilePos.y]);
	if (tilePos.x - 1 >=0) neighbours.push_back(worldTiles[(GetWorldSize()*(tilePos.x - 1)) + tilePos.y]);
	if (tilePos.y + 1 < GetWorldSize()) neighbours.push_back(worldTiles[(GetWorldSize()*(tilePos.x)) + (tilePos.y+1)]);
	if (tilePos.y- 1 >= 0) neighbours.push_back(worldTiles[(GetWorldSize()*(tilePos.x)) + (tilePos.y-1)]);
	return neighbours;
}

bool World::IsTileWalkable(sf::Vector2i tilePos, Entity* refEntity)
{
	return worldTiles[(GetWorldSize()*tilePos.x) + tilePos.y]->contentsVolume+ refEntity->volume <= tileVolume && worldTiles[(GetWorldSize()*tilePos.x) + tilePos.y]->wallTile ==nullptr;
}

bool World::IsTileWalkable(WorldTile* tile, Entity* refEntity)
{
	return tile->contentsVolume + refEntity->volume <=tileVolume && tile->wallTile == nullptr;
}

void World::DrawTile(WorldTile* tile)
{
	if (tile->wallTile != nullptr) {
		sf::Vector2i checkBotPos = tile->GetPosition() + sf::Vector2i(0, 1);
		sf::Vector2i checkTopPos = tile->GetPosition() + sf::Vector2i(0, -1);
		bool isTop = false;
		if(checkTopPos.y>=0) isTop = GetWorldTile(checkTopPos)->wallTile == nullptr;
		bool isBottom = false;
		if (checkBotPos.y < GetWorldSize()) isBottom = GetWorldTile(checkBotPos)->wallTile == nullptr;
		tile->wallTile->Draw(true, isBottom, isTop);

	}
	else {
		tile->groundTile->Draw(false, false, false);


		if (drawDesignations) {
			for (auto desig : designations) {
				for (auto t : desig->tiles) {
					if (t->GetPosition() == tile->GetPosition()) {
						DrawDesignation(tile, desig->type);
					}
				}
			}
		}

		const int c = tile->tileEntitiesCount;
		for (int i = 0; i < c; i++){
			tile->tileEntities[i]->DrawEntity();
		}
	}
}

void World::AddEntity(WorldTile* tile, Entity* entity)
{
	entity->SetPosition(tile->x, tile->y);
	tile->tileEntities.push_back(entity);
	tile->tileEntitiesCount = tile->tileEntities.size();
	tile->contentsVolume += entity->volume;
}

void World::RemoveEntity(WorldTile* tile, Entity* entity)
{
	tile->tileEntities.erase(std::remove(tile->tileEntities.begin(), tile->tileEntities.end(), entity), tile->tileEntities.end());
	tile->tileEntities.shrink_to_fit();
	tile->tileEntitiesCount = tile->tileEntities.size();
	tile->contentsVolume -= entity->volume;
}

void World::SetGroundTileID(WorldTile* tile, wchar_t id)
{
	tile->groundTile->SetTile(id);
	tileToTick.push_back(tile);
}

int World::GetGroundTileID(WorldTile* tile)
{
	return tile->groundTile->GetTileID();
}

void World::SetWallTileID(WorldTile* tile, int id)
{
	if (id == -1) {
		if (tile->wallTile != nullptr) {
			delete tile->wallTile;
			tile->wallTile = nullptr;
		}
	}
	else {
		if (tile->wallTile == nullptr)tile->wallTile = new GroundTile(id,*this, tile->x, tile->y);
		tile->wallTile->SetTile(id);
	}

	std::vector<WorldTile*> neighbors = GetNeighborsOfTile(tile->GetPosition());
	for (int i = 0; i < neighbors.size();i++) {
		tileToTick.push_back(neighbors[i]);
	}
	tileToTick.push_back(tile);
}

int World::GetWallTileID(WorldTile* tile)
{
	if (tile->wallTile != nullptr) {
		return tile->wallTile->GetTileID();
	}
	else {
		return -1;
	}
}