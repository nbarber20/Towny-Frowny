#pragma once
#include "Entity_Living.h"
#include "TileManager.h"
#include "World.h"


class InputHandler;
struct targetedTaskStep;
struct TargetedBehaviorStep;
class Entity_Human : public Entity_Living {
public:
	

	Entity_Human(wchar_t EntityID, sf::Vector2i spritePos, InputHandler* inputHandler, TaskManager* manager, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity_Living(EntityID, spritePos, manager, worldref, TargetedBehaviors)
	{
		volume = 6;
		this->inputHandler = inputHandler;
	};
	void SetBehavoir(Entity::Behaviors b);
	void SetTargetedBehavior(TargetedBehaviorStep b, targetedTaskStep* step, bool doOverride);
	void Initilize() override;
	void Tick() override;
	void TaskComplete(Task* t) override;
	void TaskFail(Task* t) override;
	void clearAllTasks() override;
	void NewTaskList(std::vector<Task*> SetCurrent);
	void SetAI(bool to) {
		if (to == true) {
			clearAllTasks();
		}
		UseAI = to;
	};
	bool GetAI() {
		return UseAI;
	};
	bool isTargetedBehaviorPossible(TargetedHumanBehaviors behaviour) { //Check if the player is capable of doing this
		return true;
	}
	virtual Entity_Human* clone() const { return new Entity_Human(*this); };
	void DestroyTaskSteps();
	bool MoveToTile(short dx, short dy) override;
private:

	std::vector<std::pair<Entity::Behaviors, float>> behaviorWeights = {
		std::make_pair(Entity::GatherWood, 0.4),
		std::make_pair(Entity::SlaughterAnimal, 0.4),
		std::make_pair(Entity::Eat, 0.75),
		std::make_pair(Entity::Pase, 1.0),
		std::make_pair(Entity::Wait, 1.0),
	};

	InputHandler* inputHandler;
	bool UseAI = true;
	std::vector<targetedTaskStep*> currentTargetedTasks;
};

