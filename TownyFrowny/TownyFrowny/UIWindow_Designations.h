#pragma once
#include "UIWindow.h"
#include "Designation.h"
class PlayerController;
class UIWindow_Designations: public UIWindow
{
public:
	UIWindow_Designations(PlayerController* playerController, sf::RenderWindow* window, UIHandler* handler) : UIWindow(window, handler) {
		this->playerController = playerController;
	}
	void Draw(sf::RenderTexture* renderTexture, sf::Text* text) override;
	void Tick(sf::Vector2f MousePos) override;
	void OnMouseLeftClickDown(sf::Vector2f MousePos) override;
	void OnMouseLeftClickUp(sf::Vector2f MousePos) override;
	void OnMouseRightClickDown(sf::Vector2f MousePos) override;
	void OnMouseRightClickUp(sf::Vector2f MousePos) override;
private:
	sf::Vector2f selectionBoxScreenStart;
	sf::Vector2f selectionBoxScreenEnd;
	bool Clicking = false;
	PlayerController* playerController;
	Designation::Type selectedType = Designation::GeneralRoom;
};

