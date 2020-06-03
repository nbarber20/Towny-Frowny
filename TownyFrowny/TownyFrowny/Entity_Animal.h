#pragma once
#include "Entity_Living.h"
class EntityHandler;
class Entity_Animal : public Entity_Living {
public:
	Entity_Animal(wchar_t EntityID, sf::Vector2i spritePos, int volume, wchar_t MeatID, TaskManager* manager, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors); 	
	void Tick() override;
	void Die(Entity_Living* source) override;
	void InitilizeSpawnInventory() override;
	virtual Entity_Animal* clone() const { return new Entity_Animal(*this); };
	void getTargetedBehaviors(std::vector<TargetedHumanBehaviors>* list, std::vector<TargetedHumanBehaviors>* failList = new std::vector<TargetedHumanBehaviors>) override {
		for (auto b : TargetedBehaviors) {
			if (b == Entity::Targeted_SlaughterAnimal) {
				if (dead == false) {
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
		}
		Entity_Living::getTargetedBehaviors(list,failList);
	}
private:
	std::vector<std::pair<Entity::Behaviors, float>> behaviorWeights = {
		std::make_pair(Entity::Wait, 1),
		std::make_pair(Entity::Pase, 0.5),
		std::make_pair(Entity::Roam, 0.5),
		std::make_pair(Entity::Graze, 0.5),
		std::make_pair(Entity::Eat, 0.5),
	};
	void SetBehavoir(Entity::Behaviors b);
	wchar_t MeatID;
};