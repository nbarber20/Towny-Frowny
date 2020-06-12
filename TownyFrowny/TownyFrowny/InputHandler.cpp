#include "InputHandler.h"
#include "EntityHandler.h"
#include "Recipies.h"
#include "Entity_Crafter.h"
void InputHandler::GetInput(sf::Event e)
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
	if (e.type == sf::Event::MouseButtonPressed)
	{
		if (e.mouseButton.button == sf::Mouse::Left)
		{
			selectionBoxScreenStart = window->mapPixelToCoords(sf::Mouse::getPosition(*window)) / 800.0f;
			if (selectionBoxScreenStart.x * 800 < 800)Clicking = true;
		}
		if (e.mouseButton.button == sf::Mouse::Right)
		{
			selectionBoxScreenStart = window->mapPixelToCoords(sf::Mouse::getPosition(*window)) / 800.0f;
			if (selectionBoxScreenStart.x * 800 < 800)Clicking = true;
			
		}
	}
	selectionBoxScreenEnd = window->mapPixelToCoords(sf::Mouse::getPosition(*window)) / 800.0f;
	if (e.type == sf::Event::MouseButtonReleased)
	{		
		if (Clicking == true && selectionBoxScreenEnd.x * 800 < 800) {
			sf::Vector2i CursorStart;
			sf::Vector2i CursorEnd;
			Camera::Instance().ScreenToWorld(selectionBoxScreenStart, CursorStart);
			Camera::Instance().ScreenToWorld(selectionBoxScreenEnd, CursorEnd);
			CursorStart = VectorHelper::ClampVector(CursorStart, sf::Vector2i(0, 0), sf::Vector2i(pilotingPlayer->getWorld()->GetWorldSize(), pilotingPlayer->getWorld()->GetWorldSize()));
			CursorEnd = VectorHelper::ClampVector(CursorEnd, sf::Vector2i(0, 0), sf::Vector2i(pilotingPlayer->getWorld()->GetWorldSize(), pilotingPlayer->getWorld()->GetWorldSize()));
			sf::Vector2i indexStart;
			indexStart.x = (CursorStart.x < CursorEnd.x) ? CursorStart.x : CursorEnd.x;
			indexStart.y = (CursorStart.y < CursorEnd.y) ? CursorStart.y : CursorEnd.y;

			sf::Vector2i indexEnd;
			indexEnd.x = (CursorStart.x < CursorEnd.x) ? CursorEnd.x : CursorStart.x;
			indexEnd.y = (CursorStart.y < CursorEnd.y) ? CursorEnd.y : CursorStart.y;

			if (UiHandler::Instance().GetWindow() == UiHandler::Tasks) {		
				if (e.mouseButton.button == sf::Mouse::Left) {
					pilotingPlayer->getWorld()->SelectArea(std::make_pair(indexStart, indexEnd));

					//Get All Selected Entities
					std::vector<Entity*> selectedEntities;
					std::vector<sf::Vector2i> selectedTiles;
					for (int x = indexStart.x; x <= indexEnd.x; x++)
					{
						for (int y = indexStart.y; y <= indexEnd.y; y++)
						{
							std::vector<Entity*> list = pilotingPlayer->getWorld()->GetWorldTile(sf::Vector2i(x, y))->tileEntities;
							for (auto e : list) {
								if (e != pilotingPlayer) {
									e->selected = true;
									e->getWorld()->tileToTick.push_back(e->getWorld()->GetWorldTile(e->GetPosition()));
									selectedEntities.push_back(e);
								}
							}
							selectedTiles.push_back(sf::Vector2i(x, y));
						}
					}
					Selections.push_back(new Selection(selectedEntities, selectedTiles));
					UpdateBehaviorList();
				}
				if (e.mouseButton.button == sf::Mouse::Right) {
					bool deletedAny = false;
					for (int x = indexStart.x; x <= indexEnd.x; x++)
					{
						for (int y = indexStart.y; y <= indexEnd.y; y++)
						{
							for (auto selection : Selections) {
								for (auto tile : selection->tiles) {
									if (tile == sf::Vector2i(x, y)) {
										selection->markedForDeletion = true;
										deletedAny = true;
										break;
									}
								}
							}
						}
					}
					if (deletedAny == false) {
						for (auto selection : Selections) {
							selection->markedForDeletion = true;
						}
					}
					DeleteMarkedSelections();
					UpdateBehaviorList();
				}
			}
			else if (UiHandler::Instance().GetWindow() == UiHandler::Designation) {
				if (e.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2i size = sf::Vector2i(indexEnd.x, indexEnd.y) - sf::Vector2i(indexStart.x, indexStart.y);
					pilotingPlayer->getWorld()->NewDesignation(sf::Vector2i(indexStart.x, indexStart.y), size, SelectedDesignationType);
				}
				if (e.mouseButton.button == sf::Mouse::Right)
				{
					for (int x = indexStart.x; x <= indexEnd.x; x++) {
						for (int y = indexStart.y; y <= indexEnd.y; y++) {
							pilotingPlayer->getWorld()->DeleteDesignation(sf::Vector2i(x,y));
						}
					}
				}
			}			
			Clicking = false;
		}
	}

	UiHandler::Instance().DrawSelectionCursor(selectionBoxScreenStart, selectionBoxScreenEnd - selectionBoxScreenStart, Clicking);

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

	if (UiHandler::Instance().GetWindow() == UiHandler::Tasks) {
		if (task >= 0 && task < ReleventBehaviors.size()) {
			AssignTasks(ReleventBehaviors[task]);
		}
	}

	//TODO: make these buttons
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		UiHandler::Instance().SetWindow(UiHandler::Logs);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
	{
		UiHandler::Instance().SetWindow(UiHandler::Tasks);
		UpdateBehaviorList();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
	{
		UiHandler::Instance().SetWindow(UiHandler::Inventory);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
	{
		UiHandler::Instance().SetWindow(UiHandler::Designation);
	}

	if (UiHandler::Instance().GetWindow() == UiHandler::Inventory) {
		std::vector<std::string> names;
		std::vector<Entity*> inventory = *pilotingPlayer->GetInventory();
		InventoryCollapsed.clear();
		for (int i = 0; i < inventory.size(); i++) {			
			bool found = false;
			for (int j = 0; j < InventoryCollapsed.size();j++) {
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
}

void InputHandler::ClearBehaviorList()
{
	for (int i = 0; i < steps.size(); i++) {
		delete steps[i];
	}
	steps.clear();
	ReleventBehaviorOffset = 0;
	DeleteMarkedSelections();
	UpdateBehaviorList();
}

void InputHandler::DeleteMarkedSelections()
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

void InputHandler::AssignTasks(TargetedBehaviorStep behaviorStep)
{

	if (behaviorStep.behavior == Entity::Targeted_ClearAll) {
		pilotingPlayer->SetTargetedBehavior(Entity::Targeted_ClearAll, nullptr, false);
		Selections.clear();
		ClearBehaviorList();
	}

	

	for (auto selection : Selections) {
		if(selection->markedForDeletion==true)continue;
		//Reorder list if task is directional
		if (isTaskDirectional(behaviorStep.behavior)) {
			for (int i=0;i<selection->tiles.size();i++)
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
						steps.push_back(new targetedTaskStep(e, new sf::Vector2i(e->GetPosition())));
						selection->markedForDeletion = true;
					}
			}
		}
		//Get all steps for empty tiles
		for (auto tile : selection->tiles)
		{
			if (selection->singletile==false) {
				if (isTaskSingularTile(behaviorStep.behavior) == true) {
					continue;
				}
			}
			std::vector<Entity::TargetedHumanBehaviors> tempEmptyTileBehaviors = getEmptyTileBehaviors(tile);
			for (int j = 0; j < tempEmptyTileBehaviors.size(); j++) {
				if (behaviorStep.behavior == tempEmptyTileBehaviors[j]) { //this task can be used on empty tiles
					steps.push_back(new targetedTaskStep(nullptr, new sf::Vector2i(tile)));
					selection->markedForDeletion = true;
					break;
				}
			}
			//Item relevant tasks
			if (ReleventHeldItemID != -1) {
				std::vector<Entity*> inv = *pilotingPlayer->GetInventory();
				for (int item = 0; item < inv.size(); item++) {
					bool unused = true;
					for (int i = 0; i < InventoryItemsMarkedForUse.size(); i++) {
						if(InventoryItemsMarkedForUse[i] == inv[item]){
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
							steps.push_back(new targetedTaskStep(inv[item], new sf::Vector2i(tile)));
							selection->markedForDeletion = true;
						}
						break;
					}
				}
			}			
		}
	}

	for (int l = ReleventBehaviorOffset; l < steps.size(); l++) {
		pilotingPlayer->SetTargetedBehavior(behaviorStep, steps[l], false);
	}
	ReleventBehaviorOffset = steps.size();

}

void InputHandler::UpdateBehaviorList()
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

	std::vector<std::string> names;
	for (int i = 0; i < ReleventBehaviors.size(); i++) {
		names.push_back(getTargetedBehaviorNames(ReleventBehaviors[i].behavior, ReleventBehaviors[i].variantID));
	}
	UiHandler::Instance().UpdateTaskList(names);
}

bool InputHandler::BehaviorListContains(std::vector<TargetedBehaviorStep> steps, Entity::TargetedHumanBehaviors behavior, int variantID)
{
	for (int i = 0; i < steps.size(); i++) {
		if (getTargetedBehaviorNames(behavior, variantID) == getTargetedBehaviorNames(steps[i].behavior, steps[i].variantID)) {
			return true;
		}
	}
	return false;
}

bool InputHandler::TargetedBehaviorListContains(std::vector<Entity::TargetedHumanBehaviors> steps, TargetedBehaviorStep behavior)
{
	for (int i = 0; i < steps.size(); i++) {
		if (steps[i] == behavior.behavior) return true;
	}
	return false;
}

std::vector<Entity::TargetedHumanBehaviors> InputHandler::getEmptyTileBehaviors(sf::Vector2i tilePos)
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

std::string InputHandler::getTargetedBehaviorNames(Entity::TargetedHumanBehaviors behaviour, int EntityRefID /*= 0*/)
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
		return "Drop " +EntityHandler::Instance().GetEntityNameByID(EntityRefID);
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
