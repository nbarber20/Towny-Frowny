#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Screen
{
public:
	Screen();
	~Screen();
	static std::vector<sf::RectangleShape*> SpritestoRender;
};

