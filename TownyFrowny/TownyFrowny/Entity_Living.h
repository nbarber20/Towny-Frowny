#pragma once
#include "Entity.h"
#include <string>
#include "NameGenerator.h"
#include "Entity_Container.h"
#include "BehaviorBranch.h"
class Task;
class TaskManager;
/*

struct BehaviorBranch {

	BehaviorBranch(std::vector<Task*> Branch, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = {};
		this->retries = 0;
		this->taskManager = taskManager;
	}

	BehaviorBranch(std::vector<Task*> Branch,int retries, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = {};
		this->retries = retries;
		this->taskManager = taskManager;
	}
	BehaviorBranch(std::vector<Task*> Branch, std::vector <BehaviorBranch*>  FailTree, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = FailTree;
		this->retries = 0;
		this->taskManager = taskManager;
	}

	BehaviorBranch(std::vector<Task*> Branch, std::vector <BehaviorBranch*>  FailTree , int retries, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = FailTree;
		this->retries = retries;
		this->taskManager = taskManager;
	}

	~BehaviorBranch() {
		for (int i = 0; i < Branch.size();i++) {
			taskManager->DeleteTask(Branch[i]);
		}
		for (int j = 0; j < FailTree.size(); j++) {
			delete this->FailTree[j];
		}
		this->Branch.clear();
		this->FailTree.clear();
	}

	std::vector<Task*> Branch;
	int retries;
	std::vector <BehaviorBranch*>  FailTree;
	TaskManager* taskManager;
};
*/

class Entity_Living : public Entity_Container {
public:
	Entity_Living(wchar_t EntityID, sf::Vector2i spritePos, TaskManager* manager, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity_Container(EntityID,spritePos, worldref,TargetedBehaviors)
	{
		this->taskManager = manager; 
		canMove = true;
	}; 	
	~Entity_Living() {};
	void Tick() override;
	virtual void TaskComplete(Task* t);
	virtual void TaskFail(Task* t);
	virtual void OverwriteTasks(std::vector<Task*> newTree, Entity_Living* issuer);

	std::vector<Entity*>* GetBodilyInventory();
	void AddItemToBodilyInventory(Entity* e);
	void RemoveItemFromInventory(Entity* e) override;

	void Eat(Entity* e);
	void AttackEntity(Entity_Living* e, int dammage);
	void TakeDamage(int ammount, Entity_Living* source);
	virtual void Die(Entity_Living* source);
	bool IsAlive();
	void DropItem(Entity* item);
	virtual std::string GetIndividualName();
	virtual void clearAllTasks();
	void startTaskQueue();
	void startTaskQueue(Entity_Living* issuer);

	void getTargetedBehaviors(std::vector<TargetedHumanBehaviors>* list, std::vector<TargetedHumanBehaviors>* failList = new std::vector<TargetedHumanBehaviors>) override {
		for (auto b : TargetedBehaviors) {
			if (b == Entity::Targeted_Attack) {
				if (dead == false) {
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
			else if (b == Entity::Targeted_LootBody) {
				if (dead == true&& bodilyInventory.size()>0){
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
		}		
		Entity::getTargetedBehaviors(list,failList);
	}
	virtual Entity_Living* clone() const { return new Entity_Living(*this); };
	void CraftItem(int id); 

	bool GetCraftable(std::vector<std::vector<int>> r);
	std::vector<int> GetAllCraftable();

protected:
	bool ConsumeCraftingItem(std::vector<int > recipeItem);
	bool holdingItem(std::vector<Entity*>* List, std::vector<int > recipeItem); 
	std::vector< std::vector<int>> getRecipe(int id);
	std::vector < std::pair<int, std::vector<std::vector<int>>>> getRecipes();

	std::string individualName;
	short hungerLevel = 0;
	bool idle = true;
	int hp = 100;
	int decompositionLvl;
	bool dead = false;
	std::vector<Entity*> bodilyInventory;

	//Tasks
	int taskIndex = 0;
	int taskTreeIndex = 0;
	std::vector <BehaviorBranch*> TaskTree;
	TaskManager* taskManager;
};

