#include "Entity_Food.h"
#include "LogHandler.h"
#include "EntityHandler.h"

void Entity_Food::Tick()
{
	Entity::Tick();
	if (spoiled==false && age > spoilAge) {
		spoiled = true;
		LogHandler::Instance().WriteLog(objectName+" spoiled", this->GetPosition(), logContext::WORLD);
	}
}

bool Entity_Food::IsSpoiled()
{
	return spoiled;
}
