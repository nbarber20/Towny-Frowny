#include "BehaviorBranch.h"
#include "TaskManager.h"
#include "Task.h"

void BehaviorBranch::DeleteFailTree()
{
	for (int j = 0; j < FailTree.size(); j++) {
		delete this->FailTree[j];
	}
	this->FailTree.clear();
}

BehaviorBranch::~BehaviorBranch()
{
	for (int i = 0; i < Branch.size(); i++) {
		taskManager->DeleteTask(Branch[i]);
	}
	this->Branch.clear();
}
