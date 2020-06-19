#pragma once

#include <SFML/Graphics.hpp>
#include "UIWindow.h"
#include "UIWindow_Tasks.h"
#include "UIWindow_Inventory.h"
#include "UIWindow_Designations.h"
#include "UIWindow_Logs.h"
class UIHandler {
public:
	static UIHandler& Instance()
	{
		static UIHandler INSTANCE;
		return INSTANCE;
	}

	UIHandler() {};

	enum Windows {
		Logs,
		Tasks,
		Inventory,
		Designation,
		Needs,
		System,
	};
	struct WindowButton{
		WindowButton(sf::Vector2f StartPos,	sf::Vector2f EndPos,sf::Sprite* sprite,	Windows windowToOpen) {
			this->StartPos = StartPos;
			this->EndPos = EndPos;
			this->sprite = sprite;
			this->windowToOpen = windowToOpen;
			this->sprite->setPosition(StartPos);
		};
		sf::Vector2f StartPos;
		sf::Vector2f EndPos;
		sf::Sprite* sprite;
		Windows windowToOpen;
	};
	
	void Setup(PlayerController* playerController, sf::RenderWindow* window) {
		this->window = window;
		UIPanelTex->loadFromFile("Tile/UI.png");
		RightUIPanel = new sf::Sprite(*UIPanelTex, sf::IntRect(0, 0, 200, 800));
		RightUIPanel->setPosition(800, 0);
		uiTex.create(1000, 800);
		if (!font.loadFromFile("Data/Minecraft.ttf"))
		{
			// error...
		}

		sf::Texture& fontTexture = const_cast<sf::Texture&>(font.getTexture(13));
		fontTexture.setSmooth(false);
		screenText.setFont(font);
		screenText.setCharacterSize(26);
		screenText.setScale(0.5f, 0.5f);
		selectionCursor = new sf::RectangleShape(sf::Vector2f(1, 1));
		selectionCursor->setOutlineThickness(1);

		sf::Sprite* LogSprite = new sf::Sprite(*UIPanelTex,sf::IntRect(402,0,598,757));
		LogSprite->setPosition(402, 0);
		//Setup all windows
		UIWindowList = {
			std::make_pair(Tasks,new UIWindow_Tasks(playerController,window,this)),
			std::make_pair(Inventory,new UIWindow_Inventory(playerController,window,this)),
			std::make_pair(Designation,new UIWindow_Designations(playerController,window,this)),
			std::make_pair(Needs,new UIWindow(window,this)),
			std::make_pair(Logs,new UIWindow_Logs(LogSprite,window,this)),
			std::make_pair(System,new UIWindow(window,this)),
		};

		buttons = {
			WindowButton(sf::Vector2f(804, 0), sf::Vector2f(857, 19),new sf::Sprite(*UIPanelTex, sf::IntRect(204, 0, 54, 20)),Tasks),
			WindowButton(sf::Vector2f(858, 0), sf::Vector2f(911, 19),new sf::Sprite(*UIPanelTex, sf::IntRect(258, 0, 54, 20)),Inventory),
			WindowButton(sf::Vector2f(912, 0), sf::Vector2f(955, 19),new sf::Sprite(*UIPanelTex, sf::IntRect(312, 0, 44, 20)),Designation),
			WindowButton(sf::Vector2f(956, 0), sf::Vector2f(999, 19),new sf::Sprite(*UIPanelTex, sf::IntRect(356, 0, 44, 20)),Needs),
			WindowButton(sf::Vector2f(956, 780), sf::Vector2f(999, 799),new sf::Sprite(*UIPanelTex, sf::IntRect(267, 21, 44, 20)),Logs),
			WindowButton(sf::Vector2f(893, 780), sf::Vector2f(954, 799),new sf::Sprite(*UIPanelTex, sf::IntRect(204, 21, 63, 20)),System),
		};

		SetWindow(Tasks);
	}


	void SetWindow(Windows window) {
		activeUIWindowType = window;
		for (int i = 0; i < UIWindowList.size(); i++) {
			if (UIWindowList[i].first == window) {
				activeUIWindow = UIWindowList[i].second; 
				UpdateUI();
				return;
			}
		}
		activeUIWindow = UIWindowList[0].second; //fail out to first window
	}	

	Windows GetActiveWindowType() {
		return activeUIWindowType;
	}
	
	void TickWindow(sf::Event e) {
		sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window)) / 800.0f;		
		if (e.type == sf::Event::MouseButtonPressed)
		{
			if (e.mouseButton.button == sf::Mouse::Left) {
				for (auto button : buttons) {
					if (MouseWithinRect(window->mapPixelToCoords(sf::Mouse::getPosition(*window)), button.StartPos, button.EndPos)== true)SetWindow(button.windowToOpen);
				}
				activeUIWindow->OnMouseLeftClickDown(mousePos);
			}
			if (e.mouseButton.button == sf::Mouse::Right)activeUIWindow->OnMouseRightClickDown(mousePos);
		}
		else if (e.type == sf::Event::MouseButtonReleased)
		{
			if (e.mouseButton.button == sf::Mouse::Left)activeUIWindow->OnMouseLeftClickUp(mousePos);
			if (e.mouseButton.button == sf::Mouse::Right)activeUIWindow->OnMouseRightClickUp(mousePos);
		}
		activeUIWindow->Tick(mousePos);
	}
	
	void UpdateUI() {
		uiTex.clear(sf::Color(0,0,0,0));
		uiTex.draw(*RightUIPanel);
		for (auto button : buttons) {
			if (activeUIWindowType == button.windowToOpen) {
				uiTex.draw(*button.sprite);
			}
		}
		activeUIWindow->Draw(&uiTex, &screenText);
	}

	//Draws the ui container
	void DrawUI() {
		sf::Texture* texture = new sf::Texture(uiTex.getTexture());
		sf::Sprite sprite(*texture);
		sprite.scale(1, -1);
		sprite.move(0, 800);
		window->draw(sprite);
		delete texture;
		window->draw(*selectionCursor);
	}

	void DrawSelectionCursor(sf::Vector2f pos, sf::Vector2f size,bool show) {
		if (show) {
			selectionCursor->setFillColor(sf::Color(255, 255, 255, 10));
			selectionCursor->setOutlineColor(sf::Color(255, 255, 255, 255));
			selectionCursor->setPosition(pos * 800.0f);
			selectionCursor->setSize(size * 800.0f);
		}
		else {
			selectionCursor->setFillColor(sf::Color(255, 255, 255, 0));
			selectionCursor->setOutlineColor(sf::Color(255, 255, 255, 0));
		}
	}

private:

	bool MouseWithinRect(sf::Vector2f mousePos, sf::Vector2f start,sf::Vector2f end) {
		if (mousePos.x >= start.x&&mousePos.x <= end.x && mousePos.y >= start.y && mousePos.y<=end.y) {
			return true;
		}
		return false;
	}

	std::vector <WindowButton> buttons;

	std::vector<std::pair<Windows,UIWindow*>> UIWindowList;
	UIWindow* activeUIWindow;
	Windows activeUIWindowType;
	sf::RenderTexture uiTex;
	sf::Texture* UIPanelTex = new sf::Texture();
	sf::Sprite* RightUIPanel;
	sf::Text screenText;
	sf::Font font;
	sf::RectangleShape* selectionCursor;
	sf::RenderWindow* window;
};
