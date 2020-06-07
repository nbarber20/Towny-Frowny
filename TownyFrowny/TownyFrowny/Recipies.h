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
		std::vector<std::vector<int>> Bed{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		};
		list.push_back(std::make_pair(23, Bed));

		std::vector<std::vector<int>> Chair{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		};
		list.push_back(std::make_pair(24, Chair));

		std::vector<std::vector<int>> LargeTable{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		};
		list.push_back(std::make_pair(25, LargeTable));

		std::vector<std::vector<int>> SmallTable{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		};
		list.push_back(std::make_pair(26, SmallTable));

		std::vector<std::vector<int>> Chest{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		};
		list.push_back(std::make_pair(31, Chest));

		std::vector<std::vector<int>> Bin{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		};
		list.push_back(std::make_pair(32, Bin));

		std::vector<std::vector<int>> Bucket{
				EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),
		};
		list.push_back(std::make_pair(33, Bucket));

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