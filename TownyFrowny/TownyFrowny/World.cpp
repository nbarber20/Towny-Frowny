#include "World.h"
#include "vectorHelper.h"
#include "Camera.h"
#include "LogHandler.h"
#include "EntityHandler.h"
#include "FastNoise.h"
#include "InputHandler.h"

void World::GenerateOverworld(FastNoise* noiseGen)
{
	worldTiles.clear();
	for (int x = 0; x < GetWorldSize(); x++) {
		for (int y = 0; y < GetWorldSize(); y++) {

			int tileID = 0;
			float randCol = noiseGen->GetNoise(x, y) * 4;
			if (randCol <= -0.5) {
				tileID = 2;
			}
			else if (randCol <= 1) {
				tileID = 2;
			}
			else if (randCol <= 1.5) {
				tileID = 3;
			}
			else {
				tileID = 5;
			}
			WorldTile* tile = new WorldTile(*this, x, y, tileID,-1);
			worldTiles.push_back(tile);
			tileToTick.push_back(tile);
		}
	}
	delete noiseGen;
	SetAmbientLightLevel(200);
}

void World::GenerateCave(FastNoise* noiseGen)
{
	worldTiles.clear();
	for (int x = 0; x < GetWorldSize(); x++) {
		for (int y = 0; y < GetWorldSize(); y++) {
			int tileID = 0;
			float randCol = noiseGen->GetNoise(x, y) * 4;
			if (randCol <= -0.5) {
				tileID = 0;
			}
			else if (randCol <= 1.0) {
				tileID = 14;
			}
			else if (randCol <= 1.5) {
				tileID = 15;
			}
			else if (randCol <= 2.0) {
				tileID = 16;
			}
			else if (randCol <= 2.5) {
				tileID = 17;
			}
			else if (randCol <= 3) {
				tileID = 18;
			}
			WorldTile* tile = new WorldTile(*this, x, y,0, tileID);
			worldTiles.push_back(tile);
			tileToTick.push_back(tile);
		}
	}
	delete noiseGen;
	SetAmbientLightLevel(50);
}

void World::GenerateEntities()
{
	FastNoise* biomeMap = new FastNoise(time(NULL));
	biomeMap->SetNoiseType(FastNoise::Cellular);
	biomeMap->SetFrequency(0.07f);

	FastNoise* TreeDeletionNoise = new FastNoise(time(NULL));
	TreeDeletionNoise->SetNoiseType(FastNoise::WhiteNoise);

	std::vector<Entity*> treepos;

	for (int x = 0; x < GetWorldSize(); x++) {
		for (int y = 0; y < GetWorldSize(); y++) {
			if (GetWorldTile(sf::Vector2i(x, y))->groundTile->GetTileID() == 2) {
				bool dospawn = false;
				int EntityID = 0;
				float randCol = (biomeMap->GetNoise(x, y) + 1) * 8;
				if (randCol <= 0.5) {
					EntityID = 15;
					dospawn = true;
				}
				else if (randCol <= 1) {
					EntityID = 16;
					dospawn = true;
				}
				else if (randCol <= 1.5) {
					EntityID = 17;
					dospawn = true;
				}
				else if (randCol <= 2.0) {
					EntityID = 18;
					dospawn = true;
				}
				if (dospawn) {
					treepos.push_back(EntityHandler::Instance().CreateAndSpawnEntity(EntityID, this, sf::Vector2i(x, y)));
					tileToTick.push_back(GetWorldTile(sf::Vector2i(x, y)));
				}
			}
		}
	}
	
	for (int i = 0; i < treepos.size();i++) {
		std::vector<WorldTile*> neighbors = GetNeighborsOfTile(treepos[i]->GetPosition());
		for (auto neighbor : neighbors) {
			if (neighbor == nullptr)continue;
			if (DoesTileContainEntity(neighbor->GetPosition(), treepos[i]->GetID(), false) == false) {
				if (TreeDeletionNoise->GetNoise(treepos[i]->GetPosition().x, treepos[i]->GetPosition().y) < 0) {
					EntityHandler::Instance().DestroyEntity(treepos[i], this);
					break;
				}
			}
		}
	}

	delete biomeMap;
}

void World::Draw() {
	const int num = tileToTick.size();
	WorldTile** ptr = (num > 0) ? tileToTick.data() : nullptr;
	for (int i = 0; i < num; i++)
	{
		DrawTile(ptr[i]);	
	}
	tileToTick.clear();

	if (worldSelections.size()>0) {
		for (auto worldselection : worldSelections)
		{
			sf::RectangleShape rect = TileManager::Instance().GetSelectionRectangle();
			sf::Vector2f start;
			start.x = worldselection->SelectionStart.x;
			start.y = worldselection->SelectionStart.y;
			rect.setPosition(start*8.0f);
			sf::Vector2f size;
			size.x = worldselection->SelectionEnd.x - worldselection->SelectionStart.x;
			size.y = worldselection->SelectionEnd.y - worldselection->SelectionStart.y;
			rect.setSize((size + sf::Vector2f(1, 1))*8.0f);
			Camera::Instance().getHighLod()->draw(rect);
			for (int x = worldselection->SelectionStart.x; x <= worldselection->SelectionEnd.x; x++) {
				for (int y = worldselection->SelectionStart.y; y <= worldselection->SelectionEnd.y; y++) {
					Camera::Instance().getLowLod()->setPixel(x, GetWorldSize() - 1 - y, sf::Color(255, 255, 255, 255));
				}
			}
		}
	}
	
}

void World::NewLightSource(LightSource* ls)
{
	lights.push_back(ls);
	UpdateLighting();
}

void World::RemoveLightSource(LightSource* ls)
{
	if (std::find(lights.begin(), lights.end(), ls) != lights.end()) {
		lights.erase(std::remove(lights.begin(), lights.end(), ls), lights.end());
		UpdateLighting();
	}
}

void World::UpdateLighting()
{
	std::vector<sf::Vector2i> positions;
	Camera::Instance().getShading()->create(GetWorldSize(), GetWorldSize(), sf::Color(0, 0, 0, 255-ambientLightLevel));
	const int num = lights.size();
	LightSource** ptr = (num > 0) ? lights.data() : nullptr;
	for (int i = 0; i < num; i++)
	{
		if (std::find(positions.begin(), positions.end(), (ptr[i])->position) != positions.end()) {
			
			continue;
		}
		positions.push_back((ptr[i])->position);
		for (int xpos = (ptr[i])->position.x - (ptr[i])->lightDistance; xpos <= (ptr[i])->position.x + (ptr[i])->lightDistance; xpos++)
		{
			for (int ypos = (ptr[i])->position.y - (ptr[i])->lightDistance; ypos <= (ptr[i])->position.y + (ptr[i])->lightDistance; ypos++)
			{
				sf::Vector2i pos = sf::Vector2i(xpos, ypos);
				float dist = VectorHelper::GetMagnitude((ptr[i])->position - pos);
				WorldTile* tile = GetWorldTile(pos);
				int level = VectorHelper::Lerpf(255, ambientLightLevel, VectorHelper::Clampf(dist / (ptr[i])->lightDistance, 0, 1));
				sf::Color c = Camera::Instance().getShading()->getPixel(xpos, GetWorldSize() - 1 - ypos);
				int color = (255-c.a) + ((level - ambientLightLevel));
				Camera::Instance().getShading()->setPixel(xpos, GetWorldSize()-1-ypos, sf::Color(0, 0, 0,255- VectorHelper::Clampf(color,0,255)));

			}
		}
	}
	positions.clear();
}

void World::SetAmbientLightLevel(int to)
{
	ambientLightLevel = to;
	UpdateLighting();
}

void World::ReDraw()
{
	const int num = worldTiles.size();
	WorldTile** ptr = (num > 0) ? worldTiles.data() : nullptr;
	for (int i = 0; i < num; i++)
	{
		DrawTile(ptr[i]);
	}
	UpdateLighting();
}

void World::SelectArea(std::pair<sf::Vector2i, sf::Vector2i> newSelection)
{
	worldSelections.push_back(new worldSelection(newSelection.first, newSelection.second));
}
void World::ClearSelections()
{
	for (auto selection : worldSelections) {
		for (int x = selection->SelectionStart.x; x <= selection->SelectionEnd.x; x++) {
			for (int y = selection->SelectionStart.y; y <= selection->SelectionEnd.y; y++) {
				tileToTick.push_back(GetWorldTile(sf::Vector2i(x, y)));
			}
		}
	}
	worldSelections.clear();
}

void World::SpawnEntity(Entity* entity, sf::Vector2i tilePosition)
{
	tilePosition = VectorHelper::ClampVector(tilePosition, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	AddEntity(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y],entity);
	tileToTick.push_back(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y]);
	entity->OnSpawn(this);
}

void World::DespawnEntity(Entity* entity, sf::Vector2i tilePosition)
{
	tilePosition = VectorHelper::ClampVector(tilePosition, sf::Vector2i(0, 0), sf::Vector2i(GetWorldSize() - 1, GetWorldSize() - 1));
	if (DoesTileContainEntity(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y], entity)) {
		RemoveEntity(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y], entity);
		tileToTick.push_back(worldTiles[(GetWorldSize()*tilePosition.x) + tilePosition.y]);
		entity->OnDespawn(this);
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
	else neighbours.push_back(nullptr);
	if (tilePos.x - 1 >=0) neighbours.push_back(worldTiles[(GetWorldSize()*(tilePos.x - 1)) + tilePos.y]);
	else neighbours.push_back(nullptr);
	if (tilePos.y + 1 < GetWorldSize()) neighbours.push_back(worldTiles[(GetWorldSize()*(tilePos.x)) + (tilePos.y + 1)]);
	else neighbours.push_back(nullptr);
	if (tilePos.y - 1 >= 0) neighbours.push_back(worldTiles[(GetWorldSize()*(tilePos.x)) + (tilePos.y - 1)]);
	else neighbours.push_back(nullptr);
	return neighbours;
}

bool World::IsTileWalkable(sf::Vector2i tilePos, Entity* refEntity)
{
	if (DoesTileContainEntity(tilePos, 47, false) == true && dynamic_cast<Entity_Human*>(refEntity)!=NULL) { //this is a door and we are human
		return true;
	}
	if (DoesTileContainEntity(tilePos, 49, false) == true && dynamic_cast<Entity_Human*>(refEntity) != NULL) { //this is a door and we are human
		return true;
	}
	WorldTile* tile = GetWorldTile(tilePos);
	int newTileSize = GetWorldTile(tilePos)->contentsVolume + refEntity->GetVolume();
	return newTileSize <= tileVolume && tile->wallTile ==nullptr;
}

bool World::IsTileWalkable(WorldTile* tile, Entity* refEntity)
{
	if (DoesTileContainEntity(tile->GetPosition(),47,false) == true && dynamic_cast<Entity_Human*>(refEntity) != NULL) { //this is a door and we are human
		return true;
	}
	if (DoesTileContainEntity(tile->GetPosition(), 49, false) == true && dynamic_cast<Entity_Human*>(refEntity) != NULL) { //this is a door and we are human
		return true;
	}
	int newTileSize = tile->contentsVolume + refEntity->GetVolume();
	return tile->contentsVolume + refEntity->GetVolume() <=tileVolume && tile->wallTile == nullptr;
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
	tile->contentsVolume += entity->GetVolume();
}

void World::RemoveEntity(WorldTile* tile, Entity* entity)
{
	tile->tileEntities.erase(std::remove(tile->tileEntities.begin(), tile->tileEntities.end(), entity), tile->tileEntities.end());
	tile->tileEntities.shrink_to_fit();
	tile->tileEntitiesCount = tile->tileEntities.size();
	tile->contentsVolume -= entity->GetVolume();
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
		if(neighbors[i]==nullptr)continue;
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