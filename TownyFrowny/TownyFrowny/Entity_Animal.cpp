#include "Entity_Animal.h"
#include "TaskManager.h"
#include "EntityHandler.h"

Entity_Animal::Entity_Animal(wchar_t EntityID, sf::Vector2i spritePos, int volume, wchar_t MeatID, TaskManager* manager, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity_Living(EntityID, spritePos, manager, worldref, TargetedBehaviors)
{
	individualName = NameGenerator::Instance().GenNameFirstName() + " the" + GetObjectName();
	this->volume = volume;
	this->MeatID = MeatID;
}

void Entity_Animal::Tick()
{
	Entity_Living::Tick();
	if (!dead) {

		if (idle) {
			std::vector<Entity::Behaviors> selected;
			float weightCheck = ((float)rand()) / (float)RAND_MAX;
			for (int i = 0; i < behaviorWeights.size(); i++) {
				if (behaviorWeights[i].second >= weightCheck) selected.push_back(behaviorWeights[i].first);
			}
			int s = selected.size();
			if (s == 0) return;
			if (s == 1)SetBehavoir(selected[0]);
			else SetBehavoir(selected[rand() % (s - 1)]);
		}
		else {
			TaskTree[taskTreeIndex]->Branch[taskIndex]->Tick();
		}
	}
	else {
		if (decompositionLvl > 20) {
			SetSpriteVariant(spriteX,2);
		}
	}
}

void Entity_Animal::Die(Entity_Living* source)
{
	Entity_Living::Die(source);
	SetSpriteVariant(spriteX, 1);
}

void Entity_Animal::InitilizeSpawnInventory()
{
	AddItemToBodilyInventory(EntityHandler::Instance().CreateEntity(MeatID));
}

void Entity_Animal::SetBehavoir(Entity::Behaviors b)
{
	clearAllTasks();
	switch (b)
	{
	case Entity::Wait:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_Wait(20)));
		break;
	case Entity::Pase:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_Pase(10)));
		break;
	case Entity::Roam:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_Pase(10)));
		break;
	case Entity::Graze:
		return;
	case Entity::Eat:
		return;
	default:
		return;
	}
	startTaskQueue();
}