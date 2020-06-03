#include "Entity_Living.h"
#include "World.h"
#include "VectorHelper.h"
#include "LogHandler.h"
#include "EntityHandler.h"
#include "Task.h"
void Entity_Living::Tick()
{
	if (dead == true)decompositionLvl++;
	hungerLevel++;
}

void Entity_Living::TaskComplete(Task* t)
{	

	if (taskIndex < TaskTree[taskTreeIndex]->Branch.size()-1) { //if we are not at the end of the current branch
		taskIndex++;
	}
	else {
		if (taskTreeIndex < TaskTree.size() - 1) { //move on to next branch
			taskTreeIndex++; 
			taskIndex=0;
			startTaskQueue();
		}
		else { //we are complete
			clearAllTasks();
			//idle = true;
		}
	}
}

void Entity_Living::TaskFail(Task* t)
{
	if (dead == true)clearAllTasks();
	taskIndex = 0;
	if (TaskTree[taskTreeIndex]->retries > 1) {
		taskIndex = 0;
		startTaskQueue();
		TaskTree[taskTreeIndex]->retries--;
	}
	else {
		if (TaskTree[taskTreeIndex]->FailTree.size() > 0) { //there is a taskfallback for this tree set
			std::vector<BehaviorBranch*> temp;
			std::vector<BehaviorBranch*> failTemp = TaskTree[taskTreeIndex]->FailTree;
			for (int i = taskTreeIndex+1; i < TaskTree.size();i++) {
				temp.push_back(TaskTree[i]);
			}
			TaskTree.clear();			
			TaskTree = failTemp; //mark the fail tree to be executed

			for (int j = 0; j < temp.size(); j++) { //continue from where we left off
				TaskTree.push_back(temp[j]);
			}
			taskIndex = 0;
			taskTreeIndex = 0;
			startTaskQueue();
		}
		else { //there is no fall back
			if (taskTreeIndex < TaskTree.size() - 1) { //move on to next branch
				taskTreeIndex++;
				startTaskQueue();
			}
			else { //we are complete
				clearAllTasks();
				//idle = true;
			}
		}		
	}	
}

void Entity_Living::OverwriteTasks(std::vector<Task*> newTree, Entity_Living* issuer)
{
	if (dead == true)return;
	clearAllTasks();
	TaskTree.push_back(new BehaviorBranch(newTree, {}, 0));
	startTaskQueue(issuer);	
}

void Entity_Living::AddItemToBodilyInventory(Entity* e)
{
	e->doTick = false;
	e->SetParent(this);
	bodilyInventory.push_back(e);
}

void Entity_Living::RemoveItemFromInventory(Entity* e)
{
	if (std::find(heldItems.begin(), heldItems.end(), e) != heldItems.end()) {
		heldItems.erase(std::remove(heldItems.begin(), heldItems.end(), e), heldItems.end());
	}
	else if (std::find(bodilyInventory.begin(), bodilyInventory.end(), e) != bodilyInventory.end()) {
		e->doTick = true;
		bodilyInventory.erase(std::remove(bodilyInventory.begin(), bodilyInventory.end(), e), bodilyInventory.end());
	}
}


void Entity_Living::Eat(Entity* e)
{
	EntityHandler::Instance().DestroyEntity(e, world);
	hungerLevel = 0;
	std::string s = individualName + " ate a " + e->GetObjectName();
	LogHandler::Instance().WriteLog(s, this->GetPosition(), logContext::WORLD);
}

void Entity_Living::AttackEntity(Entity_Living* e, int dammage)
{
	e->TakeDamage(dammage,this);
}

void Entity_Living::TakeDamage(int ammount, Entity_Living* source)
{
	hp -= ammount;
	if (hp <= 0) {
		Die(source);
	}
}

void Entity_Living::Die(Entity_Living* source)
{
	LogHandler::Instance().WriteLog(individualName + " was killed by " + source->individualName, this->GetPosition(), logContext::WORLD);
	clearAllTasks();
	for (auto item : heldItems) {
		DropItem(item);
	}
	dead = true;
}

bool Entity_Living::IsAlive()
{
	return !dead;
}

void Entity_Living::DropItem(Entity* item)
{
	if (item->GetParent() == this) {
		item->SetPosition(this->x, this->y);
		item->SetParent(nullptr);
		world->SpawnEntity(item, item->GetPosition());
	}
}

std::vector<Entity*>* Entity_Living::GetBodilyInventory()
{
	return &bodilyInventory;
}

std::string Entity_Living::GetIndividualName()
{
	if (individualName == "") {
		return this->objectName;
	}
	return this->individualName;
}

void Entity_Living::clearAllTasks()
{	
	idle = true;
	taskIndex = 0;
	taskTreeIndex = 0;
	for (int i = 0; i < TaskTree.size(); i++) {
		delete TaskTree[i];
	}
	TaskTree.clear();
}

void Entity_Living::startTaskQueue()
{
	for (auto p : TaskTree[taskTreeIndex]->Branch)
	{
		p->Setup(world, this, this);
		p->Reset();
	}
	idle = false;
}
void Entity_Living::startTaskQueue(Entity_Living* issuer)
{
	for (auto a: TaskTree[taskTreeIndex]->Branch)
	{
		//TODO: sometimes this breaks for no reason
		a->Setup(world,this, issuer);
		a->Reset();
	}
	idle = false;
}

void Entity_Living::CraftItem(int id)
{
	clearAllTasks();
	std::vector< std::vector<int>> recipe = getRecipe(id);
	if (recipe.size() == 0) return; //This is an invalid recipe
	std::vector<Entity*> tempHeldItems = heldItems;
	bool matchingItemFound = false;

	if (GetCraftable(recipe) == false) {
		LogHandler::Instance().WriteLog("Crafting Impossible");
		return;
	}

	int stepsCompleted = 0;
	for (int r = 0; r < recipe.size(); r++) {
		if(ConsumeCraftingItem(recipe[r])) stepsCompleted++;
	}
	if (stepsCompleted >= recipe.size()) {
		LogHandler::Instance().WriteLog("Crafting Complete");
		AddItemToInventory(EntityHandler::Instance().CreateEntity(id));
	}
	else {
		LogHandler::Instance().WriteLog("Crafting MegaFail");
	}
}

std::vector<int> Entity_Living::GetAllCraftable()
{
	std::vector<int> result;
	std::vector < std::pair<int, std::vector<std::vector<int>>>> recipies = getRecipes();
	for (auto recipie : recipies) {
		if (GetCraftable(recipie.second)) {
			result.push_back(recipie.first);
		}
	}
	return result;
}

bool Entity_Living::GetCraftable(std::vector<std::vector<int>> r)
{
	std::vector<Entity*> tempHeldItems = heldItems;
	for (int i = 0; i < r.size(); i++) {
		if (holdingItem(&tempHeldItems, r[i]) == false) {
			return false;
		}
	}
	return true;
}

bool Entity_Living::ConsumeCraftingItem(std::vector<int > recipeItem)
{
	for (int i = 0; i < heldItems.size(); i++) {
		for (int j = 0; j < recipeItem.size(); j++) {
			if (heldItems[i]->GetID() == recipeItem[j]) {
				EntityHandler::Instance().DestroyEntity(heldItems[i], world);
				return true;
			}
		}
	}
	return false;
}

bool Entity_Living::holdingItem(std::vector<Entity*>* List, std::vector<int > recipeItem)
{
	for (int i = 0; i < List->size(); i++) {
		for (int j = 0; j < recipeItem.size(); j++) {
			if ((*List)[i]->GetID() == recipeItem[j]) {
				List->erase(std::remove(List->begin(), List->end(), (*List)[i]), List->end());
				return true;
			}
		}
	}
	return false;
}
std::vector<std::vector<int>> Entity_Living::getRecipe(int id)
{
	std::vector < std::pair<int, std::vector<std::vector<int>>>> recipies = getRecipes();
	for (auto recipie : recipies) {
		if (recipie.first == id) {
			return recipie.second;
		}
	}
}
std::vector < std::pair<int, std::vector<std::vector<int>>>> Entity_Living::getRecipes()
{
	std::vector < std::pair<int, std::vector<std::vector<int>>>> list;
	std::vector<std::vector<int>> Bed{
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
	};
	list.push_back(std::make_pair(23, Bed));

	std::vector<std::vector<int>> Chair{
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
	};
	list.push_back(std::make_pair(24, Chair));

	std::vector<std::vector<int>> LargeTable{
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
	};
	list.push_back(std::make_pair(25, LargeTable));

	std::vector<std::vector<int>> SmallTable{
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
	};
	list.push_back(std::make_pair(26, SmallTable));

	return list;
}

