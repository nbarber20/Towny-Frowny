#pragma once 
#include <SFML/Graphics.hpp>

struct Tile
{
	Tile(sf::Texture* textureAtlas, int x, int y, sf::Color* c) {
		this->sprite = new sf::Sprite(*textureAtlas, sf::IntRect(x*8, y*8, 8, 8));
		this->tileColor = c;
	}
	sf::Sprite* sprite;
	sf::Color* tileColor;
};


class TileManager
{
public:
	static TileManager& Instance()
	{
		static TileManager INSTANCE;
		return INSTANCE;
	}
	TileManager() {
		//Load world tiles
		worldtextureAtlas = new sf::Texture();
		if (!worldtextureAtlas->loadFromFile("Tile/worldTileset.png"))
		{
			// error...
		}
		sf::Image* worldColorLookup = new sf::Image();
		if (!worldColorLookup->loadFromFile("Tile/worldColorLookup.png"))
		{
			// error...
		}
		for (int i = 0; i < worldtextureAtlas->getSize().x / 8.0f; i++) {
			std::vector<Tile*> col;
			for (int j = 0; j < worldtextureAtlas->getSize().y / 8.0f; j++) {
				col.push_back(new Tile(worldtextureAtlas, i, j, new sf::Color(worldColorLookup->getPixel(i, 0))));
			}
			worldtiles.push_back(col);
		}
		delete worldColorLookup;

		sf::Image* entitytextureAtlasImage = new sf::Image();
		if (!entitytextureAtlasImage->loadFromFile("Tile/EntityTileset.png"))
		{
			// error...
		}
		entitytextureAtlas = new sf::Texture();
		entitytextureAtlas->loadFromImage(*entitytextureAtlasImage);
		sf::Image* entityColorLookup = new sf::Image();
		if (!entityColorLookup->loadFromFile("Tile/EntityColorLookup.png"))
		{
			// error...
		}	
		
		for (int i = 0; i < entitytextureAtlasImage->getSize().x / 8.0f; i++) {
			std::vector<Tile*> col;
			for (int j = 0; j < entitytextureAtlasImage->getSize().y / 8.0f; j++) {
				if (isEmpty(entitytextureAtlasImage,sf::Vector2i(i*8,j* 8)) == false)
				{
					Tile* t = new Tile(entitytextureAtlas, i, j, new sf::Color(entityColorLookup->getPixel(i, j)));
					col.push_back(t);
				}
			}
			entitytiles.push_back(col);
		}
		
		delete entityColorLookup;
		delete entitytextureAtlasImage;
	};

	Tile* GetWorldTileByID(wchar_t id, bool isWall, bool isFrontWall, bool isTopWall)
	{
		if (isWall) {
			if (isFrontWall&&isTopWall) {
				return worldtiles[id][4];
			}
			else if (isFrontWall) {
				return worldtiles[id][3];
			}
			else if (isTopWall) {
				return worldtiles[id][1];
			}
			else {
				return worldtiles[id][2];
			}
		}
		else {
			return worldtiles[id][0];
		}
	}
	Tile* GetEntityTileByID(wchar_t id, wchar_t offset)
	{
		return entitytiles[id][offset];
	}
	int GetNumEntityVariants(wchar_t id)
	{
		return entitytiles[id].size();
	}
private:
	bool isEmpty(sf::Image* t, sf::Vector2i offset) {
		for (int x = offset.x; x < offset.x + 8; x++) {
			for (int y = offset.y; y < offset.y + 8; y++) {
				float c = t->getPixel(x, y).a;
				if (t->getPixel(x, y).a != 0) {
					return false;
				}
			}
		}
		return true;		
	}

	sf::Texture* worldtextureAtlas;
	sf::Texture* entitytextureAtlas;
	std::vector< std::vector<Tile*>> entitytiles;
	std::vector < std::vector<Tile*>> worldtiles;
};

