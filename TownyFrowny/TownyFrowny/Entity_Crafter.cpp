#include "Entity_Crafter.h"
#include "LogHandler.h"
#include "EntityHandler.h"
#include "Recipies.h"

std::vector<int> Entity_Crafter::GetCraftable()
{
	return craftable;
}

bool Entity_Crafter::CraftItem(int id, Entity_Living* crafterOwner, Entity* outEntity)
{
	if (std::find(craftable.begin(), craftable.end(), id) == craftable.end()) return false; //this crafter cannot craft this item

	std::vector< std::vector<int>> recipe = Recipies::getRecipe(id);
	if (recipe.size() == 0) return false; //This is an invalid recipe
	std::vector<Entity*> tempHeldItems = heldItems;
	bool matchingItemFound = false;

	if (Recipies::GetCraftable(recipe,this) == false) {
		LogHandler::Instance().WriteLog("Crafting Impossible");
		return false;
	}

	int stepsCompleted = 0;
	for (int r = 0; r < recipe.size(); r++) {
		if (ConsumeCraftingItem(recipe[r])) stepsCompleted++;
	}
	if (stepsCompleted >= recipe.size()) {
		LogHandler::Instance().WriteLog("Crafting Complete");
		outEntity = EntityHandler::Instance().CreateEntity(id);
		crafterOwner->AddItemToInventory(outEntity);
		return true;
	}
	else {
		LogHandler::Instance().WriteLog("Crafting MegaFail");
		return false;
	}
	return false;
}


bool Entity_Crafter::ConsumeCraftingItem(std::vector<int > recipeItem)
{
	for (int i = 0; i < heldItems.size(); i++) {
		for (int j = 0; j < recipeItem.size(); j++) {
			if (heldItems[i]->GetID() == recipeItem[j]) {
				EntityHandler::Instance().DestroyEntity(heldItems[i], world);
				return true;
			}
		}
	}
	return false;
}