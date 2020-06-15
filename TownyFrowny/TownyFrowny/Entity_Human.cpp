#include "Entity_Human.h"
#include "TaskManager.h"
#include "PlayerController.h"
#include "Recipies.h"
void Entity_Human::Tick()
{
	Entity_Living::Tick();
	if (idle) {
		if (UseAI) {
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
	}
	else {
		TaskTree[taskTreeIndex]->Branch[taskIndex]->Tick();
	}
}

void Entity_Human::TaskComplete(Task* t)
{
	//LogHandler::Instance().WriteLog(typeid(*t).name() + std::string(" complete"), this->GetPosition(), logContext::WORLD);
	Entity_Living::TaskComplete(t);
}

void Entity_Human::TaskFail(Task* t)
{
	//LogHandler::Instance().WriteLog(typeid(*t).name() + std::string(" fail"), this->GetPosition(), logContext::ERROR);
	Entity_Living::TaskFail(t);
}

void Entity_Human::clearAllTasks()
{
	playerController->ClearBehaviorList();
	Entity_Living::clearAllTasks();
}

void Entity_Human::NewTaskList(std::vector<Task*> SetCurrent)
{
	OverwriteTasks(SetCurrent, this);
}


bool Entity_Human::MoveToTile(short dx, short dy)
{
	if (dx > world->GetWorldSize() - 1 || dx < 0 || dy<0 || dy>world->GetWorldSize() - 1)return false; //Out of bounds
	if (world->IsTileWalkable(sf::Vector2i(dx, dy), this) == false)return false; //Cannot walk here

	if (world->DoesTileContainEntity(sf::Vector2i(dx, dy), 47, true) == true) { //this is a door
		Entity_Door* door = dynamic_cast<Entity_Door*>(world->GetEntityInTileByID(47, sf::Vector2i(dx, dy)));
		door->OpenDoor();
	}

	if (world->DoesTileContainEntity(sf::Vector2i(x, y), 47, true) == true) { //this is a door
		Entity_Door* door = dynamic_cast<Entity_Door*>(world->GetEntityInTileByID(47, sf::Vector2i(x, y)));
		door->CloseDoor();
	}

	if (world->DoesTileContainEntity(sf::Vector2i(dx, dy), 49, true) == true) { //this is a Gate
		Entity_FenceGate* door = dynamic_cast<Entity_FenceGate*>(world->GetEntityInTileByID(49, sf::Vector2i(dx, dy)));
		door->OpenDoor();
	}

	if (world->DoesTileContainEntity(sf::Vector2i(x, y), 49, true) == true) { //this is a Gate
		Entity_FenceGate* door = dynamic_cast<Entity_FenceGate*>(world->GetEntityInTileByID(49, sf::Vector2i(x, y)));
		door->CloseDoor();
	}

	world->MoveEntity(this, x, y, dx, dy);
	for (int i = 0; i < heldItems.size(); i++) {
		heldItems[i]->SetPosition(dx, dy);
	}
	return true;
}

void Entity_Human::SetBehavoir(Entity::Behaviors b)
{
	clearAllTasks();
	switch (b)
	{
	case Entity::GatherWood:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_HarvestWood(20),taskManager));
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_HarvestWood(20), taskManager));
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_HarvestWood(20), taskManager));
		break;
	case Entity::SlaughterAnimal:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_SlaughterAnimal(20), 4, taskManager));
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_StoreFood(20), taskManager));
		break;
	case Entity::Eat:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_EatHeldFood(), {
			//FailTasks
			new BehaviorBranch(taskManager->TREE_FindFood(20),taskManager),
			new BehaviorBranch(taskManager->TREE_EatHeldFood(),taskManager)
			}, taskManager));
		break;
	case Entity::Pase:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_Pase(10), taskManager));
		break;
	case Entity::Wait:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_Wait(10), taskManager));
		break;
	default:
		break;
	}
	startTaskQueue();
}

void Entity_Human::SetTargetedBehavior(TargetedBehaviorStep* b)
{
	switch (b->behavior)
	{
	case Entity::Targeted_ClearAll:
		clearAllTasks();
		return;
	case TargetedHumanBehaviors::Targeted_HarvestWood:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_HarvestWood(&b->entity), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_SlaughterAnimal:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_SlaughterAnimal(&b->entity),5, taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_PickUp:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_PickUp(&b->entity), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_WalkTo:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_WalkTo(&b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_ConstructWall:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_ConstructWall(&b->entity, &b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_DestroyWall:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_DeconstructWall(&b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_ConstructFloor:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_ConstructFloor(&b->entity, &b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_DestroyFloor:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_DeconstructFloor(&b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_LootBody:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_LootBody(&b->entity), 2, taskManager));
		break; 
	case TargetedHumanBehaviors::Targeted_Attack:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_Attack(&b->entity), 5, taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_Rotate:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_Rotate(&b->entity), 5, taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_DropItem:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_DropItem(&b->entity, &b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_Craft:
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_SourceMaterials(b->variantID), taskManager));
		TaskTree.push_back(new BehaviorBranch(taskManager->TREE_CraftItem(b->variantID), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_MakeStaircase:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_MakeStairCase(&b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_UseStaircase:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_UseStairCase(&b->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_UseDoor:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_UseDoor(&b->entity), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_TurnOnLight:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_TurnLightOn(&b->entity), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_TurnOffLight:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_TurnLightOff(&b->entity), taskManager));
		break;
	}

	startTaskQueue();
}

void Entity_Human::Initilize()
{
	individualName = NameGenerator::Instance().GenName();
	srand(time(NULL));
	int id = rand() % TileManager::Instance().GetNumEntityVariants(spriteX);
	SetSpriteVariant(0, id);
}
