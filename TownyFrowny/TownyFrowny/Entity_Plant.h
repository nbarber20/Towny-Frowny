#pragma once
#include "Entity_Container.h"
#include <vector>
class Entity_Plant : public Entity_Container {
public:
	Entity_Plant(wchar_t EntityID, sf::Vector2i spritePos, sf::Vector2i GrownSpritePos, int GrowthSteps, int GrowthSpeed, float growthChance, bool showGrowth, std::vector<std::pair<wchar_t, int>> StarterMatierialIDs, std::vector<std::pair<wchar_t, int>> MatierialIDs,  std::vector<TargetedHumanBehaviors> TargetedBehaviors);
	void Initilize(World* worldref) override;
	virtual Entity_Plant* clone() const { return new Entity_Plant(*this); };
	void SetGrowthStep(int to, bool forceGrow);
	void Tick() override;
	void getTargetedBehaviors(std::vector<TargetedHumanBehaviors>* list, std::vector<TargetedHumanBehaviors>* failList = new std::vector<TargetedHumanBehaviors>) override {
		for (auto b : TargetedBehaviors) {
			if (b == Entity::Targeted_HarvestCrop|| b == Entity::Targeted_HarvestWood) {
				if (GrowthStep == GrowthSteps-1) { //this IS a mature plant
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
			if (b == Entity::Targeted_UpRoot) {
				if (GrowthStep != GrowthSteps - 1) { //This is not a mature plant
					list->push_back(b);
				}
				else {
					failList->push_back(b);
				}
			}
		}
		Entity_Container::getTargetedBehaviors(list, failList);
	}
private:
	std::vector<std::pair<wchar_t, int>> StarterMatierialIDs;
	std::vector<std::pair<wchar_t, int>> MatierialIDs;
	int GrowthStep = 0;
	int GrowthSteps;
	int GrowthSpeed;
	float growthChance;
	bool showGrowth;
	sf::Vector2i GrownSpritePos;
	bool destri = false;
};