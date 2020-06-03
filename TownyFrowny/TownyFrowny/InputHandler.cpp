#include "InputHandler.h"
#include "EntityHandler.h"

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
	}
	selectionBoxScreenEnd = window->mapPixelToCoords(sf::Mouse::getPosition(*window)) / 800.0f;
	if (e.type == sf::Event::MouseButtonReleased)
	{
		if (e.mouseButton.button == sf::Mouse::Left)
		{
			if (Clicking == true && selectionBoxScreenEnd.x * 800 < 800) {
				sf::Vector2i CursorStart;
				sf::Vector2i CursorEnd;
				Camera::Instance().ScreenToWorld(selectionBoxScreenStart, CursorStart);
				Camera::Instance().ScreenToWorld(selectionBoxScreenEnd, CursorEnd);
				CursorStart = VectorHelper::ClampVector(CursorStart, sf::Vector2i(0, 0), sf::Vector2i(world->GetWorldSize(), world->GetWorldSize()));
				CursorEnd = VectorHelper::ClampVector(CursorEnd, sf::Vector2i(0, 0), sf::Vector2i(world->GetWorldSize(), world->GetWorldSize()));
				sf::Vector2i indexStart;
				indexStart.x = (CursorStart.x < CursorEnd.x) ? CursorStart.x : CursorEnd.x;
				indexStart.y = (CursorStart.y < CursorEnd.y) ? CursorStart.y : CursorEnd.y;

				sf::Vector2i indexEnd;
				indexEnd.x = (CursorStart.x < CursorEnd.x) ? CursorEnd.x : CursorStart.x;
				indexEnd.y = (CursorStart.y < CursorEnd.y) ? CursorEnd.y : CursorStart.y;

				for (int x = indexStart.x; x <= indexEnd.x; x++)
				{
					for (int y = indexStart.y; y <= indexEnd.y; y++)
					{
						std::vector<Entity*> list = world->GetWorldTile(sf::Vector2i(x, y))->tileEntities;
						selectedPositions.push_back(sf::Vector2i(x, y));
						for (auto e : list) {
							selectedEntity.push_back(e);
						}
					}
				}
				ReleventBehaviors.clear();
				ReleventBehaviors.push_back(Entity::TargetedHumanBehaviors::Targeted_ClearAll);
				for (auto e : selectedEntity) {
					std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
					e->getTargetedBehaviors(&tempBehaviors);
					for (int j = 0; j < tempBehaviors.size(); j++) {
						if (pilotingPlayer->isTargetedBehaviorPossible(tempBehaviors[j]) == true) { //is possible
							if (std::find(ReleventBehaviors.begin(), ReleventBehaviors.end(), tempBehaviors[j]) == ReleventBehaviors.end()) { // not in list
								ReleventBehaviors.push_back(tempBehaviors[j]);
							}
						}
					}
				}
				for (int pos = 0; pos < selectedPositions.size(); pos++) {
					std::vector<Entity::TargetedHumanBehaviors> tempBehaviors = getEmptyTileBehaviors(selectedPositions[pos]);
					for (int j = 0; j < tempBehaviors.size(); j++) {
						if (pilotingPlayer->isTargetedBehaviorPossible(tempBehaviors[j]) == true) {
							if (std::find(ReleventBehaviors.begin(), ReleventBehaviors.end(), tempBehaviors[j]) == ReleventBehaviors.end()) { // not in list
								ReleventBehaviors.push_back(tempBehaviors[j]);
							}
						}
					}
				}
				if (ReleventHeldItemID != -1) {
					std::vector<Entity*> inv = *pilotingPlayer->GetInventory();
					for (int i = 0; i < inv.size(); i++) {
						if (inv[i]->GetID() == ReleventHeldItemID) {
							std::vector<Entity::TargetedHumanBehaviors> tempBehaviors;
							inv[i]->getTargetedBehaviors(&tempBehaviors);
							for (int j = 0; j < tempBehaviors.size(); j++) {
								if (pilotingPlayer->isTargetedBehaviorPossible(tempBehaviors[j]) == true) {
									if (std::find(ReleventBehaviors.begin(), ReleventBehaviors.end(), tempBehaviors[j]) == ReleventBehaviors.end()) { // not in list
										ReleventBehaviors.push_back(tempBehaviors[j]);
									}
								}
							}
						}
					}
				}

				std::vector<std::string> names;
				for (int i = 0; i < ReleventBehaviors.size(); i++) {
					names.push_back(getTargetedBehaviorNames(ReleventBehaviors[i]));
				}
				UiHandler::Instance().UpdateTaskList(names);

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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
	{
		//task = 9;
		pilotingPlayer->CraftItem(24);
	}

	if (UiHandler::Instance().GetWindow() == UiHandler::Tasks) {
		if (task >= 0 && task < ReleventBehaviors.size()) {
			AssignTasks(task);
		}
	}


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		UiHandler::Instance().SetWindow(UiHandler::Logs);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
	{
		UiHandler::Instance().SetWindow(UiHandler::Tasks);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
	{
		UiHandler::Instance().SetWindow(UiHandler::Inventory);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
	{
		UiHandler::Instance().SetWindow(UiHandler::Crafting);
	}

	if (UiHandler::Instance().GetWindow() == UiHandler::Inventory) {
		std::vector<std::string> names;
		std::vector<Entity*> inventory = *pilotingPlayer->GetInventory();
		InventoryList.clear();
		for (int i = 0; i < inventory.size(); i++) {
			InventoryList.push_back(std::make_pair(false,inventory[i]));
			names.push_back(inventory[i]->GetObjectName());
		}
		UiHandler::Instance().UpdateTaskList(names);
	}
	if (UiHandler::Instance().GetWindow() == UiHandler::Crafting) {
		std::vector<std::string> names;
		std::vector<int> craftable = pilotingPlayer->GetAllCraftable();
		CraftingList.clear();
		for (int i = 0; i < craftable.size(); i++) {
			CraftingList.push_back(craftable[i]);
			names.push_back(EntityHandler::Instance().GetEntityNameByID(craftable[i]));
		}
		UiHandler::Instance().UpdateTaskList(names);
	}
}
