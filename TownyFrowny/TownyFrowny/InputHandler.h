 #pragma once
#include <SFML/Graphics.hpp>
#include "Entity_Human.h"
#include "UIHandler.h"
#include "VectorHelper.h"
#include "PathFinder.h"


struct Selection {
	Selection(std::vector< Entity*> entities, std::vector<sf::Vector2i> tiles) {
		this->entities = entities;
		this->tiles = tiles;
		if (tiles[0] == tiles[tiles.size()-1])singletile = true;
	}
	std::vector< Entity*> entities;	
	std::vector<sf::Vector2i> tiles;
	bool markedForDeletion = false;
	bool singletile = false;
};
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
				AssignTasks(ReleventBehaviors[index]);
			}
		}
		if (UiHandler::Instance().GetWindow() == UiHandler::Inventory)
		{
			if (index >= 0 && index < InventoryCollapsed.size()) {
				ReleventHeldItemID = InventoryCollapsed[index].first;
				UiHandler::Instance().SetWindow(UiHandler::Tasks);
				UpdateBehaviorList();
			}
		}
		if (UiHandler::Instance().GetWindow() == UiHandler::Designation)
		{
			if (index >= 0 && index < 4) {
				SelectedDesignationType = (Designation::Type)index;
			}
		}
	}

	void ClearBehaviorList();

	std::vector <Selection*> GetActiveSelections() {
		return Selections;
	}

private:
	void DeleteMarkedSelections();
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

	void AssignTasks(TargetedBehaviorStep index);


	bool isTaskDirectional(Entity::TargetedHumanBehaviors behavior) { //true is closest last
		if(behavior == Entity::Targeted_DestroyWall){
			return true;
		}
		else if (behavior == Entity::Targeted_PickUp) {
			return true;
		}
		return false;
	}
	bool isTaskSingularTile(Entity::TargetedHumanBehaviors behavior) {
		if (behavior == Entity::Targeted_MakeStaircase){
			return true;
		}
		else if (behavior == Entity::Targeted_WalkTo) {
			return true;
		}
		return false;
	}

	void UpdateBehaviorList();
	bool BehaviorListContains(std::vector<TargetedBehaviorStep> steps, Entity::TargetedHumanBehaviors behavior, int variantID);
	bool TargetedBehaviorListContains(std::vector<Entity::TargetedHumanBehaviors> steps, TargetedBehaviorStep behavior); 
	std::vector<Entity::TargetedHumanBehaviors> getEmptyTileBehaviors(sf::Vector2i tilePos);


	Designation::Type SelectedDesignationType;
	std::string getTargetedBehaviorNames(Entity::TargetedHumanBehaviors behaviour, int EntityRefID = 0);
	std::vector<Entity*> InventoryItemsMarkedForUse;
	std::vector<std::pair<int, int>> InventoryCollapsed;
	int ReleventHeldItemID=-1;
	std::vector<TargetedBehaviorStep> ReleventBehaviors;
	int ReleventBehaviorOffset = 0;
	std::vector<targetedTaskStep*> steps;

	//UI selection
	sf::Vector2f selectionBoxScreenStart;
	sf::Vector2f selectionBoxScreenEnd;
	bool Clicking = false;

	std::vector <Selection*> Selections;

	Entity_Human* pilotingPlayer;
	sf::RenderWindow* window;
	PathFinder* pathfinder;
};
