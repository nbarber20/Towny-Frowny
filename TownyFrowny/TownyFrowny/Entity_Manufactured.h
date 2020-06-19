#pragma once
#include "Entity.h"
class Entity_Manufactured : public Entity {
public:
	Entity_Manufactured(wchar_t EntityID, sf::Vector2i spritePos, bool doesRotate, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID, spritePos, TargetedBehaviors)
	{
		this->doesRotate = doesRotate;
	};
	virtual Entity_Manufactured* clone() const { return new Entity_Manufactured(*this); };

	void Rotate() {
		rotation++;
		if (rotation >= 4)rotation = 0;
		SetSpriteVariant(spriteX, rotation);
	}


	void getTargetedBehaviors(std::vector<TargetedHumanBehaviors>* list, std::vector<TargetedHumanBehaviors>* failList = new std::vector<TargetedHumanBehaviors>) override {
		for (auto b : TargetedBehaviors) {
			if (b == Entity::Targeted_Rotate) {
				if (doesRotate == true) {
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
		}
		Entity::getTargetedBehaviors(list,failList);
	}

private:
	bool doesRotate;
	int rotation = 0;
};