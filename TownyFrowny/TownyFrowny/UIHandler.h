#pragma once

#include <SFML/Graphics.hpp>
class UiHandler {
public:
	static UiHandler& Instance()
	{
		static UiHandler INSTANCE;
		return INSTANCE;
	}
	UiHandler() {
		RightUIPanelTex->loadFromFile("Tile/RightPanel.png");
		RightUIPanel = new sf::Sprite(*RightUIPanelTex, sf::IntRect(0, 0, 200, 800));
		RightUIPanel->setPosition(800, 0);
		uiTex.create(1000, 800);
		if (!font.loadFromFile("Data/Minecraft.ttf"))
		{
			// error...
		}
		logText.setFont(font);
		logText.setCharacterSize(8);
		uiTex.draw(*RightUIPanel);
		selectionCursor = new sf::RectangleShape(sf::Vector2f(1,1));
		selectionCursor->setOutlineThickness(1);
	}


	enum Windows {
		Logs,
		Tasks,
		Inventory,
		Crafting,
	};

	void SetWindow(Windows window) {
		currentWindow = window;
	}	
	Windows GetWindow() {
		return currentWindow;
	}
	
	void UpdateLogs(std::vector<std::string> logsToShow, std::vector<sf::Color> logColors) {
		if (currentWindow == Logs) {
			uiTex.draw(*RightUIPanel);
			const int num = logsToShow.size();
			std::string* ptr = (num > 0) ? logsToShow.data() : nullptr;
			for (int i = 0; i < num; i++)
			{
				logText.setFillColor(logColors[i]);
				logText.setPosition(808, i * 10);
				logText.setString(logsToShow[i]);
				uiTex.draw(logText);
			}
		}
	}

	void UpdateTaskList(std::vector<std::string> taskNames) {
			uiTex.draw(*RightUIPanel);
			for (int i = 0; i < taskNames.size(); i++)
			{
				logText.setFillColor(sf::Color(248, 255, 138));
				logText.setPosition(808, i * 10);
				logText.setString(taskNames[i]);
				uiTex.draw(logText);
			}
	}

	//Draws the ui container
	void DrawUI(sf::RenderWindow* w) {
		sf::Texture* texture = new sf::Texture(uiTex.getTexture());
		sf::Sprite sprite(*texture);
		sprite.scale(1, -1);
		sprite.move(0, 800);
		w->draw(sprite);
		delete texture;
		w->draw(*selectionCursor);
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

	Windows currentWindow = Windows::Tasks;

	sf::RenderTexture uiTex;
	sf::Texture* RightUIPanelTex = new sf::Texture();
	sf::Sprite* RightUIPanel;
	sf::Text logText;
	sf::Font font;
	sf::RectangleShape* selectionCursor;
};
