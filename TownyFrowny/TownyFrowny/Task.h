#pragma once
class Entity_Living;
#include "World.h"
#include <iostream>
#include "PathFinder.h"
#include "Designation.h"
class Task {
	
public:
	Task(int tickSpeed, int stepCount) {
		this->tickspeed = tickSpeed;
		this->length = stepCount;
	};
	virtual ~Task() {}
	virtual void Setup(World* w, Entity_Living* o, Entity_Living* i) {
		world = w;
		ownerEntity = o;
		issuer = i;
	};

	virtual void Tick();
	virtual void Reset() {
		ticks = 0;
		tickCount = 0;
		began = false;
		finished = false;
	};
	Entity_Living* GetIssuer() {
		return issuer;
	}
protected:	
	enum TaskStatus {
		Running,
		Complete,
		Fail,
		Loop,
	};
	virtual TaskStatus Start() {
		began = true;
		return TaskStatus::Running;
	};
	virtual TaskStatus Execute() { return TaskStatus::Complete; };
	virtual void Cancel() {};
	Entity_Living* ownerEntity;
	Entity_Living* issuer;
	int ticks;
	int tickCount;
	int length;
	int tickspeed;
	World* world;
	bool began;
	bool finished;
	int loopCount = 0;
};
class Task_EntityRetrieval : public Task {
public:
	Task_EntityRetrieval(int range, bool isDeepSearch, int tickSpeed, int length) :Task(tickspeed, length) {
		this->range = (range * 2) + 1;
		this->rangeOffset = (int)floor(((range * 2) + 1) / 2.0) - 1.0f;
		this->deepSearch = isDeepSearch;
		spiral = get_spiral(this->range);
	};
	Task_EntityRetrieval( int range, bool isDeepSearch, bool OnlyLiving, std::vector<int> targetIDs,  int tickSpeed, int length) :Task(tickspeed, length) {
		this->range = (range * 2) + 1;
		this->targetIDs = targetIDs;
		this->rangeOffset = (int)floor(((range * 2) + 1) / 2.0) - 1.0f;
		this->onlyLiving = OnlyLiving;
		this->deepSearch = isDeepSearch;
		spiral = get_spiral(this->range);
	};
	virtual ~Task_EntityRetrieval(){
		spiral.clear();
		spiral.shrink_to_fit();
	}
	std::vector<sf::Vector2i> get_spiral(int size)
	{
		std::vector<sf::Vector2i> result;
		int x = 0; // current position; x
		int y = 0; // current position; y
		int d = 0; // current direction; 0=RIGHT, 1=DOWN, 2=LEFT, 3=UP
		int c = 0; // counter
		int s = 1; // chain size

		// starting point
		x = ((int)floor(size / 2.0)) - 1;
		y = ((int)floor(size / 2.0)) - 1;

		for (int k = 1; k <= (size - 1); k++)
		{
			for (int j = 0; j < (k < (size - 1) ? 2 : 3)+1; j++)
			{
				for (int i = 0; i < s; i++)
				{
					result.push_back(sf::Vector2i(x, y));
					c++;
					switch (d)
					{
					case 0: y = y + 1; break;
					case 1: x = x + 1; break;
					case 2: y = y - 1; break;
					case 3: x = x - 1; break;
					}
				}
				d = (d + 1) % 4;
			}
			s = s + 1;
		}
		return result;
	}
protected:
	bool deepSearch;
	bool onlyLiving;
	int range;
	int rangeOffset;
	std::vector<int> targetIDs;
	std::vector<sf::Vector2i> spiral;
	int sprialIndex;
	int spiralTicks;
};
class Task_Search_Singluar: public Task_EntityRetrieval {
public:
	Task_Search_Singluar( int range,bool isDeepSearch, bool OnlyLiving, std::vector<int> TargetIDs,int tickSpeed, int length):Task_EntityRetrieval(range, isDeepSearch, OnlyLiving,TargetIDs,tickspeed,length) {
	
	}; 
	virtual ~Task_Search_Singluar() {};
	TaskStatus Execute() override;
	sf::Vector2i* FoundTarget;
	Entity* FoundTargetEntity;
}; 
class Task_Search_For : public Task_EntityRetrieval {
public:
	Task_Search_For(Entity** toSearchFor, int range, bool isDeepSearch, int tickSpeed, int length) :Task_EntityRetrieval(range, isDeepSearch, tickspeed, length) {
		this->toSearchFor = toSearchFor;
	};
	virtual ~Task_Search_For() {};
	TaskStatus Execute() override;
private:
	Entity** toSearchFor;
};

class Task_Search : public Task_EntityRetrieval {
public:
	Task_Search(int range, bool isDeepSearch, bool OnlyLiving, std::vector<int> TargetIDs, int tickSpeed, int length) :Task_EntityRetrieval(range, isDeepSearch, OnlyLiving, TargetIDs, tickspeed, length) {

	};
	virtual ~Task_Search() {};
	TaskStatus Execute() override;
	std::vector<sf::Vector2i*> FoundTargets;
	std::vector<Entity*> FoundTargetEntities;
};
class Task_SelectRandomEntityFromList : public Task {
public:
	Task_SelectRandomEntityFromList(std::vector<Entity*>* EntityList, int tickSpeed, int length) :Task(tickspeed, length) {
		this->EntityList = EntityList;
	};
	~Task_SelectRandomEntityFromList() {
		delete selectedTarget;
	}
	TaskStatus Execute() override;
	Entity* selected;
	sf::Vector2i* selectedTarget;
private:
	std::vector<Entity*>* EntityList;
};

class Task_WalkTo : public Task {
public:	
	Task_WalkTo(PathFinder* pathFinder, int stopDist, Entity** targetEntity, int tickSpeed, int length) :Task(tickspeed, length) {
		this->targetEntity = targetEntity;
		this->stopDist = stopDist;
		this->pathfinder = pathfinder;
	};
	Task_WalkTo(PathFinder* pathFinder, int stopDist, sf::Vector2i** target,int tickSpeed, int length) :Task(tickspeed, length) {
		this->target = target;
		this->stopDist = stopDist;
		this->pathfinder = pathfinder;
	};
	TaskStatus Start() override;
	TaskStatus Execute() override;
private:
	std::vector<sf::Vector2i> path;
	int pathIndex = 0;
	sf::Vector2i** target; 
	Entity** targetEntity;
	PathFinder* pathfinder;
	int stopDist;
	bool startedAtTarget = false;
	const int maxDist = 40;
};

class Task_Take : public Task {
public:
	Task_Take(Entity** eToCheck, std::vector<int> TargetIDs,bool takeAll, int tickSpeed, int length) :Task(tickspeed, length) {
		this->ToCheck = eToCheck;
		this->targetIDs = TargetIDs;
		this->foundItem = nullptr;
		this->takeAll = takeAll;
	};
	TaskStatus Execute() override;
	Entity* foundItem;
private:
	Entity** ToCheck;
	std::vector<int> targetIDs;
	bool takeAll;
};

class Task_Give : public Task {
public:
	Task_Give(Entity** eToGiveto, std::vector< std::vector<int>> giveList, int tickSpeed, int length) :Task(tickspeed, length) {
		this->eToGiveto = eToGiveto;
		this->giveList = giveList;
	};
	TaskStatus Execute() override;
private:
	Entity** eToGiveto;
	std::vector< std::vector<int>> giveList;
};

class Task_PickUp : public Task {
public:
	Task_PickUp(std::vector<int> TargetIDs,  int tickSpeed, int length) :Task(tickspeed,length) {
		this->targetIDs = TargetIDs;
	}; 
	Task_PickUp(Entity** entity, int tickSpeed, int length) :Task(tickspeed, length) {
		this->entity = entity;
	};
	TaskStatus Execute() override;
private:
	std::vector<int> targetIDs;
	Entity** entity;
};

class Task_Drop : public Task {
public:
	Task_Drop(Entity** entity, sf::Vector2i** targetPos, int tickSpeed, int length) :Task(tickspeed, length) {
		this->toDrop = entity;
		this->targetPos = targetPos;
	};
	TaskStatus Execute() override;
private:
	Entity** toDrop;
	sf::Vector2i** targetPos;
};

class Task_DropInStockDesignation : public Task {
public:
	Task_DropInStockDesignation(Entity** entity, Designation::Type chceckfor, int tickSpeed, int length) :Task(tickspeed, length) {
		this->toDrop = entity;
		this->typetoCheck = chceckfor;
	};
	TaskStatus Execute() override;
private:
	Entity** toDrop;
	Designation::Type typetoCheck;
};


class Task_CheckEntityAlive : public Task {
public:
	Task_CheckEntityAlive(Entity** ToCheck, int tickSpeed, int length) :Task(tickspeed, length) {
		this->ToCheck = ToCheck;
	};
	TaskStatus Execute() override;
private:
	Entity** ToCheck;
};

class Task_Pase : public Task {
public:
	Task_Pase(int tickSpeed, int length) :Task(tickspeed,length) {
	};
	TaskStatus Execute() override;
};

class Task_GetItemFromOwner : public Task {
public:
	Task_GetItemFromOwner(std::vector<int> TargetIDs,int tickSpeed, int length) :Task(tickspeed, length) {
		this->targetIDs = TargetIDs;
		this->foundEntity = nullptr;
	};
	TaskStatus Execute() override;
	Entity* foundEntity;
private:
	std::vector<int> targetIDs;
};

class Task_EatEntity : public Task {
public:
	Task_EatEntity(Entity** ToEat, int tickSpeed, int length) :Task(tickspeed, length) {
		this->ToEat = ToEat;
	};
	TaskStatus Execute() override;
private:
	Entity** ToEat;
};

class Task_RestrainLivingEntity :public Task {
public:
	Task_RestrainLivingEntity(Entity** ToRestrain,int tickSpeed, int length) :Task(tickspeed, length) {
		this->ToRestrain = ToRestrain;
	};
	TaskStatus Execute() override;
private:
	Entity** ToRestrain;
};

class Task_AttackLivingEntityUntilDead :public Task {
public:
	Task_AttackLivingEntityUntilDead(Entity** ToKill, int tickSpeed, int length) :Task(tickspeed, length) {
		this->ToKill = ToKill;
	};
	TaskStatus Execute() override;
private:
	Entity** ToKill;
};


class Task_LootBodilyInventory :public Task {
public:
	Task_LootBodilyInventory(Entity** ToLoot, int tickSpeed, int length) :Task(tickspeed, length) {
		this->ToLoot = ToLoot;
	};
	TaskStatus Execute() override;
private:
	Entity** ToLoot;
};

class Task_Wait : public Task {
public:
	Task_Wait(int tickSpeed, int length) :Task(tickspeed, length) {
	};
	TaskStatus Execute() override;
};

class Task_AssignTaskToEntity : public Task {
public:
	Task_AssignTaskToEntity(std::vector<Task*> tree,Entity** target, int tickSpeed, int length) :Task(tickspeed, length) {
		this->OrderTarget = target;
		this->taskstoAssign = tree;
	};
	TaskStatus Execute() override;
private:
	std::vector<Task*> taskstoAssign;
	Entity** OrderTarget;
};


class Task_LocateStockDesignation : public Task_EntityRetrieval {
public:
	Task_LocateStockDesignation(int range, Designation::Type typetoCheck, Entity** refEntity, int tickSpeed, int length) :Task_EntityRetrieval(range, false,tickspeed, length) {
		this->refEntity = refEntity;
		this->typetoCheck = typetoCheck; 
	};
	virtual ~Task_LocateStockDesignation() {};
	TaskStatus Execute() override;

	sf::Vector2i* FoundTarget;
private:
	Entity** refEntity;
	Designation::Type typetoCheck;
};

class Task_ConstructFloor : public Task {
public:
	Task_ConstructFloor(Entity** targetEntity, sf::Vector2i** target, int tickSpeed, int length) :Task(tickspeed, length) {
		this->target = target;
		this->targetEntity = targetEntity;
	};
	TaskStatus Execute() override;
private:
	sf::Vector2i** target;
	Entity** targetEntity;
};

class Task_DeconstructFloor : public Task {
public:
	Task_DeconstructFloor(sf::Vector2i** target, int tickSpeed, int length) :Task(tickspeed, length) {
		this->target = target;
	};
	TaskStatus Execute() override;
private:
	sf::Vector2i** target;
};

class Task_ConstructWall : public Task {
public:
	Task_ConstructWall(Entity** targetEntity, sf::Vector2i** target, int tickSpeed, int length) :Task(tickspeed, length) {
		this->target = target;
		this->targetEntity = targetEntity;
	};
	TaskStatus Execute() override;
private:
	sf::Vector2i** target;
	Entity** targetEntity;
};

class Task_DeconstructWall : public Task {
public:
	Task_DeconstructWall(sf::Vector2i** target, int tickSpeed, int length) :Task(tickspeed, length) {
		this->target = target;
	};
	TaskStatus Execute() override;
private:
	sf::Vector2i** target;
};

class Task_Rotate : public Task {
public:
	Task_Rotate(Entity** target, int tickSpeed, int length) :Task(tickspeed, length) {
		this->target = target;
	};
	TaskStatus Execute() override;
private:
	Entity** target;
};

class Task_Craft : public Task {
public:
	Task_Craft(int id, Entity** targetCrafter, int tickSpeed, int length) :Task(tickspeed, length) {
		this->id = id;
		this->targetCrafter = targetCrafter;
	};
	TaskStatus Execute() override;
	Entity* crafted;
private:
	int id;
	Entity** targetCrafter;
};

class Task_SourceMaterials : public Task {
public:
	Task_SourceMaterials(int id,PathFinder* pathfinder, int tickSpeed, int length) :Task(tickspeed, length) {
		this->id = id;
		this->pathfinder = pathfinder;
	};
	TaskStatus Execute() override;
private:
	int id;
	PathFinder* pathfinder;
};

class Task_CreateStaircase : public Task {
public:
	Task_CreateStaircase(World* OverWorld, World* UnderWorld, int tickSpeed, int length) :Task(tickspeed, length) {
		this->OverWorld = OverWorld;
		this->UnderWorld = UnderWorld;
	};
	TaskStatus Execute() override;
private:
	World* OverWorld;
	World* UnderWorld;
};

class Task_UseStaircase : public Task {
public:
	Task_UseStaircase(World* OverWorld, World* UnderWorld, int tickSpeed, int length) :Task(tickspeed, length) {
		this->OverWorld = OverWorld;
		this->UnderWorld = UnderWorld;
	};
	TaskStatus Execute() override;
private:
	World* OverWorld;
	World* UnderWorld;
};

class Task_UseDoor : public Task {
public:
	Task_UseDoor(Entity** targetDoor, int tickSpeed, int length) :Task(tickspeed, length) {
		this->targetDoor = targetDoor;
	};
	TaskStatus Execute() override;
private:
	Entity** targetDoor;
};

class Task_TurnOnLight : public Task {
public:
	Task_TurnOnLight(Entity** targetLight, int tickSpeed, int length) :Task(tickspeed, length) {
		this->targetLight = targetLight;
	};
	TaskStatus Execute() override;
private:
	Entity** targetLight;
};

class Task_TurnOffLight : public Task {
public:
	Task_TurnOffLight(Entity** targetLight, int tickSpeed, int length) :Task(tickspeed, length) {
		this->targetLight = targetLight;
	};
	TaskStatus Execute() override;
private:
	Entity** targetLight;
};