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
				temp.push_back(TaskTree[i]); //add the tasks we had queued
			}
			delete TaskTree[taskTreeIndex]; //delete the current task branch

			TaskTree.clear();			
			TaskTree = failTemp; //mark the fail tree to be executed

			for (int j = 0; j < temp.size(); j++) { //append the branches onto the end after the failtree
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

void Entity_Living::OverwriteTasks(std::vector<Task*> newBranch, Entity_Living* issuer)
{
	if (dead == true)return;
	clearAllTasks();
	TaskTree.push_back(new BehaviorBranch(newBranch, {}, 0));
	startTaskQueue(issuer);	
}

void Entity_Living::SwitchCurrentTask(std::vector<Task*> newBranch, Entity_Living* issuer)
{
	if (dead == true)return;
	std::vector<BehaviorBranch*> temp = TaskTree;
	std::vector<BehaviorBranch*> newTree;
	newTree.push_back(new BehaviorBranch(newBranch, {}, 0));
	for (int i = 1; i < temp.size(); i++) {
		newTree.push_back(temp[i]);
	}
	TaskTree = newTree;
	taskIndex = 0;
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
	EntityHandler::Instance().DestroyEntity(e, world,true);
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
	while (heldItems.size() > 0) {
		DropItem(heldItems[0], this->world);
	}
	dead = true;
}

bool Entity_Living::IsAlive()
{
	return !dead;
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
		TaskTree[i]->DeleteFailTree();
		delete TaskTree[i];
	}
	TaskTree.clear();
}

void Entity_Living::startTaskQueue()
{
	if (TaskTree.size() > 0) {
		for (auto p : TaskTree[taskTreeIndex]->Branch)
		{
			p->Setup(this, this);
			p->Reset();
		}
		idle = false;
	}
}

void Entity_Living::startTaskQueue(Entity_Living* issuer)
{
	for (auto a: TaskTree[taskTreeIndex]->Branch)
	{
		a->Setup(this, issuer);
		a->Reset();
	}
	idle = false;
}

bool Entity_Living::holdingItem(int id, Entity** outEntity = nullptr)
{
	for (int j = 0; j < heldItems.size(); j++) {
		if (id == heldItems[j]->GetID()) {
			*outEntity = heldItems[j];
			return true;
		}
	}
	return false;
}