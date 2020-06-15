#include "UIWindow_Tasks.h"
#include "Camera.h"
#include "VectorHelper.h"
#include "PlayerController.h"
#include "UIHandler.h"

void UIWindow_Tasks::Draw(sf::RenderTexture* renderTexture, sf::Text* text)
{
	std::vector<std::string> taskNames = playerController->GetTaskNames();
	for (int i = 0; i < taskNames.size(); i++)
	{
		text->setFillColor(sf::Color(248, 255, 138));
		text->setPosition(809, 22+(i * 16));
		text->setString(taskNames[i]);
		renderTexture->draw(*text);
	}
}

void UIWindow_Tasks::Tick(sf::Vector2f MousePos)
{
	if (Clicking == true) {
		selectionBoxScreenEnd = MousePos;
	}
	handler->DrawSelectionCursor(selectionBoxScreenStart, selectionBoxScreenEnd - selectionBoxScreenStart, Clicking);
}

void UIWindow_Tasks::OnMouseLeftClickDown(sf::Vector2f MousePos)
{
	selectionBoxScreenStart = MousePos;
	if (selectionBoxScreenStart.x * 800 < 800)
	{
		Clicking = true;
	}
	else {
		float i = floor(((MousePos.y*800.0f) - 22) / 16.0f);
		playerController->OnTaskUIClick(i);
	}
}

void UIWindow_Tasks::OnMouseLeftClickUp(sf::Vector2f MousePos)
{
	if (Clicking == true) {
		World* currentWorld = playerController->GetPilotingPlayer()->getWorld();
		sf::Vector2i CursorStart;
		sf::Vector2i CursorEnd;
		Camera::Instance().ScreenToWorld(selectionBoxScreenStart, CursorStart);
		Camera::Instance().ScreenToWorld(selectionBoxScreenEnd, CursorEnd);
		CursorStart = VectorHelper::ClampVector(CursorStart, sf::Vector2i(0, 0), sf::Vector2i(currentWorld->GetWorldSize(), currentWorld->GetWorldSize()));
		CursorEnd = VectorHelper::ClampVector(CursorEnd, sf::Vector2i(0, 0), sf::Vector2i(currentWorld->GetWorldSize(), currentWorld->GetWorldSize()));
		sf::Vector2i indexStart;
		indexStart.x = (CursorStart.x < CursorEnd.x) ? CursorStart.x : CursorEnd.x;
		indexStart.y = (CursorStart.y < CursorEnd.y) ? CursorStart.y : CursorEnd.y;

		sf::Vector2i indexEnd;
		indexEnd.x = (CursorStart.x < CursorEnd.x) ? CursorEnd.x : CursorStart.x;
		indexEnd.y = (CursorStart.y < CursorEnd.y) ? CursorEnd.y : CursorStart.y;

		currentWorld->SelectArea(std::make_pair(indexStart, indexEnd));

		//Get All Selected Entities
		std::vector<Entity*> selectedEntities;
		std::vector<sf::Vector2i> selectedTiles;
		for (int x = indexStart.x; x <= indexEnd.x; x++)
		{
			for (int y = indexStart.y; y <= indexEnd.y; y++)
			{
				std::vector<Entity*> list = currentWorld->GetWorldTile(sf::Vector2i(x, y))->tileEntities;
				for (auto e : list) {
					if (e != playerController->GetPilotingPlayer()) {
						e->selected = true;
						currentWorld->tileToTick.push_back(currentWorld->GetWorldTile(e->GetPosition()));
						selectedEntities.push_back(e);
					}
				}
				selectedTiles.push_back(sf::Vector2i(x, y));
			}
		}
		playerController->Selections.push_back(new PlayerController::Selection(selectedEntities, selectedTiles));
		playerController->UpdateBehaviorList();
	}
	Clicking = false;
};

void UIWindow_Tasks::OnMouseRightClickDown(sf::Vector2f MousePos)
{
	selectionBoxScreenStart = MousePos;
	if (selectionBoxScreenStart.x * 800 < 800)Clicking = true;
};

void UIWindow_Tasks::OnMouseRightClickUp(sf::Vector2f MousePos)
{
	World* currentWorld = playerController->GetPilotingPlayer()->getWorld();
	sf::Vector2i CursorStart;
	sf::Vector2i CursorEnd;
	Camera::Instance().ScreenToWorld(selectionBoxScreenStart, CursorStart);
	Camera::Instance().ScreenToWorld(selectionBoxScreenEnd, CursorEnd);
	CursorStart = VectorHelper::ClampVector(CursorStart, sf::Vector2i(0, 0), sf::Vector2i(currentWorld->GetWorldSize(), currentWorld->GetWorldSize()));
	CursorEnd = VectorHelper::ClampVector(CursorEnd, sf::Vector2i(0, 0), sf::Vector2i(currentWorld->GetWorldSize(), currentWorld->GetWorldSize()));
	sf::Vector2i indexStart;
	indexStart.x = (CursorStart.x < CursorEnd.x) ? CursorStart.x : CursorEnd.x;
	indexStart.y = (CursorStart.y < CursorEnd.y) ? CursorStart.y : CursorEnd.y;

	sf::Vector2i indexEnd;
	indexEnd.x = (CursorStart.x < CursorEnd.x) ? CursorEnd.x : CursorStart.x;
	indexEnd.y = (CursorStart.y < CursorEnd.y) ? CursorEnd.y : CursorStart.y;
	bool deletedAny = false;
	for (int x = indexStart.x; x <= indexEnd.x; x++)
	{
		for (int y = indexStart.y; y <= indexEnd.y; y++)
		{
			for (auto selection : playerController->Selections) {
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
		for (auto selection : playerController->Selections) {
			selection->markedForDeletion = true;
		}
	}
	playerController->DeleteMarkedSelections();
	playerController->UpdateBehaviorList();
	Clicking = false;
};

