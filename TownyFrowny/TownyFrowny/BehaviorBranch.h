#pragma once
#include <SFML/Graphics.hpp>

class TaskManager;
class Task;

class BehaviorBranch {
public:
	BehaviorBranch(std::vector<Task*> Branch, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = {};
		this->retries = 0;
		this->taskManager = taskManager;
	}

	BehaviorBranch(std::vector<Task*> Branch, int retries, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = {};
		this->retries = retries;
		this->taskManager = taskManager;
	}
	BehaviorBranch(std::vector<Task*> Branch, std::vector <BehaviorBranch*>  FailTree, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = FailTree;
		this->retries = 0;
		this->taskManager = taskManager;
	}

	BehaviorBranch(std::vector<Task*> Branch, std::vector <BehaviorBranch*>  FailTree, int retries, TaskManager* taskManager) {
		this->Branch = Branch;
		this->FailTree = FailTree;
		this->retries = retries;
		this->taskManager = taskManager;
	}


	void DeleteFailTree();

	~BehaviorBranch(); 
	std::vector<Task*> Branch;
	int retries;
	std::vector <BehaviorBranch*>  FailTree;
	TaskManager* taskManager;
};