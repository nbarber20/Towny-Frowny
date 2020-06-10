#pragma once
#include <SFML/Graphics.hpp>
class World;
class Entity_Living;
class Entity_Container;
class Designation;
struct Entity
{
public: 

	enum TargetedHumanBehaviors {
		Targeted_ClearAll,
		Targeted_PickUp,
		Targeted_HarvestWood,
		Targeted_SlaughterAnimal,
		Targeted_WalkTo,
		Targeted_ConstructWall,
		Targeted_DestroyWall,
		Targeted_ConstructFloor,
		Targeted_DestroyFloor,
		Targeted_LootBody,
		Targeted_Attack,
		Targeted_Rotate,
		Targeted_DropItem,
		Targeted_Craft,
		Targeted_MakeStaircase,
		Targeted_UseStaircase,
		Targeted_UseDoor,
	};

	


	Entity(wchar_t EntityID, sf::Vector2i spritePos, World* worldref, std::vector<TargetedHumanBehaviors> TargetedBehaviors);
	virtual ~Entity() {};
	virtual void OnSpawn(World* newworld);
	virtual void OnDespawn(World* newworld);
	virtual void RemoveFromWorld();
	virtual void Tick();
	virtual void SetPosition(short x, short y);
	virtual sf::Vector2i GetPosition();
	virtual void DrawEntity();
	virtual bool MoveToTile(short dx, short dy);
	virtual bool StepTiles(short dx, short dy);
	virtual wchar_t GetID();
	virtual bool isInWorld();
	virtual void SetObjectName(std::string to);
	virtual std::string GetObjectName();
	virtual Entity_Container* GetParent();
	virtual void SetParent(Entity_Container* e);
	virtual Designation* GetDesignationParent();
	virtual void SetDesignationParent(Designation* d);

	void SetMovementCapability(bool to);
	bool GetMovementCapability();
	virtual void SetSpriteVariant(wchar_t x, wchar_t y);
	bool doTick = true;
	int GetVolume();
	void SetVolume(int to);
	virtual void Initilize() {};

	virtual Entity* clone() const { return new Entity(*this); };
	virtual void getTargetedBehaviors(std::vector<TargetedHumanBehaviors>* list, std::vector<TargetedHumanBehaviors>* failList = new std::vector<TargetedHumanBehaviors>) {
		for (auto b : TargetedBehaviors) {
			if (b == Entity::Targeted_DropItem) {
				if (parent != nullptr) list->push_back(b);
			}
			else if (b == Entity::Targeted_ConstructWall) {
				if (parent != nullptr) list->push_back(b);
			}
			else if (b == Entity::Targeted_ConstructFloor) {
				if (parent != nullptr) list->push_back(b);
			}
			else if (b == Entity::Targeted_PickUp) {
				if (parent == nullptr) list->push_back(b);
			}
			else {
				bool inList = false;
				for (auto a : *failList) {
					if (b == a) { //add this task 
						inList = true;
					}
				}
				if(inList==false)list->push_back(b); //we have not marked this task as fail
			}
		}
		delete failList;
	}

	World* getWorld() { return world; };

protected:

	enum Behaviors {
		//Human
		GatherWood,
		SlaughterAnimal,
		//Animals
		Roam,
		Graze,
		//Global
		Eat,
		Pase,
		Wait,
	};

	std::vector<TargetedHumanBehaviors> TargetedBehaviors;
	wchar_t EntityID = 0;
	wchar_t spriteX = 0;
	wchar_t spriteY = 0;
	short x;
	short y;
	short age; 
	int volume = 0;
	World* world;
	bool inworld = false;
	bool canMove = false;
	std::string objectName;
	Entity_Container* parent;
	Designation* designationParent;
};

