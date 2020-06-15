#include "UIWindow_Inventory.h"
#include "EntityHandler.h"
void UIWindow_Inventory::Draw(sf::RenderTexture* renderTexture, sf::Text* text)
{
	std::vector<std::string> names;
	std::vector<Entity*> inventory = *(playerController->GetPilotingPlayer()->GetInventory()); 
	InventoryCollapsed.clear();
	for (int i = 0; i < inventory.size(); i++) {
		bool found = false;
		for (int j = 0; j < InventoryCollapsed.size(); j++) {
			if (InventoryCollapsed[j].first == inventory[i]->GetID()) {
				InventoryCollapsed[j].second++;
				found = true;
			}
		}
		if (found == false) {
			InventoryCollapsed.push_back(std::make_pair(inventory[i]->GetID(), 1));
		}
	}
	for (int j = 0; j < InventoryCollapsed.size(); j++) {
		names.push_back(EntityHandler::Instance().GetEntityNameByID(InventoryCollapsed[j].first) + " x" + std::to_string(InventoryCollapsed[j].second));
	}
	for (int i = 0; i < names.size(); i++)
	{
		text->setFillColor(sf::Color(248, 255, 138));
		text->setPosition(809, 22 + (i * 16));
		text->setString(names[i]);
		renderTexture->draw(*text);
	}
}

void UIWindow_Inventory::OnMouseLeftClickDown(sf::Vector2f MousePos)
{
	if (MousePos.x * 800 > 800)
	{
		if (MousePos.y * 800 > 22) {
			float i = floor(((MousePos.y*800.0f) - 22) / 16.0f);
			if (i >= 0 && i < InventoryCollapsed.size()) {
				playerController->SetReleventHeldItemID(InventoryCollapsed[i].first);
			}
			else {
				playerController->SetReleventHeldItemID(-1);
			}
			playerController->UpdateBehaviorList();
			UIHandler::Instance().SetWindow(UIHandler::Tasks);
		}
	}
}
