#pragma once
#include "UIWindow.h"
class UIWindow_Logs : public UIWindow
{
public:
	UIWindow_Logs(sf::Sprite* LogWindowSprite, sf::RenderWindow* window, UIHandler* handler) : UIWindow(window, handler) {
		this->LogWindowSprite = LogWindowSprite;
	};
	void Draw(sf::RenderTexture* renderTexture, sf::Text* text) override;
	void OnMouseLeftClickDown(sf::Vector2f MousePos) override;
private:
	sf::Sprite* LogWindowSprite; 
};

