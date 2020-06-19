#pragma once
#include "Entity.h"
class Entity_Fence : public Entity {
public:
	Entity_Fence(wchar_t EntityID, sf::Vector2i spritePos, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID, spritePos, TargetedBehaviors)
	{
		volume = 9;
	};
	virtual Entity_Fence* clone() const { return new Entity_Fence(*this); };
	void UpdateFencePost();
	void OnSpawn(World* newworld) override;
	void OnDespawn(World* newworld,bool doDropItems) override;
private:
	int rotation = 0;
	int closedVolume = 10;
};