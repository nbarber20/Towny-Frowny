#pragma once
#include <SFML/Graphics.hpp>
#include "WorldTile.h"
#include "World.h"
#include <vector>
#include <math.h>   
#include <cmath>
#include <iostream>
#include "VectorHelper.h"
class PathFinder {
public:
	PathFinder() {};
	bool FindPath(Entity* refEntity,int stopDistance, std::vector<sf::Vector2i>& steps, sf::Vector2i startPos, sf::Vector2i targetPos, World* world, bool complex = true) {
		WorldTile* startNode = world->GetWorldTile(startPos);
		WorldTile* targetNode = world->GetWorldTile(targetPos);

		std::vector<WorldTile*> openSet;
		std::vector<WorldTile*> closedSet;
		openSet.push_back(startNode);

		int ticks = 0;
		int maxTicks = floor(VectorHelper::GetMagnitude(startPos - targetPos));
		maxTicks *= floor(VectorHelper::GetMagnitude(startPos - targetPos));

		if (complex) {
			if (maxTicks < 1024)maxTicks = 1024;
		}
		else {
			if (maxTicks < 16)maxTicks = 16;
		}

		while (openSet.size() > 0 && ticks < maxTicks) {
			ticks++;
			WorldTile* currentNode = openSet[0];
			for (int i = 1; i < openSet.size(); i++) {
				if (openSet[i]->fcost() < currentNode->fcost() || openSet[i]->fcost() == currentNode->fcost() && openSet[i]->hcost < currentNode->hcost) {
					currentNode = openSet[i];
				}
			}
			openSet.erase(std::remove(openSet.begin(), openSet.end(), currentNode), openSet.end());
			closedSet.push_back(currentNode);

			if (currentNode == targetNode) {
				steps= RetracePath(startNode, targetNode);
				return true;
			}
			std::vector<WorldTile*> neighbors = world->GetNeighborsOfTile(currentNode->GetPosition());
			for (auto neighbor : neighbors)
			{
				if (neighbor == nullptr) continue;
				if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end())continue;
				int dist = floor(VectorHelper::GetMagnitude(neighbor->GetPosition() - targetNode->GetPosition()));
				if (dist >= stopDistance) {
					if (world->IsTileWalkable(neighbor, refEntity) == false) {
					
						continue;
					}
				}
				int costToNeighbor = currentNode->gcost + GetDistance(currentNode, neighbor);
				if (costToNeighbor < neighbor->gcost || std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
					neighbor->gcost = costToNeighbor;
					neighbor->hcost = GetDistance(neighbor, targetNode);
					neighbor->pathingParent = currentNode;
					if (std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
						openSet.push_back(neighbor);
					}
				}
			}
		}
		std::cout << "failed pathing \n";
		return false;
	}
private:
	std::vector<sf::Vector2i> RetracePath(WorldTile* startNode, WorldTile* endNode) {
		std::vector<sf::Vector2i> pathSteps;
		WorldTile* currentNode = endNode;
		while (currentNode != startNode) {
			pathSteps.push_back(currentNode->GetPosition() - currentNode->pathingParent->GetPosition());
			currentNode = currentNode->pathingParent;
		}
		std::reverse(std::begin(pathSteps), std::end(pathSteps));
		return pathSteps;
	}
	int GetDistance(WorldTile* a, WorldTile* b) {
		int distX = abs(a->x - b->x);
		int distY = abs(a->y - b->y);
		if (distX > distY) 
			return 14 * distY + 10 * (distX - distY);
		return 14 * distX + 10 * (distY - distX);
	}

};