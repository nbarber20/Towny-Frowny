#pragma once
#include "Entity.h"
class Entity_Door : public Entity {
public:
	Entity_Door(wchar_t EntityID, sf::Vector2i spritePos, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID, spritePos, worldref, TargetedBehaviors)
	{
	};
	virtual Entity_Door* clone() const { return new Entity_Door(*this); };
	virtual void Rotate();
	virtual void UseDoor();
	virtual void OpenDoor();
	virtual void CloseDoor();
	void OnSpawn(World* newworld) override;
	void Tick() override;
protected:
	bool horizontal = true;
	int rotation = 0;
	int closedVolume = 10;
};