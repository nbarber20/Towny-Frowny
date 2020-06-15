#pragma once
#include <SFML/Graphics.hpp>
#include "Entity_Human.h"
#include "VectorHelper.h"
#include "PathFinder.h"

struct TargetedBehaviorStep {
	TargetedBehaviorStep(Entity::TargetedHumanBehaviors behavior, int variantID = 0) {
		this->behavior = behavior;
		this->variantID = variantID;
		this->Pos = nullptr;
		this->entity = nullptr;
	}
	TargetedBehaviorStep(Entity* entity, sf::Vector2i* Pos, Entity::TargetedHumanBehaviors behavior, int variantID = 0) {
		this->behavior = behavior;
		this->variantID = variantID;
		this->Pos = Pos;
		this->entity = entity;
	}
	Entity::TargetedHumanBehaviors behavior;
	int variantID;
	sf::Vector2i* Pos;
	Entity* entity;
};

class PlayerController {
public:
	struct Selection {
		Selection(std::vector< Entity*> entities, std::vector<sf::Vector2i> tiles) {
			this->entities = entities;
			this->tiles = tiles;
			if (tiles[0] == tiles[tiles.size() - 1])singletile = true;
		}
		std::vector< Entity*> entities;
		std::vector<sf::Vector2i> tiles;
		bool markedForDeletion = false;
		bool singletile = false;
	};


	PlayerController(PathFinder* pathfinder, sf::RenderWindow* window) {
		this->window = window;
		this->pathfinder = pathfinder;
		ReleventBehaviors.push_back(Entity::TargetedHumanBehaviors::Targeted_ClearAll);
	};
	void GetInput(sf::Event e);
	void SetPilotingPlayer(Entity_Human* to);
	Entity_Human* GetPilotingPlayer();
	void OnTaskUIClick(int index);
	void ClearBehaviorList();
	void UpdateBehaviorList();
	void SetReleventHeldItemID(int id);
	void DeleteMarkedSelections();
	std::vector<std::string> GetTaskNames();
	std::vector <Selection*> Selections;
private:
	void AssignTasks(TargetedBehaviorStep index);
	std::vector<sf::Vector2i> sortArr(std::vector<sf::Vector2i> arr, int n, sf::Vector2i p);
	std::vector<Entity*> sortEntityArr(std::vector<Entity*> arr, int n, sf::Vector2i p);
	bool isTaskDirectional(Entity::TargetedHumanBehaviors behavior);
	bool isTaskSingularTile(Entity::TargetedHumanBehaviors behavior);
	bool BehaviorListContains(std::vector<TargetedBehaviorStep> steps, Entity::TargetedHumanBehaviors behavior, int variantID);
	bool TargetedBehaviorListContains(std::vector<Entity::TargetedHumanBehaviors> steps, TargetedBehaviorStep behavior);
	std::vector<Entity::TargetedHumanBehaviors> getEmptyTileBehaviors(sf::Vector2i tilePos);

	Designation::Type SelectedDesignationType;
	std::string getTargetedBehaviorNames(Entity::TargetedHumanBehaviors behaviour, int EntityRefID = 0);
	std::vector<Entity*> InventoryItemsMarkedForUse;
	int ReleventHeldItemID = -1;
	std::vector<TargetedBehaviorStep> ReleventBehaviors;
	int ReleventBehaviorOffset = 0;
	std::vector<TargetedBehaviorStep*> steps;
	//UI selection
	sf::Vector2f selectionBoxScreenStart;
	sf::Vector2f selectionBoxScreenEnd;
	bool Clicking = false;
	Entity_Human* pilotingPlayer;
	sf::RenderWindow* window;
	PathFinder* pathfinder;
};
