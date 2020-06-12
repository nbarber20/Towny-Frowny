#pragma once
#include "Entity.h"
class WorldTile;
struct LightSource;
class Entity_Light : public Entity {
public:
	Entity_Light(wchar_t EntityID, sf::Vector2i spritePos, int lightDistance, int lightLevel, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors);
	~Entity_Light() {};
	void Initilize() override;
	void Tick() override;
	void OnSpawn(World* newworld) override;
	void OnDespawn(World* newworld) override;
	virtual Entity_Light* clone() const { return new Entity_Light(*this); };
	void TurnOn();
	void TurnOff();

	void getTargetedBehaviors(std::vector<TargetedHumanBehaviors>* list, std::vector<TargetedHumanBehaviors>* failList = new std::vector<TargetedHumanBehaviors>) override {
		for (auto b : TargetedBehaviors) {
			if (b == Entity::Targeted_TurnOnLight) {
				if (inworld ==true && lit == false) {
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
			else if(b == Entity::Targeted_TurnOffLight) {
				if (inworld == true && lit == true) {
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
		}
		Entity::getTargetedBehaviors(list, failList);
	}

private:
	sf::Vector2i lastPos;
	std::vector<WorldTile*> tiles;
	int lightDistance;
	int lightLevel;
	LightSource* ls;
	bool lit = false;

	int ticksSinceFlicker = 0;
};