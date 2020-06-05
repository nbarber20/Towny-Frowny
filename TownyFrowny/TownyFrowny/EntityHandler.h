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
#include "InputHandler.h"
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

			if(handler->GetPilotingPlayer() == nullptr){
				handler->SetPilotingPlayer(hcheck);
			}

			playableHumans.push_back(hcheck);
			return(newEntity);
		}
		entities.push_back(newEntity);
		return (newEntity);
	}
	void CreateAndSpawnEntity(int id, World* w, sf::Vector2i pos) {
		Entity* e = CreateEntity(id);
		if (e != nullptr)w->SpawnEntity(e, pos);
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
	void Init(TaskManager* taskManager, InputHandler* inputHandler, World* world) {
		this->taskManager = taskManager;
		this->world = world;
		this->handler = inputHandler;
		entityList = {
			std::make_pair("Human", new Entity_Human(0,sf::Vector2i(0,0),inputHandler, taskManager, world, {})),
			std::make_pair("Cow",new Entity_Animal(1,sf::Vector2i(1,0), 8,6, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Cat",new Entity_Pet(2,sf::Vector2i(2,0),7, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Dog",new Entity_Pet(3,sf::Vector2i(3,0),8, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Chicken",new Entity_Animal(4,sf::Vector2i(4,0), 3, 9, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Pig",new Entity_Animal(5,sf::Vector2i(5,0), 5,  10, taskManager, world, { Entity::Targeted_SlaughterAnimal,Entity::Targeted_Attack,Entity::Targeted_LootBody })),
			std::make_pair("Meat",new Entity_Food(6,sf::Vector2i(6,0), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Cat Meat",new Entity_Food(7,sf::Vector2i(6,1), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem})),
			std::make_pair("Dog Meat",new Entity_Food(8,sf::Vector2i(6,1), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem })),
			std::make_pair("Chicken",new Entity_Food(9,sf::Vector2i(6,2), world, {Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Pork",new Entity_Food(10,sf::Vector2i(6,3), world, {Entity::Targeted_PickUp ,Entity::Targeted_DropItem })),
			std::make_pair("Oak Wood",new Entity_Material(11,sf::Vector2i(7,0), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Walnut Wood",new Entity_Material(12,sf::Vector2i(7,1), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Maple Wood",new Entity_Material(13,sf::Vector2i(7,2), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Ash Wood",new Entity_Material(14,sf::Vector2i(7,3), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Oak Tree",new Entity_Plant(15,sf::Vector2i(8,0), 12, world, { Entity::Targeted_HarvestWood })),
			std::make_pair("Walnut Tree",new Entity_Plant(16,sf::Vector2i(8,1), 12, world, { Entity::Targeted_HarvestWood })),
			std::make_pair("Maple Tree",new Entity_Plant(17,sf::Vector2i(8,2), 12, world, { Entity::Targeted_HarvestWood })),
			std::make_pair("Ash Tree",new Entity_Plant(18,sf::Vector2i(8,3), 12, world, { Entity::Targeted_HarvestWood })),
			std::make_pair("Stone",new Entity_Material(19,sf::Vector2i(9,0), world, { Entity::Targeted_PickUp ,Entity::Targeted_DropItem,Entity::Targeted_ConstructWall,Entity::Targeted_ConstructFloor})),
			std::make_pair("Ore",new Entity_Material(20,sf::Vector2i(9,1), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,})),
			std::make_pair("Iron",new Entity_Material(21,sf::Vector2i(9,2), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,})),
			std::make_pair("Gold",new Entity_Material(22,sf::Vector2i(9,3), world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem,})),
			std::make_pair("Bed",new Entity_Manufactured(23,sf::Vector2i(11,0), true, world, { Entity::Targeted_PickUp,Entity::Targeted_Rotate,Entity::Targeted_DropItem})),
			std::make_pair("Chair",new Entity_Manufactured(24,sf::Vector2i(12,0), true, world, { Entity::Targeted_PickUp,Entity::Targeted_Rotate,Entity::Targeted_DropItem})),
			std::make_pair("Large Table",new Entity_Manufactured(25,sf::Vector2i(13,0), false, world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
			std::make_pair("Small Table",new Entity_Manufactured(26,sf::Vector2i(13,1), false, world, { Entity::Targeted_PickUp,Entity::Targeted_DropItem })),
		};
	}

	std::string GetEntityNameByID(int id) {
		return (&entityList[id])->first;
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
	};

	std::vector<int> GetIDGroup(EntityGroups group) {
		switch (group)
		{
			case Humanoids:
				return std::vector<int>({ 0 });
				break;
			case Pets:
				return std::vector<int>({ 2,3 });
				break;
			case FoodAnimals:
				return std::vector<int>({ 1,4,5 });
				break;
			case Food:
				return std::vector<int>({ 6,7,8,9,10 });
				break;
			case Tree:
				return std::vector<int>({ 15,16,17,18 });
				break;
			case Wood:
				return std::vector<int>({ 11,12,13,14 });
				break;
		}
		LogHandler::Instance().WriteLog("Requested invalid group ids", logContext::ERROR);
	}

	void TickAll() {
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities[i]->doTick == true) {
				entities[i]->Tick();
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
	std::vector<std::pair<std::string,Entity*>> entityList;


	std::vector<Entity*> entities;
	std::vector<Entity_Human*> playableHumans;

	int livingEntitiesCount = 0;
	int humanEntitiesCount = 0;
	int maxLivingEntities = 500;
	int maxHumans = 100;
	TaskManager* taskManager;
	InputHandler* handler;
	World* world;

};

