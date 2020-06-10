#pragma once
#include <SFML/Graphics.hpp>
#include "VectorHelper.h"
#include "Designation.h"
class Camera
{
private:
	float cameraZoom = 0.123f;
	sf::Vector2f cameraOffset;
	sf::RenderTexture HighLod;
	sf::Image LowLod;
	sf::Image Shading;
	std::vector<sf::RectangleShape*> designationTiles;
	const int worldSize = 400;
	bool lodLevel = 0;
	int designationOpacity= 50;

	Camera() {
		HighLod.create(worldSize *8, worldSize *8);
		LowLod.create(worldSize, worldSize);
		Shading.create(worldSize, worldSize);
		for (int i=0; i<4; i++)
		{
			makeDesignationTile(getDesignationTileColor((Designation::Type)i));
		}
		cameraOffset = sf::Vector2f((worldSize/2)*8, (worldSize / 2) * 8);
	};

	void makeDesignationTile(sf::Color color) {
		sf::RectangleShape* sprite = new sf::RectangleShape(sf::Vector2f(8,8));
		sprite->setFillColor(color);
		designationTiles.push_back(sprite);
	}
public:
	sf::RectangleShape* getDesignationTile(Designation::Type t) {
		return designationTiles[(int)t];
	}
	sf::Color getDesignationTileColor(Designation::Type t) {
		switch (t)
		{
		case Designation::GeneralStoreage:
			return sf::Color(84, 219, 235, designationOpacity);
		case Designation::FoodStorage:
			return sf::Color(117,79, 242, designationOpacity);
		case Designation::MaterialStorage:
			return sf::Color(87, 179,237, designationOpacity);
		case Designation::GeneralRoom:
			return sf::Color(237, 218, 87, designationOpacity);
		default:
			return sf::Color(255, 0, 0, designationOpacity);
		}
	}
	int getWorldSize() {
		return worldSize;
	}
	sf::RenderTexture* getHighLod() {
		return &HighLod;
	}
	sf::Image* getLowLod() {
		return &LowLod;
	}
	sf::Image* getShading() {
		return &Shading;
	}
	float getCameraZoom() {
		return cameraZoom;
	}
	void setCameraZoom(float v) {
		if (v >= 0.5) {
			lodLevel = 1;
		}
		else if (v < 0.5) {
			lodLevel = 0;
		}
		cameraZoom = v;
	}
	sf::Vector2f getCameraOffset() {
		return cameraOffset;
	}
	void setCameraOffset(sf::Vector2f v) {
		cameraOffset = v;
	}
	bool GetLodLevel() {
		return lodLevel;
	}
	bool IsOnScreen(sf::Vector2f worldPos) {
		float width = 800 / (cameraZoom*8.0f);
		sf::Vector2f offset = ((sf::Vector2f(cameraOffset.x, (worldSize * 8) - cameraOffset.y) / 8.0f) - (sf::Vector2f(1, 1)*(width / 2)));
		if (worldPos.x > offset.x && worldPos.x < offset.x + width) {
			if (worldPos.y > offset.y && worldPos.y < offset.y + width) {
				return true;
			}
		}
		return false;
	}
	bool ScreenToWorld(sf::Vector2f input, sf::Vector2i& result) {
		float width = 800 / (cameraZoom*8.0f);
		sf::Vector2f offset = ((sf::Vector2f(cameraOffset.x, (worldSize * 8) -cameraOffset.y) / 8.0f) -(sf::Vector2f(1, 1)*(width / 2)));
		result.x = VectorHelper::Lerpf(offset.x, offset.x + width, input.x);
		result.y = VectorHelper::Lerpf(offset.y, offset.y + width, input.y);
		if (result.x>0&&result.x<worldSize-1) {
			if (result.y > 0 && result.y < worldSize - 1) {
				return true;
			}
		}
		return false;
	}
	void MoveToPos(sf::Vector2f pos) {
		float width = 800 / (cameraZoom*8.0f);
		cameraOffset = sf::Vector2f(pos.x, (worldSize) - pos.y)*8.0f;
	}
	static Camera& Instance()
	{
		static Camera INSTANCE;
		return INSTANCE;
	}
};

