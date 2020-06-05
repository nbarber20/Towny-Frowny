#include "Entity_Human.h"
#include "TaskManager.h"
#include "InputHandler.h"
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
	LogHandler::Instance().WriteLog(typeid(*t).name() + std::string(" complete"), this->GetPosition(), logContext::WORLD);
	Entity_Living::TaskComplete(t);
}

void Entity_Human::TaskFail(Task* t)
{
	LogHandler::Instance().WriteLog(typeid(*t).name() + std::string(" fail"), this->GetPosition(), logContext::ERROR);
	Entity_Living::TaskFail(t);
}

void Entity_Human::clearAllTasks()
{
	DestroyTaskSteps();
	Entity_Living::clearAllTasks();
}

void Entity_Human::NewTaskList(std::vector<Task*> SetCurrent)
{
	OverwriteTasks(SetCurrent, this);
}

void Entity_Human::DestroyTaskSteps()
{
	if (currentTargetedTasks.size() > 0) {
		/*for (int i = 0; i < currentTargetedTasks.size(); i++) {
			delete (currentTargetedTasks[i]);
		}*/
		currentTargetedTasks.clear();
		currentTargetedTasks.shrink_to_fit();
	}
	inputHandler->ClearBehaviorList();
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

void Entity_Human::SetTargetedBehavior(TargetedHumanBehaviors b, targetedTaskStep* step, bool doOverride)
{
	if (doOverride == true) {
		clearAllTasks();
	}
	currentTargetedTasks.push_back(step);
	switch (b)
	{
	case Entity::Targeted_ClearAll:
		clearAllTasks();
		return;
	case TargetedHumanBehaviors::Targeted_HarvestWood:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_HarvestWood(&step->entity), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_SlaughterAnimal:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_SlaughterAnimal(&step->entity),5, taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_PickUp:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_PickUp(&step->entity), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_WalkTo:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_WalkTo(&step->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_ConstructWall:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_ConstructWall(&step->entity, &step->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_DestroyWall:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_DeconstructWall(&step->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_ConstructFloor:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_ConstructFloor(&step->entity, &step->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_DestroyFloor:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_DeconstructFloor(&step->Pos), taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_LootBody:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_LootBody(&step->entity), 2, taskManager));
		break; 
	case TargetedHumanBehaviors::Targeted_Attack:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_Attack(&step->entity), 5, taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_Rotate:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_Rotate(&step->entity), 5, taskManager));
		break;
	case TargetedHumanBehaviors::Targeted_DropItem:
		TaskTree.push_back(new BehaviorBranch(taskManager->TargetTREE_DropItem(&step->entity, &step->Pos), taskManager));
		break;
	}
	startTaskQueue();
}
