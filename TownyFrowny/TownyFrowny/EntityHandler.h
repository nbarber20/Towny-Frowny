#pragma once
#include <SFML/System.hpp>
#include "Entity.h"
#include "World.h"
#include "Entity_Living.h"
#include "Entity_Food.h"
#include "Entity_Animal.h"
#include "LogHandler.h"
#include "Entity_Human.h"
#include "Entity_Pet.h"
#include "Entity_Material.h"
#include "Entity_Plant.h"
#include "Entity_Manufactured.h"
#include "PlayerController.h"
#include "Entity_Crafter.h"
#include "Entity_Door.h"
#include "Entity_Fence.h"
#include "Entity_FenceGate.h"
#include "Entity_Light.h"
class TaskManager;

class EntityHandler
{
public:
	static EntityHandler& Instance()
	{
		static EntityHandler INSTANCE;
		return INSTANCE;
	}
	EntityHandler() {		
	};


	Entity* CreateEntity(int id) {
		if (id >= entityList.size())id =1;

		Entity* newEntity = (entityList[id].second)->clone();
		newEntity->SetObjectName(GetEntityNameByID(id));
		newEntity->Initilize();
		Entity_Living* check = dynamic_cast<Entity_Living*>(newEntity);
		if (check != NULL) { // this entity is alive
			if (livingEntitiesCount < maxLivingEntities) {
				livingEntitiesCount++;
			}
			else {
				LogHandler::Instance().WriteLog("Tried to spawn an living entity past world cap", logContext::ERROR);
				return nullptr;
			}
		}
		Entity_Human* hcheck = dynamic_cast<Entity_Human*>(newEntity);
		if (hcheck != NULL) { // this entity is alive
			if (humanEntitiesCount < maxHumans) {
				humanEntitiesCount++;
			}
			else {
				LogHandler::Instance().WriteLog("Tried to spawn an humans past world cap", logContext::ERROR);
				return nullptr;
			}

			if(playerController->GetPilotingPlayer() == nullptr){
				playerController->SetPilotingPlayer(hcheck);
			}

			playableHumans.push_back(hcheck);
			return(newEntity);
		}
		entities.push_back(newEntity);
		return (newEntity);
	}
	Entity* CreateAndSpawnEntity(int id, World* w, sf::Vector2i pos) {
		Entity* e = CreateEntity(id);
		if (e != nullptr)w->SpawnEntity(e, pos);
		return e;
	}
	void DestroyEntity(Entity * e, World* w) {
		e->SetParent(nullptr);
		if(e->isInWorld())w->DespawnEntity(e, e->GetPosition()); //remove from world
		entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
		delete e; 
	}
	
	int GetNumEntities() {
		return entities.size();
	}
	void Init(TaskManager* taskManager, PlayerController* playerController, World* world) {
		this->taskManager = taskManager;
		this->world = world;
		this->playerController = playerController;
		entityList = {
			std::make_pair("Human", new Entity_Human(0,sf::Vector2i(0,0),playerController, taskManager, world, {})),

			std::make_pair("Cow",new Entity_Animal(1,sf::Vector2i(1,0), 8,6, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Cat",new Entity_Pet(2,sf::Vector2i(2,0),7, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Dog",new Entity_Pet(3,sf::Vector2i(3,0),8, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Chicken",new Entity_Animal(4,sf::Vector2i(4,0), 3, 9, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Pig",new Entity_Animal(5,sf::Vector2i(5,0), 5,  10, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),

			std::make_pair("Meat",new Entity_Food(6,sf::Vector2i(6,0), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Cat Meat",new Entity_Food(7,sf::Vector2i(6,1), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem })),
			std::make_pair("Dog Meat",new Entity_Food(8,sf::Vector2i(6,1), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem })),
			std::make_pair("Poultry",new Entity_Food(9,sf::Vector2i(6,2), world, {Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Pork",new Entity_Food(10,sf::Vector2i(6,3), world, {Entity::Targeted_PickUp ,Entity::Targeted_DropItem })),

			std::make_pair("Oak Wood",new Entity_Material(11,sf::Vector2i(7,0), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Walnut Wood",new Entity_Material(12,sf::Vector2i(7,1), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Maple Wood",new Entity_Material(13,sf::Vector2i(7,2), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Ash Wood",new Entity_Material(14,sf::Vector2i(7,3), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),

			std::make_pair("Oak Tree",new Entity_Plant(15,sf::Vector2i(8,0), std::vector <std::pair<wchar_t, int>>({std::make_pair(11,4)}), world, { Entity::Targeted_HarvestWood })),
			std::make_pair("Walnut Tree",new Entity_Plant(16,sf::Vector2i(8,1), std::vector <std::pair<wchar_t, int>>({std::make_pair(12,4)}), world, { Entity::Targeted_HarvestWood })),
			std::make_pair("Maple Tree",new Entity_Plant(17,sf::Vector2i(8,2), std::vector <std::pair<wchar_t, int>>({std::make_pair(13,4)}), world, { Entity::Targeted_HarvestWood })),
			std::make_pair("Ash Tree",new Entity_Plant(18,sf::Vector2i(8,3), std::vector <std::pair<wchar_t, int>>({std::make_pair(14,4)}), world, { Entity::Targeted_HarvestWood })),

			std::make_pair("Stone",new Entity_Material(19,sf::Vector2i(9,0), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Dirt",new Entity_Material(20,sf::Vector2i(9,1), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Grass",new Entity_Material(21,sf::Vector2i(9,2), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Sand",new Entity_Material(22,sf::Vector2i(9,3), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Clay",new Entity_Material(23,sf::Vector2i(9,4), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			
			std::make_pair("Coal",new Entity_Material(24,sf::Vector2i(10,0), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Copper Ore",new Entity_Material(25,sf::Vector2i(10,1), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Iron Ore",new Entity_Material(26,sf::Vector2i(10,2), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Gold Ore",new Entity_Material(27,sf::Vector2i(10,3), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Silver Ore",new Entity_Material(28,sf::Vector2i(10,4), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),			
			
			std::make_pair("Brick",new Entity_Material(29,sf::Vector2i(11,0), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Copper Ingot",new Entity_Material(30,sf::Vector2i(11,1), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Iron Ingot",new Entity_Material(31,sf::Vector2i(11,2), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Gold Ingot",new Entity_Material(32,sf::Vector2i(11,3), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),
			std::make_pair("Silver Ingot",new Entity_Material(33,sf::Vector2i(11,4), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),

			std::make_pair("Bed",new Entity_Manufactured(34,sf::Vector2i(12,0), true, world, { Entity::Targeted_PickUp,Entity::Targeted_Rotate,Entity::Targeted_DropItem})),
			std::make_pair("Chair",new Entity_Manufactured(35,sf::Vector2i(13,0), true, world, { Entity::Targeted_PickUp,Entity::Targeted_Rotate,Entity::Targeted_DropItem})),
			std::make_pair("Large Table",new Entity_Manufactured(36,sf::Vector2i(14,0), false, world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Small Table",new Entity_Manufactured(37,sf::Vector2i(14,1), false, world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),

			std::make_pair("Lantern",new Entity_Light(38,sf::Vector2i(15, 0),4,50,world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_TurnOnLight,Entity::Targeted_TurnOffLight})),
			std::make_pair("Fire",new Entity_Light(39,sf::Vector2i(16, 0),4,50,world, {Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_TurnOnLight,Entity::Targeted_TurnOffLight})),

			std::make_pair("WorkBench",new Entity_Crafter(40,sf::Vector2i(17,0),taskManager,world,{34,35,36,37,44,45,46,47,48,49}, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_Craft })),
			std::make_pair("Anvil",new Entity_Crafter(41,sf::Vector2i(17,1),taskManager,world,{}, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_Craft })),
			std::make_pair("Kiln",new Entity_Crafter(42,sf::Vector2i(17,2),taskManager,world,{29,30,31,32,33}, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_Craft })),
			std::make_pair("Furnace",new Entity_Crafter(43,sf::Vector2i(17,3),taskManager,world,{29}, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_Craft })),
			
			std::make_pair("Chest",new Entity_Container(44,sf::Vector2i(18,0),world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Barrel",new Entity_Container(45,sf::Vector2i(18,1),world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Bucket",new Entity_Container(46,sf::Vector2i(18,2),world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),

			std::make_pair("Door",new Entity_Door(47,sf::Vector2i(19,0),world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_UseDoor })),

			std::make_pair("Fence",new Entity_Fence(48,sf::Vector2i(23, 0),world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem})),

			std::make_pair("Fence Gate",new Entity_FenceGate(49,sf::Vector2i(23, 1),world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_UseDoor})),		

		};
		entityGroupList = {
			std::make_pair(Humanoids,AssembleGroup({ "Human" })),
			std::make_pair(Pets, AssembleGroup({ "Cat","Dog" })),
			std::make_pair(FoodAnimals,AssembleGroup({ "Cow","Chicken","Pig" })),
			std::make_pair(Food,AssembleGroup({"Meat","Cat Meat","Dog Meat","Poultry"})),
			std::make_pair(Tree,AssembleGroup({ "Oak Tree","Walnut Tree","Maple Tree","Ash Tree" })),
			std::make_pair(Wood,AssembleGroup({ "Oak Wood","Walnut Wood","Maple Wood","Ash Wood" })),
			std::make_pair(Crafter,AssembleGroup({ "WorkBench" })),
			std::make_pair(ConstructedObject,AssembleGroup({"Bed","Chair","Large Table","Small Table","WorkBench","Anvil","Kiln","Furnace","Chest","Barrel","Bucket","Door","Fence","Fence Gate"})),
		};
	}

	std::string GetEntityNameByID(int id) {
		if (id < 0 || id >= entityList.size()) {
			LogHandler::Instance().WriteLog("Requested an name by id for a nonexistent id: " + std::to_string(id), logContext::ERROR);
			return 0;
		}
		return (&entityList[id])->first;
	}

	int GetEntityIDByName(std::string name) {
		for (int i = 0; i < entityList.size(); i++) {
			if ((&entityList[i])->first == name) {
				return (&entityList[i])->second->GetID();
			}
		}
		LogHandler::Instance().WriteLog("Requested an id by name for a nonexistent name: " + name, logContext::ERROR);
		return 0;
	}

	void SpawnAtCursor(int id, World* world, sf::RenderWindow* w) {
		sf::Vector2i worldPos;
		sf::Vector2i mousePos = sf::Mouse::getPosition(*w);
		sf::Vector2f mouseScreenPos = w->mapPixelToCoords(mousePos) / 800.0f;
		if (Camera::Instance().ScreenToWorld(mouseScreenPos, worldPos) == true) {
			if (world->DoesTileContainEntity(worldPos, id, true) == false) {
				EntityHandler::Instance().CreateAndSpawnEntity(id, world, sf::Vector2i(worldPos.x, worldPos.y));
			}
		}
	}

	enum EntityGroups {
		Humanoids,
		Pets,
		FoodAnimals,
		Food,
		Tree,
		Wood,
		Crafter,
		ConstructedObject,
	};

	bool isIdInGroup(EntityGroups group, int id) {
		std::vector<int> groupids = GetIDGroup(group);
		if (std::find(groupids.begin(), groupids.end(), id) != groupids.end()) {
			return true;
		}
		return false;
	}

	std::vector<int> GetIDGroup(EntityGroups group) {
		for (int i = 0; i < entityGroupList.size(); i++) {
			if (entityGroupList[i].first == group) {
				return entityGroupList[i].second;
			}
		}
		LogHandler::Instance().WriteLog("Requested invalid group ids", logContext::ERROR);
		return{};
	}

	std::vector<int> AssembleGroup(std::vector<std::string> names) {
		std::vector<int> list;
		for (auto name : names) {
			list.push_back(GetEntityIDByName(name));
		}
		return list;
	}

	int GetEntityFromWorldTile(int worldTileID) {
		switch (worldTileID)
		{
			case 0:	return 19; //Stone
			case 1:	return 20; //Dirt
			case 2:	return 21; //Grass
			case 3:	return 22; //Sand
			case 4:	return 23; //Clay
			case 6:	return 19; //Stone
			case 7:	return 11; //Wood
			case 8:	return 12; //Wood
			case 9:	return 13; //Wood
			case 10: return 14; //Wood
			case 11: return 29; //Brick
			case 14: return 24; //Coal
			case 15: return 25; //Copper
			case 16: return 26; //Iron
			case 17: return 27; //Gold
			case 18: return 28; //Silver
		}
		return -1;
	}

	int GetWorldTileFromEntity(int worldTileID) {
		switch (worldTileID)
		{
		case 19: return 6; //Stone
		case 20: return 1; //Dirt
		case 21: return 2; //Grass
		case 22: return 3; //Sand
		case 23: return 4; //Clay
		case 11: return 7; //Wood
		case 12: return 8; //Wood
		case 13: return 9; //Wood
		case 14: return 10; //Wood
		case 29: return 11; //Brick
		case 24: return 14; //Coal
		case 25: return 15; //Copper
		case 26: return 16; //Iron
		case 27: return 17; //Gold
		case 28: return 18; //Silver
		}
		return -1;
	}

	void TickAll() {

		const int num = entities.size();
		Entity** ptr = (num > 0) ? entities.data() : nullptr;
		for (int i = 0; i < num; i++)
		{
			if (ptr[i]->doTick == true) {
				ptr[i]->Tick();
			}
		}
		for (int i = 0; i < playableHumans.size(); i++)
		{
			if (playableHumans[i]->doTick == true) {
				playableHumans[i]->Tick();
			}
		}
	}

	bool IsEntityValid(Entity* e) {
		if (e == nullptr) return false;
		if (std::find(entities.begin(), entities.end(), e) != entities.end()) {
			return true;
		}
		else {
			return false;
		}
		return false;
	}

private:
	std::vector<std::pair<EntityGroups,std::vector<int>>> entityGroupList;
	std::vector<std::pair<std::string, Entity*>> entityList;


	std::vector<Entity*> entities;
	std::vector<Entity_Human*> playableHumans;

	int livingEntitiesCount = 0;
	int humanEntitiesCount = 0;
	int maxLivingEntities = 500;
	int maxHumans = 100;
	TaskManager* taskManager;
	PlayerController* playerController;
	World* world;

};

