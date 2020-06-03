#pragma once
#include "Entity.h"
class Entity_Material : public Entity {
public:
	Entity_Material(wchar_t EntityID, sf::Vector2i spritePos, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID,spritePos, worldref,TargetedBehaviors)
	{
	};
	virtual Entity_Material* clone() const { return new Entity_Material(*this); };

};