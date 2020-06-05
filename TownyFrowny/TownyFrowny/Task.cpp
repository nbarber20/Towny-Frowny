#include "Task.h"
#include "TaskManager.h"
#include <iostream>
#include "VectorHelper.h"
#include "LogHandler.h"
#include "Entity_Living.h"
#include "EntityHandler.h"
#include <math.h>   
#include <cmath>
void Task::Tick() {
	TaskStatus status = TaskStatus::Running;
	if (began == false && finished == false) {// 1st pass
		status = this->Start();
	}
	else if (began == true && finished == false) { //2nd - inf pass
		ticks++;
		tickCount++;
		if (ticks > tickspeed) {
			ticks = 0;
			status = Execute();			
		}
	}

	if (status == TaskStatus::Complete)
	{
		finished = true;
		if (began == true) {
			ownerEntity->TaskComplete(this);
		}
	}
	else if (status == TaskStatus::Fail)
	{
		finished = true;
		if (began == true) {
			ownerEntity->TaskFail(this);
		}
	}
	else if (status == TaskStatus::Loop)
	{
		if (loopCount < 50) {
			Reset();
			loopCount++;
		}
		else {
			finished = true;
			if (began == true) {
				ownerEntity->TaskFail(this);
			}
		}
	}
};


Task::TaskStatus Task_Search_Singluar::Execute() {
	for (int i = sprialIndex; i < spiral.size(); i++) {
		sf::Vector2i offset = (spiral[i] - sf::Vector2i(rangeOffset, rangeOffset+1)) + ownerEntity->GetPosition();
		for (int id = 0; id < targetIDs.size(); id++) {
			if (world->DoesTileContainEntity(offset, targetIDs[id],deepSearch))
			{		
				Entity* newfoundtarget = world->GetEntityInTileByID(targetIDs[id], offset);
				if (onlyLiving) {
					Entity_Living* AsLiving = dynamic_cast<Entity_Living*>(newfoundtarget);
					if (AsLiving == nullptr&& AsLiving->IsAlive()==false) continue;
				}
				FoundTarget= new sf::Vector2i(offset);
				FoundTargetEntity= newfoundtarget;
				return TaskStatus::Complete;
			}
		}
		sprialIndex++;
		spiralTicks++;
		if (spiralTicks > length) {
			spiralTicks = 0;
			return TaskStatus::Running;
		}
	}	
	return Task::Fail;
}

Task::TaskStatus Task_Search::Execute()
{
	for (int i = sprialIndex; i < spiral.size(); i++) {
		sf::Vector2i offset = (spiral[i] - sf::Vector2i(rangeOffset, rangeOffset + 1)) + ownerEntity->GetPosition();
		for (int id = 0; id < targetIDs.size(); id++) {
			if (world->DoesTileContainEntity(offset, targetIDs[id], deepSearch))
			{
				Entity* newfoundtarget = world->GetEntityInTileByID(targetIDs[id], offset);
				if (onlyLiving) {
					Entity_Living* AsLiving = dynamic_cast<Entity_Living*>(newfoundtarget);
					if (AsLiving != nullptr && AsLiving->IsAlive() == false)continue;
				}
				FoundTargets.push_back(new sf::Vector2i(offset));
				FoundTargetEntities.push_back(world->GetEntityInTileByID(targetIDs[id], offset));		
			}
		}
		sprialIndex++;
		spiralTicks++;
		if (spiralTicks > length) {
			spiralTicks = 0;
			return TaskStatus::Running;
		}
	}
	if (FoundTargets.size() > 0) {
		return TaskStatus::Complete;
	}
	return Task::Fail;
}

Task::TaskStatus Task_WalkTo::Start()
{
	Task::Start();

	sf::Vector2i pos;
	if (targetEntity != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*targetEntity)) {
			pos = (*targetEntity)->GetPosition();
		}
		else {
			return TaskStatus::Fail;
		}
	}
	else if (target != nullptr) {
			pos = **target;
	}
	else {
		LogHandler::Instance().WriteLog("WalkTo Got bad data", ownerEntity->GetPosition(), logContext::ERROR);
		return TaskStatus::Fail;
	}

	pos = VectorHelper::ClampVector(pos, ownerEntity->GetPosition() - sf::Vector2i(maxDist, maxDist), ownerEntity->GetPosition() + sf::Vector2i(maxDist, maxDist)); //max walk distance

	if (floor(VectorHelper::GetMagnitude(pos - ownerEntity->GetPosition())) < stopDist) {
		for (int x = pos.x - stopDist; x < pos.x + stopDist + 1; x++) {
			for (int y = pos.y - stopDist; y < pos.y + stopDist+1; y++) {
				if (floor(VectorHelper::GetMagnitude(pos - sf::Vector2i(x, y))) >= stopDist) {
					if (pathfinder->FindPath(ownerEntity, 0, path, ownerEntity->GetPosition(), sf::Vector2i(x, y), world,false))
					{
						startedAtTarget = true;
						pathIndex = 0;
						return TaskStatus::Running;
					}
				}
			}
		}	
		return TaskStatus::Fail;
	}

	if (pathfinder->FindPath(ownerEntity, stopDist, path, ownerEntity->GetPosition(),pos, world))
	{
		pathIndex = 0;
		return TaskStatus::Running;
	}
	else {
		return TaskStatus::Fail;
	}
		
}

Task::TaskStatus Task_WalkTo::Execute()
{
	if (ownerEntity->GetMovementCapability() == false)
	{
		return Task::Fail;
	}

	sf::Vector2i pos;
	if (targetEntity != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*targetEntity)) {
			pos = (*targetEntity)->GetPosition();
		}
		else {
			return Task::Fail;
		}
	}
	else if (target != nullptr) {
		pos = **target;
	}
	else {
		LogHandler::Instance().WriteLog("WalkTo Got bad data", ownerEntity->GetPosition(), logContext::ERROR);
		return TaskStatus::Fail;
	}

	pos = VectorHelper::ClampVector(pos, ownerEntity->GetPosition() - sf::Vector2i(maxDist, maxDist), ownerEntity->GetPosition() + sf::Vector2i(maxDist, maxDist)); //max walk distance

	if (startedAtTarget) {
			
		if (floor(VectorHelper::GetMagnitude(pos - ownerEntity->GetPosition())) >= stopDist) {
			return Task::Complete;
		}
	}
	else {
		if (floor(VectorHelper::GetMagnitude(pos - ownerEntity->GetPosition())) <= stopDist) {
			return Task::Complete; //made it to tile
		}
	}		
	if (pathIndex > path.size()-1) {
		LogHandler::Instance().WriteLog("Ran out end of pathing", ownerEntity->GetPosition(), logContext::ERROR);
		return TaskStatus::Loop;
	}
	sf::Vector2i stepDir = path[pathIndex];
	if (ownerEntity->StepTiles(stepDir.x, stepDir.y)) {
		//Successfully stepped
		pathIndex++;
	}
	else {
		//Re path
		if (pathfinder->FindPath(ownerEntity, stopDist, path, ownerEntity->GetPosition(), pos, world))
		{
			pathIndex = 0;
		}
		else {
			return TaskStatus::Fail;
		} 
	}
	return Task::Running;
}

Task::TaskStatus Task_PickUp::Execute()
{
	if (this->entity != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*entity))
		{
			if (world->DoesTileContainEntity(ownerEntity->GetPosition(), *entity)) {
				ownerEntity->AddItemToInventory(*entity);
				std::string s = ownerEntity->GetIndividualName() + " picked up " + (*entity)->GetObjectName();
				LogHandler::Instance().WriteLog(s, ownerEntity->GetPosition(), logContext::WORLD);
				return Task::Complete;
			}
		}
	}
	else {
		for (int i = 0; i < targetIDs.size(); i++) {
			if (world->DoesTileContainEntity(ownerEntity->GetPosition(), targetIDs[i], true)) {
				Entity* e = world->GetEntityInTileByID(targetIDs[i], ownerEntity->GetPosition());
				if (e != nullptr) {
					ownerEntity->AddItemToInventory(e);
					std::string s = ownerEntity->GetIndividualName() + " picked up " + e->GetObjectName();
					LogHandler::Instance().WriteLog(s, ownerEntity->GetPosition(), logContext::WORLD);
					return Task::Complete;
				}
				else {
					return Task::Fail;
				}
			}
		}
	}	
	return Task::Fail;
}

Task::TaskStatus Task_Drop::Execute()
{
	if (toDrop != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*toDrop)) {
			if ((*toDrop)->GetParent() == ownerEntity) {
				std::string s = ownerEntity->GetIndividualName() + " dropped " + (*toDrop)->GetObjectName();
				LogHandler::Instance().WriteLog(s, ownerEntity->GetPosition(), logContext::WORLD);
				ownerEntity->DropItem(*toDrop);
				return Task::Complete;
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_Pase::Execute()
{
	if (ownerEntity->GetMovementCapability() == false)
	{
		return Task::Fail;
	}
	float randDir = rand() % 10;
	if (randDir <= 2.5) {
		ownerEntity->StepTiles(1, 0);
	}
	else if (randDir > 2.5 && randDir <= 5) {
		ownerEntity->StepTiles(-1, 0);
	}
	else if (randDir > 5 && randDir <= 7.5) {
		ownerEntity->StepTiles(0, 1);
	}
	else {
		ownerEntity->StepTiles(0, -1);
	}
	if (tickCount >= length) {
		return Task::Complete;
	}
	return Task::Running;
}

Task::TaskStatus Task_GetItemFromOwner::Execute()
{
	const int num = (ownerEntity)->GetInventory()->size();
	Entity** ptr = (num > 0) ? (ownerEntity)->GetInventory()->data() : nullptr;
	for (int i = 0; i < num; i++)
	{
		wchar_t id = (*ptr[i]).GetID();
		for (int j = 0; j < targetIDs.size(); j++) {
			if (id == targetIDs[j]) {
				foundEntity = (ptr[i]);
				return Task::Complete;
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_EatEntity::Execute()
{
	if (ToEat != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*ToEat)) {
			ownerEntity->Eat(*ToEat);
			return Task::Complete;
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_RestrainLivingEntity::Execute()
{
	if (ToRestrain != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*ToRestrain)) {

			Entity_Living* target = dynamic_cast<Entity_Living*>(*ToRestrain);
			if (target != NULL) {
				target->SetMovementCapability(false);
				return Task::Complete;
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_AttackLivingEntityUntilDead::Execute()
{
	if (ToKill != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*ToKill)) {
			Entity_Living* target = dynamic_cast<Entity_Living*>(*ToKill);
			if (target != NULL) {
				if (target->IsAlive()) {
					ownerEntity->AttackEntity(target, 100);
					return Task::Running;
				}
				else {
					return Task::Complete;
				}
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_LootBodilyInventory::Execute()
{
	if (ToLoot != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*ToLoot)) {
			Entity_Living* target = dynamic_cast<Entity_Living*>(*ToLoot);
			if (target != NULL) {
				std::vector<Entity*> e(*target->GetBodilyInventory());
				for (int i = 0; i < e.size(); i++)
				{
					ownerEntity->AddItemToInventory(e[i]);
					std::string s = ownerEntity->GetIndividualName() + " removed " + e[i]->GetObjectName() + " from " + target->GetObjectName();
					LogHandler::Instance().WriteLog(s, ownerEntity->GetPosition(), logContext::WORLD);
				}
				return Task::Complete;
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_CheckEntityAlive::Execute()
{
	if (ToCheck != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*ToCheck)) {
			Entity_Living* target = dynamic_cast<Entity_Living*>(*ToCheck);
			if (target != NULL) {
				if (target->IsAlive()) {
					return Task::Complete;
				}
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_Wait::Execute()
{
	if (tickCount >= length) {
		return Task::Complete;
	}
	return Task::Running;
}

Task::TaskStatus Task_SelectRandomEntityFromList::Execute()
{
	int s = (*EntityList).size();
	if (s > 1) {
		int r = rand() % (s - 1);
		selected = (*EntityList)[r];
		selectedTarget = new sf::Vector2i(selected->GetPosition());
		return Task::Complete;
	}
	else if (s == 1) {
		selected = (*EntityList)[0];
		selectedTarget = new sf::Vector2i(selected->GetPosition());
		return Task::Complete;
	}
	return Task::Fail;
}

Task::TaskStatus Task_AssignTaskToEntity::Execute()
{
	if (OrderTarget != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*OrderTarget)) {
			Entity_Living* target = dynamic_cast<Entity_Living*>(*OrderTarget);
			if (target != NULL) {
				target->OverwriteTasks(taskstoAssign, ownerEntity);
				return Task::Complete;
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_Search_For::Execute()
{
	if (toSearchFor != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*toSearchFor)) {
			for (int i = sprialIndex; i < spiral.size(); i++) {
				sf::Vector2i offset = (spiral[i] - sf::Vector2i(rangeOffset, rangeOffset + 1)) + ownerEntity->GetPosition();
				if ((*toSearchFor)->GetPosition() == offset)
				{
					return TaskStatus::Complete;
				}
				sprialIndex++;
				spiralTicks++;
				if (spiralTicks > length) {
					spiralTicks = 0;
					return TaskStatus::Running;
				}
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_LocateStockDesignation::Execute()
{
	for (int i = sprialIndex; i < spiral.size(); i++) {
		sf::Vector2i offset = (spiral[i] - sf::Vector2i(rangeOffset, rangeOffset + 1)) + ownerEntity->GetPosition();
		if (world->DoesTileContainDesignations(offset,typetoCheck))
		{
			std::vector<Designation*> d = world->GetTileDesignations(offset);
			for (int j = 0; j < d.size(); j++) {
				if (d[j]->type == typetoCheck) {
					sf::Vector2i pos;
					if (d[j]->GetItemSlot((*refEntity)->GetID(), pos)) {
						FoundTarget = new sf::Vector2i(pos);
						return TaskStatus::Complete;
					}
					else {
						sf::Vector2i emptyPos;
						if (d[j]->GetEmptySlot(emptyPos)) {
							FoundTarget = new sf::Vector2i(emptyPos);
							return TaskStatus::Complete;
						}
					}
				}
			}
		}
		sprialIndex++;
		spiralTicks++;
		if (spiralTicks > length) {
			spiralTicks = 0;
			return TaskStatus::Running;
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_Take::Execute()
{
	if (ToCheck != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*ToCheck)) {
			Entity_Container* target = dynamic_cast<Entity_Container*>(*ToCheck);
			if (target != NULL) {
				const int num = (target)->GetInventory()->size();
				Entity** ptr = (num > 0) ? (target)->GetInventory()->data() : nullptr;
				for (int i = 0; i < num; i++)
				{
					wchar_t id = (*ptr[i]).GetID();
					for (int j = 0; j < targetIDs.size(); j++) {
						if (id == targetIDs[j]) {
							foundItem = (ptr[i]);
							ownerEntity->AddItemToInventory(ptr[i]);
							return Task::Complete;
						}
					}
				}
				delete foundItem; // this pointer is bad, so get rid of it
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_ConstructWall::Execute()
{
	if (targetEntity != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*targetEntity)) {
			int id = (*targetEntity)->GetID();
			if (id == 19) { //stone
				world->SetWallTile(**target, 6);
			}
			else if (id == 11) {
				world->SetWallTile(**target, 7);
			}
			else if (id == 12) {
				world->SetWallTile(**target, 8);
			}
			else if (id == 13) {
				world->SetWallTile(**target, 9);
			}
			else if (id == 14) {
				world->SetWallTile(**target, 10);
			}
			EntityHandler::Instance().DestroyEntity(*targetEntity, world);
			return Task::Complete;
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_DeconstructWall::Execute()
{
	world->SetWallTile(**target, -1);
	return Task::Complete;
}


Task::TaskStatus Task_ConstructFloor::Execute()
{
	if (targetEntity != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*targetEntity)) {
			int id = (*targetEntity)->GetID();
			if (id == 19) { //stone
				world->SetGroundTile(**target, 6);
			}
			else if (id == 11) {
				world->SetGroundTile(**target, 7);
			}
			else if (id == 12) {
				world->SetGroundTile(**target, 8);
			}
			else if (id == 13) {
				world->SetGroundTile(**target, 9);
			}
			else if (id == 14) {
				world->SetGroundTile(**target, 10);
			}
			EntityHandler::Instance().DestroyEntity(*targetEntity, world);
			return Task::Complete;
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_DeconstructFloor::Execute()
{
	world->SetGroundTile(**target, 0);
	return Task::Complete;
}


Task::TaskStatus Task_Rotate::Execute()
{
	if (target != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*target)) {
			Entity_Manufactured* targetManufactured = dynamic_cast<Entity_Manufactured*>(*target);
			if (targetManufactured != NULL) {
				targetManufactured->Rotate();
				return Task::Complete;
			}
		}
	}
	return Task::Fail;
}

Task::TaskStatus Task_DropInStockDesignation::Execute()
{
	if (toDrop != nullptr) {
		if (EntityHandler::Instance().IsEntityValid(*toDrop)) {
			if ((*toDrop)->GetParent() == ownerEntity) {
				std::vector<Designation*> designations = world->GetTileDesignations(ownerEntity->GetPosition());
				for (int i = 0; i < designations.size(); i++) {
					if (designations[i]->type == typetoCheck) {
						std::string s = ownerEntity->GetIndividualName() + " dropped " + (*toDrop)->GetObjectName();
						LogHandler::Instance().WriteLog(s, ownerEntity->GetPosition(), logContext::WORLD);
						ownerEntity->DropItem(*toDrop);
						(*toDrop)->SetDesignationParent(designations[i]);
						return Task::Complete;
					}
				}
			}
		}
	}
	return Task::Fail;
}
