#pragma once

#include <vector>
#include "Task.h"
#include <iostream>
#include "PathFinder.h"
#include "EntityHandler.h"
#include "Designation.h"
#include "Recipies.h"
class TaskManager {
public:
	

	TaskManager(World* OverWorld, World* UnderWorld, PathFinder* pathfinder) {
		this->OverWorld = OverWorld;
		this->UnderWorld = UnderWorld;
		this->pathFinder = pathfinder;
	};


	void DeleteTask(Task* t) {
		delete t;
	}



	//General Behaviors

	std::vector<Task*> TREE_FindFood(int searchSize) {
		Task_Search_Singluar* lookTask = new Task_Search_Singluar(searchSize,false,false, EntityHandler::Instance().GetIDGroup(EntityHandler::Food), 5, 200);
		return
		{
			lookTask,
			new Task_WalkTo(pathFinder,0,&lookTask->FoundTarget, 1, 10),
			new Task_PickUp(EntityHandler::Instance().GetIDGroup(EntityHandler::Food), 1, 10),
		};
	}

	std::vector<Task*> TREE_EatHeldFood() {
		Task_GetItemFromOwner* invSearch = new Task_GetItemFromOwner(EntityHandler::Instance().GetIDGroup(EntityHandler::Food), 5, 10);
		return
		{
			invSearch,
			new Task_EatEntity(&invSearch->foundEntity, 4, 10),
		};
	}
	std::vector<Task*> TREE_Wait(int length) {
		return
		{
			new Task_Wait(1, length),
		};
	}
	std::vector<Task*> TREE_Pase(int interval) {
		return
		{
			new Task_Pase(interval, 10),
		};
	}

	std::vector<Task*> TREE_SlaughterAnimal(int searchSize) {
		Task_Search* lookTask = new Task_Search(searchSize, false, true, EntityHandler::Instance().GetIDGroup(EntityHandler::FoodAnimals), 5, 200);
		Task_SelectRandomEntityFromList* randSelect = new Task_SelectRandomEntityFromList(&lookTask->FoundTargetEntities, 1, 10);
		return
		{
			lookTask, //search local area for animals
			randSelect,//pick one
			new Task_AssignTaskToEntity(TREE_Wait(40),&randSelect->selected,1,10), //tell it to wait
			new Task_WalkTo(pathFinder,1,&randSelect->selectedTarget, 1, 10), //go next to it
			new Task_Search_For(&randSelect->selected,1,true, 1, 200), //ensure we are next to it
			new Task_RestrainLivingEntity(&randSelect->selected,1,10), //restrain animal
			new Task_AttackLivingEntityUntilDead(&randSelect->selected,1,10), //attack till dead
			new Task_LootBodilyInventory(&randSelect->selected,1,10), //remove all meat from the cows body
		};
	}

	std::vector<Task*> TREE_StoreFood(int searchSize){
		Task_GetItemFromOwner* heldItemCheck = new Task_GetItemFromOwner(EntityHandler::Instance().GetIDGroup(EntityHandler::Food),1, 10);
		Task_LocateStockDesignation* desLook = new Task_LocateStockDesignation(searchSize,Designation::Type::FoodStorage, &heldItemCheck->foundEntity, 5, 200);
		return
		{
			heldItemCheck,
			desLook,
			new Task_WalkTo(pathFinder,0,&desLook->FoundTarget,1,10),
			new Task_Drop(&heldItemCheck->foundEntity,&desLook->FoundTarget,1,10),
		};
	}

	std::vector<Task*> TREE_HarvestWood(int searchSize) {
		Task_Search_Singluar* lookTask = new Task_Search_Singluar(searchSize, false, false, EntityHandler::Instance().GetIDGroup(EntityHandler::Tree), 5, 200);
		Task_Take* take = new Task_Take(&lookTask->FoundTargetEntity, EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),true, 1, 10);
		Task_LocateStockDesignation* desLook = new Task_LocateStockDesignation(searchSize,Designation::Type::MaterialStorage, &take->foundItem, 5, 200);
		Task_GetItemFromOwner* invsearchforwood = new Task_GetItemFromOwner(EntityHandler::Instance().GetIDGroup(EntityHandler::Wood), 1, 10);

		return
		{
			lookTask,
			new Task_WalkTo(pathFinder,1,&lookTask->FoundTarget, 1, 10), //go next to it
			take,
			desLook,
			new Task_WalkTo(pathFinder,0,&desLook->FoundTarget,1,10),
			invsearchforwood,
			new Task_DropInStockDesignation(&invsearchforwood->foundEntity,Designation::Type::MaterialStorage,1,10),
		};
	}
	std::vector<Task*> TREE_SourceMaterials(int recipeID) {
		return {
			new Task_SourceMaterials(recipeID,pathFinder, 1, 10),
		};
	}
	std::vector<Task*> TREE_CraftItem(int recipeID) {
		Task_Search_Singluar* lookTask = new Task_Search_Singluar(20, false, false, EntityHandler::Instance().GetIDGroup(EntityHandler::Crafter), 5, 200);
		return { 
			lookTask,
			new Task_WalkTo(pathFinder, 1, &lookTask->FoundTarget, 1, 10),
			new Task_Give(&lookTask->FoundTargetEntity,Recipies::getRecipe(recipeID),1,10),
			new Task_Craft(recipeID,&lookTask->FoundTargetEntity, 1, 10),
		};
	}

	//Targeted Behaviors

	std::vector<Task*> TargetTREE_HarvestWood(Entity** TreeTarget) {
		return
		{
			new Task_WalkTo(pathFinder,1,TreeTarget, 1, 10), //go next to it
			new Task_Take(TreeTarget, EntityHandler::Instance().GetIDGroup(EntityHandler::Wood),true, 1, 10),
		};
	}

	std::vector<Task*> TargetTREE_SlaughterAnimal(Entity** AnimalTarget) {
		return
		{		
			new Task_AssignTaskToEntity(TREE_Wait(40),AnimalTarget,1,10), //tell it to wait
			new Task_WalkTo(pathFinder,1,AnimalTarget, 1, 10), //go next to it
			new Task_Search_For(AnimalTarget,1,true, 1, 200), //ensure we are next to it
			new Task_RestrainLivingEntity(AnimalTarget,1,10), //restrain animal
			new Task_AttackLivingEntityUntilDead(AnimalTarget,1,10), //attack till dead
			new Task_LootBodilyInventory(AnimalTarget,1,10), //remove all meat from the cows body
		};
	}

	std::vector<Task*> TargetTREE_PickUp(Entity** ItemTarget) {
		return
		{
			new Task_WalkTo(pathFinder,1,ItemTarget, 1, 10),
			new Task_PickUp(ItemTarget, 1, 10),
		};
	}

	std::vector<Task*> TargetTREE_WalkTo(sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,0,Position, 1, 10),
		};
	}

	std::vector<Task*> TargetTREE_ConstructFloor(Entity** Target, sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,0,Position, 1, 10),
			new Task_ConstructFloor(Target,Position, 1, 10),
		};
	}

	std::vector<Task*> TargetTREE_DeconstructFloor(sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,0,Position, 1, 10),
			new Task_DeconstructFloor(Position, 1, 10),
		};
	}
	std::vector<Task*> TargetTREE_ConstructWall(Entity** Target, sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,1,Position, 1, 10),
			new Task_ConstructWall(Target,Position, 1, 10),
		};
	}

	std::vector<Task*> TargetTREE_DeconstructWall(sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,1,Position, 1, 10),
			new Task_DeconstructWall(Position, 1, 10),
		};
	}

	std::vector<Task*> TargetTREE_Attack(Entity** Target) {
		return
		{
			new Task_WalkTo(pathFinder,1,Target, 1, 10),
			new Task_AttackLivingEntityUntilDead(Target,1,10),
		};
	}

	std::vector<Task*> TargetTREE_LootBody(Entity** Target) {
		return
		{
			new Task_WalkTo(pathFinder,1,Target, 1, 10),
			new Task_LootBodilyInventory(Target, 1, 10),
		};
	}
	std::vector<Task*> TargetTREE_Rotate(Entity** Target) {
		return
		{
			new Task_WalkTo(pathFinder,1,Target, 1, 10),
			new Task_Rotate(Target, 1, 10),
		};
	}
	std::vector<Task*> TargetTREE_DropItem(Entity** Target, sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,1,Position, 1, 10),
			new Task_Drop(Target,Position,1,10),
		};
	}
	std::vector<Task*> TargetTREE_MakeStairCase(sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,0,Position, 1, 10),
			new Task_CreateStaircase(OverWorld,UnderWorld,1,10),
		};
	}
	std::vector<Task*> TargetTREE_UseStairCase(sf::Vector2i** Position) {
		return
		{
			new Task_WalkTo(pathFinder,0,Position, 1, 10),
			new Task_UseStaircase(OverWorld,UnderWorld,1,10),
		};
	}
	std::vector<Task*> TargetTREE_UseDoor(Entity** Door) {
		return
		{
			new Task_WalkTo(pathFinder,1,Door, 1, 10),
			new Task_UseDoor(Door,1,10),
		};
	}
	std::vector<Task*> TargetTREE_TurnLightOn(Entity** Light) {
		return
		{
			new Task_WalkTo(pathFinder,1,Light, 1, 10),
			new Task_TurnOnLight(Light,1,10),
		};
	}
	std::vector<Task*> TargetTREE_TurnLightOff(Entity** Light) {
		return
		{
			new Task_WalkTo(pathFinder,1,Light, 1, 10),
			new Task_TurnOffLight(Light,1,10),
		};
	}
private:
	World* OverWorld;
	World* UnderWorld;
	PathFinder* pathFinder;
};