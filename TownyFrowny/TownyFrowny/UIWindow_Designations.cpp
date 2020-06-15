#include "UIWindow_Designations.h"
#include "Camera.h"
#include "VectorHelper.h"
#include "PlayerController.h"
#include "UIHandler.h"

void UIWindow_Designations::Draw(sf::RenderTexture* renderTexture, sf::Text* text)
{
	std::vector<std::string> names = {
		"GeneralStoreage",
		"FoodStorage",
		"MaterialStorage",
		"GeneralRoom",
	};
	for (int i = 0; i < names.size(); i++)
	{
		text->setFillColor(sf::Color(248, 255, 138));
		text->setPosition(809, 22 + (i * 16));
		text->setString(names[i]);
		renderTexture->draw(*text);
	}
}

void UIWindow_Designations::Tick(sf::Vector2f MousePos)
{
	if (Clicking == true) {
		selectionBoxScreenEnd = MousePos;
	}
	handler->DrawSelectionCursor(selectionBoxScreenStart, selectionBoxScreenEnd - selectionBoxScreenStart, Clicking);
}

void UIWindow_Designations::OnMouseLeftClickDown(sf::Vector2f MousePos)
{
	selectionBoxScreenStart = MousePos;
	if (selectionBoxScreenStart.x * 800 < 800)
	{
		Clicking = true;
	}
	else {
		int i = floor(((MousePos.y*800.0f) - 22) / 16.0f);
		if (i >= 0 && i < 4) {
			selectedType = (Designation::Type)i;
		}
	}
}

void UIWindow_Designations::OnMouseLeftClickUp(sf::Vector2f MousePos)
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

		currentWorld->NewDesignation(indexStart,indexEnd-indexStart,selectedType, playerController->GetPilotingPlayer());
	}
	Clicking = false;
};

void UIWindow_Designations::OnMouseRightClickDown(sf::Vector2f MousePos)
{
	selectionBoxScreenStart = MousePos;
	if (selectionBoxScreenStart.x * 800 < 800)Clicking = true;
};

void UIWindow_Designations::OnMouseRightClickUp(sf::Vector2f MousePos)
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

		for (int x = indexStart.x; x <= indexEnd.x; x++) {
			for (int y = indexStart.y; y <= indexEnd.y; y++) {
				currentWorld->DeleteDesignation(sf::Vector2i(x,y));
			}
		}
	}
	Clicking = false;
};

