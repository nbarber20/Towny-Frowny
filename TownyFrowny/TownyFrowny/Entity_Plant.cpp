#include "Entity_Plant.h"
#include "EntityHandler.h"

Entity_Plant::Entity_Plant(wchar_t EntityID, sf::Vector2i spritePos, sf::Vector2i GrownSpritePos, int GrowthSteps, int GrowthSpeed, float growthChance, bool showGrowth, std::vector<std::pair<wchar_t, int>> StarterMatierialIDs, std::vector<std::pair<wchar_t, int>> MatierialIDs, std::vector<TargetedHumanBehaviors> TargetedBehaviors):Entity_Container(EntityID,spritePos,TargetedBehaviors)
{
	this->MatierialIDs = MatierialIDs;
	this->StarterMatierialIDs = StarterMatierialIDs;
	this->GrowthSteps = GrowthSteps;
	this->GrowthSpeed = GrowthSpeed;
	this->showGrowth = showGrowth;
	this->GrownSpritePos = GrownSpritePos;
	this->growthChance = growthChance;
}

void Entity_Plant::Initilize(World* worldref)
{
	Entity_Container::Initilize(worldref);
	for (int i = 0; i < StarterMatierialIDs.size(); i++) {
		for (int j = 0; j < StarterMatierialIDs[i].second; j++) {
			AddItemToInventory(EntityHandler::Instance().CreateEntity(StarterMatierialIDs[i].first, GetPosition()));
		}
	}
	StarterMatierialIDs.clear();
	StarterMatierialIDs.shrink_to_fit();
	SetGrowthStep(0,true);
}

void Entity_Plant::SetGrowthStep(int to, bool forceGrow)
{
	if (forceGrow == false) {
		srand(time(NULL));
		float r = rand() % 100;
		if (r > growthChance) {
			LogHandler::Instance().WriteLog(objectName + " failed to grow", GetPosition(), logContext::WORLD);
			return;
		}
	}

	if (to<GrowthSteps&& to>=0) {
		GrowthStep = to;
		if(showGrowth)SetSpriteVariant(spriteX, GrowthStep);
	}

	//FullAge
	if (GrowthStep == GrowthSteps-1) {
		SetSpriteVariant(GrownSpritePos.x, GrownSpritePos.y);
		LogHandler::Instance().WriteLog(objectName + " reached maturity", GetPosition(), logContext::WORLD);
		for (int i = 0; i < MatierialIDs.size(); i++) {
			for (int j = 0; j < MatierialIDs[i].second; j++) {
				AddItemToInventory(EntityHandler::Instance().CreateEntity(MatierialIDs[i].first, GetPosition()));
			}
		}
		MatierialIDs.clear();
		MatierialIDs.shrink_to_fit();
	}
}

void Entity_Plant::Tick()
{
	Entity_Container::Tick();
	if (GrowthStep < GrowthSteps-1) {
		if (age % GrowthSpeed == 0) {
			SetGrowthStep(GrowthStep + 1,false);
		}
	}
	if (heldItems.size() == 0) {
		EntityHandler::Instance().DestroyEntity(this, world, false); //Destroy this plant
	}
}
