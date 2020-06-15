#pragma once
#include "UIWindow.h"
#include "PlayerController.h"
class UIWindow_Inventory : public UIWindow {
public:
	UIWindow_Inventory(PlayerController* playerController, sf::RenderWindow* window, UIHandler* handler) : UIWindow(window, handler) {
		this->playerController = playerController;
	}
	void Draw(sf::RenderTexture* renderTexture, sf::Text* text) override;
	void OnMouseLeftClickDown(sf::Vector2f MousePos) override;
private:
	PlayerController* playerController;
	std::vector<std::pair<int, int>> InventoryCollapsed;
};

