#pragma once
#include "Entity.h"
class Entity_Seed : public Entity
{
public:
	Entity_Seed(wchar_t EntityID, sf::Vector2i spritePos, int plantID, std::vector<TargetedHumanBehaviors> TargetedBehaviors) :Entity(EntityID,spritePos,TargetedBehaviors) {
		this->plantID = plantID;
	};
	virtual Entity_Seed* clone() const {return new Entity_Seed(*this);};
	void getTargetedBehaviors(std::vector<TargetedHumanBehaviors>* list, std::vector<TargetedHumanBehaviors>* failList = new std::vector<TargetedHumanBehaviors>) override {
		for (auto b : TargetedBehaviors) {
			if (b == Entity::Targeted_PlantSeed) {
				if (parent != nullptr)
				{
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
		}
		Entity::getTargetedBehaviors(list, failList);
	}
	int plantID;
};

