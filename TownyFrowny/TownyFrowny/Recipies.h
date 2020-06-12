#pragma once
#include <vector>
#include "EntityHandler.h"
class Recipies {
public:
	static std::vector<std::vector<int>> getRecipe(int id)
	{
		std::vector < std::pair<int, std::vector<std::vector<int>>>> recipies = getRecipes();
		for (auto recipie : recipies) {
			if (recipie.first == id) {
				return recipie.second;
			}
		}
	}

	static std::vector<std::pair<int, std::vector<std::vector<int>>>> getRecipes()
	{

		std::vector < std::pair<int, std::vector<std::vector<int>>>> list;
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Brick"), std::vector<std::vector<int>>({ {EntityHandler::Instance().GetEntityIDByName("Clay")} })));
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Copper Ingot"), std::vector<std::vector<int>>({ {EntityHandler::Instance().GetEntityIDByName("Copper Ore")} })));
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Iron Ingot"), std::vector<std::vector<int>>({ {EntityHandler::Instance().GetEntityIDByName("Iron Ore")} }))); //Iron
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Gold Ingot"), std::vector<std::vector<int>>({ {EntityHandler::Instance().GetEntityIDByName("Gold Ore")} }))); //Gold
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Silver Ingot"), std::vector<std::vector<int>>({ {EntityHandler::Instance().GetEntityIDByName("Silver Ore")} }))); //Silver

		//Bed
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Bed"), std::vector<std::vector<int>>{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		})); 

		//Chair
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Chair"), std::vector<std::vector<int>>{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Large Table
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Large Table"), std::vector<std::vector<int>>{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Small Table
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Small Table"), std::vector<std::vector<int>>{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Chest		
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Chest"), std::vector<std::vector<int>>{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Bin		
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Barrel"), std::vector<std::vector<int>>{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Bucket
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Bucket"), std::vector<std::vector<int>>{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Door
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Door"), std::vector<std::vector<int>>{
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Fence
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Fence"), std::vector<std::vector<int>>{
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));

		//Fence
		list.push_back(std::make_pair(EntityHandler::Instance().GetEntityIDByName("Fence Gate"), std::vector<std::vector<int>>{
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
			EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		}));
		return list;
	};

	static  std::vector<int> GetAllCraftable(Entity_Container* toCheck)
	{
		std::vector<int> result;
		std::vector < std::pair<int, std::vector<std::vector<int>>>> recipies = Recipies::getRecipes();
		for (auto recipie : recipies) {
			if (GetCraftable(recipie.second, toCheck)) {
				result.push_back(recipie.first);
			}
		}
		return result;
	}

	static bool GetCraftable(std::vector<std::vector<int>> r, Entity_Container* toCheck)
	{
		std::vector<Entity*> tempHeldItems = *(toCheck->GetInventory());
		for (int i = 0; i < r.size(); i++) {
			if (markholdingItem(&tempHeldItems, r[i]) == false) {
				return false;
			}
		}
		return true;
	}

	static bool markholdingItem(std::vector<Entity*>* List, std::vector<int > recipeItem)
	{
		for (int i = 0; i < List->size(); i++) {
			for (int j = 0; j < recipeItem.size(); j++) {
				if ((*List)[i]->GetID() == recipeItem[j]) {
					List->erase(std::remove(List->begin(), List->end(), (*List)[i]), List->end());
					return true;
				}
			}
		}
		return false;
	}

	/// <summary>Returns a list of item variants needed to craft a recipe
	/// </summary>
	static std::vector<std::vector<int>> GetGatherList(int recipeID, Entity_Container* toCheck) {
		std::vector<std::vector<int>> returnList;
		std::vector<Entity*> tempHeldItems = *(toCheck->GetInventory());
		std::vector<std::vector<int>> recipe = getRecipe(recipeID);
		for (int i = 0; i < recipe.size(); i++) {
			if (markholdingItem(&tempHeldItems, recipe[i]) == false) {
				returnList.push_back(recipe[i]);
			}
		}
		return returnList;
	}
};