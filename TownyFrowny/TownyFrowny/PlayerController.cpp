#include "PlayerController.h"
#include "EntityHandler.h"
#include "Recipies.h"
#include "Entity_Crafter.h"
void PlayerController::GetInput(sf::Event e)
{
	if (e.type == sf::Event::KeyPressed)
	{
		if (e.key.code == sf::Keyboard::W)
		{
			pilotingPlayer->StepTiles(0, -1);
		}
		if (e.key.code == sf::Keyboard::S)
		{
			pilotingPlayer->StepTiles(0, 1);
		}
		if (e.key.code == sf::Keyboard::A)
		{
			pilotingPlayer->StepTiles(-1, 0);
		}
		if (e.key.code == sf::Keyboard::D)
		{
			pilotingPlayer->StepTiles(1, 0);
		}
	}
	int task = -1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		task = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		task = 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		task = 2;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
	{
		task = 3;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
	{
		task = 4;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
	{
		task = 5;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
	{
		task = 6;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8))
	{
		task = 7;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
	{
		task = 8;
	}
	if (task >= 0 && task < ReleventBehaviors.size()) {
		AssignTasks(ReleventBehaviors[task]);
	}

	
	/*
	if (UiHandler::Instance().GetWindow() == UiHandler::Inventory) {
		std::vector<std::string> names;
		std::vector<Entity*> inventory = *pilotingPlayer->GetInventory();
		InventoryCollapsed.clear();
		for (int i = 0; i < inventory.size(); i++) {
			bool found = false;
			for (int j = 0; j < InventoryCollapsed.size(); j++) {
				if (InventoryCollapsed[j].first == inventory[i]->GetID()) {
					InventoryCollapsed[j].second++;
					found = true;
				}
			}
			if (found == false) {
				InventoryCollapsed.push_back(std::make_pair(inventory[i]->GetID(), 1));
			}
		}
		for (int j = 0; j < InventoryCollapsed.size(); j++) {
			names.push_back(EntityHandler::Instance().GetEntityNameByID(InventoryCollapsed[j].first) + " x" + std::to_string(InventoryCollapsed[j].second));
		}
		UiHandler::Instance().UpdateTaskList(names);
	}
	if (UiHandler::Instance().GetWindow() == UiHandler::Designation) {
		std::vector<std::string> names;
		names = {
			"GeneralStoreage",
			"FoodStorage",
			"MaterialStorage",
			"GeneralRoom",
		};
		UiHandler::Instance().UpdateTaskList(names);
	}
	*/
}

void PlayerController::SetPilotingPlayer(Entity_Human* to)
{
	if (pilotingPlayer != nullptr) {
		pilotingPlayer->SetAI(true);
	}
	pilotingPlayer = to;
	pilotingPlayer->SetAI(false);
}

Entity_Human* PlayerController::GetPilotingPlayer()
{
	return pilotingPlayer;
}

void PlayerController::OnTaskUIClick(int index)
{
	if (index >= 0 && index < ReleventBehaviors.size()) {
		AssignTasks(ReleventBehaviors[index]);
	}
}

void PlayerController::ClearBehaviorList()
{
	for (int i = 0; i < steps.size(); i++) {
		delete steps[i];
	}
	steps.clear();
	ReleventBehaviorOffset = 0;
	DeleteMarkedSelections();
	UpdateBehaviorList();
}

void PlayerController::DeleteMarkedSelections()
{
	pilotingPlayer->getWorld()->ClearSelections();
	std::vector <Selection*> SelectionsToDelete;
	for (int i = 0; i < Selections.size(); i++) {
		if (Selections[i]->markedForDeletion) {
			SelectionsToDelete.push_back(Selections[i]);
			//clear selected
			for (auto e : Selections[i]->entities) {
				if (EntityHandler::Instance().IsEntityValid(e)) {
					e->selected = false;
					e->getWorld()->tileToTick.push_back(e->getWorld()->GetWorldTile(e->GetPosition()));
				}
			}
		}
	}
	for (int j = 0; j < SelectionsToDelete.size(); j++) {
		Selections.erase(std::remove(Selections.begin(), Selections.end(), SelectionsToDelete[j]), Selections.end());
	}
	SelectionsToDelete.clear();
	for (int i = 0; i < Selections.size(); i++) {
		pilotingPlayer->getWorld()->SelectArea(std::make_pair(Selections[i]->tiles[0], Selections[i]->tiles[Selections[i]->tiles.size() - 1]));
	}
}

std::vector<std::string> PlayerController::GetTaskNames()
{
	std::vector<std::string> names;
	for (int i = 0; i < ReleventBehaviors.size(); i++) {
		names.push_back(std::to_string(i+1)+". " + getTargetedBehaviorNames(ReleventBehaviors[i].behavior, ReleventBehaviors[i].variantID));
	}
	return names;
}

void PlayerController::AssignTasks(TargetedBehaviorStep behaviorStep)
{

	if (behaviorStep.behavior == Entity::Targeted_ClearAll) {
		steps.push_back(new TargetedBehaviorStep(Entity::Targeted_ClearAll));
		pilotingPlayer->SetTargetedBehavior(steps[0]);
		Selections.clear();
		ClearBehaviorList();
	}

	for (auto selection : Selections) {
		if (selection->markedForDeletion == true)continue;
		//Reorder list if task is directional
		if (isTaskDirectional(behaviorStep.behavior)) {
			for (int i = 0; i < selection->tiles.size(); i++)
			{
				std::vector<sf::Vector2i> walksteps;
				if (pathfinder->FindPath(pilotingPlayer, 1, walksteps, pilotingPlayer->GetPosition(), selection->tiles[i], pilotingPlayer->getWorld())) {
					selection->tiles = sortArr(selection->tiles, selection->tiles.size(), selection->tiles[i]);
					break;
				}
			}
			for (int j = 0; j < selection->entities.size(); j++)
			{
				std::vector<sf::Vector2i> walksteps;
				if (pathfinder->FindPath(pilotingPlayer, 1, walksteps, pilotingPlayer->GetPosition(), selection->entities[j]->GetPosition(), pilotingPlayer->getWorld())) {
					selection->entities = sortEntityArr(selection->entities, selection->entities.size(), selection->entities[j]->GetPosition());
					break;
				}
			}
		}
		//Get all steps for entities
		for (auto e : selection->entities) {
			if (EntityHandler::Instance().IsEntityValid(e)) {
				//Does this task variant match this entity? Otherwise skip it.
				if (behaviorStep.behavior != Entity::Targeted_Craft) {
					if (getTargetedBehaviorNames(behaviorStep.behavior, behaviorStep.variantID) !=
						getTargetedBehaviorNames(behaviorStep.behavior, e->GetID())) {
						continue;
					}
				}
				std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
				e->getTargetedBehaviors(&tempBehaviors);
				if (TargetedBehaviorListContains(tempBehaviors, behaviorStep)) { // in list
					steps.push_back(new TargetedBehaviorStep(e, new sf::Vector2i(e->GetPosition()),behaviorStep.behavior,behaviorStep.variantID));
					selection->markedForDeletion = true;
				}
			}
		}
		//Get all steps for empty tiles
		for (auto tile : selection->tiles)
		{
			if (selection->singletile == false) {
				if (isTaskSingularTile(behaviorStep.behavior) == true) {
					continue;
				}
			}
			std::vector<Entity::TargetedHumanBehaviors> tempEmptyTileBehaviors = getEmptyTileBehaviors(tile);
			for (int j = 0; j < tempEmptyTileBehaviors.size(); j++) {
				if (behaviorStep.behavior == tempEmptyTileBehaviors[j]) { //this task can be used on empty tiles
					steps.push_back(new TargetedBehaviorStep(nullptr, new sf::Vector2i(tile), behaviorStep.behavior, behaviorStep.variantID));
					selection->markedForDeletion = true;
					break;
				}
			}
			//Item relevant tasks
			if (ReleventHeldItemID != -1) {
				std::vector<Entity*> inv = *pilotingPlayer->GetInventory();
				for (int item = 0; item < inv.size(); item++) {
					if (inv[item]->GetID() == ReleventHeldItemID) {
						bool unused = true;
						for (int i = 0; i < InventoryItemsMarkedForUse.size(); i++) {
							if (InventoryItemsMarkedForUse[i] == inv[item]) {
								unused = false;
								break;
							}
						}
						if (unused == true)
						{
							InventoryItemsMarkedForUse.push_back(inv[item]);
							std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
							inv[item]->getTargetedBehaviors(&tempBehaviors);
							if (TargetedBehaviorListContains(tempBehaviors, behaviorStep)) { // in list
								steps.push_back(new TargetedBehaviorStep(inv[item], new sf::Vector2i(tile), behaviorStep.behavior, behaviorStep.variantID));
								selection->markedForDeletion = true;
							}
							break;
						}
					}
				}
			}
		}
	}
	//Send Behaviors to player
	for (int l = ReleventBehaviorOffset; l < steps.size(); l++) {
		pilotingPlayer->SetTargetedBehavior(steps[l]);
	}
	ReleventBehaviorOffset = steps.size();
}

std::vector<sf::Vector2i> PlayerController::sortArr(std::vector<sf::Vector2i> arr, int n, sf::Vector2i p)
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

std::vector<Entity*> PlayerController::sortEntityArr(std::vector<Entity*> arr, int n, sf::Vector2i p)
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

void PlayerController::UpdateBehaviorList()
{

	//Fill the list of behaviors will all possible behaviors
	ReleventBehaviors.clear();
	ReleventBehaviors.push_back(Entity::TargetedHumanBehaviors::Targeted_ClearAll);
	for (auto selection : Selections) {
		//Tasks for entities
		for (auto e : selection->entities) {
			if (EntityHandler::Instance().IsEntityValid(e)) {
				Entity_Crafter* crafter = dynamic_cast<Entity_Crafter*>(e);
				if (crafter != NULL) { //this is a crafter
					std::vector<int> craftable = crafter->GetCraftable();
					for (int i = 0; i < craftable.size(); i++) {
						if (BehaviorListContains(ReleventBehaviors, Entity::Targeted_Craft, craftable[i]) == false) { // not in list
							ReleventBehaviors.push_back(TargetedBehaviorStep(Entity::Targeted_Craft, craftable[i]));
						}
					}
				}
				else {
					std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
					e->getTargetedBehaviors(&tempBehaviors);
					for (int j = 0; j < tempBehaviors.size(); j++) {
						if (BehaviorListContains(ReleventBehaviors, tempBehaviors[j], e->GetID()) == false) { // not in list
							ReleventBehaviors.push_back(TargetedBehaviorStep(tempBehaviors[j], e->GetID()));
						}
					}
				}
			}
		}

		//Tasks for empty tiles
		for (auto tile : selection->tiles)
		{
			std::vector<Entity::TargetedHumanBehaviors> tempBehaviors = getEmptyTileBehaviors(tile);
			for (int j = 0; j < tempBehaviors.size(); j++) {
				if (BehaviorListContains(ReleventBehaviors, tempBehaviors[j], 0) == false) { // not in list
					if (selection->singletile != true) { //multi tile selection
						if (isTaskSingularTile(tempBehaviors[j]) == false) { //ignore singular tasks
							ReleventBehaviors.push_back(tempBehaviors[j]);
						}
					}
					else {
						ReleventBehaviors.push_back(tempBehaviors[j]);
					}
				}
			}
		}

		//Tasks for held items
		if (ReleventHeldItemID != -1) {
			std::vector<Entity*> inv = *pilotingPlayer->GetInventory();
			for (int i = 0; i < inv.size(); i++) {
				if (inv[i]->GetID() == ReleventHeldItemID) {
					std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
					inv[i]->getTargetedBehaviors(&tempBehaviors);
					for (int j = 0; j < tempBehaviors.size(); j++) {
						if (BehaviorListContains(ReleventBehaviors, tempBehaviors[j], ReleventHeldItemID) == false) { // not in list
							ReleventBehaviors.push_back(TargetedBehaviorStep(tempBehaviors[j], ReleventHeldItemID));
						}
					}
				}
			}
		}
	}	
	UIHandler::Instance().UpdateUI();
}

void PlayerController::SetReleventHeldItemID(int id)
{
	ReleventHeldItemID = id;
}

bool PlayerController::isTaskDirectional(Entity::TargetedHumanBehaviors behavior)
{ //true is closest last
	if (behavior == Entity::Targeted_DestroyWall) {
		return true;
	}
	else if (behavior == Entity::Targeted_PickUp) {
		return true;
	}
	return false;
}

bool PlayerController::isTaskSingularTile(Entity::TargetedHumanBehaviors behavior)
{
	if (behavior == Entity::Targeted_MakeStaircase) {
		return true;
	}
	else if (behavior == Entity::Targeted_WalkTo) {
		return true;
	}
	return false;
}

bool PlayerController::BehaviorListContains(std::vector<TargetedBehaviorStep> steps, Entity::TargetedHumanBehaviors behavior, int variantID)
{
	for (int i = 0; i < steps.size(); i++) {
		if (getTargetedBehaviorNames(behavior, variantID) == getTargetedBehaviorNames(steps[i].behavior, steps[i].variantID)) {
			return true;
		}
	}
	return false;
}

bool PlayerController::TargetedBehaviorListContains(std::vector<Entity::TargetedHumanBehaviors> steps, TargetedBehaviorStep behavior)
{
	for (int i = 0; i < steps.size(); i++) {
		if (steps[i] == behavior.behavior) return true;
	}
	return false;
}

std::vector<Entity::TargetedHumanBehaviors> PlayerController::getEmptyTileBehaviors(sf::Vector2i tilePos)
{
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
			if (tile->groundTile->GetTileID() != 12 && tile->groundTile->GetTileID() != 13 && tile->groundTile->GetTileID() != 0 && tile->tileEntitiesCount == 0) temp.push_back(b);
		}
		else if (b == Entity::Targeted_MakeStaircase) {
			if (tile->groundTile->GetTileID() != 12 && tile->groundTile->GetTileID() != 13 && tile->tileEntitiesCount == 0)temp.push_back(b);
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

std::string PlayerController::getTargetedBehaviorNames(Entity::TargetedHumanBehaviors behaviour, int EntityRefID /*= 0*/)
{
	switch (behaviour)
	{
	case Entity::Targeted_Craft:
		return "Craft " + EntityHandler::Instance().GetEntityNameByID(EntityRefID);
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
		return "Construct " + EntityHandler::Instance().GetEntityNameByID(EntityRefID) + " Wall";
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
		return "Rotate" + EntityHandler::Instance().GetEntityNameByID(EntityRefID);
	case Entity::Targeted_DropItem:
		if (EntityRefID != 0) {
			if (EntityHandler::Instance().isIdInGroup(EntityHandler::ConstructedObject, EntityRefID)) return "Install " + EntityHandler::Instance().GetEntityNameByID(EntityRefID);
		}
		return "Drop " + EntityHandler::Instance().GetEntityNameByID(EntityRefID);
	case Entity::Targeted_MakeStaircase:
		return "Make Staircase";
	case Entity::Targeted_UseStaircase:
		return "Use Staircase";
	case Entity::Targeted_UseDoor:
		return "Use " + EntityHandler::Instance().GetEntityNameByID(EntityRefID);
	case Entity::Targeted_TurnOffLight:
		if (EntityHandler::Instance().GetEntityNameByID(EntityRefID) == "Fire") return "Put Out Fire";
		return "Turn off " + EntityHandler::Instance().GetEntityNameByID(EntityRefID);
	case Entity::Targeted_TurnOnLight:
		if (EntityHandler::Instance().GetEntityNameByID(EntityRefID) == "Fire") return "Light Fire";
		return "Turn On " + EntityHandler::Instance().GetEntityNameByID(EntityRefID);
	default:
		return "Null Task";
	}
}
