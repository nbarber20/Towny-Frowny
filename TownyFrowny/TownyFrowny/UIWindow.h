#pragma once
#include "SFML/Graphics.hpp"
class UIHandler;
class UIWindow {
public:
	UIWindow(sf::RenderWindow* window, UIHandler* handler) {
		this->window = window;
		this->handler = handler;
	}	
	virtual void Draw(sf::RenderTexture* renderTexture, sf::Text* text) {

	}
	virtual void Tick(sf::Vector2f MousePos) {

	}
	virtual void OnMouseLeftClickDown(sf::Vector2f MousePos) {

	}
	virtual void OnMouseRightClickDown(sf::Vector2f MousePos) {

	}
	virtual void OnMouseLeftClickUp(sf::Vector2f MousePos) {

	}
	virtual void OnMouseRightClickUp(sf::Vector2f MousePos) {

	}
	virtual void OnNumKeyPressed(int key) {

	}
protected:
	UIHandler* handler;
	sf::RenderWindow* window;
};