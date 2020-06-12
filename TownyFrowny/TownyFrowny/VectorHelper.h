#pragma once
#include <SFML/Graphics.hpp>   
#include <math.h>   
#include <cmath>
class VectorHelper {
public:
	static  sf::Vector2i ClampVector(sf::Vector2i input, sf::Vector2i min, sf::Vector2i max) {
		sf::Vector2i v = input;
		if (input.x < min.x)v.x = min.x;
		else if (input.x > max.x)v.x = max.x;
		if (input.y < min.y)v.y = min.y;
		else if (input.y > max.y)v.y = max.y;
		return v;
	}
	static  sf::Vector2f ClampVector(sf::Vector2f input, sf::Vector2f min, sf::Vector2f max) {
		sf::Vector2f v = input;
		if (input.x < min.x)v.x = min.x;
		if (input.x > max.x)v.x = max.x;
		if (input.y < min.y)v.y = min.y;
		if (input.y > max.y)v.y = max.y;
		return v;
	}
	static sf::Vector2i NormalizeVector(sf::Vector2i input) {
		sf::Vector2i dir = sf::Vector2i(abs(input.x), abs(input.y));
		float mag = sqrt(dir.x*dir.x + dir.y* dir.y);
		if (mag > 0) {
			sf::Vector2f s((float)input.x / mag,(float)input.y / mag);
			s.x = s.x < 0 ? floor(s.x) : ceil(s.x);
			s.y = s.y < 0 ? floor(s.y) : ceil(s.y);
			return sf::Vector2i(s.x, s.y);
		}
		else {
			return input;
		}
	}
	static sf::Vector2f NormalizeVector(sf::Vector2f input) {
		sf::Vector2f dir = sf::Vector2f(abs(input.x), abs(input.y));
		float mag = sqrt(dir.x*dir.x + dir.y* dir.y);
		if (mag > 0) {
			return sf::Vector2f(input.x / mag, input.y / mag);
		}
		else {
			return input;
		}
	}
	static float GetMagnitude(sf::Vector2i input) {
		sf::Vector2i dir = sf::Vector2i(abs(input.x), abs(input.y));
		return sqrt(dir.x*dir.x + dir.y* dir.y);
	}
	static float GetMagnitude(sf::Vector2f input) {
		sf::Vector2f dir = sf::Vector2f(abs(input.x), abs(input.y));
		return sqrt(dir.x*dir.x + dir.y* dir.y);
	}
	static sf::Vector2i Lerp(sf::Vector2i a, sf::Vector2i b,float f) {
		sf::Vector2i r;
		r.x = Lerpf(a.x, b.x, f);
		r.y = Lerpf(a.y, b.y, f);
		return r;
	}
	
	static sf::Vector2f Lerp(sf::Vector2f a, sf::Vector2f b, float f) {
		sf::Vector2f r;
		r.x = Lerpf(a.x, b.x, f);
		r.y = Lerpf(a.y, b.y, f);
		return r;
	}

	static float Lerpf(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	static float Clampf(float a, float from, float to) {
		if (a < from)a = from;
		else if (a > to)a = to;
		return a;
	}
};