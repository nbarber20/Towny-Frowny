#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "World.h"
#include "FastNoise.h"
#include <vector>
#include <time.h>  
#include "Camera.h"
#include "Entity_Living.h"
#include "TileManager.h"
#include <math.h>
#include <sstream>
#include <iomanip>  
#include "EntityHandler.h"
#include "Entity_Food.h"
#include "LogHandler.h"
#include  <cstddef>
#include "TaskManager.h"
#include "PlayerController.h"
#include "UIHandler.h"

int main()
{	
	const float gameSpeed = 0.05f;
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(1000, 800), "Towny Frowny");
	window.setFramerateLimit(60);
	Camera& cameraInstance = Camera::Instance();
	
	TileManager* tileManager = new TileManager();

	FastNoise* noiseGen = new FastNoise(time(NULL));
	noiseGen->SetNoiseType(FastNoise::Perlin);

	FastNoise* noiseGen2 = new FastNoise(time(NULL));
	noiseGen2->SetNoiseType(FastNoise::WhiteNoise);

	World* OverWorld = new World();
	OverWorld->GenerateOverworld(noiseGen);

	World* UnderWorld = new World();
	UnderWorld->GenerateCave(noiseGen2);

	World* selectedWorld = OverWorld;

	PathFinder* pathFinder = new PathFinder();

	PlayerController* playerController = new PlayerController(pathFinder, &window);
	EntityHandler::Instance().Init(new TaskManager(OverWorld,UnderWorld, pathFinder), playerController);

	UIHandler::Instance().Setup(playerController, &window);

	sf::Clock clock;

	int frames = 0;
	sf::Clock frameClock;
	sf::Font font;
	if (!font.loadFromFile("Data/Minecraft.ttf"))
	{
		// error...
	}
	sf::Texture& texture = const_cast<sf::Texture&>(font.getTexture(16));
	texture.setSmooth(false);
	sf::Text FpsText;
	FpsText.setFont(font);
	FpsText.setCharacterSize(16);
	FpsText.setPosition(800 - 32, 0);
	FpsText.setFillColor(sf::Color(248,255,138));
	LogHandler::Instance().WriteLog("Began Game");

	EntityHandler::Instance().CreateAndSpawnEntity(0, selectedWorld, sf::Vector2i(selectedWorld->GetWorldSize()*.5f, selectedWorld->GetWorldSize()*.5f));
	OverWorld->GenerateEntities();
	selectedWorld->UpdateLighting();
	while (window.isOpen())
	{
//////////
		//Input
//////////
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
			{
				EntityHandler::Instance().SpawnAtCursor(52, selectedWorld, &window);
			}
			/*
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
			{
				EntityHandler::Instance().SpawnAtCursor(11, selectedWorld, &window);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
			{
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4))
			{
				EntityHandler::Instance().SpawnAtCursor(47, selectedWorld, &window);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5))
			{
				EntityHandler::Instance().SpawnAtCursor(19, selectedWorld, &window);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6))
			{
				EntityHandler::Instance().SpawnAtCursor(11, selectedWorld, &window);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad7))
			{
				EntityHandler::Instance().SpawnAtCursor(23, selectedWorld, &window);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8))
			{
				EntityHandler::Instance().SpawnAtCursor(24, selectedWorld, &window);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9))
			{
				EntityHandler::Instance().SpawnAtCursor(25, selectedWorld, &window);
			}*/
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				selectedWorld = OverWorld;
				selectedWorld->ReDraw();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
			{
				selectedWorld = UnderWorld;
				selectedWorld->ReDraw();
			}
			playerController->GetInput(event);
			Camera::Instance().GetCameraPolledInput(event);
		}
		UIHandler::Instance().TickWindow(event);
		Camera::Instance().GetCameraInput(event);

//////////
		//Logic Update
//////////
		sf::Time elapsed1 = clock.getElapsedTime();
		if (elapsed1.asSeconds() > gameSpeed) {
			clock.restart();
			FpsText.setString(std::to_string(int(frames / gameSpeed)));
			frames = 0;
			EntityHandler::Instance().TickAll();
			LogHandler::Instance().UpdateLogs();
		}
//////////
		//RenderPass
//////////
		window.clear();
		selectedWorld->Draw();
		if (Camera::Instance().GetLodLevel() == 0) {
			sf::Texture* lowLod = new sf::Texture();
			lowLod->loadFromImage(*cameraInstance.getLowLod(), sf::IntRect(0, 0, selectedWorld->GetWorldSize(), selectedWorld->GetWorldSize()));
			sf::Sprite lowLodsprite(*lowLod);
			lowLodsprite.setOrigin((selectedWorld->GetWorldSize()) / 2, (selectedWorld->GetWorldSize()) / 2);
			lowLodsprite.scale(cameraInstance.getCameraZoom()*8.0f, -1.0f*(cameraInstance.getCameraZoom()*8.0f));
			lowLodsprite.setOrigin(cameraInstance.getCameraOffset()/8.0f);
			lowLodsprite.setPosition(400,400);
			window.draw(lowLodsprite);

			//Shading Pass
			sf::Texture* shading = new sf::Texture();
			shading->loadFromImage(*cameraInstance.getShading(), sf::IntRect(0, 0, selectedWorld->GetWorldSize(), selectedWorld->GetWorldSize()));
			sf::Sprite shadingsprite(*shading);
			shadingsprite.setOrigin((selectedWorld->GetWorldSize()) / 2, (selectedWorld->GetWorldSize()) / 2);
			shadingsprite.scale(cameraInstance.getCameraZoom()*8.0f, -1.0f*(cameraInstance.getCameraZoom()*8.0f));
			shadingsprite.setOrigin(cameraInstance.getCameraOffset() / 8.0f);
			shadingsprite.setPosition(400, 400);
			window.draw(shadingsprite);
			delete lowLod;
			delete shading;
		}
		else {
			sf::Texture* texture = new sf::Texture(cameraInstance.getHighLod()->getTexture());
			sf::Sprite sprite(*texture);
			sprite.setOrigin((selectedWorld->GetWorldSize()*8.0f) / 2, (selectedWorld->GetWorldSize()*8.0f) / 2);
			sprite.scale(cameraInstance.getCameraZoom(), -1.0f*cameraInstance.getCameraZoom());
			sprite.setOrigin(cameraInstance.getCameraOffset());
			sprite.setPosition(400,400);
			window.draw(sprite);

			//Shading Pass
			sf::Texture* shading = new sf::Texture();
			shading->loadFromImage(*cameraInstance.getShading(), sf::IntRect(0, 0, selectedWorld->GetWorldSize(), selectedWorld->GetWorldSize()));
			sf::Sprite shadingsprite(*shading);
			shadingsprite.setOrigin((selectedWorld->GetWorldSize()) / 2, (selectedWorld->GetWorldSize()) / 2);
			shadingsprite.scale(cameraInstance.getCameraZoom()*8.0f, -1.0f*(cameraInstance.getCameraZoom()*8.0f));
			shadingsprite.setOrigin(cameraInstance.getCameraOffset() / 8.0f);
			shadingsprite.setPosition(400, 400);
			window.draw(shadingsprite);
			delete texture;
			delete shading;
		}
		UIHandler::Instance().DrawUI();
		window.draw(FpsText);
		window.display();
		frames++;
	}
	return 0;
}