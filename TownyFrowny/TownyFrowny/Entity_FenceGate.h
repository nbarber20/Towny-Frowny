#pragma once
#include "Entity_Door.h"
class Entity_FenceGate : public Entity_Door {
public:
	Entity_FenceGate(wchar_t EntityID, sf::Vector2i spritePos, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity_Door(EntityID, spritePos, worldref, TargetedBehaviors)
	{
	};
	virtual Entity_FenceGate* clone() const { return new Entity_FenceGate(*this); };
	void Rotate() override;
	void OnSpawn(World* newworld) override;
	void Tick() override;
};