#pragma once
#include "Entity.h"

class Entity_Food : public Entity {
public:
	Entity_Food(wchar_t EntityID, sf::Vector2i spritePos, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID, spritePos, worldref,TargetedBehaviors)
	{
	};
	~Entity_Food() {};
	void Tick() override;
	bool IsSpoiled();
	virtual Entity_Food* clone() const { return new Entity_Food(*this); };


private:
	bool spoiled = false;
	short spoilAge = 200;
};