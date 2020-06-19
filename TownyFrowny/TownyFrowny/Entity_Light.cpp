#include "Entity_Light.h"
#include "VectorHelper.h"
#include "World.h"
#include "LogHandler.h"
Entity_Light::Entity_Light(wchar_t EntityID, sf::Vector2i spritePos, int lightDistance, int lightLevel, std::vector<TargetedHumanBehaviors> TargetedBehaviors) : Entity(EntityID, spritePos, TargetedBehaviors)
{
	this->lightDistance = lightDistance;
	this->lightLevel = lightLevel;
}

void Entity_Light::Initilize(World* worldref)
{
	Entity::Initilize(worldref);
	ls = new LightSource();
	ls->lightDistance = this->lightDistance;
	ls->lightLevel = this->lightLevel;
}

void Entity_Light::Tick()
{
	if (inworld) {
		if (lit) {
			ticksSinceFlicker++;

			if (ticksSinceFlicker > 4) {
				ticksSinceFlicker = 0;
				if (spriteY == 1)SetSpriteVariant(spriteX, 2);
				else SetSpriteVariant(spriteX, 1);
			}
			
			//Update Light
			if (lastPos != GetPosition()) {
				ls->position = this->GetPosition();
				world->UpdateLighting();
				lastPos = GetPosition();
			}
		}
		else {
			SetSpriteVariant(spriteX, 0);
		}		
	}
}

void Entity_Light::OnSpawn(World* newworld)
{
	Entity::OnSpawn(newworld); 
	ls->position = this->GetPosition();
}

void Entity_Light::OnDespawn(World* newworld,bool doDropItems)
{
	TurnOff();
	Entity::OnDespawn(newworld, doDropItems);
}

void Entity_Light::TurnOn()
{
	if (lit == false) {
		lit = true;
		world->NewLightSource(ls);
	}
}

void Entity_Light::TurnOff()
{
	if (lit == true) {
		lit = false;
		world->RemoveLightSource(ls);
	}
}
