#pragma once
#include "Entity_Animal.h"
class Entity_Pet : public Entity_Animal
{
public:
	Entity_Pet(wchar_t EntityID, sf::Vector2i spritePos, wchar_t MeatID, TaskManager* manager, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity_Animal(EntityID,spritePos,4, MeatID, manager,TargetedBehaviors)
	{
	};
	virtual Entity_Pet* clone() const { return new Entity_Pet(*this); };

};

