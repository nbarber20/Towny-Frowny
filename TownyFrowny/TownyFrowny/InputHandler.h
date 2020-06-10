 #pragma once
#include <SFML/Graphics.hpp>
#include "Entity_Human.h"
#include "UIHandler.h"
#include "VectorHelper.h"
#include "PathFinder.h"
struct targetedTaskStep {
	targetedTaskStep(Entity* entity, sf::Vector2i* Pos) {
		this->Pos = Pos;
		this->entity = entity;
	}
	sf::Vector2i* Pos;
	Entity* entity;
};

struct TargetedBehaviorStep {
	TargetedBehaviorStep(Entity::TargetedHumanBehaviors behavior, int variantID = 0) {
		this->behavior = behavior;
		this->variantID = variantID;
	}
	Entity::TargetedHumanBehaviors behavior;
	int variantID;
};

class InputHandler {


public:
	
	InputHandler(PathFinder* pathfinder, sf::RenderWindow* window) {
		this->window = window;
		this->pathfinder = pathfinder;
	};

	int movementThreshold;

	void GetInput(sf::Event e);

	void SetPilotingPlayer(Entity_Human* to) {
		if (pilotingPlayer != nullptr) {
			pilotingPlayer->SetAI(true);
		}
		pilotingPlayer = to;
		pilotingPlayer->SetAI(false);
	}

	Entity_Human* GetPilotingPlayer() {
		return pilotingPlayer;
	}

	void OnTaskUIClick(int index) {
		if (UiHandler::Instance().GetWindow() == UiHandler::Tasks) 
		{
			if (index >= 0 && index < ReleventBehaviors.size()) {
				AssignTasks(index);
			}
		}
		if (UiHandler::Instance().GetWindow() == UiHandler::Inventory)
		{
			if (index >= 0 && index < InventoryList.size()) {
				ReleventHeldItemID = InventoryCollapsed[index].first;
			}
		}
		if (UiHandler::Instance().GetWindow() == UiHandler::Designation)
		{
			if (index >= 0 && index < 4) {
				SelectedDesignationType = (Designation::Type)index;
			}
		}
	}

	void ClearBehaviorList() {
		for (int i = 0; i < steps.size(); i++) {
			delete steps[i];
		}
		steps.clear();
		ReleventBehaviorOffset = 0;
	}

private:
	std::vector<sf::Vector2i> sortArr(std::vector<sf::Vector2i> arr, int n, sf::Vector2i p)
	{
		std::vector<std::pair<float, std::pair<int, int>>>vp;
		for (int i = 0; i < n; i++) {
			int dist = VectorHelper::GetMagnitude(p - arr[i]);
			vp.push_back(std::make_pair(dist, std::make_pair(arr[i].x, arr[i].y)));
		}
		sort(vp.begin(), vp.end());
		std::vector < sf::Vector2i> temp;
		for (int i = 0; i < vp.size(); i++) {
			temp.push_back(sf::Vector2i(vp[i].second.first, vp[i].second.second));
		}
		return temp;
	}

	std::vector<Entity*> sortEntityArr(std::vector<Entity*> arr, int n, sf::Vector2i p)
	{
		std::vector<std::pair<float, int>>vp;
		for (int i = 0; i < n; i++) {
			int dist = VectorHelper::GetMagnitude(p - arr[i]->GetPosition());
			vp.push_back(std::make_pair(dist, i));
		}
		sort(vp.begin(), vp.end());
		std::vector <Entity*> temp;
		for (int i = 0; i < vp.size(); i++) {
			temp.push_back(arr[vp[i].second]);
		}
		return temp;
	}

	void AssignTasks(int index) {

		if (ReleventBehaviors[index].behavior == Entity::Targeted_ClearAll) {
			pilotingPlayer->SetTargetedBehavior(Entity::Targeted_ClearAll, nullptr, false);
		}

		//Reorder list if task is directional
		if (isTaskDirectional(ReleventBehaviors[index].behavior)) {
			for (int i = 0; i < selectedPositions.size(); i++) {
				std::vector<sf::Vector2i> walksteps;
				if (pathfinder->FindPath(pilotingPlayer, 1, walksteps, pilotingPlayer->GetPosition(), selectedPositions[i], pilotingPlayer->getWorld())){
					selectedPositions = sortArr(selectedPositions, selectedPositions.size(), selectedPositions[i]);
					selectedEntity = sortEntityArr(selectedEntity, selectedEntity.size(), selectedPositions[i]);
					break;
				}
			}
		}

		for (auto e : selectedEntity) {
			std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
			e->getTargetedBehaviors(&tempBehaviors);
			if (TargetedBehaviorListContains(tempBehaviors,ReleventBehaviors[index])) { // in list
				steps.push_back(new targetedTaskStep(e, new sf::Vector2i(e->GetPosition())));
			}
		}

		for (int pos = 0; pos < selectedPositions.size(); pos++) {
			std::vector<Entity::TargetedHumanBehaviors> tempEmptyTileBehaviors = getEmptyTileBehaviors(selectedPositions[pos]);
			for (int j = 0; j < tempEmptyTileBehaviors.size(); j++) {
				if (ReleventBehaviors[index].behavior == tempEmptyTileBehaviors[j]) { //this task can be used on empty tiles
					steps.push_back(new targetedTaskStep(nullptr, new sf::Vector2i(selectedPositions[pos])));
					break;
				}
			}
		}

		if (ReleventHeldItemID != -1) {
			int posIndex = 0;
			for (int i = 0; i < InventoryList.size(); i++) {
				if(InventoryList[i].first == true) continue;
				//TODO: this sometimes breaks
				if (InventoryList[i].second->GetID() == ReleventHeldItemID) {
					std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
					InventoryList[i].second->getTargetedBehaviors(&tempBehaviors);
					if (TargetedBehaviorListContains(tempBehaviors, ReleventBehaviors[index])){ // in list
						if (posIndex < selectedPositions.size()) {
							InventoryList[i].first = true;
							steps.push_back(new targetedTaskStep(InventoryList[i].second, new sf::Vector2i(selectedPositions[posIndex])));
							posIndex++;
						}
					}					
				}
			}
		}

		for (int l = ReleventBehaviorOffset; l < steps.size(); l++) {
			pilotingPlayer->SetTargetedBehavior(ReleventBehaviors[index], steps[l], false);
		}
		ReleventBehaviorOffset = steps.size();
		selectedPositions.clear();
		selectedEntity.clear();
	}


	bool isTaskDirectional(Entity::TargetedHumanBehaviors behavior) { //true is closest last
		if(behavior == Entity::Targeted_DestroyWall){
			return true;
		}
		else if (behavior == Entity::Targeted_PickUp) {
			return true;
		}
		return false;
	}

	bool BehaviorListContains(std::vector<TargetedBehaviorStep> steps, Entity::TargetedHumanBehaviors behavior, int variantID = 0);
	bool TargetedBehaviorListContains(std::vector<Entity::TargetedHumanBehaviors> steps, TargetedBehaviorStep behavior);

	std::vector<Entity::TargetedHumanBehaviors> getEmptyTileBehaviors(sf::Vector2i tilePos) {
		std::vector<Entity::TargetedHumanBehaviors> List = {
			Entity::Targeted_WalkTo,
			Entity::Targeted_DestroyWall,
			Entity::Targeted_DestroyFloor,
			Entity::Targeted_MakeStaircase,
			Entity::Targeted_UseStaircase,
		};
		WorldTile* tile = pilotingPlayer->getWorld()->GetWorldTile(tilePos);
		std::vector<Entity::TargetedHumanBehaviors> temp;
		for (auto b : List) {
			if (b == Entity::Targeted_DestroyWall) {
				if (tile->wallTile != nullptr) temp.push_back(b);
			}
			else if (b == Entity::Targeted_DestroyFloor) {
				if (tile->groundTile->GetTileID() != 12 && tile->groundTile->GetTileID() != 13&&tile->groundTile->GetTileID() != 0) temp.push_back(b);
			}
			else if (b == Entity::Targeted_MakeStaircase) {
				if (tile->groundTile->GetTileID() != 12 && tile->groundTile->GetTileID() != 13)temp.push_back(b);
			}
			else if (b == Entity::Targeted_UseStaircase) {
				if (tile->groundTile->GetTileID() == 12 || tile->groundTile->GetTileID() == 13)temp.push_back(b);
			}
			else { //this behavior has no specification
				temp.push_back(b);
			}
		}
		return temp;
	}


	std::string getTargetedBehaviorNames(Entity::TargetedHumanBehaviors behaviour) {
		switch (behaviour)
		{
		case Entity::Targeted_ClearAll:
			return "Clear All Tasks";
		case Entity::Targeted_PickUp:
			return "Pick Up";
		case Entity::Targeted_HarvestWood:
			return "HarvestWood";
		case Entity::Targeted_SlaughterAnimal:
			return "Slaughter";
		case Entity::Targeted_WalkTo:
			return "Walk To";
		case Entity::Targeted_ConstructWall:
			return "Construct Wall";
		case Entity::Targeted_DestroyWall:
			return "Destroy Wall";
		case Entity::Targeted_ConstructFloor:
			return "Lay Floor";
		case Entity::Targeted_DestroyFloor:
			return "Remove Floor";
		case Entity::Targeted_LootBody:
			return "Loot Body";
		case Entity::Targeted_Attack:
			return "Attack";
		case Entity::Targeted_Rotate:
			return "Rotate";
		case Entity::Targeted_DropItem:
			return "Drop Item";
		case Entity::Targeted_MakeStaircase:
			return "Make Staircase";
		case Entity::Targeted_UseStaircase:
			return "Use Staircase";
		default:
			return "Null Task";
		}
	}

	std::vector<int> CraftingList;
	std::vector<std::pair<bool,Entity*>> InventoryList;
	std::vector<std::pair<int, int>> InventoryCollapsed;


	int ReleventHeldItemID=-1;
	std::vector<TargetedBehaviorStep> ReleventBehaviors;
	int ReleventBehaviorOffset = 0;


	sf::Vector2f selectionBoxScreenStart;
	sf::Vector2f selectionBoxScreenEnd;
	bool Clicking = false;

	std::vector <sf::Vector2i> selectedPositions;
	std::vector < Entity*> selectedEntity;

	Entity_Human* pilotingPlayer;
	//World* world;
	sf::RenderWindow* window;

	std::vector<targetedTaskStep*> steps;
	//int stepIndexOffset = 0;

	PathFinder* pathfinder;

	Designation::Type SelectedDesignationType;
};
