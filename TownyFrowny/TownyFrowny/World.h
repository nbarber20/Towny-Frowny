#pragma once
#include "WorldTile.h"
#include "FastNoise.h"
#include "Designation.h"
#include <vector>

class World
{
public:
	std::vector <WorldTile*> tileToTick;
	World(FastNoise* noiseGen);
	~World();
	void GenerateTiles(FastNoise* noiseGen);
	void GenerateEntities(FastNoise* noiseGen);
	void Draw();
	void SpawnEntity(Entity* entity, sf::Vector2i tilePosition);
	void DespawnEntity(Entity* entity, sf::Vector2i tilePosition);
	void MoveEntity(Entity* entity, short fromx, short fromy, short tox, short toy);
	WorldTile* GetWorldTile(sf::Vector2i tilePos);
	std::vector<WorldTile*> GetNeighborsOfTile(sf::Vector2i tilePos);
	bool IsTileWalkable(sf::Vector2i tilePos, Entity* refEntity);
	bool IsTileWalkable(WorldTile* tile, Entity* refEntity);
	void SetGroundTile(sf::Vector2i tile, wchar_t id);
	void SetWallTile(sf::Vector2i tile, int id);
	int GetGroundTileIDAtPosition(sf::Vector2i tile);
	int GetWallTileIDAtPosition(sf::Vector2i tile);
	bool DoesTileContainEntity(sf::Vector2i tile, wchar_t entityId, bool deepSearch);
	bool DoesTileContainEntity(sf::Vector2i tile, Entity* entity);
	Entity* GetEntityInTileByID(wchar_t entityId, sf::Vector2i tile);
	int GetWorldSize();
	//Designations
	void NewDesignation(sf::Vector2i pos, sf::Vector2i size, Designation::Type t);
	void DeleteDesignation(sf::Vector2i pos);
	void DeleteDesignation(Designation* d);
	std::vector<Designation*> GetTileDesignations(sf::Vector2i pos);
	std::vector<Designation::Type> GetTileDesignationsTypes(sf::Vector2i pos);
	bool DoesTileContainDesignations(sf::Vector2i tilepos, Designation::Type t);
private:
	void DrawDesignation(WorldTile* tile, Designation::Type t);
	void AddEntity(WorldTile* tile, Entity* entity);
	void RemoveEntity(WorldTile* tile, Entity* entity);
	void DrawTile(WorldTile* tile);
	bool ContainsEntity(WorldTile* tile, Entity* entity);
	void SetGroundTileID(WorldTile* tile, wchar_t id);
	int GetGroundTileID(WorldTile* tile);
	void SetWallTileID(WorldTile* tile, int id);
	int GetWallTileID(WorldTile* tile);

	bool drawDesignations = true;

	std::vector <WorldTile*> worldTiles;
	std::vector <Designation*> designations;
	int tileCount;


	int tileVolume = 10;
};

